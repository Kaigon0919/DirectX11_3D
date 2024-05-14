#include "DepthStencilState.h"
#include "../Device.h"

ENGINE_USING

CDepthStencilState::CDepthStencilState()
{
	m_iStencilRef = 0xffffffff;
	m_eType = RS_DEPTH;
}

CDepthStencilState::~CDepthStencilState()
{
}

bool CDepthStencilState::CreateState(const string& strName, BOOL bEnable,
	D3D11_DEPTH_WRITE_MASK eWriteMask, 
	D3D11_COMPARISON_FUNC eDepthFunc)
{
	SetTag(strName.c_str());
	D3D11_DEPTH_STENCIL_DESC	tDesc = {};

	tDesc.DepthEnable = bEnable;
	tDesc.DepthWriteMask = eWriteMask;
	tDesc.DepthFunc = eDepthFunc;
	tDesc.StencilEnable = FALSE;
	tDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	tDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	tDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	tDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	tDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	tDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	tDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	tDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	tDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	tDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	if (FAILED(_DEVICE->CreateDepthStencilState(&tDesc,
		(ID3D11DepthStencilState**)&m_pState)))
		return false;

	return true;
}

void CDepthStencilState::SetState()
{
	_CONTEXT->OMGetDepthStencilState((ID3D11DepthStencilState**)&m_pOldState,
		&m_iOldStencilRef);
	_CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pState,
		m_iStencilRef);
}

void CDepthStencilState::ResetState()
{
	_CONTEXT->OMSetDepthStencilState((ID3D11DepthStencilState*)m_pOldState,
		m_iOldStencilRef);
	SAFE_RELEASE(m_pOldState);
}
