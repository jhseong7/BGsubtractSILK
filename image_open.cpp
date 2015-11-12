#include "opencv_inc.h"
#include "Image_open.h"


char identity_[10] = "lqf";
int identity_num=4;

Mat* Image_open(int number_of_image)
{
    int i;
    Mat *image_array = new Mat[number_of_image];
    char image_path[80];
    
    for(i=1;i<number_of_image;i++)
    {
        if(i<10)
        {
			sprintf(image_path, "/users/iyongju/Design_code/Sample/%s/00_%d/%s-00_%d-00%d.png", identity_, identity_num, identity_, identity_num, i);
        }
        else
        {
			sprintf(image_path, "/users/iyongju/Design_code/Sample/%s/00_%d/%s-00_%d-0%d.png", identity_, identity_num, identity_, identity_num, i);
        }
        
        image_array[i-1] = imread(image_path,0);
        
        if(!(image_array[i-1].data))
        {
            cout << "Could not open or find the image" << endl;
            abort();
        }
    }
	return image_array;
}

void Image_save(Mat* image_array)
{
    int i;
    char save_path[100];
    int size;
    
    if(i<10)
    {
		sprintf(save_path, "/users/iyongju/output/%s/%s-00_%d/result_image/%s-00_%d-00%d.png", identity_, identity_, identity_num, identity_, identity_num, i);
    }
    else
    {
		sprintf(save_path, "/users/iyongju/output/%s/%s-00_%d/result_image/%s-00_%d-0%d.png", identity_, identity_, identity_num, identity_, identity_num, i);
    }
    
    size = sizeof(image_array);
    for(i=0;i<size;i++)
    {
        imwrite(save_path,image_array[i]);
    }
}