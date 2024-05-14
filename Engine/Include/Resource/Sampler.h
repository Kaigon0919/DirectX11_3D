#pragma once
#include "../Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CSampler :
	public CRef
{
	friend class CResourcesManager;

private:
	CSampler();
	~CSampler();

private:
	ID3D11SamplerState*	m_pSampler;

public:
	// default : D3D11_FILTER_MIN_MAG_MIP_LINEAR
	bool CreateSampler(const string& strName, D3D11_FILTER eFilter,
		D3D11_TEXTURE_ADDRESS_MODE eAddrU,
		D3D11_TEXTURE_ADDRESS_MODE eAddrV,
		D3D11_TEXTURE_ADDRESS_MODE eAddrW);
	void VSSetShader(int iRegister);
	void PSSetShader(int iRegister);
};

ENGINE_END
