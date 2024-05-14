#pragma once
#include "RenderState.h"

ENGINE_BEGIN

class ENGINE_DLL CRasterizerState :
	public CRenderState
{
	friend class CRenderManager;

private:
	CRasterizerState();
	~CRasterizerState();

public:
	bool CreateState(const string& strName, D3D11_FILL_MODE eFill = D3D11_FILL_SOLID,
		D3D11_CULL_MODE eCull = D3D11_CULL_BACK);

public:
	virtual void SetState();
	virtual void ResetState();
};

ENGINE_END
