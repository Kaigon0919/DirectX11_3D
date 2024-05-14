#pragma once
#include "../Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CShader :
	public CRef
{
	friend class CShaderManager;

private:
	CShader();
	~CShader();

private:
	ID3D11VertexShader*		m_pVS;
	ID3DBlob*				m_pVSBlob;
	ID3D11GeometryShader*	m_pGS;
	ID3DBlob*				m_pGSBlob;
	ID3D11PixelShader*		m_pPS;
	ID3DBlob*				m_pPSBlob;

public:
	const void* GetVSCode()	const;
	size_t GetVSCodeSize()	const;

public:
	bool LoadShader(const string& strName, const TCHAR* pFileName,
		string pEntry[ST_END], const string& strPathKey = SHADER_PATH);
	void SetShader();

private:
	bool LoadVertexShader(const TCHAR* pFullPath, const char* pEntry);
	bool LoadPixelShader(const TCHAR* pFullPath, const char* pEntry);
	bool LoadGeometryShader(const TCHAR* pFullPath, const char* pEntry);
};

ENGINE_END
