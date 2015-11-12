#include "DataMotion.h"

using namespace cv;

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