#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL CColliderOBB :
	public CCollider
{
	friend class CGameObject;

protected:
	CColliderOBB();
	CColliderOBB(const CColliderOBB& com);
	virtual ~CColliderOBB();

private:
	OBBInfo	m_tRelative;
	OBBInfo	m_tInfo;

public:
	void SetOBBInfo(const Vector3& vCenter, const Vector3& vHalfLength);
	OBBInfo GetInfo()	const;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CColliderOBB* Clone()	const;
	virtual bool Collision(CCollider* pDest);
};

ENGINE_END
