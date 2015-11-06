// StereoD3D.cpp : Defines the entry point for the application.
//

#include "framerate.h"
#include "DataMotion.h"
#include "LearnBackground.h"
#include "PushRelabel.h"
#include "EvaluateMask.h"
#include <fstream>

//-----------------------------------------------------------------------------
//Define
//-----------------------------------------------------------------------------

#define WEBCAM_MODE true
#define WEBCAM_NUMBER 1

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
FrameRate g_frameRate;
DataMotion g_data;
std::ofstream g_fpReport;

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
D3DDEVTYPE g_devType = D3DDEVTYPE_HAL;

LearnBackground *g_pLearnBackground = NULL;
PushRelabel *g_pPushRelabel = NULL;
EvaluateMask *g_pEvaluateMask = NULL;

int g_nColumn = 1, g_nRow = 1, g_nSpan = 1, g_nPosX = 50, g_nPosY = 100;
bool g_bPlayStarted = false;

int g_nPulseStep = 20;
float g_rThreshold = 0.70f;
float g_rCutoffBack = 0.40f, g_rCutoffFore = 0.05f;
float g_rDamping = 0.99f;


//OpenCV Global Variables
char videoFilename[] = "[mix]IMG_1455backH264(AVC1)302015_11_02.avi";
char* SaveFilename = "Result.avi";
cv::VideoCapture capture;
cv::VideoCapture camera;

cv::Mat Current_Frame;
cv::Mat Background_Frame;
cv::Mat Shadow_Map;
cv::Mat BackgroundMOG;
cv::Mat Silouette_Final;
cv::Mat Silouette_SILK;

int Rows, Cols;

using namespace std;
using namespace cv;

VOID DetectMotion()
{
	g_pLearnBackground->CombinationScore( g_data.m_texCenter, g_rCutoffBack );
	g_pPushRelabel->InitializeExcessResidual( g_pLearnBackground->GetScoreTex(), g_rThreshold );
	for ( int i=0; i<g_nPulseStep; i++ )
		g_pPushRelabel->PulseOperation( i>g_nPulseStep/2 );
	g_pLearnBackground->GenerateWeight
		( g_pLearnBackground->GetScoreTex(), g_rThreshold, g_rCutoffFore, g_rCutoffBack );
//	g_pLearnBackground->GenerateWeight
//		( g_pPushRelabel->GetExcessTex(), g_nPulseStep/2/255.f, g_rCutoffFore, g_rCutoffBack );
//	g_pLearnBackground->GenerateSlopeWeight
//		( g_pLearnBackground->GetLossTex(), g_rThresBack, g_rThresFore, g_rCutoffFore, g_rCutoffBack );
	g_pLearnBackground->UpdateMemory( g_rDamping );
}

VOID Render( HWND hWnd )
{
	if( NULL == g_pd3dDevice )
		return;

	cv::Mat ImageData(g_data.m_nWidth, g_data.m_nHeight, CV_8UC4);

	LPDIRECT3DSURFACE9 pSurfOldRenderTarget = NULL;
	std::ostringstream strbuff;
	strbuff << "Frame: " << g_data.m_nCurFrame << ", Pulse=" << g_nPulseStep;

	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0xFFFFFF, 1, 0 );
		g_pd3dDevice->GetRenderTarget( 0, &pSurfOldRenderTarget );
		DetectMotion();

		if ( g_data.m_texTruth!=NULL )
		{
			float true_pos, true_neg, false_pos, false_neg;
			g_pEvaluateMask->EvaluateMotionMask
				( g_pPushRelabel->GetExcessTex(), g_data.m_texTruth,
				g_nPulseStep/255.f, true_pos, true_neg, false_pos, false_neg );
			strbuff << ", T=" << g_rThreshold
				<< " -> FPR=" << std::setprecision(3) << (false_pos/(false_pos+true_neg)) << '%'
				<< ", TPR=" << std::setprecision(3) << (true_pos/(true_pos+false_neg)) << '%';
			g_fpReport << g_rThreshold << '\t'
				<< false_pos/(false_pos+true_neg) << '\t'
				<< true_pos/(true_pos+false_neg) << std::endl;
		}


		g_pEvaluateMask->ShowGraphCutsResult                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
			( g_pLearnBackground->GetScoreTex(), g_pPushRelabel->GetExcessTex(),
			g_nPulseStep/2, pSurfOldRenderTarget );
				

		
		//D3DXSaveSurfaceToFileA("Current_Map.bmp", D3DXIFF_BMP, pSurfOldRenderTarget, NULL, NULL);
		//D3DXSaveTextureToFile("Current_Map.bmp", D3DXIFF_BMP, pTexture, NULL);
		//g_pEvaluateMask->GenerateDisplay(g_pPushRelabel->GetExcessTex(),
		//	true, g_nPulseStep / 2 / 255.f, pSurfOldRenderTarget);
	
	
		LPDIRECT3DSURFACE9 CV_CopySurface = NULL;
		D3DLOCKED_RECT CV_SurfRect;
		::ZeroMemory(&CV_SurfRect, sizeof(D3DLOCKED_RECT));


		D3DSURFACE_DESC pDesc;

		pSurfOldRenderTarget->GetDesc(&pDesc);
		pSurfOldRenderTarget->Release();
		HANDLE *handle = NULL;

		HRESULT cc = g_pd3dDevice->CreateOffscreenPlainSurface(pDesc.Width, pDesc.Height, pDesc.Format, D3DPOOL_SYSTEMMEM, &CV_CopySurface, NULL);
		//HRESULT locked = CV_CopySurface->LockRect(&CV_SurfRect, NULL, 0);
		//HRESULT back = g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &CV_CopySurface);


		HRESULT gettarget = g_pd3dDevice->GetRenderTargetData(pSurfOldRenderTarget, CV_CopySurface);
		HRESULT locked = CV_CopySurface->LockRect(&CV_SurfRect, NULL, 0);

		static int framestep_y = Silouette_SILK.step[0];
		static int framestep_x = Silouette_SILK.step[1];
		static int m_nWidth = Silouette_SILK.cols;
		static int m_nHeight = Silouette_SILK.rows;

		BYTE *SurfacePtr = (BYTE*)(CV_SurfRect.pBits);
		
		for (int x = 0; x < m_nWidth; x++)
		{
			for (int y = 0; y < m_nHeight; y++)
			{
				BYTE bdata = SurfacePtr[y * 4 * m_nWidth + x * 4 + 0];
				BYTE gdata = SurfacePtr[y * 4 * m_nWidth + x * 4 + 1];
				BYTE rdata = SurfacePtr[y * 4 * m_nWidth + x * 4 + 2];

				Silouette_SILK.data[y*framestep_y + x*framestep_x + 0] = SurfacePtr[y * 4 * m_nWidth + x * 4 + 0];

			}
		}

		HRESULT unlocked = CV_CopySurface->UnlockRect();
//		HRESULT saved = D3DXSaveSurfaceToFileA("Current_Map2.bmp", D3DXIFF_BMP, CV_CopySurface, NULL, NULL);
		
	}
	HRESULT failss = g_pd3dDevice->EndScene();
	SAFE_RELEASE( pSurfOldRenderTarget );

	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	if ( g_frameRate.updateFrameRate() )
		SetWindowText( hWnd, g_frameRate.getFrameRateText().c_str() );
	else if ( ! g_frameRate.isTimeStarted() )
		SetWindowText( hWnd, strbuff.str().c_str() );
}

void SaveScreen()
{
	LPDIRECT3DSURFACE9 pSurfScreen = NULL;

	if( FAILED( g_pd3dDevice->GetRenderTarget( 0, &pSurfScreen ) ) )
	{
		MessageBox( NULL, "Cannot get rendering result from screen",
			"Error", MB_ICONERROR | MB_OK );
		return;
	}
	std::ostringstream strbuff;
	strbuff << "D:/Temp/" << g_data.m_strName << "_GC."
		<< std::setfill('0') << std::setw(3) << g_data.m_nCurFrame << ".bmp";
	D3DXSaveSurfaceToFile( strbuff.str().c_str(), D3DXIFF_BMP, pSurfScreen, NULL, NULL );

	SAFE_RELEASE( pSurfScreen );
}

HRESULT CheckDevice()
{
	D3DCAPS9 caps;

	if( FAILED( g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
		return E_FAIL;
	if( caps.PixelShaderVersion < D3DPS_VERSION(2,0) )
		return E_FAIL;

	return S_OK;
}

HRESULT InitD3D( HWND hWnd )
{
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	if ( FAILED( CheckDevice() ) )
		g_devType = D3DDEVTYPE_REF;
	

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;



	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, g_devType, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pd3dDevice ) ) )
		return E_FAIL;

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetFVF( VERTEX_TYPE1::FVF );

	for ( int i=0 ; i<MAX_STEREO_PAIR ; i++ )
	{
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
		g_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
	}

	return S_OK;
}

VOID Cleanup()
{
	delete g_pLearnBackground;
	delete g_pPushRelabel;
	delete g_pEvaluateMask;

	SAFE_RELEASE( g_pd3dDevice );
	SAFE_RELEASE( g_pD3D );
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_DESTROY:
			Cleanup();
			PostQuitMessage( 0 );
			break;

		case WM_PAINT:
			Render(hWnd);
			ValidateRect( hWnd, NULL );
			break;

		case WM_RBUTTONUP:
			//SaveScreen();
			break;

		case WM_KEYDOWN:
			if ( g_frameRate.isTimeStarted() )
			{
				if ( wParam == VK_ESCAPE )
					g_frameRate.stopTimer();
				break;
			}
			switch ( wParam )
			{
			case VK_ESCAPE :
				g_bPlayStarted = false;
				break;
			case VK_SPACE :
				g_bPlayStarted = true;
				break;
			case 'T' :
				g_frameRate.startTimer();
				break;
			case VK_UP :
			case VK_LEFT :
				if ( g_data.LoadNextFrame(-1) )
				{
					g_pLearnBackground->SwapBuffer();
					Render( hWnd );
				}
				break;
			case VK_DOWN :
			case VK_RIGHT :
				if ( g_data.LoadNextFrame(1) )
				{
					g_pLearnBackground->SwapBuffer();
					Render( hWnd );
				}
				break;
			}
			break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, INT)
{
	//OPENCV initialize
	capture = VideoCapture(videoFilename);

	if (WEBCAM_MODE)
	{
		camera.open(WEBCAM_NUMBER);

		if (!camera.isOpened())  //소스 영상 에러체크
		{
			//error in opening the video input
			cerr << "Unable to open Camera Stream"<< endl;
			exit(EXIT_FAILURE);
		}

		
		camera >> Current_Frame;
	}
	
	else
	{
		if (!capture.isOpened())  //소스 영상 에러체크
		{
			//error in opening the video input
			cerr << "Unable to open video file: " << videoFilename << endl;
			exit(EXIT_FAILURE);
		}

		if (!capture.read(Current_Frame))
		{
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}
	}

	imwrite("Reference.bmp", Current_Frame);

	if (Background_Frame.data == NULL) //초기화가 안되어있다면 초기화단계를 거친다.
	{
		//최초의 한번만 확인을 거친다
		Rows = Current_Frame.rows;
		Cols = Current_Frame.cols;

		Background_Frame = Mat(Rows, Cols, CV_8UC1);
		Shadow_Map = Mat(Rows, Cols, CV_8UC1);
		Silouette_Final = Mat(Rows, Cols, CV_8UC1);
		Silouette_SILK = Mat(Rows, Cols, CV_8UC1);

		Current_Frame.copyTo(Background_Frame); //시작하는 루프에서는 배경 = 첫프레임
	}

	//WebCam Option
	


 	// TODO: Place code here.
	if ( ! g_data.SetUpDataset( lpCmdLine ) )
		return E_FAIL;

	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
					hInstance, NULL,
					LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH),
					NULL, "Direct3D Shader", NULL };
	if ( ! RegisterClassEx( &wc ) )
		return E_FAIL;
	
	// Set the window's initial style
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
					WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
	RECT rc;
	SetRect( &rc, g_nPosX, g_nPosY,
		g_data.m_nWidth*g_nColumn+g_nSpan*(g_nColumn-1)+g_nPosX,
		g_data.m_nHeight*g_nRow+g_nSpan*(g_nRow-1)+g_nPosY );
	AdjustWindowRect( &rc, dwStyle, false );

	// Create the application's window
	HWND hWnd = CreateWindow( wc.lpszClassName, "Local Reliable Matches",
							dwStyle, 0, 0,
							(rc.right-rc.left), (rc.bottom-rc.top), NULL,
							NULL, hInstance, NULL );

	// Initialize Direct3D
	if( FAILED( InitD3D( hWnd ) )
		|| FAILED( g_data.ReloadImage() ) )
	{
		UnregisterClass( wc.lpszClassName, hInstance );
		return E_FAIL;
	}

	g_pLearnBackground = new LearnBackground( g_data );
	if ( FAILED( g_pLearnBackground->CreateResources() ) )
		return E_FAIL;

	g_pPushRelabel = new PushRelabel( g_data );
	if ( FAILED( g_pPushRelabel->CreateResources() ) )
		return E_FAIL;

	g_pEvaluateMask = new EvaluateMask( g_data, g_nColumn, g_nRow, g_nSpan );
	if ( FAILED( g_pEvaluateMask->CreateResources() ) )
		return E_FAIL;

	// Show the window
	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );

	// Enter the message loop
	MSG msg;
	msg.message = WM_NULL;
	
	
	/*
	while( msg.message != WM_QUIT )
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate and dispatch the message
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if ( g_devType==D3DDEVTYPE_HAL )
		{
			if ( g_frameRate.isTimeStarted() )
				Render(hWnd);
			else if ( g_bPlayStarted )
			{
				if ( g_data.LoadNextFrame(1) )
				{
					g_pLearnBackground->SwapBuffer();
					Render( hWnd );
				}
				else
					g_bPlayStarted = false;
			}
		}
	}
	*/



	while (1)
	{
		if (WEBCAM_MODE)
		{
			camera >> Current_Frame;
		}

		else if (!capture.read(Current_Frame)) 
		{
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}




		g_frameRate.isTimeStarted();
		g_data.LoadNextFrame(1);
		g_pLearnBackground->SwapBuffer();
		Render(hWnd);

		//Silouette_SILK=imread("Current_Map2.bmp" );

		ShadowMapCreator(&Shadow_Map, &Current_Frame, &Background_Frame);
		ImageAbsSubtract(&Silouette_Final, &Silouette_SILK, &Shadow_Map, 1);
		
		static Mat element2 = getStructuringElement(MORPH_CROSS, Size(2 * 2 + 1, 2 * 2 + 1), Point(2, 2));
		morphologyEx(Silouette_Final, Silouette_Final, MORPH_OPEN, element2);

		imshow("Input", Current_Frame);
		imshow("Shadow Map", Shadow_Map);
		//imshow("Silouette SILK", Silouette_SILK);
		imshow("Silouette Final", Silouette_Final);

		if (waitKey(1) == 27) 
			break;
	}


	UnregisterClass( wc.lpszClassName, hInstance );
	return S_OK;
}
