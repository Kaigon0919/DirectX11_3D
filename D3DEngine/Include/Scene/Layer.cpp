#include "Layer.h"
#include "Scene.h"
#include "../GameObject.h"
#include "../Render/RenderManager.h"

ENGINE_USING

CLayer::CLayer()	:
	m_iZOrder(0)
{
}

CLayer::~CLayer()
{
	Safe_Release_VecList(m_StartList);
	Safe_Release_VecList(m_ObjList);
}

int CLayer::GetZOrder() const
{
	return m_iZOrder;
}

void CLayer::SetZOrder(int iZOrder)
{
	m_iZOrder = iZOrder;

	m_pScene->SortLayer();
}

void CLayer::Start()
{
	if (m_StartList.empty())
		return;

	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_StartList.end();

	for (iter = m_StartList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Start();
	}

	Safe_Release_VecList(m_StartList);
}

bool CLayer::Init()
{
	return true;
}

int CLayer::Input(float fTime)
{
	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
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

int CLayer::Update(float fTime)
{
	Start();

	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(fTime);
		++iter;
	}

	return 0;
}

int CLayer::LateUpdate(float fTime)
{
	Start();

	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->LateUpdate(fTime);
		++iter;
	}

	return 0;
}

int CLayer::Collision(float fTime)
{
	Start();

	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
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

int CLayer::Render(float fTime)
{
	Start();

	list<CGameObject*>::iterator	iter;
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		GET_SINGLE(CRenderManager)->AddRenderObject(*iter);
		++iter;
	}

	/*for (iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Render(fTime);
		++iter;
	}*/

	return 0;
}

void CLayer::AddObject(CGameObject * pObj)
{
	pObj->SetScene(m_pScene);
	pObj->SetLayer(this);
	pObj->AddRef();
	pObj->AddRef();

	m_StartList.push_back(pObj);
	m_ObjList.push_back(pObj);
}
