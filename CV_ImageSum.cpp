#include "DataMotion.h"
#include "OpenCL.h"

using namespace cv;

void ShowSteadyContour(Mat* Output_Image, Mat* BW_Contour, Mat* Resize_Points)
{
	static int Out_Step_y = Output_Image->step[0];
	static int Out_Step_x = Output_Image->step[1];

	int BW_Step_y = BW_Contour->step[0];
	int BW_Step_x = BW_Contour->step[1];

	int BW_width = BW_Contour->rows;
	int BW_height = BW_Contour->cols;

	int RS_Step_y = Resize_Points->step[0];
	int RS_Step_x = Resize_Points->step[1];

	int RS_width = Resize_Points->rows;
	int RS_height = Resize_Points->cols;
	
	int BW_MoveOrigin_x = (Cols - BW_width) / 2;
	int RS_MoveOrigin_x = (3 * Cols - RS_width) / 2;

	int BW_x_border = BW_MoveOrigin_x + BW_width;
	int BW_y_border = BW_height;

	int RS_x_border = RS_MoveOrigin_x + RS_width;
	int RS_y_border = RS_height;


	//검은 색으로 초기화
	for (int x = 0; x < 2 * Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{
			if ((x >= BW_MoveOrigin_x && x < BW_x_border) && (y < BW_y_border) && (x < Cols))
			{
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 0] = BW_Contour->data[y*BW_Step_y + (x - BW_MoveOrigin_x) * BW_Step_x];
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 1] = BW_Contour->data[y*BW_Step_y + (x - BW_MoveOrigin_x) * BW_Step_x];
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 2] = BW_Contour->data[y*BW_Step_y + (x - BW_MoveOrigin_x) * BW_Step_x];
			}

			else if ((x >= RS_MoveOrigin_x && x < RS_x_border) && (y < RS_y_border) && (x >= Cols))
			{
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 0] = Resize_Points->data[y*RS_Step_y + (x - RS_MoveOrigin_x) * RS_Step_x + 0];
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 1] = Resize_Points->data[y*RS_Step_y + (x - RS_MoveOrigin_x) * RS_Step_x + 1];
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 2] = Resize_Points->data[y*RS_Step_y + (x - RS_MoveOrigin_x) * RS_Step_x + 2];
			}
			else
			{
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 0] = 0;
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 1] = 0;
				Output_Image->data[y*Out_Step_y + x * Out_Step_x + 2] = 0;
			}
						
		}
	}




}


bool CheckEmpty(Mat* Input_Image) // 단일 채널 이미지를 가정
{
	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{
			if (Input_Image->data[y*Cols + x] != 0)
				return false;
		}
	}

	return true;
}


void ImageMath(Mat* Result, Mat* Image1, Mat* Image2, char mode) //mode에 OR, AND, XOR을 받는다
{

	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{

			if (mode == 0)
				Result->data[y*Cols + x] = Image1->data[y*Cols + x] & Image2->data[y*Cols + x];

			if (mode == 1)
				Result->data[y*Cols + x] = Image1->data[y*Cols + x] | Image2->data[y*Cols + x];

			if (mode == 2)
				Result->data[y*Cols + x] = Image1->data[y*Cols + x] ^ Image2->data[y*Cols + x];



		}
	}


}

void ImageAbsSubtract(Mat* Result, Mat* Image1, Mat* Image2, char mode) //Result = Image1 - Image2 (Image1>Image2) else result=0;
{
	cl_mem memobj_Result = NULL;
	cl_mem memobj_Image1 = NULL;
	cl_mem memobj_Image2 = NULL;

	size_t Memory_Size = Rows*Cols*sizeof(uchar);

	global_work_size = Memory_Size;
	/*
	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{

			if (Image1->data[y*frame_step_y + x*frame_step_x + 0] > Image2->data[y*Cols + x])
				Result->data[y*Cols + x] = 255;
			else
				Result->data[y*Cols + x] = 0;
		}
	}
	*/

	memobj_Result = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size, NULL, &ret);
	memobj_Image1 = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size, NULL, &ret);
	memobj_Image2 = clCreateBuffer(context, CL_DEVICE_TYPE_DEFAULT, Memory_Size, NULL, &ret);

	ret = clEnqueueWriteBuffer(command_queue, memobj_Result, CL_TRUE, 0, Memory_Size, Result->data, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, memobj_Image1, CL_TRUE, 0, Memory_Size, Image1->data, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, memobj_Image2, CL_TRUE, 0, Memory_Size, Image2->data, 0, NULL, NULL);

	//Argument 전달 
	ret = clSetKernelArg(kernel_ImageAbsSubtract, 0, sizeof(cl_mem), (void*)&memobj_Result);
	ret = clSetKernelArg(kernel_ImageAbsSubtract, 1, sizeof(cl_mem), (void*)&memobj_Image1);
	ret = clSetKernelArg(kernel_ImageAbsSubtract, 2, sizeof(cl_mem), (void*)&memobj_Image2);

	//커널 실행
	ret = clEnqueueNDRangeKernel(command_queue, kernel_ImageAbsSubtract, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

	if (ret)
		ret += 1;
	//메모리 버퍼로부터 결과를 얻음 (Background Model만)
	ret = clEnqueueReadBuffer(command_queue, memobj_Result, CL_TRUE, 0, Memory_Size, Result->data, 0, NULL, NULL);



}