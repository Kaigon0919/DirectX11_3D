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
	// ������ ������ ���¸� ���´�.
	_CONTEXT->RSGetState((ID3D11RasterizerState**)&m_pOldState);

	// ���ο� ���¸� �����Ѵ�.
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pState);
}

void CRasterizerState::ResetState()
{
	// ���� ���·� �����ش�.
	_CONTEXT->RSSetState((ID3D11RasterizerState*)m_pOldState);

	// Get�� ���־����� ���۷��� ī��Ʈ�� �����Ǿ� �ִ�.
	// �̰��� ���ҽ����־�� �Ѵ�.
	SAFE_RELEASE(m_pOldState);
}
