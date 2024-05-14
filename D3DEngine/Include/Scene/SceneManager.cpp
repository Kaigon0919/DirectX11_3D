#include "SceneManager.h"
#include "Scene.h"
#include "../Input.h"

ENGINE_USING

DEFINITION_SINGLE(CSceneManager)

CSceneManager::CSceneManager()	:
	m_pScene(nullptr),
	m_pNextScene(nullptr)
{
}


CSceneManager::~CSceneManager()
{
	SAFE_DELETE(m_pScene);
	SAFE_DELETE(m_pNextScene);
}

CScene * CSceneManager::GetScene() const
{
	return m_pScene;
}

Matrix CSceneManager::GetViewMatrix() const
{
	return m_pScene->GetViewMatrix();
}

Matrix CSceneManager::GetProjMatrix() const
{
	return m_pScene->GetProjMatrix();
}

Vector3 CSceneManager::GetMainCameraPos() const
{
	return m_pScene->GetMainCameraPos();
}

void CSceneManager::SetNextScene(CScene * pScene)
{
	m_pNextScene = pScene;
}

bool CSceneManager::Init()
{
	m_pScene = CreateSceneEmpty();

	GET_SINGLE(CInput)->ChangeMouseScene(m_pScene);

	return true;
}

int CSceneManager::Input(float fTime)
{
	m_pScene->Input(fTime);

	return ChangeScene();
}

int CSceneManager::Update(float fTime)
{
	m_pScene->Update(fTime);

	return ChangeScene();
}

int CSceneManager::LateUpdate(float fTime)
{
	m_pScene->LateUpdate(fTime);

	return ChangeScene();
}

int CSceneManager::Collision(float fTime)
{
	m_pScene->Collision(fTime);

	return ChangeScene();
}

int CSceneManager::Render(float fTime)
{
	m_pScene->Render(fTime);

	return ChangeScene();
}

int CSceneManager::ChangeScene()
{
	if (m_pNextScene)
	{
		SAFE_DELETE(m_pScene);
		m_pScene = m_pNextScene;
		m_pNextScene = nullptr;

		GET_SINGLE(CInput)->ChangeMouseScene(m_pScene);

		return SC_NEXT;
	}

	return SC_NONE;
}

CScene * CSceneManager::CreateSceneEmpty()
{
	CScene* pScene = new CScene;

	if (!pScene->Init())
	{
		SAFE_DELETE(pScene);
		return nullptr;
	}

	return pScene;
}
