#include "DataMotion.h"



#include <iomanip>

using namespace cv::directx;

DataMotion::DataMotion()
{

	m_rectTiles = NULL;

	m_nWidth = m_nHeight = m_nMaxFrame = 0;
	m_nCurFrame = 0;
	m_nBufferSize = 0;
	m_nTile = m_nRow = m_nColumn = 0;

	m_texCenter = m_texTruth = NULL;
}

DataMotion::~DataMotion()
{
	SAFE_RELEASE( m_texCenter );
	SAFE_RELEASE( m_texTruth );
}

bool DataMotion::VerifyImageSize()
{
	D3DXIMAGE_INFO info;
	if( FAILED( D3DXGetImageInfoFromFile( m_strCenter.c_str(), &info ) ) )
	{
		MessageBox(NULL, "Cannot open the center image",
			"Error", MB_ICONERROR | MB_OK);
		return false;
	}
	m_nWidth = info.Width, m_nHeight = info.Height;

	return true;
}

bool DataMotion::LoadNextFrame( int d )
{
	if ( m_nCurFrame+d < 0 || m_nCurFrame+d > m_nMaxFrame )
		return false;

	m_nCurFrame += d;
	ReloadImage();

	return true;
}

HRESULT DataMotion::ReloadImage()
{
	::ZeroMemory(&CV_Rect, sizeof(D3DLOCKED_RECT));

	D3DXCreateTexture(g_pd3dDevice, m_nWidth, m_nHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &CV_CurrentTexture);
	D3DXCreateTexture(g_pd3dDevice, m_nWidth, m_nHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texCenter);
		
	CV_CurrentTexture->LockRect(0, &CV_Rect, NULL, 0);

	int framestep_y = Current_Frame.step[0];
	int framestep_x = Current_Frame.step[1];

	BYTE *TexturePtr = (BYTE*)(CV_Rect.pBits);

	for (int x = 0; x < m_nWidth; x++)
	{
		for (int y = 0; y < m_nHeight; y++)
		{
			TexturePtr[y * 4 * m_nWidth + x * 4 + 0] = Current_Frame.data[y*framestep_y + x*framestep_x + 0];
			TexturePtr[y * 4 * m_nWidth + x * 4 + 1] = Current_Frame.data[y*framestep_y + x*framestep_x + 1];
			TexturePtr[y * 4 * m_nWidth + x * 4 + 2] = Current_Frame.data[y*framestep_y + x*framestep_x + 2];
			TexturePtr[y * 4 * m_nWidth + x * 4 + 3] = 0;
		}
	}
	
	CV_CurrentTexture->UnlockRect(0);

	g_pd3dDevice->UpdateTexture(CV_CurrentTexture, m_texCenter);


	std::ostringstream strbuff;

	/*
	//strbuff << m_strPrefix << ""
	//	<< std::setfill('0') << std::setw(5) << m_nCurFrame << m_strExtension;

	strbuff << "Current frame.bmp";
	m_strCenter = strbuff.str();

	

	if( FAILED( D3DXCreateTextureFromFileExA( g_pd3dDevice, m_strCenter.c_str(),
			m_nWidth, m_nHeight, 1, 0, D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE,
			0, NULL, NULL, &m_texCenter ) ) )
		return E_FAIL;

	//D3DXSaveTextureToFileA("m_texCenter.jpg", D3DXIFF_JPG, m_texCenter, NULL);

	//unsigned int uiSize = GetTextureSize();
	D3DLOCKED_RECT rect;
	//ARGB BlackColor = { (char)0xFF, (char)0xFF, (char)0xFF, (char)0x00 };
	::ZeroMemory(&rect, sizeof(D3DLOCKED_RECT));

	if (m_texCenter == NULL)
	{
		return ERROR;
	}

	HRESULT reslt = m_texCenter->LockRect(0, &rect, NULL, 0);
	void* ptrs = rect.pBits;

	*/

	/*---------------test-----------------------------

	Mat TestFrame = imread("Current Frame.bmp");

	D3DLOCKED_RECT lr;
	ZeroMemory(&lr, sizeof(lr));
	LPDIRECT3DSURFACE9 suf = NULL;
	m_texCenter->GetSurfaceLevel(0, &suf);

	D3DSURFACE_DESC sr;
	ZeroMemory(&sr, sizeof(sr));

	HRESULT d = suf->GetDesc(&sr);
	HRESULT l = suf->LockRect(&lr, NULL, D3DLOCK_READONLY);

	HRESULT k = m_texCenter->LockRect(0, &lr, NULL, D3DLOCK_READONLY);

	BYTE *surfPtr = (BYTE*)(lr.pBits);

	cv::Mat m_image;
	TestFrame.copyTo(m_image);

	unsigned char *pSurfPtr = static_cast<unsigned char*>(lr.pBits);

	IplImage p_image = m_image;
	BYTE* pBufferData;
	for (UINT h = 0; h < sr.Height; h++)
	{
		for (UINT w = 0; w < sr.Width; w++)
		{
			pBufferData = (BYTE*)lr.pBits + (h * (lr.Pitch)) + w * 4;

			pBufferData[0] = (BYTE)(p_image.imageData[h*p_image.widthStep + w * 3 + 0]);
			pBufferData[1] = (BYTE)(p_image.imageData[h*p_image.widthStep + w * 3 + 1]);
			pBufferData[2] = (BYTE)(p_image.imageData[h*p_image.widthStep + w * 3 + 2]);
		}
	}

	imshow("m_image", m_image);

	//------------------------------------------------------------------------------------------*/

	//cv::Mat ImageData(m_nWidth, m_nHeight, CV_8UC4);
	//convertFromDirect3DSurface9(&m_texCenter, ImageData, NULL);
	//PALETTEENTRY palettesample = { 255, 255, 255, 255 };
	//D3DXSaveTextureToFile("test.jpg", D3DXIFF_JPG, m_texCenter, &palettesample);
	//strbuff.str("");
	//strbuff << m_strPrefix << "_truth."
	//	<< std::setfill('0') << std::setw(3) << m_nCurFrame << m_strExtension;

	m_strTruth = strbuff.str();

	if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice, m_strTruth.c_str(),
			m_nWidth, m_nHeight, 1, 0, D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE,
			0, NULL, NULL, &m_texTruth ) ) )
		m_texTruth = NULL;

	

	return S_OK;
}

void DataMotion::AdjustParameters()
{
	m_nTile = m_nBufferSize;
	m_nColumn = MAX_TEXTURE_SIZE / m_nWidth;
	m_nRow = (m_nTile + m_nColumn - 1) / m_nColumn;
	m_nColumn = (m_nTile + m_nRow - 1) / m_nRow;

	m_rectTiles = new RECT[m_nTile];
	for ( int r=0, k=0; r<m_nRow; r++ )
		for ( int c=0; c<m_nColumn && k<m_nTile; c++, k++ )
			SetRect( &m_rectTiles[k], c*m_nWidth, r*m_nHeight,
				(c+1)*m_nWidth, (r+1)*m_nHeight );
}

bool DataMotion::SetUpCapture()
{
	m_strName = "Reference";
	m_strCenter = "Reference.bmp";
	m_nMaxFrame = 3000;
	m_nBufferSize = 20;

	m_strPrefix = "";
	m_strExtension = ".bmp";

	if (!VerifyImageSize())
		return false;
	AdjustParameters();
	return true;
}

bool DataMotion::SetUpTrees()
{
	m_strName = "trees";
	m_strCenter = "Data/trees/00000.bmp";
	m_nMaxFrame = 100;
	m_nBufferSize = 50;

	m_strPrefix = "Data/trees/";
	m_strExtension = ".bmp";

	if ( ! VerifyImageSize() )
		return false;
	AdjustParameters();
	return true;
}

bool DataMotion::SetUpLights()
{
	m_strName = "lights";
	m_strCenter = "H:/Data/motion/lights/lights.000.bmp";
	m_nMaxFrame = 100;
	m_nBufferSize = 50;

	m_strPrefix = "H:/Data/motion/lights/lights";
	m_strExtension = ".bmp   ";

	if ( ! VerifyImageSize() )
		return false;
	AdjustParameters();
	return true;
}

bool DataMotion::SetUpJug()
{
	m_strName = "jug";
	m_strCenter = "H:/Data/motion/jug/jug.000.bmp";
	m_nMaxFrame = 100;
	m_nBufferSize = 50;

	m_strPrefix = "H:/Data/motion/jug/jug";
	m_strExtension = ".bmp   ";

	if ( ! VerifyImageSize() )
		return false;
	AdjustParameters();
	return true;
}

bool DataMotion::SetUpBeach()
{
	m_strName = "beach";
	m_strCenter = "H:/Data/motion/beach/beach.000.bmp";
	m_nMaxFrame = 140;
	m_nBufferSize = 50;

	m_strPrefix = "H:/Data/motion/beach/beach";
	m_strExtension = ".bmp   ";

	if ( ! VerifyImageSize() )
		return false;
	AdjustParameters();
	return true;
}

bool DataMotion::SetUpRailway()
{
	m_strName = "railway";
	m_strCenter = "H:/Data/motion/railway/railway.000.bmp";
	m_nMaxFrame = 100;
	m_nBufferSize = 50;

	m_strPrefix = "H:/Data/motion/railway/railway";
	m_strExtension = ".bmp   ";

	if ( ! VerifyImageSize() )
		return false;
	AdjustParameters();
	return true;
}

bool DataMotion::SetUpDataset(LPSTR name)
{
	if ( _stricmp(name, "trees") == 0 )
		return SetUpTrees();
	else if ( _stricmp(name, "lights") == 0 )
		return SetUpLights();
	else if ( _stricmp(name, "jug") == 0 )
		return SetUpJug();
	else if ( _stricmp(name, "beach") == 0 )
		return SetUpBeach();
	else if ( _stricmp(name, "railway") == 0 )
		return SetUpRailway();
	else
		return SetUpCapture();

	return false;
}
