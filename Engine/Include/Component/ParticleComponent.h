#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CParticleComponent :
	public CComponent
{
	friend class CGameObject;

protected:
	CParticleComponent();
	CParticleComponent(const CParticleComponent& com);
	~CParticleComponent();

private:
	ParticleCBuffer	m_tCBuffer;
	class CMaterial*	m_pMaterial;
	string			m_strTextureName;

public:
	bool LoadTextureSet(const string& strTexName,
		const TCHAR* pFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
	bool LoadTextureSetFromFullPath(const string& strTexName,
		const TCHAR* pFullPath = nullptr);
	bool LoadTextureSet(const string& strName, const vector<TCHAR*>& vecFileName,
		const string& strPathKey = TEXTURE_PATH);
	bool LoadTextureSetFromFullPath(const string& strName, const vector<TCHAR*>& vecFullPath);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CParticleComponent* Clone()	const;
};

ENGINE_END
