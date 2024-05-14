#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL CColliderSphere :
	public CCollider
{
	friend class CGameObject;

protected:
	CColliderSphere();
	CColliderSphere(const CColliderSphere& com);
	virtual ~CColliderSphere();

private:
	SphereInfo	m_tRelative;
	SphereInfo	m_tInfo;
	bool		m_bScaleEnable;

public:
	void SetSphereInfo(const Vector3& vCenter, float fRadius);
	void SetScaleEnable(bool bEnable);
	SphereInfo GetInfo()	const;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CColliderSphere* Clone()	const;
	virtual bool Collision(CCollider* pDest);
};

ENGINE_END
