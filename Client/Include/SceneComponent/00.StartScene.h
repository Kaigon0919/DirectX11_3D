#pragma once

#include "Scene/SceneComponent.h"

ENGINE_USING

class CStartScene : public CSceneComponent
{
public:
	CStartScene();
	~CStartScene();

public:
	virtual bool Init();

private:
	void CreatePrototype();
	void StartButton(float fTime);
};