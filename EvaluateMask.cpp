#include "EvaluateMask.h"
#include "DataMotion.h"

EvaluateMask::EvaluateMask( const DataMotion& data, int column, int row, int span )
{
	m_nWidth = data.m_nWidth, m_nHeight = data.m_nHeight;

	m_texMask = NULL;
	m_surfMask = m_surfOffscreen = NULL;

	m_nRow = row, m_nColumn = column, m_nSpan = span;

	m_vbDisplay = NULL;

	m_shViewThres = m_shViewGraph = m_shViewReliable = NULL;
	m_tabViewThres = m_tabViewGraph = m_tabViewReliable = NULL;
	m_shEvalMask = NULL;
	m_tabEvalMask = NULL;
}

EvaluateMask::~EvaluateMask()
{
	SAFE_RELEASE( m_texMask );

	SAFE_RELEASE( m_surfMask );
	SAFE_RELEASE( m_surfOffscreen );

	SAFE_RELEASE( m_vbDisplay );

	SAFE_RELEASE( m_shViewThres );
	SAFE_RELEASE( m_shViewGraph );
	SAFE_RELEASE( m_shViewReliable );
	SAFE_RELEASE( m_shEvalMask );

	SAFE_RELEASE( m_tabViewThres );
	SAFE_RELEASE( m_tabViewGraph );
	SAFE_RELEASE( m_tabViewReliable );
	SAFE_RELEASE( m_tabEvalMask );
}

void EvaluateMask::GenerateDisplay( LPDIRECT3DTEXTURE9 texScore,
	bool isGraph, float thres, LPDIRECT3DSURFACE9 surfDisplay )
{
	D3DXVECTOR4 display( thres, 0, 0, isGraph?4.f:2.f );

	g_pd3dDevice->SetStreamSource( 0, m_vbDisplay, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, surfDisplay );
	{
		g_pd3dDevice->SetTexture( 0, texScore );
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->SetPixelShader( m_shViewThres );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}
//D3DXSaveSurfaceToFile( "C:/Minglun/Temp/display.bmp", D3DXIFF_BMP, surfDisplay, NULL, NULL );
}

void EvaluateMask::ShowThresholdResult( LPDIRECT3DTEXTURE9 texScore,
	float thres, LPDIRECT3DSURFACE9 surfDisplay )
{
	D3DXVECTOR4 display( thres, .2f, .98f, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbDisplay, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, surfDisplay );
	{
		g_pd3dDevice->SetTexture( 0, texScore );
		g_pd3dDevice->SetPixelShader( m_shViewThres );

		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		display.w = 1;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 4, 2 );
		display.w = 2;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 8, 2 );
		display.w = 3;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 12, 2 );
	}
//D3DXSaveSurfaceToFile( "C:/Minglun/Temp/display.bmp", D3DXIFF_BMP, surfDisplay, NULL, NULL );
}

void EvaluateMask::ShowGraphCutsResult( LPDIRECT3DTEXTURE9 texScore,
	LPDIRECT3DTEXTURE9 texGraph, int split, LPDIRECT3DSURFACE9 surfDisplay )
{
	D3DXVECTOR4 display( split/255.f, 0, 0, 2 );

	g_pd3dDevice->SetStreamSource( 0, m_vbDisplay, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, surfDisplay );
	{
		//g_pd3dDevice->SetTexture( 0, texScore );
		g_pd3dDevice->SetTexture( 1, texGraph );
		g_pd3dDevice->SetPixelShader( m_shViewGraph );

		m_tabViewGraph->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	/*	display.w = 1;
		m_tabViewGraph->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 4, 2 );
		display.w = 2;
		m_tabViewGraph->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 8, 2 );
		display.w = 3;
		m_tabViewGraph->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 12, 2 );*/
	}


	//g_pd3dDevice->SetTexture( 1, 0 );


	//D3DXSaveTextureToFile("Texture.bmp", D3DXIFF_BMP, texGraph, NULL);
//D3DXSaveSurfaceToFile( "display.bmp", D3DXIFF_BMP, surfDisplay, NULL, NULL );

}

void EvaluateMask::ShowDynamicProgResult( LPDIRECT3DTEXTURE9 texNodeScore,
	LPDIRECT3DTEXTURE9 texEdgeScore, LPDIRECT3DTEXTURE9 texLabel,
	float thres, LPDIRECT3DSURFACE9 surfDisplay )
{
	D3DXVECTOR4 display( thres, .2f, .98f, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbDisplay, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, surfDisplay );
	{
		g_pd3dDevice->SetTexture( 0, texNodeScore );
		g_pd3dDevice->SetTexture( 1, texLabel );
		g_pd3dDevice->SetTexture( 2, texEdgeScore );
		g_pd3dDevice->SetPixelShader( m_shViewReliable );

		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		display.w = 1;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 4, 2 );
		display.w = 2;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 8, 2 );
		display.w = 3;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 12, 2 );
	}
//D3DXSaveSurfaceToFile( "C:/Minglun/Temp/display.bmp", D3DXIFF_BMP, surfDisplay, NULL, NULL );
}

void EvaluateMask::CompareLocalGlobal( LPDIRECT3DTEXTURE9 texScore,
	LPDIRECT3DTEXTURE9 texGraph, float thres, int split, LPDIRECT3DSURFACE9 surfDisplay )
{
	D3DXVECTOR4 display( thres, 0, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbDisplay, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, surfDisplay );
	{
		g_pd3dDevice->SetTexture( 0, texScore );
		g_pd3dDevice->SetTexture( 1, texGraph );
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->SetPixelShader( m_shViewThres );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		display.w = 1;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 4, 2 );
		display.w = 2;
		m_tabViewThres->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 8, 2 );

		display.x = split/255.f;
		m_tabViewGraph->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->SetPixelShader( m_shViewGraph );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 12, 2 );
	}
	g_pd3dDevice->SetTexture( 1, 0 );
//D3DXSaveSurfaceToFile( "C:/Minglun/Temp/display.bmp", D3DXIFF_BMP, surfDisplay, NULL, NULL );
}

void EvaluateMask::EvaluateMotionMask
	( LPDIRECT3DTEXTURE9 texScore, LPDIRECT3DTEXTURE9 texTruth, float thres,
	float &true_pos, float &true_neg, float &false_pos, float &false_neg )
{
	int i;
	D3DLOCKED_RECT rect;
	COLOR *ptr;
	D3DXVECTOR4 display( thres, 0, 0, thres<0?1:0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbDisplay, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfMask );
	{
		g_pd3dDevice->SetTexture( 0, texScore );
		g_pd3dDevice->SetTexture( 1, texTruth );
		m_tabEvalMask->SetVector( g_pd3dDevice, "vDisplay", &display );
		g_pd3dDevice->SetPixelShader( m_shEvalMask );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		g_pd3dDevice->GetRenderTargetData( m_surfMask, m_surfOffscreen );
	}

	m_surfOffscreen->LockRect( &rect, NULL, D3DLOCK_READONLY );
	ptr = (COLOR*)rect.pBits;
	true_pos = true_neg = false_pos = false_neg = 0;
	for ( i=0 ; i<m_nWidth*m_nHeight ; i++, ptr++ )
		true_pos += (ptr->m_Channel.m_nR > .5), true_neg += (ptr->m_Channel.m_nG > .5),
		false_pos += (ptr->m_Channel.m_nB > .5), false_neg += (ptr->m_Channel.m_nA > .5);
	m_surfOffscreen->UnlockRect();

	true_pos *= 100.f / m_nWidth / m_nHeight;
	true_neg *= 100.f / m_nWidth / m_nHeight;
	false_pos *= 100.f / m_nWidth / m_nHeight;
	false_neg *= 100.f / m_nWidth / m_nHeight;
//D3DXSaveSurfaceToFile( "C:/Minglun/Temp/display.bmp", D3DXIFF_BMP, surfDisplay, NULL, NULL );
}

HRESULT EvaluateMask::CreateTextures()
{
	if( FAILED( g_pd3dDevice->CreateTexture( m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			&m_texMask, NULL ) ) )
		return E_FAIL;
	m_texMask->GetSurfaceLevel( 0, &m_surfMask );
	if( FAILED( g_pd3dDevice->CreateOffscreenPlainSurface
			( m_nWidth, m_nHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM,
			&m_surfOffscreen, NULL ) ) )
		return E_FAIL;

	return S_OK;
}

HRESULT EvaluateMask::CreateShaders()
{
	LPD3DXBUFFER pCode = NULL;

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/evaluate_motionmask.psh", NULL, NULL,
			"ViewThreshold", "ps_3_0", NULL, &pCode, NULL, &m_tabViewThres) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shViewThres) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/evaluate_motionmask.psh", NULL, NULL,
			"ViewGraphCuts", "ps_3_0", NULL, &pCode, NULL, &m_tabViewGraph) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shViewGraph) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/evaluate_motionmask.psh", NULL, NULL,
			"ViewDynamicProg", "ps_3_0", NULL, &pCode, NULL, &m_tabViewReliable) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shViewReliable) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/evaluate_motionmask.psh", NULL, NULL,
			"EvaluateMask", "ps_3_0", NULL, &pCode, NULL, &m_tabEvalMask) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shEvalMask) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	return S_OK;
}

HRESULT EvaluateMask::CreateGeometries()
{
	VOID* ptr = NULL;

	VERTEX_TYPE1 section[] =
	{
		{ -.5f,			-.5f,			1.0f, 1.0f, 0.0f, 0.0f, },
		{ m_nWidth-.5f,	-.5f,			1.0f, 1.0f, 1.0f, 0.0f, },
		{ m_nWidth-.5f,	m_nHeight-.5f,	1.0f, 1.0f, 1.0f, 1.0f, },
		{ -.5f,			m_nHeight-.5f,	1.0f, 1.0f, 0.0f, 1.0f, },
	};

	if( FAILED( g_pd3dDevice->CreateVertexBuffer( m_nColumn*m_nRow*4*sizeof(VERTEX_TYPE1),
			D3DUSAGE_WRITEONLY, VERTEX_TYPE1::FVF, D3DPOOL_DEFAULT,
			&m_vbDisplay, NULL ) ) )
		return E_FAIL;
	if( FAILED( m_vbDisplay->Lock( 0, m_nColumn*m_nRow*4*sizeof(VERTEX_TYPE1), &ptr, 0 ) ) )
		return E_FAIL;

	for ( int v=0; v<m_nRow; v++ )
		for ( int u=0; u<m_nColumn; u++ )
		{
			section[0].x = section[3].x = (m_nWidth + m_nSpan) * u - .5f;
			section[1].x = section[2].x = section[0].x + m_nWidth;
			section[0].y = section[1].y = (m_nHeight + m_nSpan) * v - .5f;
			section[2].y = section[3].y = section[0].y + m_nHeight;
			memcpy( ptr, section, 4*sizeof(VERTEX_TYPE1) );
			ptr = (BYTE*)ptr + 4*sizeof(VERTEX_TYPE1);
		}

	if( FAILED( m_vbDisplay->Unlock() ) )
		return E_FAIL;

	return S_OK;
}
