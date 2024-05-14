#include "Renderer.h"
#include "../Render/Shader.h"
#include "../Render/ShaderManager.h"
#include "../Resource/Mesh.h"
#include "../Resource/ResourcesManager.h"
#include "../Device.h"
#include "Transform.h"
#include "Material.h"
#include "../GameObject.h"
#include "../Render/RenderManager.h"
#include "../Render/RenderState.h"
#include "Light.h"
#include "Animation.h"
#include "ColliderSphere.h"

ENGINE_USING

CRenderer::CRenderer()	:
	m_pShader(nullptr),
	m_pMesh(nullptr),
	m_pInputLayout(nullptr),
	m_pMaterial(nullptr)
{
	memset(m_pRenderState, 0, sizeof(CRenderState*) * RS_END);
	m_eComType = CT_RENDERER;
}

CRenderer::CRenderer(const CRenderer & com)	:
	CComponent(com)
{
	*this = com;
	m_iRefCount = 1;

	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();

	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->AddRef();
	}
}

CRenderer::~CRenderer()
{
	for (int i = 0; i < RS_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
}

CMesh * CRenderer::GetMesh() const
{
	if (m_pMesh)
		m_pMesh->AddRef();
	return m_pMesh;
}

void CRenderer::SetMesh(const string & strKey)
{
	SAFE_RELEASE(m_pMesh);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);

	if (m_pMesh)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());

		CColliderSphere*	pPickSphere = FindComponentFromTag<CColliderSphere>("PickSphere");

		pPickSphere->SetSphereInfo(m_pMesh->GetCenter(), m_pMesh->GetRadius());
		pPickSphere->PickEnable();

		SAFE_RELEASE(pPickSphere);
	}

	m_pTransform->SetLocalRelativeView(m_pMesh->GetView());
}

void CRenderer::SetMesh(const string & strKey, const TCHAR * pFileName, const Vector3& vView, const string & strPathKey)
{
	SAFE_RELEASE(m_pMesh);
	GET_SINGLE(CResourcesManager)->LoadMesh(strKey, pFileName, vView, strPathKey);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);

	if (m_pMesh)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());

		CColliderSphere*	pPickSphere = FindComponentFromTag<CColliderSphere>("PickSphere");

		pPickSphere->SetSphereInfo(m_pMesh->GetCenter(), m_pMesh->GetRadius());
		pPickSphere->PickEnable();

		SAFE_RELEASE(pPickSphere);

		CMaterial*	pMaterial = m_pMesh->CloneMaterial();

		if (pMaterial)
		{
			EraseComponentFromType(CT_MATERIAL);
			m_pObject->AddComponent(pMaterial);
			m_pMaterial = pMaterial;
		}

		CAnimation*	pAnimation = m_pMesh->CloneAnimation();

		if (pAnimation)
		{
			EraseComponentFromType(CT_ANIMATION);
			m_pObject->AddComponent(pAnimation);
			SAFE_RELEASE(pAnimation);
		}
	}

	m_pTransform->SetLocalRelativeView(m_pMesh->GetView());
}

void CRenderer::SetMeshFromFullPath(const string & strKey, const TCHAR * pFileName, const Vector3 & vView)
{
	SAFE_RELEASE(m_pMesh);
	GET_SINGLE(CResourcesManager)->LoadMeshFromFullPath(strKey, pFileName, vView);
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh(strKey);

	if (m_pMesh)
	{
		SetShader(m_pMesh->GetShaderKey());
		SetInputLayout(m_pMesh->GetInputLayoutKey());

		CColliderSphere*	pPickSphere = FindComponentFromTag<CColliderSphere>("PickSphere");

		pPickSphere->SetSphereInfo(m_pMesh->GetCenter(), m_pMesh->GetRadius());
		pPickSphere->PickEnable();

		SAFE_RELEASE(pPickSphere);

		CMaterial*	pMaterial = m_pMesh->CloneMaterial();

		if (pMaterial)
		{
			EraseComponentFromType(CT_MATERIAL);
			m_pObject->AddComponent(pMaterial);
			m_pMaterial = pMaterial;
		}

		CAnimation*	pAnimation = m_pMesh->CloneAnimation();

		if (pAnimation)
		{
			EraseComponentFromType(CT_ANIMATION);
			m_pObject->AddComponent(pAnimation);
			SAFE_RELEASE(pAnimation);
		}
	}

	m_pTransform->SetLocalRelativeView(m_pMesh->GetView());
}

void CRenderer::SetShader(const string & strKey)
{
	SAFE_RELEASE(m_pShader);
	m_pShader = GET_SINGLE(CShaderManager)->FindShader(strKey);
}

void CRenderer::SetInputLayout(const string & strKey)
{
	m_pInputLayout = GET_SINGLE(CShaderManager)->FindInputLayout(strKey);
}

void CRenderer::SetRenderState(const string & strName)
{
	CRenderState*	pState = GET_SINGLE(CRenderManager)->FindRenderState(strName);

	if (!pState)
		return;

	RENDER_STATE	eState = pState->GetRenderStateType();

	SAFE_RELEASE(m_pRenderState[eState]);
	m_pRenderState[eState] = pState;
}

void CRenderer::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);
}

bool CRenderer::Init()
{
	CColliderSphere*	pPickSphere = AddComponent<CColliderSphere>("PickSphere");
	pPickSphere->PickEnable();
	SAFE_RELEASE(pPickSphere);

	return true;
}

int CRenderer::Input(float fTime)
{
	return 0;
}

int CRenderer::Update(float fTime)
{
	return 0;
}

int CRenderer::LateUpdate(float fTime)
{
	return 0;
}

int CRenderer::Collision(float fTime)
{
	return 0;
}

int CRenderer::PrevRender(float fTime)
{
	CLight*	pLight = GET_SINGLE(CRenderManager)->GetFirstLight();

	if (pLight)
	{
		pLight->SetShader();
		SAFE_RELEASE(pLight);
	}

	return 0;
}

int CRenderer::Render(float fTime)
{
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}

	_CONTEXT->IASetInputLayout(m_pInputLayout);

	m_pShader->SetShader();

	size_t	iContainer = m_pMesh->GetContainerCount();

	for (int i = 0; i < iContainer; ++i)
	{
		size_t iSubset = m_pMesh->GetSubsetCount(i);

		if (iSubset > 0)
		{
			for (int j = 0; j < iSubset; ++j)
			{
				if (m_pMaterial)
					m_pMaterial->SetShader(i, j);

				m_pMesh->Render(i, j);
			}
		}

		else
		{
			if (m_pMaterial)
				m_pMaterial->SetShader(i, 0);

			m_pMesh->Render(i, 0);
		}
	}


	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}


	return 0;
}

CRenderer * CRenderer::Clone() const
{
	return new CRenderer(*this);
}

void CRenderer::RenderInstancing(PInstancingBuffer pBuffer, CShader * pShader, ID3D11InputLayout * pLayout, int iInstancingCount, float fTime)
{
	pShader->SetShader();

	_CONTEXT->IASetInputLayout(pLayout);

	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}


	size_t	iContainer = m_pMesh->GetContainerCount();

	for (int i = 0; i < iContainer; ++i)
	{
		size_t iSubset = m_pMesh->GetSubsetCount(i);

		if (iSubset > 0)
		{
			for (int j = 0; j < iSubset; ++j)
			{
				if (m_pMaterial)
					m_pMaterial->SetShader(i, j);

				m_pMesh->RenderInstancing(i, j, pBuffer, iInstancingCount);
			}
		}

		else
		{
			if (m_pMaterial)
				m_pMaterial->SetShader(i, 0);

			m_pMesh->RenderInstancing(i, 0, pBuffer, iInstancingCount);
		}
	}


	for (int i = 0; i < RS_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}
}
