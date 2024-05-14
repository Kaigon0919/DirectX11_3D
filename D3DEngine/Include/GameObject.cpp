#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Scene/Layer.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "CollisionManager.h"

ENGINE_USING

unordered_map<class CScene*, unordered_map<string, CGameObject*>> CGameObject::m_mapPrototype;

CGameObject::CGameObject() :
	m_pScene(nullptr),
	m_pLayer(nullptr),
	m_pTransform(nullptr),
	m_eRenderGroup(RG_DEFAULT),
	m_fLifeTime(-1.f),
	m_bInstancing(false),
	m_bFrustum(false)
{
	m_pTransform = AddComponent<CTransform>("Transform");
}

CGameObject::CGameObject(const CGameObject & obj)
{
	*this = obj;

	m_iRefCount = 1;
	m_pScene = nullptr;
	m_pLayer = nullptr;
	m_bInstancing = false;
	m_bFrustum = false;

	m_ComList.clear();
	m_ColliderList.clear();
	m_StartList.clear();

	list<CComponent*>::const_iterator	iter;
	list<CComponent*>::const_iterator	iterEnd = obj.m_ComList.end();

	for (iter = obj.m_ComList.begin(); iter != iterEnd; ++iter)
	{
		CComponent*	pCom = (*iter)->Clone();

		if (pCom->GetComponentType() == CT_TRANSFORM)
		{
			pCom->AddRef();
			m_pTransform = (CTransform*)pCom;
		}

		else if (pCom->GetComponentType() == CT_COLLIDER)
			m_ColliderList.push_back((CCollider*)pCom);

		pCom->SetGameObject(this);
		pCom->m_pTransform = m_pTransform;
		m_ComList.push_back(pCom);
		pCom->AddRef();
		m_StartList.push_back(pCom);
	}
}

CGameObject::~CGameObject()
{
	SAFE_RELEASE(m_pTransform);
	Safe_Release_VecList(m_StartList);
	Safe_Release_VecList(m_ComList);
}

CGameObject * CGameObject::CreateObject(const string & strTag,
	CLayer* pLayer)
{
	CGameObject*	pObj = new CGameObject;

	if (!pObj->Init())
	{
		SAFE_RELEASE(pObj);
		return nullptr;
	}

	pObj->SetTag(strTag.c_str());

	if (pLayer)
		pLayer->AddObject(pObj);

	return pObj;
}

CGameObject * CGameObject::CreatePrototype(const string & strTag, 
	CScene * pScene)
{
	CGameObject*	pObj = new CGameObject;

	if (!pObj->Init())
	{
		SAFE_RELEASE(pObj);
		return nullptr;
	}

	pObj->SetTag(strTag.c_str());

	if (!pScene)
		pScene = GET_SINGLE(CSceneManager)->GetScene();

	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pScene);

	if (iter == m_mapPrototype.end())
	{
		unordered_map<string, CGameObject*>	map;
		m_mapPrototype.insert(make_pair(pScene, map));
		iter = m_mapPrototype.find(pScene);
	}

	pObj->m_pScene = pScene;

	pObj->AddRef();
	iter->second.insert(make_pair(strTag, pObj));

	return pObj;
}

CGameObject * CGameObject::CreateClone(const string & strPrototype, 
	CScene * pScene, CLayer * pLayer)
{
	CGameObject*	pPrototype = FindPrototype(strPrototype, pScene);

	if (!pPrototype)
		return nullptr;

	CGameObject*	pObj = pPrototype->Clone();

	if (pLayer)
		pLayer->AddObject(pObj);

	return pObj;
}

void CGameObject::RemovePrototype(CScene * pScene)
{
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pScene);

	if (iter == m_mapPrototype.end())
		return;

	unordered_map<string, CGameObject*>::iterator	iter1;
	unordered_map<string, CGameObject*>::iterator	iter1End = iter->second.end();

	for (iter1 = iter->second.begin(); iter1 != iter1End; ++iter1)
	{
		SAFE_RELEASE(iter1->second);
	}

	iter->second.clear();

	m_mapPrototype.erase(iter);
}

void CGameObject::RemovePrototype(CScene * pScene, const string & strPrototype)
{
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pScene);

	if (iter == m_mapPrototype.end())
		return;

	unordered_map<string, CGameObject*>::iterator	iter1 = iter->second.find(strPrototype);

	if (iter1 == iter->second.end())
		return;

	SAFE_RELEASE(iter1->second);

	iter->second.erase(iter1);
}

CGameObject * CGameObject::FindPrototype(const string & strName,
	CScene* pScene)
{
	unordered_map<CScene*, unordered_map<string, CGameObject*>>::iterator	iter = m_mapPrototype.find(pScene);

	if (iter == m_mapPrototype.end())
		return nullptr;

	unordered_map<string, CGameObject*>::iterator	iterProto = iter->second.find(strName);

	if (iterProto == iter->second.end())
		return nullptr;

	return iterProto->second;
}

CScene * CGameObject::GetScene() const
{
	return m_pScene;
}

CLayer * CGameObject::GetLayer() const
{
	return m_pLayer;
}

CTransform * CGameObject::GetTransform() const
{
	m_pTransform->AddRef();
	return m_pTransform;
}

RENDER_GROUP CGameObject::GetRenderGroup() const
{
	return m_eRenderGroup;
}

float CGameObject::GetLifeTime() const
{
	return m_fLifeTime;
}
bool CGameObject::IsInstancing() const
{
	return m_bInstancing;
}

bool CGameObject::IsFrustumCulling() const
{
	return m_bFrustum;
}

void CGameObject::SetScene(CScene * pScene)
{
	m_pScene = pScene;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->m_pScene = pScene;
	}
}

void CGameObject::SetLayer(CLayer * pLayer)
{
	m_pLayer = pLayer;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->m_pLayer = pLayer;
	}
}

void CGameObject::SetRenderGroup(RENDER_GROUP eGroup)
{
	m_eRenderGroup = eGroup;
}

void CGameObject::SetLifeTime(float fTime)
{
	m_fLifeTime = fTime;
}

void CGameObject::SetInstancingEnable(bool bEnable)
{
	m_bInstancing = bEnable;
}

void CGameObject::SetFrustumCulling(bool bCulling)
{
	m_bFrustum = bCulling;
}

const list<class CCollider*>* CGameObject::GetColliderList() const
{
	return &m_ColliderList;
}

void CGameObject::Start()
{
	if (m_StartList.empty())
		return;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_StartList.end();

	for (iter = m_StartList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start();
	}

	Safe_Release_VecList(m_StartList);

	//GET_SINGLE(CCollisionManager)->AddCollider(this);
}

bool CGameObject::Init()
{
	return true;
}

int CGameObject::Input(float fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Input(fTime);
		++iter;
	}

	return 0;
}

int CGameObject::Update(float fTime)
{
	Start();

	m_bInstancing = false;
	m_bFrustum = false;

	if (m_fLifeTime != -1.f)
	{
		m_fLifeTime -= fTime;

		if (m_fLifeTime <= 0.f)
		{
			Active(false);
			return 0;
		}
	}

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		if (m_pTransform == *iter && m_pTransform->GetUpdate())
		{
			(*iter)->Update(fTime);
		}

		else
			(*iter)->Update(fTime);
		++iter;
	}

	m_pTransform->RotationLookAt();

	return 0;
}

int CGameObject::LateUpdate(float fTime)
{
	Start();

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		if (m_pTransform == *iter && m_pTransform->GetUpdate())
		{
			(*iter)->LateUpdate(fTime);
		}

		else
			(*iter)->LateUpdate(fTime);
		++iter;
	}

	return 0;
}

int CGameObject::Collision(float fTime)
{
	Start();

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Collision(fTime);
		++iter;
	}

	return 0;
}

int CGameObject::PrevRender(float fTime)
{
	Start();

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		else if ((*iter)->GetComponentType() == CT_COLLIDER)
		{
			++iter;
			continue;
		}

		(*iter)->PrevRender(fTime);
		++iter;
	}

	return 0;
}

int CGameObject::Render(float fTime)
{
	if (m_bInstancing)
		return 0;

	PrevRender(fTime);

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		else if ((*iter)->GetComponentType() == CT_COLLIDER)
		{
			++iter;
			continue;
		}

		(*iter)->Render(fTime);
		++iter;
	}
	return 0;
}

CGameObject * CGameObject::Clone() const
{
	return new CGameObject(*this);
}

bool CGameObject::CheckComponentFromTag(const string & strTag)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			return true;
		}
	}

	return false;
}

bool CGameObject::CheckComponentFromType(COMPONENT_TYPE eType)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponentType() == eType)
		{
			return true;
		}
	}

	return false;
}

CComponent* CGameObject::AddComponent(CComponent * pCom)
{
	pCom->SetScene(m_pScene);
	pCom->SetLayer(m_pLayer);
	pCom->SetGameObject(this);
	pCom->m_pTransform = m_pTransform;
	pCom->AddRef();
	pCom->AddRef();

	if (pCom->GetComponentType() == CT_COLLIDER)
		m_ColliderList.push_back((CCollider*)pCom);

	m_StartList.push_back(pCom);
	m_ComList.push_back(pCom);

	return pCom;
}

void CGameObject::EraseComponentFromTag(const string & strTag)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
		{
			SAFE_RELEASE((*iter));
			m_ComList.erase(iter);
			return;
		}
	}
}

void CGameObject::EraseComponentFromType(COMPONENT_TYPE eType)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponentType() == eType)
		{
			SAFE_RELEASE((*iter));
			m_ComList.erase(iter);
			return;
		}
	}
}

void CGameObject::EraseComponent(CComponent * pCom)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComList.end();

	for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == pCom)
		{
			SAFE_RELEASE((*iter));
			m_ComList.erase(iter);
			return;
		}
	}
}
