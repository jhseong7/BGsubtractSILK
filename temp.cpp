//
//  temp.cpp
//  Design_project
//
//  Created by 이용주 on 2015. 11. 11..
//  Copyright © 2015년 Raymond. All rights reserved.
//
#include "opencv_inc.h"
#include "temp.hpp"


Mat Make_contour(Mat input_image,vector<vector<Point> >* input_vector)
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    
    int thresh = 150;
    Canny(input_image,canny_output,thresh,thresh*3,5);
    findContours(canny_output,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(0,0));
    
    int i;
    
    Mat drawing = Mat::zeros(canny_output.size(),CV_8UC3);
    Mat Resampling_drawing = Mat::zeros(canny_output.size(),CV_8UC3);
    Mat Control = Mat::zeros(canny_output.size(),CV_8UC3);
    Scalar color;
    color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
    int length;
    double inter_length=0;
    double point_num=0;
    
    for(i=0;i<contours.size();i++)
    {
        length = arcLength(contours[0], 1);
        if(length>100)
        {
            cout << contours[i] << endl;
            drawContours(drawing,contours,i,color,1,8,hierarchy,2,Point());
            imshow("Result", drawing);
            waitKey(0);
        }
    }
    
    int sample_point = 40;
    int j;
    Point temp;
    length = contours[0].size();
    
    vector<Point> refer_point(3);
    //refer_point = Find_refer_point(&drawing);
    
    Point Head = (refer_point)[0];
    Point Left_foot = (refer_point)[1];
    Point Right_foot = (refer_point)[2];
    int Head_num=0;
    int Right_foot_num=0;
    int Left_foot_num=0;
    
    for(j=0;j<length;j++)
    {
        temp = (contours)[0][j];
        if(temp==Head){Head_num=j;}
        else if (temp==Right_foot){Right_foot_num=j;}
        else if (temp==Left_foot){Left_foot_num=j;}
        else{;};
    }
    
    vector<Point> Head_start_contour(length);
    
    if(Head_num > Right_foot_num)
    {
        for(j=Head_num;j<length;j++){Head_start_contour[j-Head_num] = contours[0][j];}
        for(j=0;j<Head_num;j++){Head_start_contour[j+(length-Head_num-1)] = contours[0][j];}
        Right_foot_num = Right_foot_num - (length - Head_num);
        Left_foot_num = Left_foot_num - (length - Head_num);
        Head_num = 0;
    }
    else if(Head_num < Right_foot_num)
    {
        for(j=Head_num;j<length;j++){Head_start_contour[j-Head_num] = contours[0][j];}
        for(j=0;j<Head_num;j++){Head_start_contour[j+(length-Head_num)] = contours[0][j];}
        Right_foot_num = Right_foot_num - Head_num;
        Left_foot_num = Left_foot_num - Head_num;
        Head_num = 0;
    }
    
    int Segment1_length = Left_foot_num;
    int Segment2_length = Right_foot_num - Left_foot_num;
    int Segment3_length = length - Right_foot_num;
    int Segment1_sample_point=15;
    int Segment2_sample_point=10;
    int Segment3_sample_point=15;
    
    double Segment1_inter_length = (int)((double)Segment1_length/(double)Segment1_sample_point);
    int Segment1_point_num = (int)((double)Segment1_length/(double)Segment1_inter_length);
    double Segment2_inter_length = (int)((double)Segment2_length/(double)Segment2_sample_point);
    int Segment2_point_num = (int)((double)Segment2_length/(double)Segment2_inter_length);
    double Segment3_inter_length = (int)((double)Segment3_length/(double)Segment3_sample_point);
    int Segment3_point_num = (int)((double)Segment3_length/(double)Segment3_inter_length);
    int Whole_point_num = Segment1_point_num + Segment2_point_num + Segment3_point_num;
    
    cout << Head_start_contour << endl;
    
    temp = Head_start_contour[0];
    Resampling_drawing.at<Vec3b>(temp.y,temp.x/3)[3]=255;
    temp = Head_start_contour[Right_foot_num];
    Resampling_drawing.at<Vec3b>(temp.y,temp.x/3)[3]=255;
    temp = Head_start_contour[Left_foot_num];
    Resampling_drawing.at<Vec3b>(temp.y,temp.x/3)[3]=255;
    
    for(j=0;j<Whole_point_num;j++)
    {
        if(j<Segment1_length)
        {
            inter_length = Segment1_inter_length;
        }
        else if(j<Segment2_length)
        {
            inter_length = Segment2_inter_length;
        }
        else
        {
            inter_length = Segment3_inter_length;
        }
        temp = Head_start_contour[j*inter_length];
        cout << temp << endl;
        Resampling_drawing.at<Vec3b>(temp.y,temp.x/3)[0]=255;
        imshow("Drawing_2",Resampling_drawing);
        waitKey(0);
    }
    
    imshow("Result",Resampling_drawing);
    waitKey(0);
    //Canny(drawing,canny_output,thresh,thresh*3,5);
    //findContours(canny_output,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE,Point(0,0));
    
    *input_vector = contours;
    return drawing;
}
