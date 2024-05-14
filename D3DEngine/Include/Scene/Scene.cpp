#include "Scene.h"
#include "SceneComponent.h"
#include "Layer.h"
#include "../GameObject.h"
#include "../Component/Camera.h"
#include "../Component/Transform.h"
#include "../Component/Renderer.h"
#include "../Component/Material.h"
#include "../Device.h"
#include "../Component/Light.h"
#include "../SoundManager.h"
ENGINE_USING

CScene::CScene()	:
	m_pMainCameraObj(nullptr),
	m_pMainCamera(nullptr),
	m_pMainCameraTransform(nullptr),
	m_pUICameraObj(nullptr),
	m_pUICamera(nullptr),
	m_pUICameraTransform(nullptr)
{
}

CScene::~CScene()
{
	GET_SINGLE(CSoundManager)->DeleteSound(this);
	SAFE_RELEASE(m_pSky);
	Safe_Release_Map(m_mapCamera);
	SAFE_RELEASE(m_pMainCameraObj);
	SAFE_RELEASE(m_pMainCamera);
	SAFE_RELEASE(m_pMainCameraTransform);
	SAFE_RELEASE(m_pUICameraObj);
	SAFE_RELEASE(m_pUICamera);
	SAFE_RELEASE(m_pUICameraTransform);

	CGameObject::RemovePrototype(this);

	list<CSceneComponent*>::iterator	iter;
	list<CSceneComponent*>::iterator	iterEnd = m_SceneComList.end();

	for (iter = m_SceneComList.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	m_SceneComList.clear();

	list<CLayer*>::iterator	iter1;
	list<CLayer*>::iterator	iter1End = m_LayerList.end();

	for (iter1 = m_LayerList.begin(); iter1 != iter1End; ++iter1)
	{
		SAFE_DELETE((*iter1));
	}

	m_LayerList.clear();
}

CGameObject * CScene::GetMainCameraObj() const
{
	m_pMainCameraObj->AddRef();
	return m_pMainCameraObj;
}

CCamera * CScene::GetMainCamera() const
{
	m_pMainCamera->AddRef();
	return m_pMainCamera;
}

CTransform * CScene::GetMainCameraTransform() const
{
	m_pMainCameraTransform->AddRef();
	return m_pMainCameraTransform;
}

CGameObject * CScene::GetUICameraObj() const
{
	m_pUICameraObj->AddRef();
	return m_pUICameraObj;
}

CCamera * CScene::GetUICamera() const
{
	m_pUICamera->AddRef();
	return m_pUICamera;
}

CTransform * CScene::GetUICameraTransform() const
{
	m_pUICameraTransform->AddRef();
	return m_pUICameraTransform;
}

Matrix CScene::GetViewMatrix() const
{
	return m_pMainCamera->GetViewMatrix();
}

Matrix CScene::GetProjMatrix() const
{
	return m_pMainCamera->GetProjMatrix();
}

Vector3 CScene::GetMainCameraPos() const
{
	return m_pMainCameraTransform->GetWorldPos();
}

Matrix CScene::GetUIViewMatrix() const
{
	return m_pUICamera->GetViewMatrix();
}

Matrix CScene::GetUIProjMatrix() const
{
	return m_pUICamera->GetProjMatrix();
}

Vector3 CScene::GetUICameraPos() const
{
	return m_pUICameraTransform->GetWorldPos();
}

bool CScene::Init()
{
	AddLayer("Default", 0);
	AddLayer("UI", INT_MAX - 2);

	/*AddCamera("MainCamera", Vector3(0.f, 0.f, -3.f), Vector3::Zero, CT_PERSPECTIVE,
		_RESOLUTION.iWidth, _RESOLUTION.iHeight, 90.f, 0.03f, 1000.f);*/

	AddCamera("MainCamera", Vector3(-10.f, 10.f, 0.f), Vector3(0.0, 0.f, 0.f), CT_PERSPECTIVE, _RESOLUTION.iWidth, _RESOLUTION.iHeight, 90.f, 0.03f, 1000.f);

	AddCamera("UICamera", Vector3(0.f, 0.f, 0.f), Vector3::Zero, CT_ORTHO, _RESOLUTION.iWidth, _RESOLUTION.iHeight, 90.f, 0.f, 1000.f);

	m_pUICameraObj = FindCamera("UICamera");
	m_pUICamera = m_pUICameraObj->FindComponentFromType<CCamera>(CT_CAMERA);
	m_pUICameraTransform = m_pUICameraObj->GetTransform();

	m_pSky = CGameObject::CreateObject("Sky");

	m_pSky->SetScene(this);

	CTransform*	pSkyTr = m_pSky->GetTransform();

	pSkyTr->SetWorldScale(100000.f, 100000.f, 100000.f);

	// 행렬 연산을 해놓기 위함이다.
	pSkyTr->LateUpdate(0.f);

	SAFE_RELEASE(pSkyTr);

	CRenderer*	pSkyRenderer = m_pSky->AddComponent<CRenderer>("SkyRenderer");

	pSkyRenderer->SetMesh("Sky");
	pSkyRenderer->SetRenderState("CullNone");
	pSkyRenderer->SetRenderState("LessEqual");

	SAFE_RELEASE(pSkyRenderer);

	CMaterial*	pMaterial = m_pSky->AddComponent<CMaterial>("SkyMaterial");

	pMaterial->AddTextureSet(0, 0, 10, "EngineSky", TEXT("Sky.dds"));

	SAFE_RELEASE(pMaterial);
	m_pSky->EraseComponentFromTag("PickSphere");
	m_pSky->Start();

	// 조명 생성

	CLayer*	pDefaultLayer = FindLayer("Default");

	/*CGameObject*	pGlobalLightObj = CGameObject::CreateObject("GlobalLight",
		pDefaultLayer);

	CTransform*	pLightTr = pGlobalLightObj->GetTransform();

	pLightTr->SetWorldRotX(90.f);
	pLightTr->SetWorldPos(0.f, 2.f, 0.f);

	SAFE_RELEASE(pLightTr);

	CLight*	pGlobalLight = pGlobalLightObj->AddComponent<CLight>("GlobalLight");

	pGlobalLight->SetLightType(LT_SPOT);
	pGlobalLight->SetLightDistance(10.f);
	pGlobalLight->SetLightAngle(60.f, 90.f);
	pGlobalLight->SetLightColor(Vector4::Red, Vector4::Red * 0.2f,
		Vector4::Red);

	SAFE_RELEASE(pGlobalLight);

	SAFE_RELEASE(pGlobalLightObj);

	CGameObject* pGlobalLightObj1 = CGameObject::CreateObject("GlobalLight",
		pDefaultLayer);

	CTransform* pLightTr1 = pGlobalLightObj1->GetTransform();

	pLightTr1->SetWorldPos(0.f, -1.f, 0.f);

	SAFE_RELEASE(pLightTr1);

	CLight* pGlobalLight1 = pGlobalLightObj1->AddComponent<CLight>("GlobalLight");

	pGlobalLight1->SetLightType(LT_POINT);
	pGlobalLight1->SetLightDistance(10.f);

	SAFE_RELEASE(pGlobalLight1);

	SAFE_RELEASE(pGlobalLightObj1);*/

	CGameObject* pGlobalLightObj2 = CGameObject::CreateObject("GlobalLight",
		pDefaultLayer);

	CTransform* pLightTr2 = pGlobalLightObj2->GetTransform();

	pLightTr2->SetWorldRotX(45.f);
	pLightTr2->SetWorldRotY(90.f);

	SAFE_RELEASE(pLightTr2);

	CLight* pGlobalLight2 = pGlobalLightObj2->AddComponent<CLight>("GlobalLight");

	pGlobalLight2->SetLightType(LT_DIR);

	SAFE_RELEASE(pGlobalLight2);

	SAFE_RELEASE(pGlobalLightObj2);

	return true;
}

int CScene::Input(float fTime)
{
	list<CSceneComponent*>::iterator	iter;
	list<CSceneComponent*>::iterator	iterEnd = m_SceneComList.end();

	for (iter = m_SceneComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_DELETE((*iter));
			iter = m_SceneComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Input(fTime);

		++iter;
	}

	list<CLayer*>::iterator	iter1;
	list<CLayer*>::iterator	iter1End = m_LayerList.end();

	for (iter1 = m_LayerList.begin(); iter1 != iter1End;)
	{
		if (!(*iter1)->IsActive())
		{
			SAFE_DELETE((*iter1));
			iter1 = m_LayerList.erase(iter1);
			continue;
		}

		else if (!(*iter1)->IsEnable())
		{
			++iter1;
			continue;
		}

		(*iter1)->Input(fTime);

		++iter1;
	}

	return 0;
}

int CScene::Update(float fTime)
{
	list<CSceneComponent*>::iterator	iter;
	list<CSceneComponent*>::iterator	iterEnd = m_SceneComList.end();

	for (iter = m_SceneComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_DELETE((*iter));
			iter = m_SceneComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(fTime);

		++iter;
	}

	list<CLayer*>::iterator	iter1;
	list<CLayer*>::iterator	iter1End = m_LayerList.end();

	for (iter1 = m_LayerList.begin(); iter1 != iter1End;)
	{
		if (!(*iter1)->IsActive())
		{
			SAFE_DELETE((*iter1));
			iter1 = m_LayerList.erase(iter1);
			continue;
		}

		else if (!(*iter1)->IsEnable())
		{
			++iter1;
			continue;
		}

		(*iter1)->Update(fTime);

		++iter1;
	}

	m_pMainCameraObj->Update(fTime);

	return 0;
}

int CScene::LateUpdate(float fTime)
{
	list<CSceneComponent*>::iterator	iter;
	list<CSceneComponent*>::iterator	iterEnd = m_SceneComList.end();

	for (iter = m_SceneComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_DELETE((*iter));
			iter = m_SceneComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->LateUpdate(fTime);

		++iter;
	}

	list<CLayer*>::iterator	iter1;
	list<CLayer*>::iterator	iter1End = m_LayerList.end();

	for (iter1 = m_LayerList.begin(); iter1 != iter1End;)
	{
		if (!(*iter1)->IsActive())
		{
			SAFE_DELETE((*iter1));
			iter1 = m_LayerList.erase(iter1);
			continue;
		}

		else if (!(*iter1)->IsEnable())
		{
			++iter1;
			continue;
		}

		(*iter1)->LateUpdate(fTime);

		++iter1;
	}
	
	m_pMainCameraObj->LateUpdate(fTime);

	return 0;
}

int CScene::Collision(float fTime)
{
	list<CSceneComponent*>::iterator	iter;
	list<CSceneComponent*>::iterator	iterEnd = m_SceneComList.end();

	for (iter = m_SceneComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_DELETE((*iter));
			iter = m_SceneComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Collision(fTime);

		++iter;
	}

	list<CLayer*>::iterator	iter1;
	list<CLayer*>::iterator	iter1End = m_LayerList.end();

	for (iter1 = m_LayerList.begin(); iter1 != iter1End;)
	{
		if (!(*iter1)->IsActive())
		{
			SAFE_DELETE((*iter1));
			iter1 = m_LayerList.erase(iter1);
			continue;
		}

		else if (!(*iter1)->IsEnable())
		{
			++iter1;
			continue;
		}

		(*iter1)->Collision(fTime);

		++iter1;
	}

	m_pMainCameraObj->Collision(fTime);


	return 0;
}

int CScene::Render(float fTime)
{
	list<CSceneComponent*>::iterator	iter;
	list<CSceneComponent*>::iterator	iterEnd = m_SceneComList.end();

	for (iter = m_SceneComList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			SAFE_DELETE((*iter));
			iter = m_SceneComList.erase(iter);
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Render(fTime);

		++iter;
	}

	m_pSky->Render(fTime);

	list<CLayer*>::iterator	iter1;
	list<CLayer*>::iterator	iter1End = m_LayerList.end();

	for (iter1 = m_LayerList.begin(); iter1 != iter1End;)
	{
		if (!(*iter1)->IsActive())
		{
			SAFE_DELETE((*iter1));
			iter1 = m_LayerList.erase(iter1);
			continue;
		}

		else if (!(*iter1)->IsEnable())
		{
			++iter1;
			continue;
		}

		(*iter1)->Render(fTime);

		++iter1;
	}

	m_pMainCameraObj->Render(fTime);


	return 0;
}

void CScene::AddLayer(const string & strName, int iZOrder)
{
	CLayer*	pLayer = new CLayer;

	pLayer->SetTag(strName.c_str());
	pLayer->m_pScene = this;

	if (!pLayer->Init())
	{
		SAFE_DELETE(pLayer);
		return;
	}

	m_LayerList.push_back(pLayer);

	pLayer->SetZOrder(iZOrder);
}

void CScene::SortLayer()
{
	m_LayerList.sort(CScene::Sort);
}

CLayer * CScene::FindLayer(const string & strName)
{
	list<CLayer*>::iterator	iter;
	list<CLayer*>::iterator	iterEnd = m_LayerList.end();

	for (iter = m_LayerList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strName)
			return *iter;
	}

	return nullptr;
}

bool CScene::AddCamera(const string & strName, const Vector3 & vPos,
	const Vector3 & vRot, CAMERA_TYPE eType, UINT iWidth, UINT iHeight,
	float fAngle, float fNear, float fFar)
{
	CGameObject*	pCameraObj = FindCamera(strName);

	if (pCameraObj)
	{
		SAFE_RELEASE(pCameraObj);
		return false;
	}

	pCameraObj = CGameObject::CreateObject(strName);

	pCameraObj->SetScene(this);

	CTransform*	pTransform = pCameraObj->GetTransform();

	pTransform->SetWorldPos(vPos);
	pTransform->SetWorldRot(vRot);

	SAFE_RELEASE(pTransform);

	CCamera*	pCamera = pCameraObj->AddComponent<CCamera>(strName);

	pCamera->SetCameraInfo(eType, iWidth, iHeight, fAngle, fNear, fFar);

	// 제일 처음 추가되는 카메라를 MainCamera로 지정해준다.
	if (!m_pMainCameraObj)
	{
		pCameraObj->AddRef();
		m_pMainCameraObj = pCameraObj;

		pCamera->AddRef();
		m_pMainCamera = pCamera;

		m_pMainCameraTransform = pCameraObj->GetTransform();
	}

	SAFE_RELEASE(pCamera);

	m_mapCamera.insert(make_pair(strName, pCameraObj));

	return true;
}

bool CScene::ChangeCamera(const string & strName)
{
	CGameObject*	pCameraObj = FindCamera(strName);

	if (!pCameraObj)
		return false;

	SAFE_RELEASE(m_pMainCameraObj);
	SAFE_RELEASE(m_pMainCamera);
	SAFE_RELEASE(m_pMainCameraTransform);

	m_pMainCameraObj = pCameraObj;
	m_pMainCamera = pCameraObj->FindComponentFromType<CCamera>(CT_CAMERA);
	m_pMainCameraTransform = pCameraObj->GetTransform();

	return true;
}

CGameObject * CScene::FindCamera(const string & strName)
{
	unordered_map<string, CGameObject*>::iterator	iter = m_mapCamera.find(strName);

	if (iter == m_mapCamera.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

bool CScene::Sort(CLayer * pSrc, CLayer * pDest)
{
	int	iSrc = pSrc->GetZOrder();
	int	iDest = pDest->GetZOrder();

	return iSrc > iDest;
}
