//
//  Contour.cpp
//  Design_project_rebuild
//
//  Created by 이용주 on 2015. 11. 3..
//  Copyright © 2015년 Raymond. All rights reserved.
//

#include "opencv_essential_include.h"
#include "YJ_Contour.hpp"

///////////////////////
/* Contour parameter */

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;

static int trackbarposition = 100;

///////////////////////
///////////////////////

Mat Erosion(int erosion_elem, int erosion_size, Mat input_image, void*);
Mat Dilation(int dilation_elem, int dilation_size, Mat input_image, void*);
Mat Make_contour(Mat input_image, vector<vector<Point> >* input_vector);

/* Main function */


void InitContourWindow()
{
	namedWindow("Contour data", WINDOW_AUTOSIZE);
	createTrackbar("Track bar contour Length", "Contour data", &trackbarposition, 1000, NULL);
}

Mat contour(Mat* input_image, vector<vector<Point> >* contour_vector)
{
    Mat image;
    Mat out_image(image.rows,image.cols,CV_8UC1);
    Mat temp(image.rows,image.cols,CV_8UC1);
    
    out_image = Scalar(1);
    temp = Scalar(1);
    image = *input_image;
    
    erosion_elem = 3; erosion_size = 3;
    dilation_elem = 3; dilation_size = 3;
    
    temp = Dilation(dilation_elem,dilation_size,image,0);
    temp = Dilation(dilation_elem,dilation_size,temp,0);
    temp = Erosion(erosion_elem,erosion_size,temp,0);

    out_image = Make_contour(temp,contour_vector);
    
    return out_image;
}

/* Erosion Function */

Mat Erosion( int erosion_elem, int erosion_size, Mat input_image, void* )
{
    int erosion_type=0;
    if( erosion_elem  == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem  == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem  == 2) { erosion_type = MORPH_ELLIPSE; }
    
    Mat element = getStructuringElement( erosion_type,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );
    Mat out_image;
    
    erode( input_image,out_image, element );
    
    return out_image;
}

/* Dilation Function */

Mat Dilation( int dilation_elem, int dilation_size, Mat input_image,  void* )
{
    int dilation_type=0;
    if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
    else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
    else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
    
    Mat element = getStructuringElement( dilation_type,Size( 2*dilation_size + 1, 2*dilation_size+1 ),Point( dilation_size, dilation_size ) );
    Mat out_image;
    
    dilate( input_image, out_image, element );
    
    return out_image;
}

/* Contour make Function */

Mat Make_contour(Mat input_image,vector<vector<Point> >* input_vector)
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<vector<Point> > screened_contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    
    int thresh = 150;
    Canny(input_image,canny_output,thresh,thresh*3,5);
    findContours(canny_output,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(0,0));
    //screened_contours[contours.size()];
    
    int i;
    
    Mat drawing = Mat::zeros(canny_output.size(),CV_8UC3);
    Scalar color;
    for(i=0;i<contours.size();i++)
    {
        int length = arcLength(contours[i], 1);
        color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
		if (length>trackbarposition)//250)
        {
            drawContours(drawing,contours,i,color,1,8,hierarchy[0][0],0,Point());
            //screened_contours[i] = contours[i];
        }
    }
    
    //*input_vector = screened_contours;
    return drawing;
}
