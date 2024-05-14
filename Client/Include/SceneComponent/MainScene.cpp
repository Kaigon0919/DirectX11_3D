#include "MainScene.h"
#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "GameObject.h"
#include "CollisionManager.h"
#include "Input.h"
#include "Component/Renderer.h"
#include "Component/Transform.h"
#include "Component/Material.h"
#include "Component/ParticleComponent.h"
#include "Component/AnimationFrame.h"
#include "Component/LandScape.h"
#include "Component/Arm.h"
#include "Component/ColliderAABB.h"

#include "../UserComponent/Player.h"
#include "../UserComponent/Minion.h"
#include "../UserComponent/Bullet.h"
#include "../UserComponent/TestMonster.h"

#include "Component/MeshSocket.h"

CMainScene::CMainScene()
{
}

CMainScene::~CMainScene()
{
}

bool CMainScene::Init()
{
	GET_SINGLE(CCollisionManager)->AddChannel("Player");
	GET_SINGLE(CCollisionManager)->AddChannel("Enemy");
	GET_SINGLE(CCollisionManager)->AddChannel("PlayerAttack");
	GET_SINGLE(CCollisionManager)->AddChannel("EnemyAttack");
	GET_SINGLE(CCollisionManager)->AddChannel("Trace");

	GET_SINGLE(CCollisionManager)->CreateProfile("Player");
	GET_SINGLE(CCollisionManager)->CreateProfile("Enemy");
	GET_SINGLE(CCollisionManager)->CreateProfile("Trace");

	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Player", "Player", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Player", "WorldStatic", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Player", "WorldDynamic", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Player", "EnemyAttack", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Player", "Enemy", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Player", "Trace", CCS_BLOCK);

	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Enemy", "PlayerAttack", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Enemy", "Player", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Enemy", "WorldStatic", CCS_BLOCK);
	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Enemy", "WorldDynamic", CCS_BLOCK);

	GET_SINGLE(CCollisionManager)->SetProfileChannelState("Trace", "Player", CCS_BLOCK);

	GET_SINGLE(CInput)->AddActionKey("MouseEnable", DIK_LCONTROL, SE_CTRL);
	GET_SINGLE(CInput)->BindAction("MouseEnable", KEY_STATE::KS_PRESS, this, &CMainScene::MouseEnable);


	CLayer*	pDefaultLayer = m_pScene->FindLayer("Default");

	CreatePrototype();

	CGameObject*	pPlayerObj = CGameObject::CreateObject("Player", pDefaultLayer);

	CPlayer*	pPlayer = pPlayerObj->AddComponent<CPlayer>("Player");


	CGameObject*	pMainCameraObj = m_pScene->GetMainCameraObj();
	CArm*	pCameraArm = pMainCameraObj->AddComponent<CArm>("CameraArm");
	CTransform* pCamTr = pMainCameraObj->GetTransform();
	pCamTr->SetWorldPos(0, 10.0f, 10.0f);
	pCameraArm->MouseEnable(true);
	pCameraArm->SetTarget(pPlayerObj);

	SAFE_RELEASE(pCamTr);
	SAFE_RELEASE(pCameraArm);
	SAFE_RELEASE(pMainCameraObj);

	SAFE_RELEASE(pPlayer);

	//CGameObject*	pMinionObj = CGameObject::CreateClone("Minion", m_pScene, pDefaultLayer);
	//CTransform*	pTransform = pMinionObj->GetTransform();

	//pTransform->SetWorldPos(15.f, 0.f, 15.f);
	//pTransform->LookAt(pPlayerObj);

	//SAFE_RELEASE(pTransform);

	//SAFE_RELEASE(pMinionObj);

	//SAFE_RELEASE(pPlayerObj);

	//Socket Test.
	CMeshSocket *pSocket = pPlayerObj->AddComponent<CMeshSocket>();

	CGameObject *pBoxObj = CGameObject::CreateObject("BoxCol", pDefaultLayer);
	CColliderAABB *pBoxCol = pBoxObj->AddComponent<CColliderAABB>();
	pBoxCol->SetAABBInfo(Vector3::Zero, Vector3(1.0f, 1.0f, 1.0f));


	CTransform * pBoxTr = pBoxObj->GetTransform();
	pBoxTr->SetWorldScale(1.0f, 1.0f, 1.0f);


	pSocket->SetSoket("Bip01-L-Hand");
	pSocket->SetObject(pBoxObj);
	SAFE_RELEASE(pBoxCol);
	SAFE_RELEASE(pBoxObj);
	SAFE_RELEASE(pBoxTr);

	SAFE_RELEASE(pSocket);
	SAFE_RELEASE(pPlayerObj);

	CGameObject*	pExplosion = CGameObject::CreateClone("Explosion", m_pScene, pDefaultLayer);
	SAFE_RELEASE(pExplosion);
	CGameObject* pTestMonsterObj = CGameObject::CreateObject("TestMonster", pDefaultLayer);
	CTestMonster* pTestMonster = pTestMonsterObj->AddComponent<CTestMonster>();

	SAFE_RELEASE(pTestMonster);

	SAFE_RELEASE(pTestMonsterObj);






	CGameObject*	pLandScapeObj = CGameObject::CreateObject("LandScape", pDefaultLayer);

	CLandScape*	pLandScape = pLandScapeObj->AddComponent<CLandScape>("LandScape");

	pLandScape->CreateLandScape("LandScape", "LandScape/Height3.bmp");


	SAFE_RELEASE(pLandScape);

	SAFE_RELEASE(pLandScapeObj);


	return true;
}

int CMainScene::Update(float fTime)
{

	return 0;
}

void CMainScene::CreatePrototype()
{
	CGameObject*	pMinionPrototype = CGameObject::CreatePrototype("Minion", m_pScene);

	CMinion*	pMinion = pMinionPrototype->AddComponent<CMinion>("Minion");

	SAFE_RELEASE(pMinion);

	SAFE_RELEASE(pMinionPrototype);

	CGameObject*	pBulletPrototype = CGameObject::CreatePrototype("Bullet", m_pScene);

	CBullet*	pBullet = pBulletPrototype->AddComponent<CBullet>("Bullet");

	SAFE_RELEASE(pBullet);

	SAFE_RELEASE(pBulletPrototype);

	CGameObject*	pExplosionPrototype = CGameObject::CreatePrototype("Explosion");

	CTransform*	pExplosionTr = pExplosionPrototype->GetTransform();

	pExplosionTr->SetWorldScale(3.f, 3.f, 1.f);

	SAFE_RELEASE(pExplosionTr);

	CRenderer*	pRenderer = pExplosionPrototype->AddComponent<CRenderer>("ExplosionRenderer");

	pRenderer->SetMesh("Particle");
	pRenderer->SetRenderState("AlphaBlend");

	SAFE_RELEASE(pRenderer);

	CParticleComponent*	pParticle = pExplosionPrototype->AddComponent<CParticleComponent>("Explosion");

	vector<TCHAR*>	vecParticleFileName;
	vecParticleFileName.reserve(89);

	for (size_t i = 1; i <= 89; ++i)
	{
		TCHAR*	pFileName = new TCHAR[MAX_PATH];
		memset(pFileName, 0, sizeof(TCHAR) * MAX_PATH);

		wsprintf(pFileName, TEXT("Explosion/Explosion%d.png"), i);

		vecParticleFileName.push_back(pFileName);
	}

	pParticle->LoadTextureSet("Explosion", vecParticleFileName);

	SAFE_RELEASE(pParticle);

	CAnimationFrame*	pExplosionFrame = pExplosionPrototype->AddComponent<CAnimationFrame>("ExplosionAnimation");

	vector<TextureCoord>	vecCoord;
	vecCoord.reserve(89);

	for (int i = 0; i < 89; ++i)
	{
		TextureCoord	tCoord = {};
		tCoord.vStart = Vector2(0.f, 0.f);
		tCoord.vEnd = Vector2(320.f, 240.f);
		vecCoord.push_back(tCoord);
	}

	pExplosionFrame->CreateClip("Idle", AO_LOOP, Vector2(320.f, 240.f),
		vecCoord, 1.f, 10, "Explosion", vecParticleFileName);

	SAFE_RELEASE(pExplosionFrame);

	Safe_Delete_Array_VecList(vecParticleFileName);

	SAFE_RELEASE(pExplosionPrototype);
}

void CMainScene::MouseEnable(float fTime)
{
	CGameObject*	pMainCameraObj = m_pScene->GetMainCameraObj();
	CArm*	pCameraArm = pMainCameraObj->FindComponentFromTag<CArm>("CameraArm");

	bool bMouse = pCameraArm->GetMouseEnable() ? false : true;
	pCameraArm->MouseEnable(bMouse);

	SAFE_RELEASE(pCameraArm);
	SAFE_RELEASE(pMainCameraObj);
}
