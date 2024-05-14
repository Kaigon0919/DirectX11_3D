#pragma once

#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CSceneManager
{
private:
	class CScene*	m_pScene;
	class CScene*	m_pNextScene;

public:
	class CScene* GetScene()	const;
	Matrix GetViewMatrix()	const;
	Matrix GetProjMatrix()	const;
	Vector3 GetMainCameraPos()	const;
public:
	void SetNextScene(class CScene* pScene);

public:
	bool Init();
	int Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	int Render(float fTime);
private:
	int ChangeScene();
public:
	template <typename T>
	CScene* CreateScene()
	{
		CScene* pScene = new CScene;

		if (!pScene->Init())
		{
			SAFE_DELETE(pScene);
			return nullptr;
		}

		pScene->AddSceneComponent<T>();

		return pScene;
	}

	template <typename T>
	bool AddSceneComponent(bool bCurrent = true)
	{
		if (bCurrent)
			return m_pScene->AddSceneComponent<T>();

		return m_pNextScene->AddSceneComponent<T>();
	}

	class CScene* CreateSceneEmpty();

	DECLARE_SINGLE(CSceneManager)
};

ENGINE_END
