#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CUI : public CComponent
{
	friend class CGameObject;
protected:
	CUI();
	CUI(const CUI& other);
	virtual ~CUI() = 0;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CUI* Clone()	const;
};
ENGINE_END

