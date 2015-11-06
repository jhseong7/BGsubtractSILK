#include "general.h"

class EvaluateMask
{
private:
	int m_nWidth, m_nHeight;
	int m_nRow, m_nColumn, m_nSpan;

	LPDIRECT3DTEXTURE9 m_texMask;
	LPDIRECT3DSURFACE9 m_surfMask, m_surfOffscreen;

	LPDIRECT3DVERTEXBUFFER9 m_vbDisplay;

	LPDIRECT3DPIXELSHADER9 m_shViewThres, m_shViewGraph, m_shViewReliable;
	LPDIRECT3DPIXELSHADER9 m_shEvalMask;
	LPD3DXCONSTANTTABLE m_tabViewThres, m_tabViewGraph, m_tabViewReliable;
	LPD3DXCONSTANTTABLE m_tabEvalMask;
public:
	EvaluateMask( const DataMotion& data, int column, int row, int span );
	~EvaluateMask();

	HRESULT		CreateResources();

	void		GenerateDisplay( LPDIRECT3DTEXTURE9 texScore,
					bool isGraph, float thres, LPDIRECT3DSURFACE9 surfDisplay );
	void		ShowThresholdResult( LPDIRECT3DTEXTURE9 texScore,
					float thres, LPDIRECT3DSURFACE9 surfDisplay );
	void		ShowGraphCutsResult( LPDIRECT3DTEXTURE9 texScore,
					LPDIRECT3DTEXTURE9 texGraph, int split,
					LPDIRECT3DSURFACE9 surfDisplay );
	void		ShowDynamicProgResult( LPDIRECT3DTEXTURE9 texNodeScore,
					LPDIRECT3DTEXTURE9 texEdgeScore, LPDIRECT3DTEXTURE9 texLabel,
					float thres, LPDIRECT3DSURFACE9 surfDisplay );
	void		CompareLocalGlobal( LPDIRECT3DTEXTURE9 texScore,
					LPDIRECT3DTEXTURE9 texGraph, float thres, int split,
					LPDIRECT3DSURFACE9 surfDisplay );

	void		EvaluateMotionMask
					( LPDIRECT3DTEXTURE9 texScore, LPDIRECT3DTEXTURE9 texTruth, float thres,
					float &true_pos, float &true_neg, float &false_pos, float &false_neg );

	LPDIRECT3DTEXTURE9	GetMaskTex() { return m_texMask; };
	LPDIRECT3DSURFACE9	GetMaskSurf() { return m_surfMask; };
private:
	HRESULT		CreateShaders();
	HRESULT		CreateTextures();
	HRESULT		CreateGeometries();
};

inline HRESULT EvaluateMask::CreateResources()
{
	if ( FAILED( CreateShaders() ) )
		return E_FAIL;
	if ( FAILED( CreateTextures() ) )
		return E_FAIL;
	if ( FAILED( CreateGeometries() ) )
		return E_FAIL;
	return S_OK;
}
