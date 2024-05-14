#pragma once

#include "Component/UserComponent.h"
#include "Component/Animation.h"
#include "Component/Collider.h"

ENGINE_USING

class CTestMonster : public CUserComponent
{
public:
	CTestMonster();
	CTestMonster(const CTestMonster& player);
	~CTestMonster();

private:
	CAnimation*	m_pAnimation;

private:
	bool m_bMove;
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CTestMonster* Clone()	const;
};

