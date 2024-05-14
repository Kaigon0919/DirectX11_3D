#include "SoundManager.h"
#include "PathManager.h"

ENGINE_USING

DEFINITION_SINGLE(CSoundManager)

CSoundManager::CSoundManager() :
	m_pSystem(nullptr)
{
}

CSoundManager::~CSoundManager()
{
	unordered_map<string, PSoundInfo>::iterator	iter;
	unordered_map<string, PSoundInfo>::iterator	iterEnd = m_mapSound.end();

	for (iter = m_mapSound.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pSound->release();
		SAFE_DELETE(iter->second);
	}

	m_mapSound.clear();

	if (m_pSystem)
	{
		m_pSystem->close();
		m_pSystem->release();
	}
}

System * CSoundManager::GetSoundSystem() const
{
	return m_pSystem;
}

bool CSoundManager::Init()
{
	// System »ý¼º
	System_Create(&m_pSystem);

	m_pSystem->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, nullptr);

	return true;
}

bool CSoundManager::LoadSound(const string & strName,
	CScene * pScene, bool bLoop, const char * pFileName,
	const string & strPathName)
{
	PSoundInfo	pInfo = FindSound(strName);

	if (pInfo)
		return false;

	pInfo = new SoundInfo;

	pInfo->bLoop = bLoop;

	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathName);

	string	strPath;
	if (pPath)
		strPath = pPath;

	strPath += pFileName;

	FMOD_MODE	eMode = FMOD_LOOP_NORMAL;

	if (!bLoop)
		eMode = FMOD_DEFAULT;

	if (m_pSystem->createSound(strPath.c_str(), eMode, nullptr,
		&pInfo->pSound) != FMOD_OK)
	{
		SAFE_DELETE(pInfo);
		return false;
	}

	pInfo->pScene = pScene;

	m_mapSound.insert(make_pair(strName, pInfo));

	return true;
}

void CSoundManager::Play(const string & strName)
{
	PSoundInfo	pSoundInfo = FindSound(strName);

	if (!pSoundInfo)
		return;

	Channel*	pChannel = nullptr;
	m_pSystem->playSound(pSoundInfo->pSound, nullptr,
		false, &pChannel);

	pSoundInfo->ChannelList.push_back(pChannel);
}

void CSoundManager::Play(PSoundInfo pSound)
{
	Channel*	pChannel = nullptr;
	m_pSystem->playSound(pSound->pSound, nullptr,
		false, &pChannel);

	pSound->ChannelList.push_back(pChannel);
}

void CSoundManager::Stop(const string & strName)
{
	PSoundInfo	pSoundInfo = FindSound(strName);

	if (!pSoundInfo)
		return;

	list<Channel*>::iterator	iter;
	list<Channel*>::iterator	iterEnd = pSoundInfo->ChannelList.end();

	for (iter = pSoundInfo->ChannelList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->stop();
	}

	pSoundInfo->ChannelList.clear();
}

void CSoundManager::Stop(PSoundInfo pSound)
{
	list<Channel*>::iterator	iter;
	list<Channel*>::iterator	iterEnd = pSound->ChannelList.end();

	for (iter = pSound->ChannelList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->stop();
	}

	pSound->ChannelList.clear();
}

void CSoundManager::Update(float fTime)
{
	unordered_map<string, PSoundInfo>::iterator	iter;
	unordered_map<string, PSoundInfo>::iterator	iterEnd = m_mapSound.end();
	for (iter = m_mapSound.begin(); iter != iterEnd; ++iter)
	{
		list<Channel*>::iterator	iter1;
		list<Channel*>::iterator	iter1End = iter->second->ChannelList.end();

		for (iter1 = iter->second->ChannelList.begin(); iter1 != iter1End;)
		{
			bool	bIsPlay = false;
			(*iter1)->isPlaying(&bIsPlay);

			if (!bIsPlay)
			{
				iter1 = iter->second->ChannelList.erase(iter1);
				iter1 = iter->second->ChannelList.end();
			}

			else
				++iter1;
		}
	}
}

void CSoundManager::DeleteSound(CScene * pScene)
{
	unordered_map<string, PSoundInfo>::iterator	iter;
	unordered_map<string, PSoundInfo>::iterator	iterEnd = m_mapSound.end();
	for (iter = m_mapSound.begin(); iter != iterEnd;)
	{
		if (iter->second->pScene == pScene)
		{
			iter->second->pSound->release();
			SAFE_DELETE(iter->second);
			iter = m_mapSound.erase(iter);
		}

		else
			++iter;
	}
}

PSoundInfo CSoundManager::FindSound(const string & strName)
{
	unordered_map<string, PSoundInfo>::iterator	iter = m_mapSound.find(strName);

	if (iter == m_mapSound.end())
		return nullptr;

	return iter->second;
}