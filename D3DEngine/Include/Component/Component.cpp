#include "Component.h"
#include "../GameObject.h"
#include "Transform.h"

ENGINE_USING

CComponent::CComponent()	:
	m_pTransform(nullptr)
{
}

CComponent::CComponent(const CComponent& com)
{
	*this = com;
	m_iRefCount = 1;
}

CComponent::~CComponent()
{
}

CScene * CComponent::GetScene() const
{
	return m_pScene;
}

CLayer * CComponent::GetLayer() const
{
	return m_pLayer;
}

CGameObject * CComponent::GetGameObject() const
{
	m_pObject->AddRef();
	return m_pObject;
}

CTransform * CComponent::GetTransform() const
{
	m_pTransform->AddRef();
	return m_pTransform;
}

bool CComponent::IsObjectEnable() const
{
	return m_pObject->IsEnable();
}

bool CComponent::IsObjectActive() const
{
	return m_pObject->IsActive();
}

void CComponent::SetScene(CScene * pScene)
{
	m_pScene = pScene;
}

void CComponent::SetLayer(CLayer * pLayer)
{
	m_pLayer = pLayer;
}

void CComponent::SetGameObject(CGameObject * pObj)
{
	m_pObject = pObj;
}

COMPONENT_TYPE CComponent::GetComponentType() const
{
	return m_eComType;
}

void CComponent::Start()
{
}

bool CComponent::Init()
{
	return true;
}

int CComponent::Input(float fTime)
{
	return 0;
}

int CComponent::Update(float fTime)
{
	return 0;
}

int CComponent::LateUpdate(float fTime)
{
	return 0;
}

int CComponent::Collision(float fTime)
{
	return 0;
}

int CComponent::PrevRender(float fTime)
{
	return 0;
}

int CComponent::Render(float fTime)
{
	return 0;
}

CComponent * CComponent::Clone() const
{
	return nullptr;
}

void CComponent::EraseComponentFromTag(const string & strTag)
{
	m_pObject->EraseComponentFromTag(strTag);
}

void CComponent::EraseComponentFromType(COMPONENT_TYPE eType)
{
	m_pObject->EraseComponentFromType(eType);
}

void CComponent::EraseComponent(CComponent * pCom)
{
	m_pObject->EraseComponent(pCom);
}
