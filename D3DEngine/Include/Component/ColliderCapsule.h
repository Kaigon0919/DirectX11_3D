#pragma once
#include "Collider.h"

ENGINE_BEGIN

class ENGINE_DLL CColliderCapsule :
	public CCollider
{
	friend class CGameObject;

protected:
	CColliderCapsule();
	CColliderCapsule(const CColliderCapsule& com);
	virtual ~CColliderCapsule();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CColliderCapsule* Clone()	const;
	virtual bool Collision(CCollider* pDest);
};

ENGINE_END
