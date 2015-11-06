#include "LearnBackground.h"
#include "DataMotion.h"

LearnBackground::LearnBackground( const DataMotion& data )
{
	m_nWidth = data.m_nWidth, m_nHeight = data.m_nHeight;

	m_nTile = data.m_nTile, m_nRow = data.m_nRow, m_nColumn = data.m_nColumn;
	m_nTexWidth = m_nWidth * m_nColumn, m_nTexHeight = m_nHeight * m_nRow;

	m_texCurrentBuff = m_texUpdateBuff = NULL;
	m_texScore = m_texWeight = m_texCostNode = NULL;
	m_surfCurrentBuff = m_surfUpdateBuff = NULL;
	m_surfScore = m_surfWeight = m_surfCostNode = NULL;

	m_vbTiles = NULL;

	m_shCombination = m_shWeight = m_shNodeCost = NULL;
	m_shInsertMost = m_shInsertLast = NULL;
	m_tabCombination = m_tabWeight = m_tabNodeCost = NULL;
	m_tabInsertMost = m_tabInsertLast = NULL;

	for ( int r=0, i=0; r<m_nRow; r++ )
		for ( int c=0; c<m_nColumn && i<m_nTile; c++, i++ )
			m_vOffsets[i].x = (float)c / m_nColumn,
			m_vOffsets[i].y = (float)r / m_nRow;
}

LearnBackground::~LearnBackground()
{
	SAFE_RELEASE( m_texCurrentBuff );
	SAFE_RELEASE( m_texUpdateBuff );
	SAFE_RELEASE( m_texScore );
	SAFE_RELEASE( m_texWeight );
	SAFE_RELEASE( m_texCostNode );

	SAFE_RELEASE( m_surfCurrentBuff );
	SAFE_RELEASE( m_surfUpdateBuff );
	SAFE_RELEASE( m_surfScore );
	SAFE_RELEASE( m_surfWeight );
	SAFE_RELEASE( m_surfCostNode );

	SAFE_RELEASE( m_vbTiles );

	SAFE_RELEASE( m_shCombination );
	SAFE_RELEASE( m_shWeight );
	SAFE_RELEASE( m_shNodeCost );
	SAFE_RELEASE( m_shInsertMost );
	SAFE_RELEASE( m_shInsertLast );

	SAFE_RELEASE( m_tabCombination );
	SAFE_RELEASE( m_tabWeight );
	SAFE_RELEASE( m_tabNodeCost );
	SAFE_RELEASE( m_tabInsertMost );
	SAFE_RELEASE( m_tabInsertLast );
}

void LearnBackground::CombinationScore( LPDIRECT3DTEXTURE9 texCen, float scale )
{
	D3DXVECTOR4 repeat( (float)m_nColumn, (float)m_nRow, 0, 0 );
	D3DXVECTOR4 threshold( scale, 0, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbTiles, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfScore );
	{
		g_pd3dDevice->SetTexture( 0, m_texCurrentBuff );
		g_pd3dDevice->SetTexture( 1, texCen );
		m_tabCombination->SetVector( g_pd3dDevice, "vRepeat", &repeat );
		m_tabCombination->SetVector( g_pd3dDevice, "vThreshold", &threshold );
		m_tabCombination->SetVectorArray( g_pd3dDevice, "vOffsets", m_vOffsets, m_nTile );
		g_pd3dDevice->SetPixelShader( m_shCombination );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}
//D3DXSaveSurfaceToFile( "H:/Temp/score.bmp", D3DXIFF_BMP, m_surfScore, NULL, NULL );
}

void LearnBackground::GenerateWeight
	( LPDIRECT3DTEXTURE9 texMask, float thres, float cut_fore, float cut_back )
{
	D3DXVECTOR4 threshold( cut_back, cut_fore, 0, thres );

	g_pd3dDevice->SetStreamSource( 0, m_vbTiles, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfWeight );
	{
		g_pd3dDevice->SetTexture( 0, m_texScore );
		g_pd3dDevice->SetTexture( 1, texMask );
		m_tabWeight->SetVector( g_pd3dDevice, "vThreshold", &threshold );
		g_pd3dDevice->SetPixelShader( m_shWeight );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, m_nTile*4, 2 );
	}
	g_pd3dDevice->SetTexture( 1, NULL );
//D3DXSaveSurfaceToFile( "H:/Temp/weight.bmp", D3DXIFF_BMP, m_surfWeight, NULL, NULL );
}

void LearnBackground::ComputeNodeCost( float cut_fore, float cut_back )
{
	D3DXVECTOR4 threshold( cut_back, cut_fore, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbTiles, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfCostNode );
	{
		g_pd3dDevice->SetTexture( 0, m_texScore );
		m_tabNodeCost->SetVector( g_pd3dDevice, "vThreshold", &threshold );
		g_pd3dDevice->SetPixelShader( m_shNodeCost );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, m_nTile*4, 2 );
	}
//D3DXSaveSurfaceToFile( "H:/Temp/weight.bmp", D3DXIFF_BMP, m_surfCostNode, NULL, NULL );
}

void LearnBackground::UpdateMemory( float rate )
{
	D3DXVECTOR4 damping( rate, 0, 0, 0 );
	D3DXVECTOR4 repeat( (float)m_nColumn, (float)m_nRow, 0, 0 );

	g_pd3dDevice->SetStreamSource( 0, m_vbTiles, 0, sizeof(VERTEX_TYPE1) );

	g_pd3dDevice->SetRenderTarget( 0, m_surfUpdateBuff );
	{
		g_pd3dDevice->SetTexture( 0, m_texCurrentBuff );
		g_pd3dDevice->SetTexture( 1, m_texWeight );
		g_pd3dDevice->SetPixelShader( m_shInsertMost );
		m_tabInsertMost->SetVector( g_pd3dDevice, "vRepeat", &repeat );
		m_tabInsertMost->SetVector( g_pd3dDevice, "vDamping", &damping );
		for ( int i=m_nTile-1; i>0; i-- )
		{
			m_tabInsertMost->SetVector( g_pd3dDevice, "vCurrent", &m_vOffsets[i] );
			m_tabInsertMost->SetVector( g_pd3dDevice, "vPrevious", &m_vOffsets[i-1] );
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, i*4, 2 );
		}

		m_tabInsertLast->SetVector( g_pd3dDevice, "vRepeat", &repeat );
		m_tabInsertLast->SetVector( g_pd3dDevice, "vDamping", &damping );
		g_pd3dDevice->SetPixelShader( m_shInsertLast );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}

//	D3DXSaveSurfaceToFile( "H:/Temp/memory.bmp", D3DXIFF_BMP, m_surfCurrentBuff, NULL, NULL );
}

HRESULT LearnBackground::CreateTextures()
{
	if( FAILED( g_pd3dDevice->CreateTexture(m_nTexWidth, m_nTexHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			&m_texCurrentBuff, NULL ) ) )
		return E_FAIL;
	m_texCurrentBuff->GetSurfaceLevel( 0, &m_surfCurrentBuff );
	if( FAILED( g_pd3dDevice->CreateTexture(m_nTexWidth, m_nTexHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			&m_texUpdateBuff, NULL ) ) )
		return E_FAIL;
	m_texUpdateBuff->GetSurfaceLevel( 0, &m_surfUpdateBuff );

	if( FAILED( g_pd3dDevice->CreateTexture(m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			&m_texScore, NULL ) ) )
		return E_FAIL;
	m_texScore->GetSurfaceLevel( 0, &m_surfScore );
	if( FAILED( g_pd3dDevice->CreateTexture(m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			&m_texWeight, NULL ) ) )
		return E_FAIL;
	m_texWeight->GetSurfaceLevel( 0, &m_surfWeight );
	if( FAILED( g_pd3dDevice->CreateTexture(m_nWidth, m_nHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			&m_texCostNode, NULL ) ) )
		return E_FAIL;
	m_texCostNode->GetSurfaceLevel( 0, &m_surfCostNode );

	g_pd3dDevice->ColorFill( m_surfCurrentBuff, NULL, 0x0 );

	return S_OK;
}

HRESULT LearnBackground::CreateShaders()
{
	LPD3DXBUFFER pCode = NULL;

	std::ostringstream name;
	name << "Combination" << m_nTile;
	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/learn_background.psh", NULL, NULL,
			name.str().c_str(), "ps_3_0", NULL, &pCode, NULL, &m_tabCombination) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shCombination) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/learn_sorting.psh", NULL, NULL,
			"InsertMost", "ps_3_0", NULL, &pCode, NULL, &m_tabInsertMost) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shInsertMost) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );
	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/learn_sorting.psh", NULL, NULL,
			"InsertLast", "ps_3_0", NULL, &pCode, NULL, &m_tabInsertLast) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shInsertLast) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/learn_weight.psh", NULL, NULL,
			"ComputeNodeWeight", "ps_3_0", NULL, &pCode, NULL, &m_tabWeight) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shWeight) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );
	if( FAILED( D3DXCompileShaderFromFile
			("Shaders/learn_weight.psh", NULL, NULL,
			"ComputeNodeCost", "ps_3_0", NULL, &pCode, NULL, &m_tabNodeCost) ) )
		return E_FAIL;
	if( FAILED( g_pd3dDevice->CreatePixelShader
		((DWORD*)pCode->GetBufferPointer(), &m_shNodeCost) ) )
		return E_FAIL;
	SAFE_RELEASE( pCode );

	return S_OK;
}

HRESULT LearnBackground::CreateGeometries()
{
	void* ptr = NULL;

	VERTEX_TYPE1 tile[] =
	{
		{ -.5f,			-.5f,			1.f, 1.f, 0.f,			0.f, },
		{ m_nWidth-.5f,	-.5f,			1.f, 1.f, 1.f/m_nColumn,0.f, },
		{ m_nWidth-.5f,	m_nHeight-.5f,	1.f, 1.f, 1.f/m_nColumn,1.f/m_nRow, },
		{ -.5f,			m_nHeight-.5f,	1.f, 1.f, 0.f,			1.f/m_nRow, },
	};

	if( FAILED( g_pd3dDevice->CreateVertexBuffer(
			(m_nTile+1)*4*sizeof(VERTEX_TYPE1),
			D3DUSAGE_WRITEONLY, VERTEX_TYPE1::FVF, D3DPOOL_DEFAULT,
			&m_vbTiles, NULL ) ) )
		return E_FAIL;
	if( FAILED( m_vbTiles->Lock( 0,
			m_nTile*4*sizeof(VERTEX_TYPE1), &ptr, 0 ) ) )
		return E_FAIL;

	for ( int r=0, k=0; r<m_nRow; r++ )
		for ( int c=0; c<m_nColumn && k<m_nTile; c++, k++ )
		{
			tile[0].x = tile[3].x = c * m_nWidth - .5f,
			tile[1].x = tile[2].x = (c + 1) * m_nWidth - .5f;
			tile[0].y = tile[1].y = r * m_nHeight - .5f,
			tile[2].y = tile[3].y = (r + 1) * m_nHeight - .5f;
			memcpy( ptr, tile, 4*sizeof(VERTEX_TYPE1) );
			ptr = (BYTE*)ptr + 4*sizeof(VERTEX_TYPE1);
		}

	VERTEX_TYPE1 whole[] =
	{
		{ -.5f,			-.5f,			1.f, 1.f, 0.f, 0.f, },
		{ m_nWidth-.5f,	-.5f,			1.f, 1.f, 1.f, 0.f, },
		{ m_nWidth-.5f,	m_nHeight-.5f,	1.f, 1.f, 1.f, 1.f, },
		{ -.5f,			m_nHeight-.5f,	1.f, 1.f, 0.f, 1.f, },
	};
	memcpy( ptr, whole, 4*sizeof(VERTEX_TYPE1) );

	if( FAILED( m_vbTiles->Unlock() ) )
		return E_FAIL;

	return S_OK;
}
