#include "general.h"

class LearnBackground
{
private:
	enum {MAX_TILE_NUM = 50};

	int		m_nWidth, m_nHeight;
	int		m_nBufferSize;
	int		m_nTile, m_nRow, m_nColumn;
	int		m_nTexWidth, m_nTexHeight;
	D3DXVECTOR4 m_vOffsets[MAX_TILE_NUM];

	LPDIRECT3DTEXTURE9 m_texCurrentBuff, m_texUpdateBuff;
	LPDIRECT3DTEXTURE9 m_texScore, m_texWeight, m_texCostNode;
	LPDIRECT3DSURFACE9 m_surfCurrentBuff, m_surfUpdateBuff;
	LPDIRECT3DSURFACE9 m_surfScore, m_surfWeight, m_surfCostNode;

	LPDIRECT3DVERTEXBUFFER9 m_vbTiles;

	LPDIRECT3DPIXELSHADER9 m_shCombination, m_shWeight, m_shNodeCost;
	LPDIRECT3DPIXELSHADER9 m_shInsertMost, m_shInsertLast;
	LPD3DXCONSTANTTABLE m_tabCombination, m_tabWeight, m_tabNodeCost;
	LPD3DXCONSTANTTABLE m_tabInsertMost, m_tabInsertLast;
public:
	LearnBackground( const DataMotion& data );
	~LearnBackground();

	HRESULT		CreateResources();

	void		CombinationScore( LPDIRECT3DTEXTURE9 texCen, float scale );
	void		GenerateWeight( LPDIRECT3DTEXTURE9 texMask,
					float thres, float cut_fore, float cut_back );
	void		ComputeNodeCost( float cut_fore, float cut_back );
	void		UpdateMemory( float rate );

	void		SwapBuffer();

	LPDIRECT3DTEXTURE9	GetScoreTex() { return m_texScore; };
	LPDIRECT3DTEXTURE9	GetNodeCostTex() { return m_texCostNode; };
private:
	HRESULT		CreateShaders();
	HRESULT		CreateTextures();
	HRESULT		CreateGeometries();
};

inline HRESULT LearnBackground::CreateResources()
{
	if ( FAILED( CreateShaders() ) )
		return E_FAIL;
	if ( FAILED( CreateTextures() ) )
		return E_FAIL;
	if ( FAILED( CreateGeometries() ) )
		return E_FAIL;
	return S_OK;
}

inline void LearnBackground::SwapBuffer()
{
	LPDIRECT3DTEXTURE9	pTex;
	LPDIRECT3DSURFACE9	pSurf;

	pTex = m_texCurrentBuff;
	m_texCurrentBuff = m_texUpdateBuff;
	m_texUpdateBuff = pTex;

	pSurf = m_surfCurrentBuff;
	m_surfCurrentBuff = m_surfUpdateBuff;
	m_surfUpdateBuff = pSurf;
}
