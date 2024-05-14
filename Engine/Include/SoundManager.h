#pragma once

#include "Engine.h"
#include "fmod.hpp"

#ifdef _WIN64
#pragma comment(lib, "fmod64_vc")
#else
#pragma comment(lib, "fmod_vc")
#endif
using namespace FMOD;

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagSoundInfo
{
	Sound*	pSound;
	bool	bLoop;
	class CScene* pScene;
	list<Channel*>	ChannelList;
}SoundInfo, *PSoundInfo;

class ENGINE_DLL CSoundManager
{
private:
	System*	m_pSystem;
	unordered_map<string, PSoundInfo>	m_mapSound;

public:
	System* GetSoundSystem()	const;

public:
	bool Init();
	bool LoadSound(const string& strName, class CScene* pScene,	bool bLoop, const char* pFileName, const string& strPathName = SOUND_PATH);
	void Play(const string& strName);
	void Play(PSoundInfo pSound);
	void Stop(const string& strName);
	void Stop(PSoundInfo pSound);
	void Update(float fTime);
	void DeleteSound(class CScene* pScene);
	PSoundInfo FindSound(const string& strName);

	DECLARE_SINGLE(CSoundManager)
};

ENGINE_END