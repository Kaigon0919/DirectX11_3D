#include "NavigationManager.h"
#include "NavigationMesh.h"
#include "../Scene/Scene.h"

ENGINE_USING

DEFINITION_SINGLE(CNavigationManager)

CNavigationManager::CNavigationManager()
{
}

CNavigationManager::~CNavigationManager()
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iterEnd = m_mapNavMesh.end();

	for (iter = m_mapNavMesh.begin(); iter != iterEnd; ++iter)
	{
		Safe_Release_Map(iter->second);
		iter->second.clear();
	}

	m_mapNavMesh.clear();
}

bool CNavigationManager::Init()
{
	return true;
}

CNavigationMesh * CNavigationManager::CreateNavMesh(CScene * pScene, const string & strName)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	unordered_map<string, class CNavigationMesh*>*	pMap;

	if (iter == m_mapNavMesh.end())
	{
		unordered_map<string, class CNavigationMesh*>	map;
		m_mapNavMesh.insert(make_pair(pScene, map));

		pMap = &m_mapNavMesh.begin()->second;
	}

	else
		pMap = &iter->second;

	CNavigationMesh*	pMesh = new CNavigationMesh;

	pMesh->SetTag(strName);
	pMesh->Init();

	pMap->insert(make_pair(strName, pMesh));

	return pMesh;
}

CNavigationMesh * CNavigationManager::CreateNavMesh(CScene * pScene, const string & strName, const char * pFileName, const string & strPathName)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	unordered_map<string, class CNavigationMesh*>*	pMap;

	if (iter == m_mapNavMesh.end())
	{
		unordered_map<string, class CNavigationMesh*>	map;
		m_mapNavMesh.insert(make_pair(pScene, map));

		pMap = &m_mapNavMesh.begin()->second;
	}

	else
		pMap = &iter->second;

	CNavigationMesh*	pMesh = new CNavigationMesh;

	pMesh->SetTag(strName);
	pMesh->Init();
	pMesh->Load(pFileName, strPathName);

	pMap->insert(make_pair(strName, pMesh));

	return pMesh;
}

CNavigationMesh * CNavigationManager::FindNavMesh(CScene * pScene, const string & strName)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	if (iter == m_mapNavMesh.end())
		return nullptr;

	unordered_map<string, class CNavigationMesh*>::iterator	iter1 =
		iter->second.find(strName);

	if (iter1 == iter->second.end())
		return nullptr;

	iter1->second->AddRef();

	return iter1->second;
}

CNavigationMesh * CNavigationManager::FindNavMesh(CScene * pScene, const Vector3 & vPos)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	if (iter == m_mapNavMesh.end())
		return nullptr;

	unordered_map<string, class CNavigationMesh*>::iterator	iter1;
	unordered_map<string, class CNavigationMesh*>::iterator	iter1End = iter->second.end();

	for (iter1 = iter->second.begin(); iter1 != iter1End; ++iter1)
	{
		CNavigationMesh*	pNavMesh = iter1->second;

		Vector3	vMin = pNavMesh->GetMin();
		Vector3	vMax = pNavMesh->GetMax();

		if (vMin.x <= vPos.x && vPos.x <= vMax.x &&
			vMin.y <= vPos.y && vPos.y <= vMax.y &&
			vMin.z <= vPos.z && vPos.z <= vMax.z)
		{
			pNavMesh->AddRef();
			return pNavMesh;
		}
	}

	return nullptr;
}

bool CNavigationManager::EraseNavMesh(CScene * pScene, const string & strName)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	if (iter == m_mapNavMesh.end())
		return false;

	unordered_map<string, class CNavigationMesh*>::iterator	iter1 =
		iter->second.find(strName);

	if (iter1 == iter->second.end())
		return false;

	SAFE_RELEASE(iter1->second);
	iter->second.erase(iter1);

	return true;
}

bool CNavigationManager::EraseNavMesh(CScene * pScene, const Vector3 & vPos)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	if (iter == m_mapNavMesh.end())
		return false;

	unordered_map<string, class CNavigationMesh*>::iterator	iter1;
	unordered_map<string, class CNavigationMesh*>::iterator	iter1End = iter->second.end();

	for (iter1 = iter->second.begin(); iter1 != iter1End; ++iter1)
	{
		CNavigationMesh*	pNavMesh = iter1->second;

		Vector3	vMin = pNavMesh->GetMin();
		Vector3	vMax = pNavMesh->GetMax();

		if (vMin.x <= vPos.x && vPos.x <= vMax.x &&
			vMin.y <= vPos.y && vPos.y <= vMax.y &&
			vMin.z <= vPos.z && vPos.z <= vMax.z)
		{
			SAFE_RELEASE(iter1->second);
			iter->second.erase(iter1);
			return true;
		}
	}

	return false;
}

bool CNavigationManager::EraseNavMesh(CScene * pScene)
{
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>::iterator	iter;
	iter = m_mapNavMesh.find(pScene);

	if (iter == m_mapNavMesh.end())
		return false;

	Safe_Release_Map(iter->second);
	m_mapNavMesh.erase(iter);

	return true;
}
