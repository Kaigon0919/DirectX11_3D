#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CMeshSocket : public CComponent
{
public:
	CMeshSocket();
	CMeshSocket(const CMeshSocket& ref);
	~CMeshSocket();
private:
	CTransform * m_pObjTransform;
	class CAnimation * m_pAni;
	string m_strBonename;
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CMeshSocket* Clone()	const;
public:
	void SetSoket(const string strBoneName);
	void SetObject(CGameObject * const pObj);
	void SetObject(CTransform * const pTransform);
};

ENGINE_END
