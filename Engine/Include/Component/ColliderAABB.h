#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL CColliderAABB : public CCollider
{
	friend class CGameObject;

protected:
	CColliderAABB();
	CColliderAABB(const CColliderAABB& com);
	virtual ~CColliderAABB();
private:
	Vector3 m_vCenter;
	Vector3 m_vPivot;
	Vector3 m_vHalfLength;
	AABBInfo m_tInfo;
public:
	void SetAABBInfo(const Vector3& vPivot, const Vector3& vHalfLength);
	AABBInfo GetInfo()	const;
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CColliderAABB* Clone()	const;
	virtual bool Collision(CCollider* pDest);
};

ENGINE_END
