#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL CColliderRay :
	public CCollider
{
	friend class CGameObject;

protected:
	CColliderRay();
	CColliderRay(const CColliderRay& com);
	virtual ~CColliderRay();
private:
	RayInfo	m_tRelative;
	RayInfo	m_tInfo;
	bool	m_bMouse;

public:
	void MouseEnable(bool bEnable = true);
	RayInfo GetInfo()	const;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CColliderRay* Clone()	const;
	virtual bool Collision(CCollider* pDest);
};

ENGINE_END
