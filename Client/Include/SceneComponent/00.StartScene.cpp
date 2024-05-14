#include "00.StartScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Layer.h"
#include "GameObject.h"
#include "Component/UIButton.h"
#include "Component/Transform.h"
#include "Component/Text.h"
#include "MainScene.h"

CStartScene::CStartScene()
{
}

CStartScene::~CStartScene()
{
}

bool CStartScene::Init()
{
	CLayer*	pDefaultLayer = m_pScene->FindLayer("Default");
	CLayer*	pUILayer = m_pScene->FindLayer("UI");
	{
		CGameObject*	pStartButtonObj = CGameObject::CreateObject("StartButton", pUILayer);
		CTransform*	pTr = pStartButtonObj->GetTransform();
		pTr->SetWorldPos(1280.f / 2.f - 100.f, 720.f / 2.f - 50.f, 0.f);
		pTr->SetWorldScale(400.f, 100.f, 1.0f);
		SAFE_RELEASE(pTr);

		CUIButton*	pStartButton = pStartButtonObj->AddComponent<CUIButton>("StartButton");
		pStartButton->SetStateTexture(BS_NORMAL, "StartButton", TEXT("Start.png"));
		pStartButton->SetCallback(BS_CLICK, this, &CStartScene::StartButton);
		SAFE_RELEASE(pStartButton);
		SAFE_RELEASE(pStartButtonObj);
	}
	{
		CGameObject*	pTextObj = CGameObject::CreateObject("Text", pUILayer);

		CTransform*	pTextTr = pTextObj->GetTransform();

		pTextTr->SetWorldPos(100.f, 100.f, 0.f);

		SAFE_RELEASE(pTextTr);

		CText*	pText = pTextObj->AddComponent<CText>("Text");

		pText->SetText(TEXT("똑똑박사 에디!!"));
		pText->Shadow(true);
		pText->SetShadowColor(1.f, 0.f, 1.f, 1.f);
		pText->SetShadowOffset(Vector3(2.f, -2.f, 0.f));
		pText->SetShadowOpacity(1.f);
		pText->SetFont("Dotum");

		pText->SetColor(1.f, 1.f, 0.f, 1.f);
		pText->SetOpacity(0.5f);
		pText->AlphaBlend(true);
		pText->SetRenderArea(0.f, 0.f, 300.f, 50.f);

		SAFE_RELEASE(pText);

		SAFE_RELEASE(pTextObj);
	}
	{
		CGameObject*	pTestObj= CGameObject::CreateObject("Test", pDefaultLayer);
		SAFE_RELEASE(pTestObj);
	}
	return true;
}

void CStartScene::CreatePrototype()
{

}

void CStartScene::StartButton(float fTime)
{
	CScene*	pScene = GET_SINGLE(CSceneManager)->CreateScene<CMainScene>();

	GET_SINGLE(CSceneManager)->SetNextScene(pScene);
}
