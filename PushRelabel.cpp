#include "PushRelabel.h"
#include "DataMotion.h"

PushRelabel::PushRelabel( const DataMotion& data )
{
	m_nWidth = data.m_nWidth, m_nHeight = data.m_nHeight;

	m_vStepX = D3DXVECTOR4(1.f/m_nWidth,0,				0, 0);
	m_vStepY = D3DXVECTOR4(0,			1.f/m_nHeight,	0, 0);

	m_texExcess = m_texResidual = m_texPush = NULL;
	m_surfExcess = m_surfResidual = m_surfPush = NULL;
	m_texExcessUpdate = m_texResidualUpdate = NULL;
	m_surfExcessUpdate = m_surfResidualUpdate = NULL;

	m_vbImagePlane = NULL;

	m_shInitExcess = m_shPushFlow = m_shUpdateExcess = NULL;
	m_tabInitExcess = m_tabPushFlow = m_tabUpdateExcess = NULL;
}

PushRelabel::~PushRelabel()
{
	SAFE_RELEASE( m_texExcess );
	SAFE_RELEASE( m_texResidual );
	SAFE_RELEASE( m_texPush );
	SAFE_RELEASE( m_texExcessUpdate );
	SAFE_RELEASE( m_texResidualUpdate );

	SAFE_RELEASE( m_surfExcess );
	SAFE_RELEASE( m_surfResidual );
	SAFE_RELEASE( m_surfPush );
	SAFE_RELEASE( m_surfExcessUpdate );
	SAFE_RELEASE( m_surfResidualUpdate );

	SAFE_RELEASE( m_vbImagePlane );

	SAFE_RELEASE( m_shInitExcess );
	SAFE_RELEASE( m_shPushFlow );
	SAFE_RELEASE( m_shUpdateExcess );

	SAFE_RELEASE( m_tabInitExcess );
	SAFE_RELEASE( m_tabPushFlow );
	SAFE_RELEASE( m_tabUpdateExcess );
}

void PushRelabel::InitializeExcessResidual( LPDIRECT3DTEXTURE9 texLoss, float thres )
{
	D3DXVECTOR4 threshold( thres, 0, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbImagePlane, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfExcess );
	g_pd3dDevice->SetRenderTarget( 1, m_surfResidual );
	{
		g_pd3dDevice->SetTexture( 0, texLoss );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vThreshold", &threshold );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vStepX", &m_vStepX );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vStepY", &m_vStepY );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vStepXY", &(m_vStepX+m_vStepY) );
		g_pd3dDevice->SetPixelShader( m_shInitExcess );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}
	g_pd3dDevice->SetRenderTarget( 1, NULL );

//D3DXSaveSurfaceToFile( "D:/Temp/excess.bmp", D3DXIFF_BMP, m_surfExcess, NULL, NULL );
//D3DXSaveSurfaceToFile( "D:/Temp/residual.bmp", D3DXIFF_BMP, m_surfResidual, NULL, NULL );
}

void PushRelabel::InitializeExcessResidual
	( LPDIRECT3DTEXTURE9 texLoss1, LPDIRECT3DTEXTURE9 texLoss2, float thres )
{
	D3DXVECTOR4 threshold( thres, 0, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbImagePlane, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfExcess );
	g_pd3dDevice->SetRenderTarget( 1, m_surfResidual );
	{
		g_pd3dDevice->SetTexture( 0, texLoss1 );
		g_pd3dDevice->SetTexture( 1, texLoss2 );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vThreshold", &threshold );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vStepX", &m_vStepX );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vStepY", &m_vStepY );
		m_tabInitExcess->SetVector( g_pd3dDevice, "vStepXY", &(m_vStepX+m_vStepY) );
		g_pd3dDevice->SetPixelShader( m_shInitExcess );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}
	g_pd3dDevice->SetTexture( 1, NULL );
	g_pd3dDevice->SetRenderTarget( 1, NULL );

//D3DXSaveSurfaceToFile( "D:/Temp/excess.bmp", D3DXIFF_BMP, m_surfExcess, NULL, NULL );
//D3DXSaveSurfaceToFile( "D:/Temp/residual.bmp", D3DXIFF_BMP, m_surfResidual, NULL, NULL );
}

void PushRelabel::PulseOperation( bool push_back )
{
	D3DXVECTOR4 pushdir( push_back?1.f:0.f, 0, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbImagePlane, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfPush );
	{
		g_pd3dDevice->SetTexture( 0, m_texExcess );
		g_pd3dDevice->SetTexture( 1, m_texResidual );
		m_tabPushFlow->SetVector( g_pd3dDevice, "vStepX", &m_vStepX );
		m_tabPushFlow->SetVector( g_pd3dDevice, "vStepY", &m_vStepY );
		m_tabPushFlow->SetVector( g_pd3dDevice, "vPushDir", &pushdir );
		g_pd3dDevice->SetPixelShader( m_shPushFlow );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}

	g_pd3dDevice->SetRenderTarget( 0, m_surfExcessUpdate );
	g_pd3dDevice->SetRenderTarget( 1, m_surfResidualUpdate );
	{
		g_pd3dDevice->SetTexture( 0, m_texPush );
		g_pd3dDevice->SetTexture( 1, m_texExcess );
		g_pd3dDevice->SetTexture( 2, m_texResidual );
		m_tabUpdateExcess->SetVector( g_pd3dDevice, "vStepX", &m_vStepX );
		m_tabUpdateExcess->SetVector( g_pd3dDevice, "vStepY", &m_vStepY );
		m_tabUpdateExcess->SetVector( g_pd3dDevice, "vPushDir", &pushdir );
		g_pd3dDevice->SetPixelShader( m_shUpdateExcess );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}
	g_pd3dDevice->SetRenderTarget( 1, NULL );
	g_pd3dDevice->SetTexture( 2, NULL );

	LPDIRECT3DTEXTURE9 tex;
	LPDIRECT3DSURFACE9 surf;
	tex = m_texExcess, m_texExcess = m_texExcessUpdate, m_texExcessUpdate = tex;
	surf = m_surfExcess, m_surfExcess = m_surfExcessUpdate, m_surfExcessUpdate = surf;
	tex = m_texResidual, m_texResidual = m_texResidualUpdate, m_texResidualUpdate = tex;
	surf = m_surfResidual, m_surfResidual = m_surfResidualUpdate, m_surfResidualUpdate = surf;

//D3DXSaveSurfaceToFile( "D:/Temp/push.bmp", D3DXIFF_BMP, m_surfPush, NULL, NULL );
//D3DXSaveSurfaceToFile( "D:/Temp/exces.bmp", D3DXIFF_BMP, m_surfExcess, NULL, NULL );
//D3DXSaveSurfaceToFile( "D:/Temp/residual.bmp", D3DXIFF_BMP, m_surfResidual, NULL, NULL );
}

HRESULT PushRelabel::CreateTextures()
{
	if( FAILED( g_pd3dDevice->CreateTexture( m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, 
			&m_texExcess, NULL ) ) )
		return E_FAIL;
	m_texExcess->GetSurfaceLevel( 0, &m_surfExcess );
	if( FAILED( g_pd3dDevice->CreateTexture( m_nWidth, m_nHeight, 1,
			D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, 
			&m_texResidual, NULL ) ) )
		return E_FAIL;
	m_texResidual->GetSurfaceLevel( 0, &m_surfResidual );

	if( FAILED( g_pd3dDevice->CreateTexture( m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, 
			&m_texPush, NULL ) ) )
		return E_FAIL;
	m_texPush->GetSurfaceLevel( 0, &m_surfPush );

	if( FAILED( g_pd3dDevice->CreateTexture( m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, 
			&m_texExcessUpdate, NULL ) ) )
		return E_FAIL;
	m_texExcessUpdate->GetSurfaceLevel( 0, &m_surfExcessUpdate );
	if( FAILED( g_pd3dDevice->CreateTexture( m_nWidth, m_nHeight, 1,
			D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, 
			&m_texResidualUpdate, NULL ) ) )
		return E_FAIL;
	m_texResidualUpdate->GetSurfaceLevel( 0, &m_surfResidualUpdate );

	return S_OK;
}

HRESULT PushRelabel::CreateShaders()
{
	LPD3DXBUFFER pCode = NULL;

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/learn_cost.psh", NULL, NULL,
			"InitExcessResidual", "ps_3_0", NULL, &pCode, NULL, &m_tabInitExcess) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shInitExcess) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/push_relabel.psh", NULL, NULL,
			"PushFlow", "ps_3_0", NULL, &pCode, NULL, &m_tabPushFlow) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shPushFlow) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/push_relabel.psh", NULL, NULL,
			"UpdateExcessResidual", "ps_3_0", NULL, &pCode, NULL, &m_tabUpdateExcess) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shUpdateExcess) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	return S_OK;
}

HRESULT PushRelabel::CreateGeometries()
{
	void* ptr = NULL;

	VERTEX_TYPE1 full[] =
	{
		{ -.5f,			-.5f,			1.0f, 1.0f, 0.0f, 0.0f, },
		{ m_nWidth-.5f,	-.5f,			1.0f, 1.0f, 1.0f, 0.0f, },
		{ m_nWidth-.5f,	m_nHeight-.5f,	1.0f, 1.0f, 1.0f, 1.0f, },
		{ -.5f,			m_nHeight-.5f,	1.0f, 1.0f, 0.0f, 1.0f, },
	};

	if( FAILED( g_pd3dDevice->CreateVertexBuffer(
			4*sizeof(VERTEX_TYPE1),
			D3DUSAGE_WRITEONLY, VERTEX_TYPE1::FVF, D3DPOOL_DEFAULT,
			&m_vbImagePlane, NULL ) ) )
		return E_FAIL;

	if( FAILED( m_vbImagePlane->Lock( 0,
			4*sizeof(VERTEX_TYPE1), &ptr, 0 ) ) )
		return E_FAIL;

	memcpy( ptr, full, 4*sizeof(VERTEX_TYPE1) );

	if( FAILED( m_vbImagePlane->Unlock() ) )
		return E_FAIL;

	return S_OK;
}
