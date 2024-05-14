#include "Sampler.h"
#include "../Device.h"

ENGINE_USING

CSampler::CSampler()	:
	m_pSampler(nullptr)
{
}

CSampler::~CSampler()
{
	SAFE_RELEASE(m_pSampler);
}

bool CSampler::CreateSampler(const string & strName, D3D11_FILTER eFilter,
	D3D11_TEXTURE_ADDRESS_MODE eAddrU,
	D3D11_TEXTURE_ADDRESS_MODE eAddrV,
	D3D11_TEXTURE_ADDRESS_MODE eAddrW)
{
	D3D11_SAMPLER_DESC	tDesc = {};

	tDesc.Filter = eFilter;
	tDesc.AddressU = eAddrU;
	tDesc.AddressV = eAddrV;
	tDesc.AddressW = eAddrW;
	tDesc.MinLOD = -FLT_MAX;
	tDesc.MaxLOD = FLT_MAX;
	tDesc.MipLODBias = 0.f;
	tDesc.MaxAnisotropy = 1;
	tDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memcpy(tDesc.BorderColor, &Vector4(1.f, 1.f, 1.f, 1.f), sizeof(Vector4));

	if (FAILED(_DEVICE->CreateSamplerState(&tDesc, &m_pSampler)))
		return false;

	SetTag(strName.c_str());

	return true;
}

void CSampler::VSSetShader(int iRegister)
{
	_CONTEXT->VSSetSamplers(iRegister, 1, &m_pSampler);
}

void CSampler::PSSetShader(int iRegister)
{
	_CONTEXT->PSSetSamplers(iRegister, 1, &m_pSampler);
}
