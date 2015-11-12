#include "opencv_inc.h"
#include "Image_open.h"
#include "Cutting_silhouette_area.h"
#include "Contour_extraction.h"
#include "Gait_period_cal.hpp"
#include "Resampling.hpp"
#include "Find_refer_point.hpp"

const int number_of_image = 69;

int amain(void)
{
    Mat* image_array;
    Mat* Contour_out_image_array = new Mat[number_of_image];
    Mat* Cutting_image_array = new Mat[number_of_image];
    Mat* Resampled_image_array = new Mat[number_of_image];
    
    int i;
    int height=0;   int width=0;    int period=0;
    double Bounding_box_ratio[number_of_image];
    
    vector<Point> contour_point_array;
    vector<Point> refer_point;
    vector<vector<Point> > Segment;
    
    // Image_open;
    image_array = Image_open(number_of_image);
    
    // Loop
    for(i=1;i<number_of_image;i++)
    {
        // Cutting the silhouette area and Calculating the width&height ratio
        Cutting_image_array[i-1] = Cutting_silhouette_area(&image_array[i-1], &height, &width);
        Bounding_box_ratio[i-1] = (double)height/(double)width;
        
        // Contour extraction
        Contour_out_image_array[i-1] = Contour(&Cutting_image_array[i-1],&contour_point_array);
        
        // Resampling
        refer_point = Find_refer_point(contour_point_array);
        Segment = Resampling(&contour_point_array,&refer_point);
        Resampled_image_array[i-1] = Draw_Resampling(Segment,Contour_out_image_array[i-1]);
        
        imshow("Original", image_array[i-1]);
        waitKey(0);
        imshow("Cutting_image",Cutting_image_array[i-1]);
        waitKey(0);
        imshow("Contour_image",Contour_out_image_array[i-1]);
        waitKey(0);
        imshow("Resampling_image",Resampled_image_array[i-1]);
        waitKey(0);
    }
    
    Image_save(Resampled_image_array);
    //period = Gait_period_cal(number_of_image, Bounding_box_ratio);
    
    return 0;
}


