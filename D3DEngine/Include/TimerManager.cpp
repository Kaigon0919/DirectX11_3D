#include "TimerManager.h"
#include "Timer.h"

ENGINE_USING

DEFINITION_SINGLE(CTimerManager)

CTimerManager::CTimerManager()
{
}

CTimerManager::~CTimerManager()
{
	unordered_map<string, CTimer*>::iterator	iter;
	unordered_map<string, CTimer*>::iterator	iterEnd = m_mapTimer.end();

	for (iter = m_mapTimer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_mapTimer.clear();
}

bool CTimerManager::Init()
{
	CreateTimer("MainThread");

	return true;
}

bool CTimerManager::CreateTimer(const string & strName)
{
	CTimer*	pTimer = FindTimer(strName);

	if (pTimer)
		return false;

	pTimer = new CTimer;

	if (!pTimer->Init())
	{
		SAFE_DELETE(pTimer);
		return false;
	}

	m_mapTimer.insert(make_pair(strName, pTimer));

	return true;
}

CTimer * CTimerManager::FindTimer(const string & strName)
{
	unordered_map<string, CTimer*>::iterator	iter = m_mapTimer.find(strName);

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}
