#pragma once
#include "../Ref.h"
#include "DirectXTex.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex64_Debug")
#else
#pragma comment(lib, "DirectXTex64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug")
#else
#pragma comment(lib, "DirectXTex")
#endif // _DEBUG
#endif // _WIN64

ENGINE_BEGIN

class ENGINE_DLL CTexture :
	public CRef
{
	friend class CResourcesManager;

private:
	CTexture();
	~CTexture();

private:
	vector<ScratchImage*>		m_vecImage;
	ID3D11ShaderResourceView*	m_pSRV;
	vector<TCHAR*>				m_vecFullPath;

public:
	const vector<TCHAR*>* GetFullPath()	const;

public:
	bool LoadTexture(const string& strName, const TCHAR* pFileName,
		const string& strPathKey);
	bool LoadTextureFromFullPath(const string& strName, const TCHAR* pFullPath);
	bool LoadTexture(const string& strName, const vector<TCHAR*>& vecFileName,
		const string& strPathKey);
	bool LoadTextureFromFullPath(const string& strName, const vector<TCHAR*>& vecFullPath);
	void VSSetShader(int iRegister);
	void PSSetShader(int iRegister);

private:
	bool CreateShaderResourceView();
	bool CreateShaderResourceViewArray();
};

ENGINE_END
