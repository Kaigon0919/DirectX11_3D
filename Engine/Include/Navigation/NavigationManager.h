#pragma once
#include"../Engine.h"
ENGINE_BEGIN
class ENGINE_DLL CNavigationManager
{
private:
	unordered_map<class CScene*, unordered_map<string, class CNavigationMesh*>>	m_mapNavMesh;

public:
	bool Init();
	class CNavigationMesh* CreateNavMesh(class CScene* pScene, const string& strName);
	class CNavigationMesh* CreateNavMesh(class CScene* pScene, const string& strName, const char* pFileName, const string& strPathName = DATA_PATH);
	class CNavigationMesh* FindNavMesh(class CScene* pScene, const string& strName);
	class CNavigationMesh* FindNavMesh(class CScene* pScene, const Vector3& vPos);
	bool EraseNavMesh(class CScene* pScene, const string& strName);
	bool EraseNavMesh(class CScene* pScene, const Vector3& vPos);
	bool EraseNavMesh(class CScene* pScene);

	DECLARE_SINGLE(CNavigationManager)
};
ENGINE_END