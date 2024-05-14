#pragma once
#include "RenderState.h"

ENGINE_BEGIN

class ENGINE_DLL CDepthStencilState :
	public CRenderState
{
	friend class CRenderManager;

private:
	CDepthStencilState();
	~CDepthStencilState();

private:
	UINT	m_iStencilRef;
	UINT	m_iOldStencilRef;

public:
	bool CreateState(const string& strName, BOOL bEnable = TRUE,
		D3D11_DEPTH_WRITE_MASK eWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC eDepthFunc = D3D11_COMPARISON_LESS);

public:
	virtual void SetState();
	virtual void ResetState();
};

ENGINE_END
