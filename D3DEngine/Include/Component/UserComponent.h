#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CUserComponent :
	public CComponent
{
	friend class CGameObject;

protected:
	CUserComponent();
	CUserComponent(const CUserComponent& com);
	virtual ~CUserComponent() = 0;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CUserComponent* Clone()	const;
};

ENGINE_END
