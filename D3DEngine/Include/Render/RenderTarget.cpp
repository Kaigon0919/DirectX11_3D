#include "RenderTarget.h"
#include "../Device.h"
#include "../Resource/Mesh.h"
#include "../Resource/ResourcesManager.h"
#include "Shader.h"
#include "ShaderManager.h"

ENGINE_USING

CRenderTarget::CRenderTarget()	:
	m_pTargetView(nullptr),
	m_pTargetTex(nullptr),
	m_pTargetSRV(nullptr),
	m_pDepthView(nullptr),
	m_pDepthTex(nullptr),
	m_pOldTargetView(nullptr),
	m_pOldDepthView(nullptr),
	m_pMesh(nullptr),
	m_pLayout(nullptr),
	m_pShader(nullptr),
	m_bDebug(false)
{
	m_pProj = new Matrix;
}

CRenderTarget::~CRenderTarget()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pMesh);
	SAFE_DELETE(m_pProj);
	SAFE_RELEASE(m_pTargetView);
	SAFE_RELEASE(m_pTargetTex);
	SAFE_RELEASE(m_pTargetSRV);
	SAFE_RELEASE(m_pDepthView);
	SAFE_RELEASE(m_pDepthTex);
	SAFE_RELEASE(m_pOldDepthView);
	SAFE_RELEASE(m_pOldTargetView);
}

ID3D11RenderTargetView * CRenderTarget::GetRenderTargetView() const
{
	return m_pTargetView;
}

ID3D11DepthStencilView * CRenderTarget::GetDepthStencilView() const
{
	return m_pDepthView;
}

bool CRenderTarget::CreateRenderTarget(const string & strName,
	UINT iWidth, UINT iHeight, DXGI_FORMAT eFmt, float fClearColor[4],
	int iSampleCount, DXGI_FORMAT eDepthFmt)
{
	m_strName = strName;

	memcpy(m_fClearColor, fClearColor, sizeof(float) * 4);

	D3D11_TEXTURE2D_DESC	tDesc = {};

	tDesc.Width = iWidth;
	tDesc.Height = iHeight;
	tDesc.ArraySize = 1;
	tDesc.Format = eFmt;
	tDesc.MipLevels = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = 8;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pTargetTex)))
		return false;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pTargetTex, nullptr,
		&m_pTargetSRV)))
		return false;

	if (FAILED(_DEVICE->CreateRenderTargetView(m_pTargetTex, nullptr,
		&m_pTargetView)))
		return false;

	if (eDepthFmt != DXGI_FORMAT_UNKNOWN)
	{
		tDesc.Format = eDepthFmt;
		tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		if (FAILED(_DEVICE->CreateTexture2D(&tDesc, nullptr, &m_pDepthTex)))
			return false;

		if (FAILED(_DEVICE->CreateDepthStencilView(m_pDepthTex, nullptr,
			&m_pDepthView)))
			return false;
	}

	*m_pProj = XMMatrixOrthographicOffCenterLH(0.f, (float)_RESOLUTION.iWidth,
		0.f, (float)_RESOLUTION.iHeight, 0.f, 1000.f);

	return true;
}

void CRenderTarget::OnDebug(const Vector3 & vPos, const Vector3 & vScale,
	bool bDebug)
{
	m_bDebug = bDebug;
	m_vPos = vPos;
	m_vScale = vScale;

	if (!m_pMesh)
	{
		m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("Rectangle");
		m_pShader = GET_SINGLE(CShaderManager)->FindShader(DEBUG_SHADER);
		m_pLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_UV_LAYOUT);
	}
}

void CRenderTarget::ClearTarget()
{
	_CONTEXT->ClearRenderTargetView(m_pTargetView, m_fClearColor);
	if (m_pDepthView)
		_CONTEXT->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void CRenderTarget::SetTarget()
{
	// 기존에 지정된 타겟을 얻어온다.
	// 여기서 얻어오면 내부적으로 레퍼런스카운트가 증가하게 된다.
	_CONTEXT->OMGetRenderTargets(1, &m_pOldTargetView, &m_pOldDepthView);

	// 만약 DepthView가 만들어져있다면 해당 DepthView로 지정하고
	// 없다면 기존에 있는 DepthView로 지정한다.
	ID3D11DepthStencilView*	pDepth = m_pDepthView;

	if (!pDepth)
		pDepth = m_pOldDepthView;

	_CONTEXT->OMSetRenderTargets(1, &m_pTargetView, pDepth);
}

void CRenderTarget::ResetTarget()
{
	// 얻어온 타겟들로 원래대로 되돌려준다.
	_CONTEXT->OMSetRenderTargets(1, &m_pOldTargetView, m_pOldDepthView);
	SAFE_RELEASE(m_pOldTargetView);
	SAFE_RELEASE(m_pOldDepthView);
}

void CRenderTarget::SetShader(int iRegister)
{
	_CONTEXT->PSSetShaderResources(iRegister, 1, &m_pTargetSRV);
}

void CRenderTarget::ResetShader(int iRegister)
{
	ID3D11ShaderResourceView*	pSRV = nullptr;
	_CONTEXT->PSSetShaderResources(iRegister, 1, &pSRV);
}

void CRenderTarget::Render()
{
	if (!m_bDebug)
		return;
	m_pShader->SetShader();

	Matrix	matScale, matTranslate;
	matScale.Scaling(m_vScale);
	matTranslate.Translation(m_vPos);

	DebugCBuffer	tCBuffer = {};

	tCBuffer.matWVP = matScale * matTranslate * *m_pProj;
	tCBuffer.matWVP.Transpose();
	
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Debug", &tCBuffer);

	SetShader(0);

	_CONTEXT->IASetInputLayout(m_pLayout);

	m_pMesh->Render();

	ResetShader(0);
}
