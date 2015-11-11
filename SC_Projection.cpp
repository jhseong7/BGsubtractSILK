#include "DataMotion.h"

/*
Projection image creation function
by 박슬찬 11 Nov 2015

Edited by 성종현
at 11 Nov 2015
*/

#define pow2(a) a*a //제곱 계산



void SC_Projection(Mat* Input_Image, Mat* Output_Image) //Projection 이미지 생성기
{
	*Output_Image = Mat::zeros(Rows, Cols, Input_Image->type());
	
	int i, j;
	float count = 0;
	float x_sum, y_sum;
	float x_mean, y_mean;
	x_sum = 0; y_sum = 0;
	float xx, xy, yx, yy;
	xx = 0; xy = 0; yx = 0; yy = 0;
	float xd, yd;
	float A[2][2];

	float Major_Axis1, Major_Axis2, Minor_Axis1, Minor_Axis2; //Major Axis 1,2 Minor Axis 1,2


	//Contour 이미지에서 Contour 좌표의 평균을 구한다.
	for (int y = 0; y < Rows; y++)
	{
		for (int x = 0; x < Cols; x++)
		{
			if (Input_Image->at<Vec3b>(y, x)[0] > 100) //contour를 센다
			{
				x_sum += x;
				y_sum += y;
				count++;
			}
		}
	}

	if (count == 0)
		return;

	//contour들의 평균 x,y좌표 (x_mean, y_mean) 
	x_mean = x_sum / count; 
	y_mean = y_sum / count;


	for (int y = 0; y < Rows; y++)
	{
		for (int x = 0; x < Cols; x++)
		{
			//중심으로 부터의 상대좌표 계산
			xd = (float)x - x_mean; 
			yd = (float)y - y_mean;

			if (Input_Image->at<Vec3b>(y, x)[0] > 100) //Contour에서 
			{
				//covarience 계산
				xx += pow2(xd) / count;
				xy += pow2(xd) / count;
				yx += pow2(yd) / count;
				yy += pow2(yd) / count;
			}
		}
	}


	//------Eigen Vector Calculation------

	//
	// A = |xx xy|
	//     |yx yy|
	//

	A[0][0] = xx; A[0][1] = xy; 
	A[1][0] = yx; A[1][1] = yy;

	Mat CM(2, 2, CV_32FC1, A);
	Mat Eigen_Value(2, 1, CV_32FC1); //Eigen Value
	Mat	Eigen_Vector(2, 2, CV_32FC1); //Eigen Vector

	eigen(CM, Eigen_Value, Eigen_Vector);

	Major_Axis1 = Eigen_Vector.at<float>(0, 0);
	Major_Axis2 = Eigen_Vector.at<float>(0, 1);
	Minor_Axis1 = Eigen_Vector.at<float>(1, 0);
	Minor_Axis2 = Eigen_Vector.at<float>(1, 1);



	//-------Major, Minor Axis를 출력 이미지에 표기-----
	for (int y = 0; y < Rows; y++)
	{
		float x = ( (Major_Axis1 / Major_Axis2) * (y - y_mean)) + x_mean; // x = A(y-y1) + x1
		if (x > 0)
		{
			Output_Image->at<Vec3b>(y, x)[0] = 0;
			Output_Image->at<Vec3b>(y, x)[1] = 0;
			Output_Image->at<Vec3b>(y, x)[2] = 255; //Red
		}
	}

	for (int x = 0; x < Cols; x++)
	{
		float y = ((Minor_Axis2 / Minor_Axis1) * (x - x_mean)) + y_mean; // y = A(x-x1) + y1
		if (y < Rows)
		{
			if (y > 0)
			{
				Output_Image->at<Vec3b>(y, x)[0] = 0;
				Output_Image->at<Vec3b>(y, x)[1] = 255; //Green
				Output_Image->at<Vec3b>(y, x)[2] = 0;
			}
		}
	}





	float Head[2], Lfoot[2], Rfoot[2];
	float dummyx, dummyy;
	float Rmin[2], Rmaj[2], Gmin[2], Gmaj[2];


	Rmin[0] = (-1)*Major_Axis1; Rmin[1] = (-1)*Major_Axis2; 
	Rmaj[0] = Major_Axis1;      Rmaj[1] = Major_Axis2;

	Gmin[0] = (-1)*Minor_Axis1; Gmin[1] = (-1)*Minor_Axis2; 
	Gmaj[0] = Minor_Axis1;      Gmaj[1] = Minor_Axis2;


	Mat Ra(1, 2, CV_32FC1, Rmin);
	Mat Rb(1, 2, CV_32FC1, Rmaj);
	Mat Ga(1, 2, CV_32FC1, Gmin);
	Mat Gb(1, 2, CV_32FC1, Gmaj);
	Mat dumvec(1, 2, CV_32FC1);

	float Hmax, Lmax, Rmax, dum, ddum;
	Hmax = 0; Lmax = 0; Rmax = 0;

	for (int y = 0; y < Rows; y++)
	{
		for (int x = 0; x < Cols; x++)
		{
			if (Input_Image->at<Vec3b>(y, x)[0] > 100)
			{
				dummyx = (float)x - x_mean; dummyy = (float)y - y_mean;
				dumvec.at<float>(0, 0) = dummyx;
				dumvec.at<float>(0, 1) = dummyy;
				dum = Ra.dot(dumvec);

				if (Hmax < dum)
				{
					Hmax = dum;
					Head[0] = x; Head[1] = y;
				}
			}
		}
	}

	signed int ik, jk;

	for (ik = -2; ik < 3; ik++)
	{
		for (jk = -2; jk < 3; jk++)
		{
			Output_Image->at<Vec3b>(Head[1] + ik, Head[0] + jk)[0] = 255;  //Blue
			Output_Image->at<Vec3b>(Head[1] + ik, Head[0] + jk)[1] = 0;
			Output_Image->at<Vec3b>(Head[1] + ik, Head[0] + jk)[2] = 0;
		}
	}

	for (i = 0; i < Rows; i++)
	{
		for (j = 0; j < Cols; j++)
		{
			if (Input_Image->at<Vec3b>(i, j)[0] > 100)
			{
				dummyx = (float)j - x_mean; dummyy = (float)i - y_mean;
				dumvec.at<float>(0, 0) = dummyx;
				dumvec.at<float>(0, 1) = dummyy;
				dum = Ga.dot(dumvec);
				if (dum > 0)
				{
					ddum = Rb.dot(dumvec);
					if (ddum > 0)
					{
						if (Lmax < (ddum + dum))
						{
							Lmax = ddum + dum;
							Lfoot[0] = j, Lfoot[1] = i;
						}
					}
				}
			}
		}
	}

	for (ik = -2; ik < 3; ik++){
		for (jk = -2; jk < 3; jk++){
			Output_Image->at<Vec3b>(Lfoot[1] + ik, Lfoot[0] + jk)[0] = 255;
			Output_Image->at<Vec3b>(Lfoot[1] + ik, Lfoot[0] + jk)[1] = 0;
			Output_Image->at<Vec3b>(Lfoot[1] + ik, Lfoot[0] + jk)[2] = 0;
		}
	}

	for (i = 0; i < Rows; i++){
		for (j = 0; j < Cols; j++){
			if (Input_Image->at<Vec3b>(i, j)[0] > 100){
				dummyx = (float)j - x_mean; dummyy = (float)i - y_mean;
				dumvec.at<float>(0, 0) = dummyx;
				dumvec.at<float>(0, 1) = dummyy;
				dum = Gb.dot(dumvec);
				if (dum > 0){
					ddum = Rb.dot(dumvec);
					if (ddum > 0){
						if (Rmax < (ddum + dum)){
							Rmax = ddum + dum;
							Rfoot[0] = j, Rfoot[1] = i;
						}
					}
				}
			}
		}
	}

	for (ik = -2; ik < 3; ik++){
		for (jk = -2; jk < 3; jk++){
			Output_Image->at<Vec3b>(Rfoot[1] + ik, Rfoot[0] + jk)[0] = 255;
			Output_Image->at<Vec3b>(Rfoot[1] + ik, Rfoot[0] + jk)[1] = 0;
			Output_Image->at<Vec3b>(Rfoot[1] + ik, Rfoot[0] + jk)[2] = 0;
		}
	}


}