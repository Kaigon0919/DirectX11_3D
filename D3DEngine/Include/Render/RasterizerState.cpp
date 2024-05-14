#include "RasterizerState.h"
#include "../Device.h"

ENGINE_USING

CRasterizerState::CRasterizerState()
{
	m_eType = RS_RASTERIZER;
}

CRasterizerState::~CRasterizerState()
{
}

bool CRasterizerState::CreateState(const string & strName,
	D3D11_FILL_MODE eFill, D3D11_CULL_MODE eCull)
{
	SetTag(strName.c_str());

	D3D11_RASTERIZER_DESC	tDesc = {};

	tDesc.FillMode = eFill;
	tDesc.CullMode = eCull;
	tDesc.FrontCounterClockwise = FALSE;
	tDesc.DepthBias = 0;
	tDesc.SlopeScaledDepthBias = 0.f;
	tDesc.DepthBiasClamp = 0.f;
	tDesc.DepthClipEnable = TRUE;
	tDesc.ScissorEnable = FALSE;
	tDesc.MultisampleEnable = FALSE;
	tDesc.AntialiasedLineEnable = FALSE;

	if (FAILED(_DEVICE->CreateRasterizerState(&tDesc,
		(ID3D11RasterizerState**)&m_pState)))
		return false;

	return true;
}

void CRasterizerState::SetState()
{
	// 기존에 지정된 상태를 얻어온다.
	_CONTEXT->RSGetState((ID3D11RasterizerState**)&m_pOldState);

	// 새로운 상태를 지정한다.
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pState);
}

void CRasterizerState::ResetState()
{
	// 원래 상태로 돌려준다.
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pOldState);

	// Get을 해주었으면 레퍼런스 카운트가 증가되어 있다.
	// 이것을 감소시켜주어야 한다.
	SAFE_RELEASE(m_pOldState);
}
