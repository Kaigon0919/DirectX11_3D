#pragma once

#include "../Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CSceneComponent	:
	public CRef
{
	friend class CScene;

protected:
	CSceneComponent();
	virtual ~CSceneComponent() = 0;

protected:
	class CScene*	m_pScene;

public:
	virtual bool Init() = 0;
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int Render(float fTime);
};

ENGINE_END
