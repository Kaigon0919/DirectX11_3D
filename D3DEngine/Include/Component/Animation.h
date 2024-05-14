#pragma once
#include "Component.h"

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagBone
{
	string		strName;
	int			iDepth;
	int			iParentIndex;
	PMatrix		matOffset;
	PMatrix		matBone;
	list<class CBoneSocket*>	SocketList;
	int			iRefCount;

	_tagBone() :
		iRefCount(1)
	{
	}
}BONE, *PBONE;

typedef struct ENGINE_DLL _tagKeyFrame
{
	double	dTime;
	Vector3	vPos;
	Vector3	vScale;
	Vector4	vRot;
}KEYFRAME, *PKEYFRAME;

typedef struct ENGINE_DLL _tagBoneKeyFrame
{
	int		iBoneIndex;
	vector<PKEYFRAME>	vecKeyFrame;
	int			iRefCount;

	_tagBoneKeyFrame() :
		iRefCount(1)
	{
	}

	~_tagBoneKeyFrame()
	{
		Safe_Delete_VecList(vecKeyFrame);
	}
}BONEKEYFRAME, *PBONEKEYFRAME;

typedef struct ENGINE_DLL _tagAnimationCallback
{
	int		iAnimationProgress;
	float	fAnimationProgress;
	function<void(float)> func;
	bool	bCall;
}ANIMATIONCALLBACK, *PANIMATIONCALLBACK;

typedef struct ENGINE_DLL _tagAnimationClip
{
	ANIMATION_OPTION	eOption;
	string				strName;
	float				fStartTime;
	float				fEndTime;
	float				fTimeLength;
	float				fFrameTime;
	float				fPlayTime;
	int					iStartFrame;
	int					iEndFrame;
	int					iFrameLength;
	int					iFrameMode;
	int					iChangeFrame;
	vector<PBONEKEYFRAME>		vecKeyFrame;
	vector<PANIMATIONCALLBACK>	vecCallback;

	_tagAnimationClip() :
		eOption(AO_LOOP),
		strName(""),
		iFrameMode(0),
		fStartTime(0),
		fEndTime(0),
		fTimeLength(0),
		iStartFrame(0),
		iEndFrame(0),
		iFrameLength(0),
		fPlayTime(1.f)
	{
	}

	~_tagAnimationClip()
	{
		for (size_t i = 0; i < vecKeyFrame.size(); ++i)
		{
			--vecKeyFrame[i]->iRefCount;

			if (vecKeyFrame[i]->iRefCount == 0)
			{
				SAFE_DELETE(vecKeyFrame[i]);
			}
		}

		vecKeyFrame.clear();
		//Safe_Delete_VecList(vecCallback);
	}
}ANIMATIONCLIP, *PANIMATIONCLIP;

typedef struct ENGINE_DLL _tagBlendInfo
{
	Vector3	vScale;
	Vector3	vPos;
	Vector4	vRot;
}BlendInfo, *PBlendInfo;

class ENGINE_DLL CAnimation :
	public CComponent
{
	friend class CGameObject;
	friend class CMesh;

protected:
	CAnimation();
	CAnimation(const CAnimation& com);
	virtual ~CAnimation();

private:
	vector<PBONE>		m_vecBones;
	ID3D11Texture2D*	m_pBoneTex;
	ID3D11ShaderResourceView*	m_pBoneRV;
	vector<PMatrix>			m_vecBoneMatrix;
	unordered_map<string, PANIMATIONCLIP>	m_mapClip;
	PANIMATIONCLIP			m_pDefaultClip;
	PANIMATIONCLIP			m_pCurClip;
	PANIMATIONCLIP			m_pNextClip;
	list<string>			m_AddClipNameList;
	bool					m_bEnd;
	float					m_fAnimationGlobalTime;
	float					m_fClipProgress;
	float					m_fChangeTime;
	float					m_fChangeLimitTime;
	vector<BlendInfo>		m_vecBlendInfo;

public:
	const list<string>*  GetAddClipNameList()	const;
	void AddBone(PBONE pBone);
	bool CreateBoneTexture();
	void AddClip(ANIMATION_OPTION eOption,	struct _tagFbxAnimationClip* pClip);
	void AddClip(const string& strName, ANIMATION_OPTION eOption, int iStartFrame, int iEndFrame, float fPlayTime,	const vector<PBONEKEYFRAME>& vecFrame);
	void AddClip(const TCHAR* pFullPath);
	void AddClipFromMultibyte(const char* pFullPath);
	PANIMATIONCLIP FindClip(const string& strName);
	bool IsAnimationEnd()	const;
	float GetAniPercent()const;
	PANIMATIONCLIP GetCurrentClip()	const;
	const unordered_map<string, PANIMATIONCLIP>* GetClips()	const;
	void GetCurrentkeyFrame(vector<PBONEKEYFRAME>& vecKeyFrame);

public:
	void ChangeClipKey(const string& strOrigin, const string& strChange);
	PBONE FindBone(const string& strBoneName);
	int FindBoneIndex(const string& strBoneName);
	int GetCurBoneTransform(const string& strBoneName, __out Vector3&vPos, __out Vector4& vRot, __out Vector3& vScale);
	Matrix GetBoneMatrix(const string& strBoneName);
	bool ChangeClip(const string& strClip);
	ID3D11ShaderResourceView** GetBoneTexture();
	bool Save(const TCHAR* pFileName, const string& strPathKey = MESH_PATH);
	bool Save(const char* pFileName, const string& strPathKey = MESH_PATH);
	bool SaveFromFullPath(const TCHAR* pFullPath);
	bool SaveFromFullPath(const char* pFullPath);
	bool Load(const TCHAR* pFileName, const string& strPathKey = MESH_PATH);
	bool Load(const char* pFileName, const string& strPathKey = MESH_PATH);
	bool LoadFromFullPath(const TCHAR* pFullPath);
	bool LoadFromFullPath(const char* pFullPath);
	bool SaveBone(const TCHAR* pFileName, const string& strPathKey = MESH_PATH);
	bool SaveBone(const char* pFileName, const string& strPathKey = MESH_PATH);
	bool SaveBoneFromFullPath(const TCHAR* pFullPath);
	bool SaveBoneFromFullPath(const char* pFullPath);
	bool LoadBone(const TCHAR* pFileName, const string& strPathKey = MESH_PATH);
	bool LoadBone(const char* pFileName, const string& strPathKey = MESH_PATH);
	bool LoadBoneFromFullPath(const TCHAR* pFullPath);
	bool LoadBoneFromFullPath(const char* pFullPath);
	bool ModifyClip(const string& strKey, const string& strChangeKey, ANIMATION_OPTION eOption, int iStartFrame, int iEndFrame,	float fPlayTime, const vector<PBONEKEYFRAME>& vecFrame);
	bool DeleteClip(const string& strKey);
	bool ReturnDefaultClip();

private:
	void LoadFbxAnimation(const char* pFullPath);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CAnimation* Clone()	const;
};

ENGINE_END
