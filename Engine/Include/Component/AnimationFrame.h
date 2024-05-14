#pragma once
#include "Component.h"

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagTextureCoord
{
	Vector2		vStart;
	Vector2		vEnd;
}TextureCoord, *PTextureCoord;

typedef struct ENGINE_DLL _tagAnimationFrameClip
{
	string					strName;
	ANIMATION_FRAME_TYPE	eType;
	ANIMATION_OPTION		eOption;
	int						iFrame;
	Vector2					vTextureSize;
	vector<TextureCoord>	vecCoord;
	float					fPlayTime;
	float					fPlayLimitTime;
	float					fFrameTime;
	class CTexture*			pTexture;
	int						iRegister;
}AnimationFrameClip, *PAnimationFrameClip;

class ENGINE_DLL CAnimationFrame :
	public CComponent
{
	friend class CGameObject;

protected:
	CAnimationFrame();
	CAnimationFrame(const CAnimationFrame& com);
	~CAnimationFrame();

private:
	unordered_map<string, PAnimationFrameClip>	m_mapClip;
	PAnimationFrameClip		m_pCurClip;
	PAnimationFrameClip		m_pDefaultClip;
	AnimationFrameCBuffer	m_tCBuffer;
	class CMaterial*		m_pMaterial;

public:
	bool CreateClip(const string& strName, ANIMATION_OPTION eOption,
		const Vector2& vTextureSize,
		const vector<TextureCoord>& vecCoord, float fLimitTime,
		int iRegister, const string& strTexName,
		const TCHAR* pFileName, 
		const string& strPathName = TEXTURE_PATH);
	bool CreateClip(const string& strName, ANIMATION_OPTION eOption,
		const Vector2& vTextureSize,
		const vector<TextureCoord>& vecCoord, float fLimitTime,
		int iRegister, const string& strTexName,
		const vector<TCHAR*>& vecFileName, 
		const string& strPathName = TEXTURE_PATH);
	void SetCurrentClip(const string& strName);
	void SetDefaultClip(const string& strName);
	void ChangeClip(const string& strName);

private:
	PAnimationFrameClip FindClip(const string& strName);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CAnimationFrame* Clone()	const;
};

ENGINE_END
