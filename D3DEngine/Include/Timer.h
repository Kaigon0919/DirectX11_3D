#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CTimer
{
	friend class CTimerManager;

private:
	CTimer();
	~CTimer();

private:
	LARGE_INTEGER	m_tSecond;
	LARGE_INTEGER	m_tTick;
	float			m_fDeltaTime;
	float			m_fFPS;
	float			m_fFPSTime;
	int				m_iTick;

public:
	float GetTime()	const;
	float GetFPS()	const;

public:
	bool Init();
	void Update();
};

ENGINE_END
