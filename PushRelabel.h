#include "general.h"

class PushRelabel
{
private:
	int m_nWidth, m_nHeight;
	D3DXVECTOR4 m_vStepX, m_vStepY;

	LPDIRECT3DTEXTURE9 m_texExcess, m_texResidual, m_texPush;
	LPDIRECT3DTEXTURE9 m_texExcessUpdate, m_texResidualUpdate;
	LPDIRECT3DSURFACE9 m_surfExcess, m_surfResidual, m_surfPush;
	LPDIRECT3DSURFACE9 m_surfExcessUpdate, m_surfResidualUpdate;

	LPDIRECT3DVERTEXBUFFER9 m_vbImagePlane;

	LPDIRECT3DPIXELSHADER9 m_shInitExcess;
	LPDIRECT3DPIXELSHADER9 m_shPushFlow, m_shUpdateExcess;
	LPD3DXCONSTANTTABLE m_tabInitExcess;
	LPD3DXCONSTANTTABLE m_tabPushFlow, m_tabUpdateExcess;
public:
	PushRelabel( const DataMotion& data );
	~PushRelabel();

	HRESULT		CreateResources();

	void		InitializeExcessResidual( LPDIRECT3DTEXTURE9 texLoss, float thres );
	void		InitializeExcessResidual
					( LPDIRECT3DTEXTURE9 texLoss1, LPDIRECT3DTEXTURE9 texLoss2, float thres );
	void		PulseOperation( bool push_back );

	LPDIRECT3DTEXTURE9	GetExcessTex() { return m_texExcess; };
	LPDIRECT3DSURFACE9	GetExcessSurf() { return m_surfExcess; };
private:
	HRESULT		CreateShaders();
	HRESULT		CreateTextures();
	HRESULT		CreateGeometries();
};

inline HRESULT PushRelabel::CreateResources()
{
	if ( FAILED( CreateShaders() ) )
		return E_FAIL;
	if ( FAILED( CreateTextures() ) )
		return E_FAIL;
	if ( FAILED( CreateGeometries() ) )
		return E_FAIL;
	return S_OK;
}
