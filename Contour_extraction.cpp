#include "opencv_inc.h"
#include "Contour_extraction.h"

Mat Erosion(int erosion_element,int erosion_size, Mat input_image, void*);
Mat Dilation(int dilation_element,int dilation_size, Mat input_image,void*);
Mat Make_contour(Mat* input_image,vector<Point>* input_vector);
vector<Point> Find_contour_point(Mat input_image);

Mat Contour(Mat* input_image,vector<Point>* contour_point)
{
    int erosion_elem = 0;   int erosion_size = 0;
    int dilation_elem = 0;  int dilation_size = 0;

    Mat image = *input_image;
	Mat out_image(image.rows,image.cols,CV_8UC1,Scalar(1));
	Mat temp(image.rows,image.cols,CV_8UC1,Scalar(1));
    
    //vector<vector<Point> >* Contour_vector;

	erosion_elem = 3;   erosion_size = 1;
	dilation_elem = 3;	dilation_size = 1;

	temp = Dilation(dilation_elem,dilation_size,image,0);
	temp = Dilation(dilation_elem,dilation_size,temp,0);
    temp = Dilation(dilation_elem,dilation_size,temp,0);
	temp = Erosion(erosion_elem,erosion_size,temp,0);
    
	out_image = Make_contour(&temp,contour_point);
    
    cvtColor(out_image,out_image,CV_BGR2GRAY);
    threshold(out_image, out_image, 100, 255, CV_THRESH_BINARY);
    //*contour_point = Find_contour_point(out_image);
    
	return out_image;
}

/**  Function Erosion  */
Mat Erosion( int erosion_elem, int erosion_size, Mat input_image, void* )
{
	  int erosion_type = 0;
	  if( erosion_elem  == 0 ){ erosion_type = MORPH_RECT; }
	  else if( erosion_elem  == 1 ){ erosion_type = MORPH_CROSS; }
	  else if( erosion_elem  == 2) { erosion_type = MORPH_ELLIPSE; }

	  Mat element = getStructuringElement( erosion_type,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );
	  Mat out_image;

	  /// Apply the erosion operation
	  erode( input_image,out_image, element );

	  return out_image;
}

/** Function Dilation */
Mat Dilation( int dilation_elem, int dilation_size, Mat input_image,  void* )
{
	  int dilation_type = 0;
	  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	  Mat element = getStructuringElement( dilation_type,Size( 2*dilation_size + 1, 2*dilation_size+1 ),Point( dilation_size, dilation_size ) );
	  Mat out_image;	  

	  /// Apply the dilation operation
	  dilate( input_image, out_image, element );

	  return out_image;
}

Mat Make_contour(Mat* input_image,vector<Point>* input_vector)
{
	Mat canny_output;
    Mat temp((*input_image).size(),CV_8UC1,Scalar(0));
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
    RNG rng(12345); Scalar color;
    int i;
    int length;
    
    threshold(*input_image, canny_output, 100, 255, CV_THRESH_BINARY);
    findContours(canny_output,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(0,0));
	
	Mat drawing = Mat::zeros(canny_output.size(),CV_8UC3);
    color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

	for(i=0;i<contours.size();i++)
	{
        length = arcLength(contours[i], 1);
        if(length>100)
        {
            cout << contours[i] << endl;
            drawContours(drawing,contours,i,color,1,8,hierarchy,2,Point());
        }
    }
    
	(*input_vector) = contours[0];
	
    return drawing;
}

/*
vector<Point> Find_contour_point(Mat input_image)
{
    Mat image(input_image.size(),CV_8UC1,Scalar(0));
    image = input_image;
    Mat temp_image(input_image.size(),CV_8UC1,Scalar(0));
    
    int i,j;
    int row_size = input_image.rows;    int col_size = input_image.cols;
    Scalar intensity;
    vector<Point> Contour_point; Point temp;
    
    for(i=0;i<row_size;i++)
    {
        for(j=0;j<col_size;j++)
        {
            intensity = image.at<uchar>(i,j);
            if(intensity.val[0]==255)
            {
                temp.x = j; temp.y = i;
                Contour_point.push_back(temp);
                temp_image.at<uchar>(i,j) = 255;
                imshow("temp",temp_image);
                waitKey();
            }
        }
    }
    
    return Contour_point;
}
*/
