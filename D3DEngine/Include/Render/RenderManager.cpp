#include "RenderManager.h"
#include "ShaderManager.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "BlendState.h"
#include "../Component/Light.h"
#include "RenderTarget.h"
#include "../Device.h"
#include "../Resource/ResourcesManager.h"
#include "../Resource/Sampler.h"
#include "Shader.h"
#include "../Resource/Mesh.h"
#include "../CollisionManager.h"
#include "../Component/Material.h"
#include "../Component/Renderer.h"
#include "../Component/Transform.h"
#include "../Component/Camera.h"
#include "../Scene/Scene.h"

ENGINE_USING

DEFINITION_SINGLE(CRenderManager)

CRenderManager::CRenderManager()
{
	m_eMode = RM_DEFERRED;
	m_tRenderCBuffer.iRenderMode = m_eMode;
	m_pDepthDisable = nullptr;
	m_pAccBlend = nullptr;
	m_pLightAccDirShader = nullptr;
	m_pLightAccPointShader = nullptr;
	m_pLightAccSpotShader = nullptr;
	m_pLightPointVolume = nullptr;
	m_pLightBlendShader = nullptr;
	m_pLightBlendRenderShader = nullptr;
	m_pAlphaBlend = nullptr;
	m_pLightPointLayout = nullptr;
	m_pCullNone = nullptr;

	m_pStaticInstancing = nullptr;
	m_pAnimFrameInstancing = nullptr;
	m_pAnimInstancing = nullptr;
	m_pColliderInstancing = nullptr;
	m_pLightInstancing = nullptr;
	m_pStaticInstancingShader = nullptr;
	m_pAnimFrameInstancingShader = nullptr;
	m_pAnimInstancingShader = nullptr;
	m_pStaticInstancingLayout = nullptr;
	m_pAnimFrameInstancingLayout = nullptr;
	m_pAnimInstancingLayout = nullptr;
}

CRenderManager::~CRenderManager()
{
	if (m_pStaticInstancing)
	{
		SAFE_DELETE_ARRAY(m_pStaticInstancing->pData);
		SAFE_RELEASE(m_pStaticInstancing->pBuffer);
		SAFE_DELETE(m_pStaticInstancing);
	}
	if (m_pAnimFrameInstancing)
	{
		SAFE_DELETE_ARRAY(m_pAnimFrameInstancing->pData);
		SAFE_RELEASE(m_pAnimFrameInstancing->pBuffer);
		SAFE_DELETE(m_pAnimFrameInstancing);
	}
	if (m_pAnimInstancing)
	{
		SAFE_DELETE_ARRAY(m_pAnimInstancing->pData);
		SAFE_RELEASE(m_pAnimInstancing->pBuffer);
		SAFE_DELETE(m_pAnimInstancing);
	}
	if (m_pColliderInstancing)
	{
		SAFE_DELETE_ARRAY(m_pColliderInstancing->pData);
		SAFE_RELEASE(m_pColliderInstancing->pBuffer);
		SAFE_DELETE(m_pColliderInstancing);
	}
	if (m_pLightInstancing)
	{
		SAFE_DELETE_ARRAY(m_pLightInstancing->pData);
		SAFE_RELEASE(m_pLightInstancing->pBuffer);
		SAFE_DELETE(m_pLightInstancing);
	}

	SAFE_RELEASE(m_pStaticInstancingShader);
	SAFE_RELEASE(m_pAnimFrameInstancingShader);
	SAFE_RELEASE(m_pAnimInstancingShader);

	Safe_Delete_Map(m_mapInstancingGemoetry);

	SAFE_RELEASE(m_pCullNone);
	SAFE_RELEASE(m_pAlphaBlend);
	SAFE_RELEASE(m_pLightBlendRenderShader);
	SAFE_RELEASE(m_pLightBlendShader);
	SAFE_RELEASE(m_pLightAccDirShader);
	SAFE_RELEASE(m_pLightAccPointShader);
	SAFE_RELEASE(m_pLightAccSpotShader);
	SAFE_RELEASE(m_pLightPointVolume);
	SAFE_RELEASE(m_pAccBlend);
	SAFE_RELEASE(m_pDepthDisable);
	Safe_Delete_Map(m_mapMRT);
	Safe_Release_Map(m_mapRenderState);
	DESTROY_SINGLE(CShaderManager);

	unordered_map<string, CRenderTarget*>::iterator	iter;
	unordered_map<string, CRenderTarget*>::iterator	iterEnd = m_mapRenderTarget.end();

	for (iter = m_mapRenderTarget.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

PInstancingGeometry CRenderManager::FindInstancingGeometry(unsigned __int64 iKey)
{
	unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iter = m_mapInstancingGemoetry.find(iKey);

	if (iter == m_mapInstancingGemoetry.end())
		return nullptr;

	return iter->second;
}

PInstancingBuffer CRenderManager::CreateInstancingBuffer(int iSize, int iCount)
{
	PInstancingBuffer	pBuffer = new InstancingBuffer;

	pBuffer->iSize = iSize;
	pBuffer->iCount = iCount;
	pBuffer->eUsage = D3D11_USAGE_DYNAMIC;
	pBuffer->pData = new char[iSize * iCount];

	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = iSize * iCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
		return nullptr;

	return pBuffer;
}

void CRenderManager::ResizeInstancingBuffer(PInstancingBuffer pBuffer, int iCount)
{
	SAFE_DELETE_ARRAY(pBuffer->pData);
	SAFE_RELEASE(pBuffer->pBuffer);
	pBuffer->iCount = iCount;
	pBuffer->pData = new char[pBuffer->iSize * iCount];

	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = pBuffer->iSize * pBuffer->iCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(_DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
		return;
}

void CRenderManager::AddInstancingData(PInstancingBuffer pBuffer, int iPos, void * pData)
{
	char* pBufferData = (char*)pBuffer->pData;
	memcpy(pBufferData + (iPos * pBuffer->iSize), pData, pBuffer->iSize);
}

void CRenderManager::CopyInstancingData(PInstancingBuffer pBuffer, int iCount)
{
	D3D11_MAPPED_SUBRESOURCE	tMap = {};

	_CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
	memcpy(tMap.pData, pBuffer->pData, pBuffer->iSize * iCount);
	_CONTEXT->Unmap(pBuffer->pBuffer, 0);
}

CLight * CRenderManager::GetFirstLight() const
{
	if (m_tLightList.iSize == 0)
		return nullptr;

	else if (m_eMode == RM_DEFERRED)
		return nullptr;

	return m_tLightList.pObjList[0]->FindComponentFromType<CLight>(CT_LIGHT);
}

bool CRenderManager::Init()
{
	if (!GET_SINGLE(CShaderManager)->Init())
		return false;

	m_pStaticInstancing = CreateInstancingBuffer(sizeof(InstancingStaticBuffer));
	m_pAnimFrameInstancing = CreateInstancingBuffer(sizeof(InstancingAnimFrameBuffer));
	m_pAnimInstancing = CreateInstancingBuffer(sizeof(InstancingStaticBuffer));

	m_pStaticInstancingShader = GET_SINGLE(CShaderManager)->FindShader(STANDARD_3D_INSTANCING_SHADER);
	m_pStaticInstancingLayout = GET_SINGLE(CShaderManager)->FindInputLayout(VERTEX3D_STATIC_INSTANCING_LAYOUT);


	CreateRasterizerState("CullNone", D3D11_FILL_SOLID, D3D11_CULL_NONE);
	CreateRasterizerState("WireFrame", D3D11_FILL_WIREFRAME);
	CreateDepthState("LessEqual", TRUE, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	CreateDepthState("DepthDisable", FALSE);

	AddTargetBlendDesc("AlphaBlend", TRUE, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
	CreateBlendState("AlphaBlend", TRUE, FALSE);

	AddTargetBlendDesc("AccBlend", TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE);
	CreateBlendState("AccBlend", TRUE, FALSE);

	m_pCullNone = FindRenderState("CullNone");
	m_pDepthDisable = FindRenderState("DepthDisable");
	m_pAccBlend = FindRenderState("AccBlend");
	m_pAlphaBlend = FindRenderState("AlphaBlend");
	m_pLightAccDirShader = GET_SINGLE(CShaderManager)->FindShader(LIGHTACC_DIR_SHADER);
	m_pLightAccPointShader = GET_SINGLE(CShaderManager)->FindShader("LightAccPointShader");
	m_pLightAccSpotShader = GET_SINGLE(CShaderManager)->FindShader("LightAccSpotShader");

	m_pLightPointLayout = GET_SINGLE(CShaderManager)->FindInputLayout(POS_LAYOUT);
	m_pLightPointVolume = GET_SINGLE(CResourcesManager)->FindMesh("Sky");

	m_pLightBlendShader = GET_SINGLE(CShaderManager)->FindShader("LightBlend");
	m_pLightBlendRenderShader = GET_SINGLE(CShaderManager)->FindShader("LightBlendRender");
	
	float	fClearColor[4] = {};

	if (!CreateRenderTarget("Albedo", _RESOLUTION.iWidth, _RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, fClearColor, 8))
		return false;

	OnDebugRenderTarget("Albedo", Vector3(0.f, 0.f, 0.f), Vector3(100.f, 100.f, 1.f));

	if (!CreateRenderTarget("Normal", _RESOLUTION.iWidth, _RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, fClearColor, 8))
		return false;

	OnDebugRenderTarget("Normal", Vector3(0.f, 100.f, 0.f), Vector3(100.f, 100.f, 1.f));

	if (!CreateRenderTarget("Depth", _RESOLUTION.iWidth, _RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, fClearColor, 8))
		return false;

	OnDebugRenderTarget("Depth", Vector3(0.f, 200.f, 0.f), Vector3(100.f, 100.f, 1.f));

	if (!CreateRenderTarget("Material", _RESOLUTION.iWidth, _RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, fClearColor, 8))
		return false;

	OnDebugRenderTarget("Material", Vector3(0.f, 300.f, 0.f), Vector3(100.f, 100.f, 1.f));

	AddMRT("GBuffer", "Albedo");
	AddMRT("GBuffer", "Normal");
	AddMRT("GBuffer", "Depth");
	AddMRT("GBuffer", "Material");

	if (!CreateRenderTarget("LightDif", _RESOLUTION.iWidth,	_RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT,fClearColor))
		return false;

	OnDebugRenderTarget("LightDif", Vector3(100.f, 0.f, 0.f),Vector3(100.f, 100.f, 1.f));

	if (!CreateRenderTarget("LightSpc", _RESOLUTION.iWidth,	_RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT,fClearColor))
		return false;

	OnDebugRenderTarget("LightSpc", Vector3(100.f, 100.f, 0.f),	Vector3(100.f, 100.f, 1.f));

	AddMRT("LightAcc", "LightDif");
	AddMRT("LightAcc", "LightSpc");

	if (!CreateRenderTarget("LightBlend", _RESOLUTION.iWidth,_RESOLUTION.iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT,fClearColor))
		return false;

	OnDebugRenderTarget("LightBlend", Vector3(200.f, 0.f, 0.f),	Vector3(100.f, 100.f, 1.f));

	return true;
}

void CRenderManager::AddRenderObject(CGameObject * pObj)
{
	if (pObj->CheckComponentFromType(CT_LIGHT))
	{
		if (m_tLightList.iSize == m_tLightList.iCapacity)
		{
			m_tLightList.iCapacity *= 2;

			CGameObject**	pList = new CGameObject*[m_tLightList.iCapacity];

			memcpy(pList, m_tLightList.pObjList, sizeof(CGameObject*) * m_tLightList.iSize);

			SAFE_DELETE_ARRAY(m_tLightList.pObjList);

			m_tLightList.pObjList = pList;
		}

		m_tLightList.pObjList[m_tLightList.iSize] = pObj;
		++m_tLightList.iSize;
	}

	if (!pObj->CheckComponentFromType(CT_RENDERER))
	{
		if (!pObj->CheckComponentFromType(CT_TEXT))
			return;
	}
	RENDER_GROUP	rg = pObj->GetRenderGroup();

	CRenderer*	pRenderer = pObj->FindComponentFromType<CRenderer>(CT_RENDERER);

	if (pRenderer)
	{
		CMesh*	pMesh = pRenderer->GetMesh();
		CMaterial*	pMaterial = pObj->FindComponentFromType<CMaterial>(CT_MATERIAL);

		// 절두체 컬링을 해야 하는지 판단한다.
		// UI는 따로 처리한다.
		if (rg <= RG_ALPHA)
		{
			Vector3	vCenter = pMesh->GetCenter();
			float	fRadius = pMesh->GetRadius();

			// Center와 Radius를 월드공간으로 변환한다.
			CTransform*	pTr = pObj->GetTransform();

			Vector3	vPos = pTr->GetWorldPos();
			Vector3	vScale = pTr->GetWorldScale();

			SAFE_RELEASE(pTr);

			vCenter *= vScale;
			vCenter += vPos;

			float	fScale = vScale.x;
			fScale = fScale < vScale.y ? vScale.y : fScale;
			fScale = fScale < vScale.z ? vScale.z : fScale;

			fRadius *= fScale;

			CCamera*	pMainCamera = pObj->GetScene()->GetMainCamera();
			bool	bFrustumCull = pMainCamera->FrustumInSphere(vCenter, fRadius);

			SAFE_RELEASE(pMainCamera);

			pObj->SetFrustumCulling(!bFrustumCull);

			if (!bFrustumCull) 
			{
				SAFE_RELEASE(pMesh);
				SAFE_RELEASE(pRenderer);
				SAFE_RELEASE(pMaterial);

				return;
			}
		}
		else // UI의 경우.
		{ }

		unsigned int	iMeshNumber = pMesh->GetSerialNumber();
		unsigned int	iMtrlNumber = pMaterial->GetSerialNumber();

		unsigned __int64	iKey = iMeshNumber;
		iKey <<= 32;
		iKey |= iMtrlNumber;

		PInstancingGeometry	pGeometry = FindInstancingGeometry(iKey);

		if (!pGeometry)
		{
			pGeometry = new InstancingGeometry;
			m_mapInstancingGemoetry.insert(make_pair(iKey, pGeometry));
		}

		if (pObj->CheckComponentFromType(CT_ANIMATIONFRAME))
			pGeometry->bAnimationFrame = true;

		else if (pObj->CheckComponentFromType(CT_ANIMATION))
			pGeometry->bAnimation = true;
		else
		{
			pGeometry->bAnimation = false;
			pGeometry->bAnimationFrame = false;
		}
		pGeometry->Add(pObj);

		SAFE_RELEASE(pMesh);
		SAFE_RELEASE(pMaterial);
	}
	SAFE_RELEASE(pRenderer);

	if (m_tRenderGroup[rg].iSize == m_tRenderGroup[rg].iCapacity)
	{
		m_tRenderGroup[rg].iCapacity *= 2;

		CGameObject**	pList = new CGameObject*[m_tRenderGroup[rg].iCapacity];

		memcpy(pList, m_tRenderGroup[rg].pObjList, sizeof(CGameObject*) * m_tRenderGroup[rg].iSize);

		SAFE_DELETE_ARRAY(m_tRenderGroup[rg].pObjList);

		m_tRenderGroup[rg].pObjList = pList;
	}

	m_tRenderGroup[rg].pObjList[m_tRenderGroup[rg].iSize] = pObj;
	++m_tRenderGroup[rg].iSize;
}

bool CRenderManager::CreateRasterizerState(const string & strName, D3D11_FILL_MODE eFill, D3D11_CULL_MODE eCull)
{
	CRasterizerState*	pState = (CRasterizerState*)FindRenderState(strName);

	if (pState)
	{
		SAFE_RELEASE(pState);
		return false;
	}

	pState = new CRasterizerState;

	if (!pState->CreateState(strName, eFill, eCull))
	{
		SAFE_RELEASE(pState);
		return false;
	}

	m_mapRenderState.insert(make_pair(strName, pState));

	return true;
}

bool CRenderManager::CreateDepthState(const string& strName, BOOL bEnable,
	D3D11_DEPTH_WRITE_MASK eWriteMask, 
	D3D11_COMPARISON_FUNC eDepthFunc)
{
	CDepthStencilState*	pState = (CDepthStencilState*)FindRenderState(strName);

	if (pState)
	{
		SAFE_RELEASE(pState);
		return false;
	}

	pState = new CDepthStencilState;

	if (!pState->CreateState(strName, bEnable, eWriteMask, eDepthFunc))
	{
		SAFE_RELEASE(pState);
		return false;
	}

	m_mapRenderState.insert(make_pair(strName, pState));

	return true;
}

bool CRenderManager::AddTargetBlendDesc(const string & strName, BOOL bEnable, D3D11_BLEND eSrcBlend, D3D11_BLEND eDestBlend, D3D11_BLEND_OP eOp, D3D11_BLEND eSrcBlendAlpha, D3D11_BLEND eDestBlendAlpha, D3D11_BLEND_OP eAlphaOp, UINT iWriteMask)
{
	CBlendState*	pState = (CBlendState*)FindRenderState(strName);

	if (!pState)
	{
		pState = new CBlendState;
		pState->AddRef();
		m_mapRenderState.insert(make_pair(strName, pState));
	}

	pState->AddTargetBlendDesc(bEnable, eSrcBlend, eDestBlend,
		eOp, eSrcBlendAlpha, eDestBlendAlpha, eAlphaOp,
		iWriteMask); 

	SAFE_RELEASE(pState);

	return true;
}

bool CRenderManager::CreateBlendState(const string & strName, BOOL bAlphaToCoverage, BOOL bIndependentBlend)
{
	CBlendState*	pState = (CBlendState*)FindRenderState(strName);

	if (!pState)
		return false;

	if (!pState->CreateState(strName, bAlphaToCoverage, bIndependentBlend))
	{
		pState->Release();

		unordered_map<string, CRenderState*>::iterator	iter =
			m_mapRenderState.find(strName);

		m_mapRenderState.erase(iter);

		SAFE_RELEASE(pState);
		return false;
	}

	SAFE_RELEASE(pState);

	return true;
}

bool CRenderManager::CreateRenderTarget(const string & strName, 
	UINT iWidth, UINT iHeight, DXGI_FORMAT eFmt, float fClearColor[4],
	int iSampleCount, DXGI_FORMAT eDepthFmt)
{
	CRenderTarget*	pTarget = (CRenderTarget*)FindRenderTarget(strName);

	if (pTarget)
		return false;

	pTarget = new CRenderTarget;

	if (!pTarget->CreateRenderTarget(strName, iWidth, iHeight, eFmt, fClearColor,
		iSampleCount, eDepthFmt))
	{
		SAFE_DELETE(pTarget);
		return false;
	}

	m_mapRenderTarget.insert(make_pair(strName, pTarget));

	return true;
}

bool CRenderManager::OnDebugRenderTarget(const string & strName, const Vector3 & vPos, const Vector3 & vScale)
{
	CRenderTarget*	pTarget = (CRenderTarget*)FindRenderTarget(strName);

	if (!pTarget)
		return false;

	pTarget->OnDebug(vPos, vScale);

	return true;
}

bool CRenderManager::AddMRT(const string & strMRTName, 
	const string & strTarget)
{
	PMRT	pMrt = FindMRT(strMRTName);

	if (!pMrt)
	{
		pMrt = new MRT;
		m_mapMRT.insert(make_pair(strMRTName, pMrt));
	}

	if (pMrt->vecTarget.size() == 8)
		return false;

	CRenderTarget*	pTarget = FindRenderTarget(strTarget);

	pMrt->vecTarget.push_back(pTarget);

	return true;
}

bool CRenderManager::AddDepth(const string & strMRTName, 
	const string & strTarget)
{
	PMRT	pMrt = FindMRT(strMRTName);

	if (!pMrt)
	{
		pMrt = new MRT;
		m_mapMRT.insert(make_pair(strMRTName, pMrt));
	}

	CRenderTarget*	pTarget = FindRenderTarget(strTarget);

	pMrt->pDepth = pTarget->GetDepthStencilView();

	return true;
}

void CRenderManager::ClearMRT(const string & strMRTName)
{
	PMRT	pMrt = FindMRT(strMRTName);

	if (!pMrt)
		return;

	for (size_t i = 0; i < pMrt->vecTarget.size(); ++i)
	{
		pMrt->vecTarget[i]->ClearTarget();
	}
}

void CRenderManager::SetMRT(const string & strMRTName)
{
	PMRT	pMrt = FindMRT(strMRTName);

	if (!pMrt)
		return;

	if (pMrt->vecOldTarget.size() < pMrt->vecTarget.size())
	{
		pMrt->vecOldTarget.clear();
		pMrt->vecOldTarget.resize(pMrt->vecTarget.size());
	}

	_CONTEXT->OMGetRenderTargets((UINT)pMrt->vecTarget.size(),
		&pMrt->vecOldTarget[0], &pMrt->pOldDepth);

	vector<ID3D11RenderTargetView*>	vecTarget;
	vecTarget.resize(pMrt->vecTarget.size());

	for (size_t i = 0; i < pMrt->vecTarget.size(); ++i)
	{
		vecTarget[i] = pMrt->vecTarget[i]->GetRenderTargetView();
	}

	ID3D11DepthStencilView*	pDepth = pMrt->pDepth;

	if (!pDepth)
		pDepth = pMrt->pOldDepth;

	_CONTEXT->OMSetRenderTargets((UINT)pMrt->vecTarget.size(),
		&vecTarget[0], pDepth);
}

void CRenderManager::ResetMRT(const string & strMRTName)
{
	PMRT	pMrt = FindMRT(strMRTName);

	if (!pMrt)
		return;

	_CONTEXT->OMSetRenderTargets((UINT)pMrt->vecTarget.size(),
		&pMrt->vecOldTarget[0], pMrt->pOldDepth);

	SAFE_RELEASE(pMrt->pOldDepth);

	for (size_t i = 0; i < pMrt->vecTarget.size(); ++i)
	{
		SAFE_RELEASE(pMrt->vecOldTarget[i]);
	}
}

CRenderState * CRenderManager::FindRenderState(const string & strName)
{
	unordered_map<string, CRenderState*>::iterator	iter = m_mapRenderState.find(strName);

	if (iter == m_mapRenderState.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

CRenderTarget * CRenderManager::FindRenderTarget(const string & strName)
{
	unordered_map<string, CRenderTarget*>::iterator	iter = m_mapRenderTarget.find(strName);

	if (iter == m_mapRenderTarget.end())
		return nullptr;

	return iter->second;
}

PMRT CRenderManager::FindMRT(const string & strName)
{
	unordered_map<string, PMRT>::iterator	iter = m_mapMRT.find(strName);

	if (iter == m_mapMRT.end())
		return nullptr;

	return iter->second;
}

void CRenderManager::ComputeInstancing()
{
	for (int i = 0; i < RG_END; ++i)
	{
		m_InstancingList[i].clear();
	}
	unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterIns;
	unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterInsEnd = m_mapInstancingGemoetry.end();
	for (iterIns = m_mapInstancingGemoetry.begin(); iterIns != iterInsEnd; ++iterIns)
	{
		PInstancingGeometry	pGeometry = iterIns->second;
		if (pGeometry->iSize >= INSTANCING_COUNT)
		{
			RENDER_GROUP	rg = pGeometry->pObjList[0]->GetRenderGroup();

			if (pGeometry->bAnimation)
			{
				if (pGeometry->iSize > m_pAnimInstancing->iCount)
				{
					ResizeInstancingBuffer(m_pAnimInstancing,
						pGeometry->iSize);
				}
			}

			else if (pGeometry->bAnimationFrame)
			{
				if (pGeometry->iSize > m_pAnimFrameInstancing->iCount)
				{
					ResizeInstancingBuffer(m_pAnimFrameInstancing,
						pGeometry->iSize);
				}
			}

			else
			{
				if (pGeometry->iSize > m_pStaticInstancing->iCount)
				{
					ResizeInstancingBuffer(m_pStaticInstancing,
						pGeometry->iSize);
				}
			}

			for (int i = 0; i < pGeometry->iSize; ++i)
			{
				pGeometry->pObjList[i]->SetInstancingEnable(true);
			}

			m_InstancingList[rg].push_back(pGeometry);
		}
	}

}

void CRenderManager::Render(float fTime)
{
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Rendering", &m_tRenderCBuffer);

	switch (m_eMode)
	{
	case RM_FORWARD:
		RenderForward(fTime);
		break;
	case RM_DEFERRED:
		RenderDeferred(fTime);
		break;
	}

	m_pDepthDisable->SetState();

	unordered_map<string, CRenderTarget*>::iterator	iter;
	unordered_map<string, CRenderTarget*>::iterator	iterEnd = m_mapRenderTarget.end();

	for (iter = m_mapRenderTarget.begin(); iter != iterEnd; ++iter)
	{
		iter->second->Render();
	}

	m_pDepthDisable->ResetState();

	m_tLightList.iSize = 0;

	unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterIns;
	unordered_map<unsigned __int64, PInstancingGeometry>::iterator	iterInsEnd =
		m_mapInstancingGemoetry.end();

	for (iterIns = m_mapInstancingGemoetry.begin(); iterIns != iterInsEnd; ++iterIns)
	{
		iterIns->second->Clear();
	}
}

void CRenderManager::RenderForward(float fTime)
{
	for (int i = 0; i < RG_END; ++i)
	{
		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjList[j]->Render(fTime);
		}

		if (i == RG_DEFAULT)
		{
			GET_SINGLE(CCollisionManager)->Render(fTime);
		}

		m_tRenderGroup[i].iSize = 0;
	}
}

void CRenderManager::RenderDeferred(float fTime)
{
	// GBuffer를 그려준다.
	RenderGBuffer(fTime);

	// 조명누적버퍼를 만든다.
	RenderLightAcc(fTime);

	// 조명누적버퍼와 Albedo를 합쳐서 최종 조명처리된
	// 타겟을 만든다.
	RenderLightBlend(fTime);

	// 조명이 합성된 최종 타겟을 화면에 그려준다.
	RenderLightBlendRender(fTime);
	
	GET_SINGLE(CCollisionManager)->Render(fTime);

	// Effect에 DepthBuffer를 넘겨준다.
	// 11번 레지스터를 이용한다.
	CRenderTarget*	pDepthTarget = FindRenderTarget("Depth");

	pDepthTarget->SetShader(11);

	for (int j = 0; j < m_tRenderGroup[RG_ALPHA].iSize; ++j)
	{
		m_tRenderGroup[RG_ALPHA].pObjList[j]->Render(fTime);
	}

	pDepthTarget->ResetShader(11);

	for (int i = RG_HUD; i < RG_END; ++i)
	{
		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjList[j]->Render(fTime);
		}
	}

	for (int i = 0; i < RG_END; ++i)
	{
		m_tRenderGroup[i].iSize = 0;
	}
}

void CRenderManager::RenderGBuffer(float fTime)
{
	// GBuffer Target을 Clear한다.
	ClearMRT("GBuffer");

	// MRT를 교체한다.
	SetMRT("GBuffer");

	for (int i = RG_LANDSCAPE; i <= RG_DEFAULT; ++i)
	{
		list<PInstancingGeometry>::iterator	iter;
		list<PInstancingGeometry>::iterator	iterEnd = m_InstancingList[i].end();

		for (iter = m_InstancingList[i].begin(); iter != iterEnd; ++iter)
		{
			CShader*	pShader = nullptr;
			ID3D11InputLayout*	pLayout = nullptr;

			// 인스턴싱 버퍼를 채워준다.
			PInstancingGeometry	pGeometry = *iter;
			for (int j = 0; j < pGeometry->iSize; ++j)
			{
				CGameObject*	pObj = pGeometry->pObjList[j];

				CTransform*	pTransform = pObj->GetTransform();
				CScene*	pScene = pObj->GetScene();
				CCamera*	pCamera = pScene->GetMainCamera();

				if (pGeometry->bAnimation)
				{
				}

				else if (pGeometry->bAnimationFrame)
				{
				}

				else
				{
					Matrix	matWVP, matWV, matWVRot;
					matWV = pTransform->GetLocalMatrix() * pTransform->GetWorldMatrix();
					matWV *= pCamera->GetViewMatrix();
					matWVP = matWV * pCamera->GetProjMatrix();

					matWVRot = pTransform->GetLocalRotMatrix() * pTransform->GetWorldRotationMatrix();
					matWVRot *= pCamera->GetViewMatrix();

					matWV.Transpose();
					matWVP.Transpose();
					matWVRot.Transpose();

					InstancingStaticBuffer	tBuffer;
					tBuffer.matWV = matWV;
					tBuffer.matWVP = matWVP;
					tBuffer.matWVRot = matWVRot;

					AddInstancingData(m_pStaticInstancing,
						j, &tBuffer);

					pShader = m_pStaticInstancingShader;
					pLayout = m_pStaticInstancingLayout;
				}

				SAFE_RELEASE(pCamera);

				SAFE_RELEASE(pTransform);
			}

			PInstancingBuffer	pBuffer = nullptr;

			if (pGeometry->bAnimation)
			{
				pBuffer = m_pAnimInstancing;
			}

			else if (pGeometry->bAnimationFrame)
			{
				pBuffer = m_pAnimFrameInstancing;
			}

			else
			{
				pBuffer = m_pStaticInstancing;
			}

			// 추가된 데이터를 버텍스버퍼에 복사한다.
			CopyInstancingData(pBuffer, pGeometry->iSize);

			// 인스턴싱 물체를 그려낸다.
			pGeometry->pObjList[0]->PrevRender(fTime);

			CRenderer*	pRenderer = pGeometry->pObjList[0]->FindComponentFromType<CRenderer>(CT_RENDERER);

			pRenderer->RenderInstancing(pBuffer, pShader, pLayout,
				pGeometry->iSize, fTime);

			SAFE_RELEASE(pRenderer);
		}
	}

	for (int i = RG_LANDSCAPE; i <= RG_DEFAULT; ++i)
	{
		for (int j = 0; j < m_tRenderGroup[i].iSize; ++j)
		{
			m_tRenderGroup[i].pObjList[j]->Render(fTime);
		}
	}

	// MRT를 원래대로 돌려준다.
	ResetMRT("GBuffer");
}

void CRenderManager::RenderLightAcc(float fTime)
{
	// LightAcc Target을 Clear한다.
	ClearMRT("LightAcc");

	// MRT를 교체한다.
	SetMRT("LightAcc");

	m_pDepthDisable->SetState();
	m_pAccBlend->SetState();

	PMRT	pGBuffer = FindMRT("GBuffer");

	pGBuffer->vecTarget[1]->SetShader(11);
	pGBuffer->vecTarget[2]->SetShader(12);
	pGBuffer->vecTarget[3]->SetShader(13);

	for (int i = 0; i < m_tLightList.iSize; ++i)
	{
		CLight*	pLight = m_tLightList.pObjList[i]->FindComponentFromType<CLight>(CT_LIGHT);

		switch (pLight->GetLightInfo().iType)
		{
		case LT_DIR:
			RenderLightDir(fTime, pLight);
			break;
		case LT_POINT:
			RenderLightPoint(fTime, pLight);
			break;
		case LT_SPOT:
			RenderLightSpot(fTime, pLight);
			break;
		}

		SAFE_RELEASE(pLight);
	}

	pGBuffer->vecTarget[1]->ResetShader(11);
	pGBuffer->vecTarget[2]->ResetShader(12);
	pGBuffer->vecTarget[3]->ResetShader(13);

	m_pDepthDisable->ResetState();
	m_pAccBlend->ResetState();

	// MRT를 원래대로 돌려준다.
	ResetMRT("LightAcc");
}

void CRenderManager::RenderLightDir(float fTime, CLight * pLight)
{
	m_pLightAccDirShader->SetShader();

	// 조명 정보를 상수버퍼에 넘겨준다.
	pLight->SetShader();

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffset = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffset);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);
}

void CRenderManager::RenderLightPoint(float fTime, CLight * pLight)
{
	m_pLightAccPointShader->SetShader();

	pLight->SetShader();

	_CONTEXT->IASetInputLayout(m_pLightPointLayout);

	m_pCullNone->SetState();

	m_pLightPointVolume->Render();

	m_pCullNone->ResetState();

	/*m_pLightAccDirShader->SetShader();

	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffset = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffset);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);*/
}

void CRenderManager::RenderLightSpot(float fTime, CLight * pLight)
{
	m_pLightAccSpotShader->SetShader();

	// 조명 정보를 상수버퍼에 넘겨준다.
	pLight->SetShader();

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffset = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffset);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);
}

void CRenderManager::RenderLightBlend(float fTime)
{
	CRenderTarget*	pLigntBlendTarget = FindRenderTarget("LightBlend");

	pLigntBlendTarget->ClearTarget();
	pLigntBlendTarget->SetTarget();

	m_pLightBlendShader->SetShader();

	m_pDepthDisable->SetState();

	PMRT	pLightAcc = FindMRT("LightAcc");
	CRenderTarget*	pAlbedo = FindRenderTarget("Albedo");
	
	pAlbedo->SetShader(10);
	pLightAcc->vecTarget[0]->SetShader(14);
	pLightAcc->vecTarget[1]->SetShader(15);

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffset = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffset);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);
	
	pAlbedo->ResetShader(10);
	pLightAcc->vecTarget[0]->ResetShader(14);
	pLightAcc->vecTarget[1]->ResetShader(15);

	m_pDepthDisable->ResetState();

	pLigntBlendTarget->ResetTarget();
}

void CRenderManager::RenderLightBlendRender(float fTime)
{
	m_pLightBlendRenderShader->SetShader();

	m_pDepthDisable->SetState();
	m_pAlphaBlend->SetState();

	CRenderTarget*	pLightBlend = FindRenderTarget("LightBlend");

	pLightBlend->SetShader(0);

	// NULL Buffer로 전체 화면크기의 사각형을 출력한다.
	_CONTEXT->IASetInputLayout(nullptr);

	UINT iOffset = 0;
	_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_CONTEXT->IASetVertexBuffers(0, 0, nullptr, 0, &iOffset);
	_CONTEXT->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	_CONTEXT->Draw(4, 0);

	pLightBlend->ResetShader(0);

	m_pAlphaBlend->ResetState();
	m_pDepthDisable->ResetState();
}
