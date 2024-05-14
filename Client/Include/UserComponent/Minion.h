#pragma once

#include "Component/UserComponent.h"
#include "Component/Collider.h"

#include "Component/Animation.h"
#include "Component/Navigation.h"
ENGINE_USING

class CMinion :	public CUserComponent
{
public:
	CMinion();
	CMinion(const CMinion& minion);
	~CMinion();
private:
	class CAnimation*	m_pAnimation;
	class CNavigation*	m_pNavigation;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CMinion* Clone()	const;

public:
	void Trace(CCollider* pSrc, CCollider* pDest, float fTime);
	void TraceExit(CCollider* pSrc, CCollider* pDest, float fTime);
};

