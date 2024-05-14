#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL CColliderPoint :
	public CCollider
{
	friend class CGameObject;

protected:
	CColliderPoint();
	CColliderPoint(const CColliderPoint& com);
	virtual ~CColliderPoint();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CColliderPoint* Clone()	const;
	virtual bool Collision(CCollider* pDest);
};

ENGINE_END
