#include "DataMotion.h"
#include "OpenCL.h"

#define INTENSITY_LEVEL 256
#define COLOUR_DEPTH 3

#define COLOUR_BLUE 0
#define COLOUR_GREEN 1
#define COLOUR_RED 2

#define ALPHA 0.8
#define BETA 0.9
#define S_THRESH 60
#define H_THRESH 10

#define min_fast(a,b) ( (a>b) ? b : a)

#define DISSOLVE_RATE 20

using namespace cv;

static int Alpha1000 = (int)(ALPHA * 1000.0);
static int Beta1000 = (int)(BETA * 1000.0);
static int S_Thresh = S_THRESH;
static int H_Thresh = H_THRESH;


void InitShadowMapWindow()
{
	namedWindow("Shadow Map", WINDOW_AUTOSIZE);
	//Trackbar 생성
	createTrackbar("Alpha", "Shadow Map", &Alpha1000, 1000, NULL);
	createTrackbar("Beta", "Shadow Map", &Beta1000, 1000, NULL);
	createTrackbar("S Threshold", "Shadow Map", &S_Thresh, 255, NULL);
	createTrackbar("H Threshold", "Shadow Map", &H_Thresh, 180, NULL);
}

void ShadowMapCreator(Mat* Shadow_Map, Mat* Input_Image, Mat* Background_Image)
{
	//OpenCL 용 UMat 생성
	UMat outBack, outBackSplit;
	UMat outInput, outInputSplit;
	UMat CL_Background_MOG;

	//OpenCl
	 cl_mem memobj_Background = NULL;
	 cl_mem memobj_Inputimage = NULL;
	 cl_mem memobj_Dissolve = NULL;
	 cl_mem memobj_BackgroundMOG = NULL;
	
	int Channel = Input_Image->channels();

	size_t Memory_Size= Rows*Cols*Channel*sizeof(uchar);
	size_t Memory_Size_Uni = Rows*Cols*1*sizeof(uchar);

	global_work_size = Memory_Size;

	//Gaussian Model
	static Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor  
	static Mat DissolveMatrix = Mat::zeros(Rows, Cols, CV_8UC3);


	if (pMOG2 == NULL)
		pMOG2 = createBackgroundSubtractorMOG2(200, 16.0);

	pMOG2->apply(CL_Current_Frame, CL_Background_MOG);

	CL_Background_MOG.copyTo(BackgroundMOG);

	cvtColor(CL_Current_Frame, outInput, CV_RGB2HSV, 0);
	//split(outBack, outBackSplit);

	cvtColor(CL_Background_Frame, outBack, CV_RGB2HSV, 0);
	//split(outInput, outInputSplit);

	outInput.copyTo(HSV_Image);
	outBack.copyTo(HSV_Background);

	/*
	//Serial Code
	int frame_step_y = Input_Image->step[0];
	int frame_step_x = Input_Image->step[1];
	int r_data, g_data, b_data;


	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{
			r_data = Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_RED];
			g_data = Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_GREEN];
			b_data = Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_BLUE];

			//MOG에서 background인 경우에만 model을 업그래이드
			if (BackgroundMOG.data[y*Cols + x] == 0 && DissolveMatrix.data[y*Cols + x] <= DISSOLVE_RATE)
				DissolveMatrix.data[y*Cols + x]++;
			else //dissolve가 안되면 감점
				DissolveMatrix.data[y*Cols + x]=0;

			if (DissolveMatrix.data[y*Cols + x] > DISSOLVE_RATE)
			{
				Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_BLUE] = Input_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_BLUE];
				Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_GREEN] = Input_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_GREEN];
				Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_RED] = Input_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_RED];
			}

		}
	}
	*/
	
	//CL = BackgroundModeler(__global uchar* Background_Model, __global uchar* Input_Image, __global uchar* Dissolve_Matrix)
	
	memobj_Background = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size, NULL, &ret);
	memobj_Inputimage = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size, NULL, &ret);
	memobj_Dissolve = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size, NULL, &ret);
	memobj_BackgroundMOG = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size_Uni, NULL, &ret);

	ret = clEnqueueWriteBuffer(command_queue, memobj_Background, CL_TRUE, 0, Memory_Size, Background_Image->data, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, memobj_Inputimage, CL_TRUE, 0, Memory_Size, Input_Image->data, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, memobj_Dissolve, CL_TRUE, 0, Memory_Size, DissolveMatrix.data, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, memobj_BackgroundMOG, CL_TRUE, 0, Memory_Size_Uni, BackgroundMOG.data, 0, NULL, NULL);

	//Argument 전달 
	ret = clSetKernelArg(kernel_BG_Model, 0, sizeof(cl_mem), (void*)&memobj_Background);
	ret = clSetKernelArg(kernel_BG_Model, 1, sizeof(cl_mem), (void*)&memobj_Inputimage);
	ret = clSetKernelArg(kernel_BG_Model, 2, sizeof(cl_mem), (void*)&memobj_Dissolve);
	ret = clSetKernelArg(kernel_BG_Model, 3, sizeof(cl_mem), (void*)&memobj_BackgroundMOG);

	//커널 실행
	ret = clEnqueueNDRangeKernel(command_queue, kernel_BG_Model, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

	if (ret)
		ret += 1;
	//메모리 버퍼로부터 결과를 얻음 (Background Model만)
	ret = clEnqueueReadBuffer(command_queue, memobj_Background, CL_TRUE, 0, Memory_Size * sizeof(uchar), Background_Image->data, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, memobj_Dissolve, CL_TRUE, 0, Memory_Size * sizeof(uchar), DissolveMatrix.data, 0, NULL, NULL);
	
	//imshow("BackMOG", BackgroundMOG);
	//imshow("Back", (*Background_Image));
	
	//Shadow Map




	//0: Hue (angle, 0~360), 1: Saturation(0~1), 2: Value(0~1)

	bool HueCondition = false;
	bool SatCondition = false;
	bool ValCondition = false;

	double HueCon_data = 0.0;
	double SatCon_data = 0.0;
	double ValCon_data = 0.0;

	int HSV_step_y = HSV_Image.step[0];
	int HSV_step_x = HSV_Image.step[1];

	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{
			HueCondition = false;
			SatCondition = false;
			ValCondition = false;

			//condition calculation

			//Val
			ValCon_data = (double)HSV_Image.data[y*HSV_step_y + x*HSV_step_x + 2] / (double)HSV_Background.data[y*HSV_step_y + x*HSV_step_x + 2];

			if (ValCon_data >= (Alpha1000 / 1000.0) && ValCon_data <= (Beta1000 / 1000.0))
				ValCondition = true;

			//Saturation
			SatCon_data = abs(HSV_Image.data[y*HSV_step_y + x*HSV_step_x + 2] - HSV_Background.data[y*HSV_step_y + x*HSV_step_x + 2]);

			if (SatCon_data <= S_Thresh)
				SatCondition = true;

			//Hue
			HueCon_data = min_fast(HSV_Image.data[y*HSV_step_y + x*HSV_step_x + 0] - HSV_Background.data[y*HSV_step_y + x*HSV_step_x + 0], 360 - (HSV_Image.data[y*HSV_step_y + x*HSV_step_x + 0] - HSV_Background.data[y*HSV_step_y + x*HSV_step_x + 0]));

			if (HueCon_data <= H_Thresh)
				HueCondition = true;


			//Shadow Map
			if (HueCondition && SatCondition && ValCondition)
				Shadow_Map->data[y*Cols + x] = 255;
			else
				Shadow_Map->data[y*Cols + x] = 0;

			if (BackgroundMOG.data[y*Cols + x] == 127) //MOG에서 detect된 shadow data도 사용
				Shadow_Map->data[y*Cols + x] = 255;


		}
	}


}