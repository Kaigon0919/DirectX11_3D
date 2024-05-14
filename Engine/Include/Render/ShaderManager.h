#pragma once

#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CShaderManager
{
private:
	unordered_map<string, class CShader*>	m_mapShader;
	unordered_map<string, ID3D11InputLayout*>	m_mapInputLayout;
	unordered_map<string, PCBuffer>		m_mapCBuffer;
	vector<D3D11_INPUT_ELEMENT_DESC>	m_vecInputDesc;
	UINT								m_iInputSize;

public:
	bool Init();
	bool LoadShader(const string& strName, const TCHAR* pFileName,
		string pEntry[ST_END], const string& strPathKey = SHADER_PATH);
	class CShader* FindShader(const string& strName);

	void AddInputDesc(const char* pSemantic, UINT iSemanticIdx,
		DXGI_FORMAT eFmt, UINT iSize, UINT iInputSlot,
		D3D11_INPUT_CLASSIFICATION eClass, UINT iStepRate);
	bool CreateInputLayout(const string& strName, const string& strShaderKey);
	ID3D11InputLayout* FindInputLayout(const string& strName);

	bool CreateCBuffer(const string& strName, int iSize, int iRegister,
		int iConstantShader);
	bool UpdateCBuffer(const string& strName, void* pData);
	PCBuffer FindCBuffer(const string& strName);
private:
	bool ShaderProcess();
	bool InputLayoutProcess();
	bool CBufferProcess();
	DECLARE_SINGLE(CShaderManager)
};

ENGINE_END
