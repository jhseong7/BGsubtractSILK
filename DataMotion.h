#ifndef DATA_MOTION_H
#define DATA_MOTION_H

#include "general.h"
#include <opencv2\opencv.hpp>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/video/background_segm.hpp>  
#include <opencv2/videoio.hpp>
#include <opencv2\core\directx.hpp>
#include <opencv2\core\ocl.hpp>
#include "CV_Kalman.h"

//Feature Extraction - Yong Joo SulChan code header

using namespace cv;
using namespace std;

#include "Image_open.h"
#include "Cutting_silhouette_area.h"
#include "Contour_extraction.h"
#include "Gait_period_cal.hpp"
#include "Resampling.hpp"
#include "Find_refer_point.hpp"

//GraphCut
extern cv::Mat Silhouette_Final;
extern cv::Mat Silhouette_Track;


//ShadowMap
extern cv::Mat Current_Frame;
extern cv::Mat BackgroundMOG;
extern cv::Mat HSV_Image, HSV_Background;
extern cv::Mat HSV_Split[3], HSV_Background_Split[3];
extern int Rows, Cols;


void ShadowMapCreator(Mat* Shadow_Map, Mat* Input_Image, Mat* Background_Image);
bool CheckEmpty(Mat* Input_Image);
void ShowSteadyContour(Mat* Output_Image, Mat* BW_Contour, Mat* Resize_Points);
void ImageAbsSubtract(Mat* Result, Mat* Image1, Mat* Image2, char mode);

//Contour
Mat contour(Mat* input_image, vector<vector<Point> >* contour_vector);
void ContourBasedFilter(Mat* output_image, Mat* input_image);
void InitContourWindow();

//ShadowMap
void InitShadowMapWindow();

//Kalman
typedef MeasurementCS PredictedCS;

MeasurementCS DetectRectangle(Mat* Input_Image, Scalar Colour);
PredictedCS KalmanTracker(MeasurementCS MeasurementData);


extern MeasurementCS CurrentMeasurementData;
extern PredictedCS KalmanPredictedData;

//슬찬 projection
void SC_Projection(Mat* Input_Image, Mat* Output_Image);

//DataMotion

class DataMotion
{
public:
	int m_nWidth, m_nHeight, m_nMaxFrame;
	int m_nCurFrame, m_nBufferSize;
	int m_nTile, m_nRow, m_nColumn;

	RECT *m_rectTiles;

	std::string m_strName;
	std::string m_strCenter, m_strTruth;
	std::string m_strPrefix, m_strExtension;

	LPDIRECT3DTEXTURE9 m_texCenter, m_texTruth;


	//CV관련 추가 변수
	LPDIRECT3DTEXTURE9 CV_CurrentTexture;

	D3DLOCKED_RECT CV_Rect;

public:
	DataMotion();
	virtual ~DataMotion();

	bool VerifyImageSize();
	void AdjustParameters();
	bool LoadNextFrame( int d );
	HRESULT ReloadImage();

	bool SetUpDataset(LPSTR name);
private:
	void ResetFileName();

	bool SetUpTrees();
	bool SetUpLights();
	bool SetUpJug();
	bool SetUpBeach();
	bool SetUpRailway();
	//추가함수
	bool SetUpCapture();
};

#endif
