#pragma once
#include "Component.h"
#include "../SoundManager.h"

ENGINE_BEGIN

class ENGINE_DLL CSound :
	public CComponent
{
	friend class CGameObject;

protected:
	CSound();
	CSound(const CSound& com);
	virtual ~CSound();

private:
	string		m_strFileName;
	struct _tagSoundInfo*	m_pSoundInfo;
	Channel*	m_pChannel;
	bool		m_bBGM;
	bool		m_bLoop;
	bool		m_bPlay;

public:
	void SetSound(const string& strName);
	void SetSound(const string& strName, class CScene* pScene,
		bool bLoop, const char* pFileName,
		const string& strPathName = SOUND_PATH);
	void EnableBGM();
	void Play();
	void Stop();
	void Pause();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CSound* Clone()	const;
};

ENGINE_END