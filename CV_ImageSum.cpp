#include "DataMotion.h"

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
	static int frame_step_y = Image1->step[0];
	static int frame_step_x = Image1->step[1];

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


}