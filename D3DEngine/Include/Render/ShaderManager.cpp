#include "ShaderManager.h"
#include "Shader.h"
#include "../Device.h"

ENGINE_USING

DEFINITION_SINGLE(CShaderManager)

CShaderManager::CShaderManager()	:
	m_iInputSize(0)
{
}

CShaderManager::~CShaderManager()
{
	unordered_map<string, PCBuffer>::iterator	iter;
	unordered_map<string, PCBuffer>::iterator	iterEnd = m_mapCBuffer.end();

	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second->pData);
		SAFE_RELEASE(iter->second->pBuffer);
		SAFE_DELETE(iter->second);
	}

	m_mapCBuffer.clear();

	Safe_Release_Map(m_mapShader);
	Safe_Release_Map(m_mapInputLayout);
}

bool CShaderManager::Init()
{
	if (!ShaderProcess())
		return false;

	if (!InputLayoutProcess())
		return false;
	
	if (!CBufferProcess())
		return false;

	return true;
}

bool CShaderManager::LoadShader(const string & strName, const TCHAR * pFileName, 
	string pEntry[ST_END], const string & strPathKey)
{
	CShader*	pShader = FindShader(strName);

	if (pShader)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	pShader = new CShader;

	if (!pShader->LoadShader(strName, pFileName, pEntry, strPathKey))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	m_mapShader.insert(make_pair(strName, pShader));

	return true;
}

CShader * CShaderManager::FindShader(const string & strName)
{
	unordered_map<string, CShader*>::iterator	iter = m_mapShader.find(strName);

	if (iter == m_mapShader.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

void CShaderManager::AddInputDesc(const char * pSemantic, UINT iSemanticIdx,
	DXGI_FORMAT eFmt, UINT iSize, UINT iInputSlot, D3D11_INPUT_CLASSIFICATION eClass,
	UINT iStepRate)
{
	D3D11_INPUT_ELEMENT_DESC	tDesc = {};
	tDesc.SemanticName = pSemantic;
	tDesc.SemanticIndex = iSemanticIdx;
	tDesc.Format = eFmt;
	tDesc.InputSlot = iInputSlot;
	tDesc.InputSlotClass = eClass;
	tDesc.InstanceDataStepRate = iStepRate;
	tDesc.AlignedByteOffset = m_iInputSize;

	m_iInputSize += iSize;


	m_vecInputDesc.push_back(tDesc);
}

bool CShaderManager::CreateInputLayout(const string & strName, const string& strShaderKey)
{
	ID3D11InputLayout*	pLayout = FindInputLayout(strName);

	if (pLayout)
		return false;

	CShader*	pShader = FindShader(strShaderKey);

	if (!pShader)
		return false;

	if (FAILED(_DEVICE->CreateInputLayout(&m_vecInputDesc[0], (UINT)m_vecInputDesc.size(), pShader->GetVSCode(), pShader->GetVSCodeSize(), &pLayout)))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	m_vecInputDesc.clear();
	m_iInputSize = 0;

	m_mapInputLayout.insert(make_pair(strName, pLayout));
	SAFE_RELEASE(pShader);

	return true;
}

ID3D11InputLayout * CShaderManager::FindInputLayout(const string & strName)
{
	unordered_map<string, ID3D11InputLayout*>::iterator	iter = m_mapInputLayout.find(strName);

	if (iter == m_mapInputLayout.end())
		return nullptr;

	return iter->second;
}

bool CShaderManager::CreateCBuffer(const string & strName, int iSize,  int iRegister, int iConstantShader)
{
	PCBuffer	pBuffer = FindCBuffer(strName);

	if (pBuffer)
		return false;

	pBuffer = new CBuffer;

	pBuffer->iSize = iSize;
	pBuffer->iRegister = iRegister;
	pBuffer->iConstant = iConstantShader;
	pBuffer->strName = strName;

	pBuffer->pData = new char[iSize];

	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iSize;
	tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	if (FAILED(_DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
	{
		SAFE_DELETE_ARRAY(pBuffer->pData);
		SAFE_DELETE(pBuffer);
		return false;
	}

	m_mapCBuffer.insert(make_pair(strName, pBuffer));

	return true;
}

bool CShaderManager::UpdateCBuffer(const string & strName, void * pData)
{
	PCBuffer	pBuffer = FindCBuffer(strName);

	if (!pBuffer)
		return false;

	// 버퍼에 데이터를 채워준다.
	D3D11_MAPPED_SUBRESOURCE	tMap = {};
	_CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD,
		0, &tMap);

	memcpy(tMap.pData, pData, pBuffer->iSize);

	_CONTEXT->Unmap(pBuffer->pBuffer, 0);

	// 갱신된 버퍼를 넘겨준다.
	if (pBuffer->iConstant & CS_VERTEX)
		_CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iConstant & CS_PIXEL)
		_CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iConstant & CS_GEOMETRY)
		_CONTEXT->GSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	return true;
}

PCBuffer CShaderManager::FindCBuffer(const string & strName)
{
	unordered_map<string, PCBuffer>::iterator	iter = m_mapCBuffer.find(strName);

	if (iter == m_mapCBuffer.end())
		return nullptr;

	return iter->second;
}

bool CShaderManager::ShaderProcess()
{

	string pEntry[ST_END] = {};

	pEntry[ST_VERTEX] = "StandardColorVS";
	pEntry[ST_PIXEL] = "StandardColorPS";
	if (!LoadShader(STANDARD_COLOR_SHADER, TEXT("Standard.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "StandardNormalColorVS";
	pEntry[ST_PIXEL] = "StandardColorNormalPS";
	if (!LoadShader(STANDARD_NORMAL_COLOR_SHADER, TEXT("Standard.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "SkyVS";
	pEntry[ST_PIXEL] = "SkyPS";
	if (!LoadShader("SkyShader", TEXT("Standard.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "LightAccPS";
	if (!LoadShader(LIGHTACC_DIR_SHADER, TEXT("Light.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "LightAccPointVS";
	pEntry[ST_PIXEL] = "LightAccPS";
	if (!LoadShader("LightAccPointShader", TEXT("Light.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "LightAccSpotVS";
	pEntry[ST_PIXEL] = "LightAccPS";
	if (!LoadShader("LightAccSpotShader", TEXT("Light.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "LightBlendPS";
	if (!LoadShader("LightBlend", TEXT("Light.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "LightAccDirVS";
	pEntry[ST_PIXEL] = "LightBlendRenderPS";
	if (!LoadShader("LightBlendRender", TEXT("Light.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "DebugVS";
	pEntry[ST_PIXEL] = "DebugPS";
	if (!LoadShader(DEBUG_SHADER, TEXT("Debug.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "StandardVertex3DVS";
	pEntry[ST_PIXEL] = "StandardVertex3DPS";
	if (!LoadShader("Vertex3D", TEXT("Share.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "StandardTexNormalVS";
	pEntry[ST_PIXEL] = "Standard3DPS";
	if (!LoadShader(STANDARD_TEX_NORMAL_SHADER, TEXT("Standard.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "Standard3DVS";
	pEntry[ST_PIXEL] = "Standard3DPS";
	if (!LoadShader(STANDARD_3D_SHADER, TEXT("Standard.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "ColliderVS";
	pEntry[ST_PIXEL] = "ColliderPS";
	if (!LoadShader(COLLIDER_SHADER, TEXT("Collider.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "ParticleVS";
	pEntry[ST_PIXEL] = "ParticlePS";
	pEntry[ST_GEOMETRY] = "ParticleGS";
	if (!LoadShader(PARTICLE_SHADER, TEXT("Particle.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "LandScapeVS";
	pEntry[ST_PIXEL] = "LandScapePS";
	pEntry[ST_GEOMETRY] = "";
	if (!LoadShader(LANDSCAPE_SHADER, TEXT("LandScape.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "ButtonVS";
	pEntry[ST_PIXEL] = "ButtonPS";

	if (!LoadShader("ButtonShader", TEXT("UI.fx"), pEntry))
		return false;

	pEntry[ST_VERTEX] = "Standard3DInstancingVS";
	pEntry[ST_PIXEL] = "Standard3DInstancingPS";

	if (!LoadShader(STANDARD_3D_INSTANCING_SHADER, TEXT("Standard.fx"), pEntry))
		return false;
	return true;
}

bool CShaderManager::InputLayoutProcess()
{
	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	if (!CreateInputLayout(POS_COLOR_LAYOUT, STANDARD_COLOR_SHADER))
		return false;

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16,0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	if (!CreateInputLayout(POS_NORMAL_COLOR_LAYOUT, STANDARD_NORMAL_COLOR_SHADER))
		return false;

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	if (!CreateInputLayout(POS_LAYOUT, "SkyShader"))
		return false;

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	if (!CreateInputLayout(POS_UV_LAYOUT, DEBUG_SHADER))
		return false;

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12,0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 8, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 12, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 16, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	if (!CreateInputLayout(VERTEX3D_LAYOUT, "Vertex3D"))
		return false;

	{
		// 인스턴싱을 사용하기 위한 레이아웃.
		AddInputDesc("POSITION"			, 0	, DXGI_FORMAT_R32G32B32_FLOAT		, 12	, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		AddInputDesc("NORMAL"			, 0	, DXGI_FORMAT_R32G32B32_FLOAT		, 12	, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		AddInputDesc("TEXCOORD"			, 0	, DXGI_FORMAT_R32G32_FLOAT			, 8		, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		AddInputDesc("TANGENT"			, 0	, DXGI_FORMAT_R32G32B32_FLOAT		, 12	, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		AddInputDesc("BINORMAL"			, 0	, DXGI_FORMAT_R32G32B32_FLOAT		, 12	, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		AddInputDesc("BLENDWEIGHTS"		, 0	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		AddInputDesc("BLENDINDICES"		, 0	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 0	, D3D11_INPUT_PER_VERTEX_DATA	, 0);
		
		// 인스턴싱 데이터 레이아웃을 만들기 전에 초기화해준다.
		m_iInputSize = 0;

		AddInputDesc("WORLD"			, 0	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLD"			, 1	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLD"			, 2	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLD"			, 3	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);

		AddInputDesc("WORLDVIEW"		, 0	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLDVIEW"		, 1	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLDVIEW"		, 2	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLDVIEW"		, 3	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);

		AddInputDesc("WORLDVIEWROT"		, 0	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLDVIEWROT"		, 1	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLDVIEWROT"		, 2	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);
		AddInputDesc("WORLDVIEWROT"		, 3	, DXGI_FORMAT_R32G32B32A32_FLOAT	, 16	, 1	, D3D11_INPUT_PER_INSTANCE_DATA	, 1);

		if (!CreateInputLayout(VERTEX3D_STATIC_INSTANCING_LAYOUT, STANDARD_3D_INSTANCING_SHADER))
			return false;
	}
	return true;
}

bool CShaderManager::CBufferProcess()
{

	// 상수버퍼 생성
	if (!CreateCBuffer("Transform", sizeof(TransformCBuffer), 0, CS_VERTEX | CS_PIXEL | CS_GEOMETRY))
		return false;

	if (!CreateCBuffer("Material", sizeof(Material), 1, CS_VERTEX | CS_PIXEL))
		return false;

	if(!CreateCBuffer("Light", sizeof(LightInfo), 2, CS_VERTEX | CS_PIXEL))
		return false;

	if (!CreateCBuffer("Rendering", sizeof(RenderCBuffer), 3, CS_VERTEX | CS_PIXEL))
		return false;

	if (!CreateCBuffer("Debug", sizeof(DebugCBuffer), 9, CS_VERTEX | CS_PIXEL))
		return false;

	if (!CreateCBuffer("Collider", sizeof(Vector4), 10, CS_PIXEL))
		return false;

	if (!CreateCBuffer("Particle", sizeof(ParticleCBuffer), 10, CS_GEOMETRY))
		return false;

	if (!CreateCBuffer("AnimationFrame", sizeof(AnimationFrameCBuffer), 8, CS_VERTEX | CS_GEOMETRY | CS_PIXEL))
		return false;

	if (!CreateCBuffer("LandScape", sizeof(LandScaleCBuffer), 10, CS_VERTEX | CS_PIXEL))
		return false;

	if (!CreateCBuffer("Button", sizeof(ButtonCBuffer), 11, CS_VERTEX | CS_PIXEL))
		return false;

	return true;
}
