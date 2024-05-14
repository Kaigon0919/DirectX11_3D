#pragma once

#include "Scene/SceneComponent.h"

ENGINE_USING

class CMainScene	:
	public CSceneComponent
{
public:
	CMainScene();
	~CMainScene();

public:
	virtual bool Init();
	virtual int Update(float fTime);
private:
	void CreatePrototype();
	void MouseEnable(float fTime);
};

