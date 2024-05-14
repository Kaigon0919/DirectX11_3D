#include "Shader.h"
#include "../PathManager.h"
#include "../Device.h"

ENGINE_USING

CShader::CShader()	:
	m_pVS(nullptr),
	m_pVSBlob(nullptr),
	m_pPS(nullptr),
	m_pPSBlob(nullptr),
	m_pGS(nullptr),
	m_pGSBlob(nullptr)
{
}

CShader::~CShader()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pVSBlob);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pGSBlob);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSBlob);
}

const void * CShader::GetVSCode() const
{
	return m_pVSBlob->GetBufferPointer();
}

size_t CShader::GetVSCodeSize() const
{
	return m_pVSBlob->GetBufferSize();
}

bool CShader::LoadShader(const string & strName, const TCHAR * pFileName,
	string pEntry[ST_END], const string & strPathKey)
{
	SetTag(strName.c_str());

	// 전체경로를 만든다.
	const TCHAR* pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);

	TCHAR	strPath[MAX_PATH] = {};
	if (pPath)
		lstrcpy(strPath, pPath);

	lstrcat(strPath, pFileName);

	if (!LoadVertexShader(strPath, pEntry[ST_VERTEX].c_str()))
		return false;

	if (!LoadPixelShader(strPath, pEntry[ST_PIXEL].c_str()))
		return false;

	if (!pEntry[ST_GEOMETRY].empty())
	{
		if (!LoadGeometryShader(strPath, pEntry[ST_GEOMETRY].c_str()))
			return false;
	}

	return true;
}

void CShader::SetShader()
{
	_CONTEXT->VSSetShader(m_pVS, nullptr, 0);
	_CONTEXT->PSSetShader(m_pPS, nullptr, 0);
	_CONTEXT->GSSetShader(m_pGS, nullptr, 0);
}

bool CShader::LoadVertexShader(const TCHAR * pFullPath, const char * pEntry)
{
	UINT	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif // _DEBUG

	ID3DBlob*	pErr = nullptr;

	if (FAILED(D3DCompileFromFile(pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pEntry, "vs_5_0", iFlag, 0, &m_pVSBlob, &pErr)))
	{
		OutputDebugStringA((char*)pErr->GetBufferPointer());
		return false;
	}

	if (FAILED(_DEVICE->CreateVertexShader(m_pVSBlob->GetBufferPointer(),
		m_pVSBlob->GetBufferSize(), nullptr, &m_pVS)))
		return false;

	return true;
}

bool CShader::LoadPixelShader(const TCHAR * pFullPath, const char * pEntry)
{
	UINT	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif // _DEBUG

	ID3DBlob*	pErr = nullptr;

	if (FAILED(D3DCompileFromFile(pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pEntry, "ps_5_0", iFlag, 0, &m_pPSBlob, &pErr)))
	{
		OutputDebugStringA((char*)pErr->GetBufferPointer());
		return false;
	}

	if (FAILED(_DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(),
		m_pPSBlob->GetBufferSize(), nullptr, &m_pPS)))
		return false;

	return true;
}

bool CShader::LoadGeometryShader(const TCHAR * pFullPath, 
	const char * pEntry)
{
	UINT	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif // _DEBUG

	ID3DBlob*	pErr = nullptr;

	if (FAILED(D3DCompileFromFile(pFullPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pEntry, "gs_5_0", iFlag, 0, &m_pGSBlob, &pErr)))
	{
		OutputDebugStringA((char*)pErr->GetBufferPointer());
		return false;
	}

	if (FAILED(_DEVICE->CreateGeometryShader(m_pGSBlob->GetBufferPointer(),
		m_pGSBlob->GetBufferSize(), nullptr, &m_pGS)))
		return false;

	return true;
}
