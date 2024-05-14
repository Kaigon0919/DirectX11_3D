#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CTimerManager
{
private:
	unordered_map<string, class CTimer*>	m_mapTimer;

public:
	bool Init();
	bool CreateTimer(const string& strName);
	class CTimer* FindTimer(const string& strName);

	DECLARE_SINGLE(CTimerManager)
};

ENGINE_END
