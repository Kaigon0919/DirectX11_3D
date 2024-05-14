#pragma once
#include "RenderState.h"

ENGINE_BEGIN

class ENGINE_DLL CBlendState :
	public CRenderState
{
	friend class CRenderManager;

private:
	CBlendState();
	~CBlendState();

private:
	vector<D3D11_RENDER_TARGET_BLEND_DESC>	m_vecBlendDesc;
	float	m_fBlendFactor[4];
	UINT	m_iSampleMask;
	float	m_fOldBlendFactor[4];
	UINT	m_iOldSampleMask;

public:
	bool CreateState(const string& strName, BOOL bAlphaToCoverage = FALSE,
		BOOL bIndependentBlend = FALSE);
	void AddTargetBlendDesc(BOOL bEnable = FALSE, 
		D3D11_BLEND eSrcBlend = D3D11_BLEND_ONE,
		D3D11_BLEND eDestBlend = D3D11_BLEND_ZERO, 
		D3D11_BLEND_OP eOp = D3D11_BLEND_OP_ADD,
		D3D11_BLEND eSrcBlendAlpha = D3D11_BLEND_ONE,
		D3D11_BLEND eDestBlendAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP eAlphaOp = D3D11_BLEND_OP_ADD,
		UINT iWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);

public:
	virtual void SetState();
	virtual void ResetState();
};

ENGINE_END
