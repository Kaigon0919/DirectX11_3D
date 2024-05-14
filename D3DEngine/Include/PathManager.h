#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CPathManager
{
private:
	unordered_map<string, TCHAR*>	m_mapPath;
	char	m_strChange[MAX_PATH];

public:
	bool Init();
	bool CreatePath(const string& strKey, const TCHAR* pAddPath,
		const string& strBaseKey = ROOT_PATH);
	const TCHAR* FindPath(const string& strKey);
	const char* FindPathMultibyte(const string& strKey);

	DECLARE_SINGLE(CPathManager)
};

ENGINE_END
