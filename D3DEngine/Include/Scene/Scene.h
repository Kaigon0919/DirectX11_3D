#pragma once

#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CScene
{
	friend class CSceneManager;

private:
	CScene();
	~CScene();

private:
	list<class CSceneComponent*>	m_SceneComList;
	list<class CLayer*>	m_LayerList;
	unordered_map<string, class CGameObject*>	m_mapCamera;
	class CGameObject*		m_pMainCameraObj;
	class CCamera*			m_pMainCamera;
	class CTransform*		m_pMainCameraTransform;
	class CGameObject*		m_pUICameraObj;
	class CCamera*			m_pUICamera;
	class CTransform*		m_pUICameraTransform;

	class CGameObject*		m_pSky;
	class CMaterial*		m_pSkyMaterial;

public:
	class CGameObject* GetMainCameraObj()	const;
	class CCamera* GetMainCamera()	const;
	class CTransform* GetMainCameraTransform()	const;

	class CGameObject* GetUICameraObj()	const;
	class CCamera* GetUICamera()	const;
	class CTransform* GetUICameraTransform()	const;

	Matrix GetViewMatrix()	const;
	Matrix GetProjMatrix()	const;
	Vector3 GetMainCameraPos()	const;


	Matrix GetUIViewMatrix()	const;
	Matrix GetUIProjMatrix()	const;
	Vector3 GetUICameraPos()	const;

public:
	bool Init();
	int Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	int Render(float fTime);
	void AddLayer(const string& strName, int iZOrder);
	void SortLayer();
	class CLayer* FindLayer(const string& strName);
	bool AddCamera(const string& strName, const Vector3& vPos, const Vector3& vRot, CAMERA_TYPE eType, UINT iWidth, UINT iHeight, float fAngle, float fNear, float fFar);
	bool ChangeCamera(const string& strName);
	class CGameObject* FindCamera(const string& strName);

private:
	static bool Sort(class CLayer* pSrc, class CLayer* pDest);

public:
	template <typename T>
	bool AddSceneComponent()
	{
		T* pCom = new T;

		pCom->m_pScene = this;

		if (!pCom->Init())
		{
			SAFE_DELETE(pCom);
			return false;
		}

		m_SceneComList.push_back(pCom);

		return true;
	}
};

ENGINE_END
