#include "PathManager.h"

ENGINE_USING

DEFINITION_SINGLE(CPathManager)

CPathManager::CPathManager()
{
}

CPathManager::~CPathManager()
{
	unordered_map<string, TCHAR*>::iterator	iter;
	unordered_map<string, TCHAR*>::iterator	iterEnd = m_mapPath.end();

	for (iter = m_mapPath.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second);
	}

	m_mapPath.clear();
}

bool CPathManager::Init()
{
	TCHAR*	pPath = new TCHAR[MAX_PATH];

	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	GetModuleFileName(nullptr, pPath, MAX_PATH);

	size_t iLength = lstrlen(pPath) - 1;

	for (size_t i = iLength; i >= 0; --i)
	{
		if (pPath[i] == '\\' || pPath[i] == '/')
		{
			memset(&pPath[i + 1], 0, sizeof(TCHAR) * (iLength - i));
			break;
		}
	}

	m_mapPath.insert(make_pair(ROOT_PATH, pPath));

	CreatePath(SHADER_PATH, TEXT("Shader\\"));
	CreatePath(TEXTURE_PATH, TEXT("Texture\\"));
	CreatePath(MESH_PATH, TEXT("Mesh\\"));
	CreatePath(DATA_PATH, TEXT("Data\\"));

	return true;
}

bool CPathManager::CreatePath(const string & strKey, const TCHAR * pAddPath,
	const string & strBaseKey)
{
	if (FindPath(strKey))
		return false;

	TCHAR*	pPath = new TCHAR[MAX_PATH];

	memset(pPath, 0, sizeof(TCHAR) * MAX_PATH);

	// BaseKey를 이용해서 기본 패스를 얻어온다.
	const TCHAR* pBasePath = FindPath(strBaseKey);

	if (pBasePath)
		lstrcpy(pPath, pBasePath);

	lstrcat(pPath, pAddPath);

	m_mapPath.insert(make_pair(strKey, pPath));

	return true;
}

const TCHAR * CPathManager::FindPath(const string & strKey)
{
	unordered_map<string, TCHAR*>::iterator	iter = m_mapPath.find(strKey);

	if (iter == m_mapPath.end())
		return nullptr;

	return iter->second;
}

const char * CPathManager::FindPathMultibyte(const string & strKey)
{
	unordered_map<string, TCHAR*>::iterator	iter = m_mapPath.find(strKey);

	if (iter == m_mapPath.end())
		return nullptr;

#ifdef UNICODE
	memset(m_strChange, 0, MAX_PATH);
	WideCharToMultiByte(CP_UTF8, 0, iter->second, -1,
		m_strChange, lstrlen(iter->second), nullptr, nullptr);
	return m_strChange;
#else
	return iter->second;
#endif // UNICODE
}
