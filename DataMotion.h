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

//ShadowMap
extern cv::Mat Current_Frame;
extern cv::Mat BackgroundMOG;
extern int Rows, Cols;

using namespace cv;

void ShadowMapCreator(Mat* Shadow_Map, Mat* Input_Image, Mat* Background_Image);
void ImageAbsSubtract(Mat* Result, Mat* Image1, Mat* Image2, char mode);

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
