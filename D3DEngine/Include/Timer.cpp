#include "Timer.h"
#include "Core.h"

ENGINE_USING

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}

float CTimer::GetTime() const
{
	return m_fDeltaTime;
}

float CTimer::GetFPS() const
{
	return m_fFPS;
}

bool CTimer::Init()
{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tTick);

	m_fDeltaTime = 0.f;
	m_fFPS = 0.f;
	m_fFPSTime = 0.f;
	m_iTick = 0;

	return true;
}

void CTimer::Update()
{
	LARGE_INTEGER	tTick;

	QueryPerformanceCounter(&tTick);

	m_fDeltaTime = (tTick.QuadPart - m_tTick.QuadPart) / (float)m_tSecond.QuadPart;

	m_tTick = tTick;

	m_fFPSTime += m_fDeltaTime;
	++m_iTick;

	if (m_iTick == 60)
	{
		m_fFPS = m_iTick / m_fFPSTime;
		m_fFPSTime = 0.f;
		m_iTick = 0;

		char	strFPS[256] = {};

		sprintf_s(strFPS, "FPS : %.5f", m_fFPS);
		SetWindowTextA(WINDOWHANDLE, strFPS);
	}
}
