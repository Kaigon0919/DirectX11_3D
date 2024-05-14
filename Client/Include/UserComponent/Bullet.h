#pragma once

#include "Component/UserComponent.h"

ENGINE_USING

class CBullet	:
	public CUserComponent
{
public:
	CBullet();
	CBullet(const CBullet& bullet);
	~CBullet();

private:
	float	m_fSpeed;
	float	m_fDist;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CBullet* Clone()	const;
};

