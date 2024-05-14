#include "BlendState.h"
#include "../Device.h"

ENGINE_USING

CBlendState::CBlendState()
{
	m_eType = RS_BLEND;
	m_iSampleMask = 0xffffffff;
	memset(m_fBlendFactor, 0, sizeof(float) * 4);
}

CBlendState::~CBlendState()
{
}

bool CBlendState::CreateState(const string & strName, 
	BOOL bAlphaToCoverage, BOOL bIndependentBlend)
{
	SetTag(strName.c_str());

	D3D11_BLEND_DESC	tDesc = {};

	tDesc.AlphaToCoverageEnable = bAlphaToCoverage;
	tDesc.IndependentBlendEnable = bIndependentBlend;

	for (size_t i = 0; i < m_vecBlendDesc.size(); ++i)
	{
		tDesc.RenderTarget[i] = m_vecBlendDesc[i];
	}

	if (FAILED(_DEVICE->CreateBlendState(&tDesc, (ID3D11BlendState**)&m_pState)))
		return false;

	return true;
}

void CBlendState::AddTargetBlendDesc(BOOL bEnable, 
	D3D11_BLEND eSrcBlend, D3D11_BLEND eDestBlend, 
	D3D11_BLEND_OP eOp, D3D11_BLEND eSrcBlendAlpha, 
	D3D11_BLEND eDestBlendAlpha, D3D11_BLEND_OP eAlphaOp,
	UINT iWriteMask)
{
	if (m_vecBlendDesc.size() == 8)
		return;

	D3D11_RENDER_TARGET_BLEND_DESC	tDesc = {};

	tDesc.BlendEnable = bEnable;
	tDesc.SrcBlend = eSrcBlend;
	tDesc.DestBlend = eDestBlend;
	tDesc.BlendOp = eOp;
	tDesc.SrcBlendAlpha = eSrcBlendAlpha;
	tDesc.DestBlendAlpha = eDestBlendAlpha;
	tDesc.BlendOpAlpha = eAlphaOp;
	tDesc.RenderTargetWriteMask = iWriteMask;

	m_vecBlendDesc.push_back(tDesc);
}

void CBlendState::SetState()
{
	_CONTEXT->OMGetBlendState((ID3D11BlendState**)&m_pOldState,
		m_fOldBlendFactor, &m_iOldSampleMask);
	_CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pState,
		m_fBlendFactor, m_iSampleMask);
}

void CBlendState::ResetState()
{
	_CONTEXT->OMSetBlendState((ID3D11BlendState*)m_pOldState,
		m_fOldBlendFactor, m_iOldSampleMask);
	SAFE_RELEASE(m_pOldState);
}
