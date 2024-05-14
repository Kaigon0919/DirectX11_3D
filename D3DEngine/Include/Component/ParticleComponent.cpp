#include "ParticleComponent.h"
#include "Transform.h"
#include "../Scene/Scene.h"
#include "../Render/ShaderManager.h"
#include "../GameObject.h"
#include "Material.h"

ENGINE_USING

CParticleComponent::CParticleComponent() : m_pMaterial(nullptr)
{
	m_eComType = CT_PARTICLE;
}

CParticleComponent::CParticleComponent(const CParticleComponent & com) : CComponent(com)
{
	m_pMaterial = nullptr;
}

CParticleComponent::~CParticleComponent()
{
	SAFE_RELEASE(m_pMaterial);
}

bool CParticleComponent::LoadTextureSet(const string & strTexName, const TCHAR * pFileName, const string & strPathName)
{
	if (!m_pMaterial)
		m_pMaterial = m_pObject->AddComponent<CMaterial>("ParticleMaterial");

	if (m_strTextureName.empty())
		m_pMaterial->AddTextureSet(0, 0, 0, strTexName, pFileName, strPathName);

	else
		m_pMaterial->ChangeTextureSet(0, 0, 0, m_strTextureName, strTexName, pFileName, strPathName);

	m_strTextureName = strTexName;

	return true;
}

bool CParticleComponent::LoadTextureSetFromFullPath(
	const string & strTexName, const TCHAR * pFullPath)
{
	if (!m_pMaterial)
		m_pMaterial = m_pObject->AddComponent<CMaterial>("ParticleMaterial");

	if (m_strTextureName.empty())
		m_pMaterial->AddTextureSetFromFullPath(0, 0, 0, strTexName, pFullPath);

	else
		m_pMaterial->ChangeTextureSetFromFullPath(0, 0, 0, m_strTextureName, strTexName, pFullPath);

	m_strTextureName = strTexName;

	return true;
}

bool CParticleComponent::LoadTextureSet(const string & strName, const vector<TCHAR*>& vecFileName, const string & strPathKey)
{
	if (!m_pMaterial)
		m_pMaterial = m_pObject->AddComponent<CMaterial>("ParticleMaterial");

	if (m_strTextureName.empty())
		m_pMaterial->AddTextureSetArray(0, 0, 10, strName, &vecFileName, strPathKey);

	else
		m_pMaterial->ChangeTextureSetArray(0, 0, 10, m_strTextureName, strName, &vecFileName, strPathKey);

	m_strTextureName = strName;

	return false;
}

bool CParticleComponent::LoadTextureSetFromFullPath(const string & strName, const vector<TCHAR*>& vecFullPath)
{
	if (!m_pMaterial)
		m_pMaterial = m_pObject->AddComponent<CMaterial>("ParticleMaterial");

	if (m_strTextureName.empty())
		m_pMaterial->AddTextureSetArrayFromFullPath(0, 0, 10, strName, &vecFullPath);

	else
		m_pMaterial->ChangeTextureSetArrayFromFullPath(0, 0, 10, m_strTextureName, strName, &vecFullPath);

	m_strTextureName = strName;

	return true;
}

void CParticleComponent::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);
}

bool CParticleComponent::Init()
{
	m_pObject->SetRenderGroup(RG_ALPHA);

	return true;
}

int CParticleComponent::Input(float fTime)
{
	return 0;
}

int CParticleComponent::Update(float fTime)
{
	return 0;
}

int CParticleComponent::LateUpdate(float fTime)
{
	m_tCBuffer.vParticlePos = m_pTransform->GetWorldPos();
	m_tCBuffer.fSizeX = m_pTransform->GetWorldScale().x;
	m_tCBuffer.fSizeY = m_pTransform->GetWorldScale().y;
	CTransform* pTr = m_pScene->GetMainCameraTransform();
	m_tCBuffer.vAxisX = pTr->GetWorldAxis(AXIS_X);
	m_tCBuffer.vAxisY = pTr->GetWorldAxis(AXIS_Y);

	SAFE_RELEASE(pTr);

	return 0;
}

int CParticleComponent::Collision(float fTime)
{
	return 0;
}

int CParticleComponent::PrevRender(float fTime)
{
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Particle", &m_tCBuffer);

	return 0;
}

int CParticleComponent::Render(float fTime)
{
	return 0;
}

CParticleComponent * CParticleComponent::Clone() const
{
	return new CParticleComponent(*this);
}
