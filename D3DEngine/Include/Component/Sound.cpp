#include "Sound.h"

ENGINE_USING

CSound::CSound() :
	m_pSoundInfo(nullptr),
	m_bBGM(false),
	m_bLoop(false),
	m_bPlay(false)
{
	m_eComType = CT_SOUND;
}

CSound::CSound(const CSound & com) :
	CComponent(com)
{
	*this = com;
	m_iRefCount = 1;
}

CSound::~CSound()
{
}

void CSound::SetSound(const string & strName)
{
	m_pSoundInfo = GET_SINGLE(CSoundManager)->FindSound(strName);
}

void CSound::SetSound(const string & strName, CScene * pScene,
	bool bLoop, const char * pFileName, const string & strPathName)
{
	if (!GET_SINGLE(CSoundManager)->LoadSound(strName, pScene,
		bLoop, pFileName, strPathName))
		return;

	m_pSoundInfo = GET_SINGLE(CSoundManager)->FindSound(strName);
}

void CSound::EnableBGM()
{
	m_bBGM = true;
}

void CSound::Play()
{
	if (m_bPlay)
	{
		m_pChannel->stop();

		list<Channel*>::iterator	iter;
		list<Channel*>::iterator	iterEnd = m_pSoundInfo->ChannelList.end();

		for (iter = m_pSoundInfo->ChannelList.begin(); iter != iterEnd; ++iter)
		{
			if (*iter == m_pChannel)
			{
				m_pSoundInfo->ChannelList.erase(iter);
				break;
			}
		}

		m_pChannel = nullptr;
	}

	m_bPlay = true;
	GET_SINGLE(CSoundManager)->Play(m_pSoundInfo);

	m_pChannel = m_pSoundInfo->ChannelList.back();
}

void CSound::Stop()
{
	if (m_bPlay)
	{
		m_pChannel->stop();

		list<Channel*>::iterator	iter;
		list<Channel*>::iterator	iterEnd = m_pSoundInfo->ChannelList.end();

		for (iter = m_pSoundInfo->ChannelList.begin(); iter != iterEnd; ++iter)
		{
			if (*iter == m_pChannel)
			{
				m_pSoundInfo->ChannelList.erase(iter);
				break;
			}
		}
	}
}

void CSound::Pause()
{
}

void CSound::Start()
{
	if (m_bPlay)
	{
		if (m_pChannel)
		{
			m_pChannel->stop();

			list<Channel*>::iterator	iter;
			list<Channel*>::iterator	iterEnd = m_pSoundInfo->ChannelList.end();

			for (iter = m_pSoundInfo->ChannelList.begin(); iter != iterEnd; ++iter)
			{
				if (*iter == m_pChannel)
				{
					m_pSoundInfo->ChannelList.erase(iter);
					break;
				}
			}

			m_pChannel = nullptr;
		}

		GET_SINGLE(CSoundManager)->Play(m_pSoundInfo);

		m_pChannel = m_pSoundInfo->ChannelList.back();
	}
}

bool CSound::Init()
{
	return true;
}

int CSound::Input(float fTime)
{
	return 0;
}

int CSound::Update(float fTime)
{
	if (m_bPlay)
	{
		bool	bIsPlay = false;
		m_pChannel->isPlaying(&bIsPlay);

		if (!bIsPlay)
		{
			m_bPlay = false;
			m_pChannel = nullptr;
		}
	}

	return 0;
}

int CSound::LateUpdate(float fTime)
{
	return 0;
}

int CSound::Collision(float fTime)
{
	return 0;
}

int CSound::PrevRender(float fTime)
{
	return 0;
}

int CSound::Render(float fTime)
{
	return 0;
}

CSound * CSound::Clone() const
{
	return new CSound(*this);
}