#include "DataMotion.h"

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


	
	//Gaussian Model
	static Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor  
	static Mat DissolveMatrix = Mat::zeros(Rows, Cols, CV_8UC1);


	if (pMOG2 == NULL)
		pMOG2 = createBackgroundSubtractorMOG2(200, 16.0);

	pMOG2->apply(CL_Current_Frame, CL_Background_MOG);

	CL_Background_MOG.copyTo(BackgroundMOG);


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
				DissolveMatrix.data[y*Cols + x]--;

			if (DissolveMatrix.data[y*Cols + x] > DISSOLVE_RATE)
			{
				Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_BLUE] = Input_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_BLUE];
				Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_GREEN] = Input_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_GREEN];
				Background_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_RED] = Input_Image->data[frame_step_y * y + frame_step_x * x + COLOUR_RED];
			}

		}
	}

	//Shadow Map


	cvtColor(CL_Current_Frame, outBack, CV_RGB2HSV, 0);
	//split(outBack, outBackSplit);

	cvtColor(CL_Background_Frame, outInput, CV_RGB2HSV, 0);
	//split(outInput, outInputSplit);

	outInput.copyTo(HSV_Image);
	outInput.copyTo(HSV_Background);

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