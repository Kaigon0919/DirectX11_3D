#include "Player.h"
#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "GameObject.h"
#include "Component/Renderer.h"
#include "Component/Animation.h"
#include "Component/Material.h"
#include "Component/Transform.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderOBB.h"
#include "Component/ColliderAABB.h"
#include "Component/Navigation.h"
#include "Input.h"

#include "Core.h"

CPlayer::CPlayer()	: m_pAnimation(nullptr), m_pFSMFunc(nullptr), m_fSpeed(5.f), m_pCameraTr(nullptr) 
{
}

CPlayer::CPlayer(const CPlayer & player)
{
}

CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pAnimation);
	SAFE_RELEASE(m_pCameraTr);

}

void CPlayer::Start()
{
}

bool CPlayer::Init()
{
	m_pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);
	m_pTransform->SetLocalRot(0.f, -90.f, 0.f);
	//m_pTransform->SetWorldPos(0.f, 0.0f, 10.f);

	SAFE_RELEASE(m_pCameraTr);
	m_pCameraTr = m_pObject->GetScene()->GetMainCameraTransform();

	CRenderer*	pRenderer = m_pObject->AddComponent<CRenderer>("PlayerRenderer");

	pRenderer->SetMesh("Player", TEXT("Player.msh"), Vector3::Axis[AXIS_Z]);

	SAFE_RELEASE(pRenderer);

	CMaterial*	pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);

	pMaterial->SetEmissiveColor(0.f);

	SAFE_RELEASE(pMaterial);

	CColliderSphere*	pBody = m_pObject->AddComponent<CColliderSphere>("PlayerBody");

	pBody->SetProfile("Player");
	pBody->SetChannel("Player");
	pBody->SetSphereInfo(Vector3::Zero, 1.f);
	pBody->SetCallback<CPlayer>(CCS_BEGIN, this, &CPlayer::Hit);
	pBody->SetCallback<CPlayer>(CCS_STAY, this, &CPlayer::HitStay);
	pBody->SetCallback<CPlayer>(CCS_LEAVE, this, &CPlayer::HitLeave);
	pBody->SetScaleEnable(false);

	SAFE_RELEASE(pBody);

	CNavigation*	pNav = m_pObject->AddComponent<CNavigation>("Nav");

	SAFE_RELEASE(pNav);

	m_pAnimation = m_pObject->FindComponentFromType<CAnimation>(CT_ANIMATION);

	if (!m_pAnimation)
	{
		m_pAnimation = m_pObject->AddComponent<CAnimation>("PlayerAnimation");

		m_pAnimation->LoadBone("Player.bne");
		m_pAnimation->Load("Player.anm");
	}
	GET_SINGLE(CInput)->AddAxisKey("Move", DIK_W, 1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", DIK_S, -1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", DIK_UP, 1.f);
	GET_SINGLE(CInput)->AddAxisKey("Move", DIK_DOWN, -1.f);
	GET_SINGLE(CInput)->BindAxis("Move", this, &CPlayer::MoveFront);

	GET_SINGLE(CInput)->AddAxisKey("MoveLR", DIK_A, -1.f);
	GET_SINGLE(CInput)->AddAxisKey("MoveLR", DIK_D, 1.f);
	GET_SINGLE(CInput)->BindAxis("MoveLR", this, &CPlayer::MoveRight);

	GET_SINGLE(CInput)->AddActionKey("PlayerLButton", DIK_LBUTTON, 0, MB_LBUTTON);

	ChangeState(PlayerState::Idle);
	return true;
}

int CPlayer::Input(float fTime)
{
	return 0;
}

int CPlayer::Update(float fTime)
{
	(*this.*m_pFSMFunc)(fTime);
	return 0;
}

int CPlayer::LateUpdate(float fTime)
{
	m_vMoveScale = Vector3::Zero;
	return 0;
}

int CPlayer::Collision(float fTime)
{
	return 0;
}

int CPlayer::PrevRender(float fTime)
{
	return 0;
}

int CPlayer::Render(float fTime)
{
	return 0;
}

CPlayer * CPlayer::Clone() const
{
	return new CPlayer(*this);
}


void CPlayer::MoveFront(float fScale, float fTime)
{
	if (fScale != 0.0f)
	{
		m_vMoveScale.z = fScale >= 0 ? 1.f : -1.f;
	}
}

void CPlayer::MoveRight(float fScale, float fTime)
{
	if (fScale != 0.0f)
	{
		m_vMoveScale.x = fScale >= 0 ? 1.f : -1.f;
	}
}

void CPlayer::Rotation(float fScale, float fTime)
{
	m_pTransform->RotationY(180.f * fScale, fTime);
}

void CPlayer::Hit(CCollider * pSrc, CCollider * pDest, float fTime)
{
	OutputDebugString(TEXT("충돌\n"));
}

void CPlayer::HitStay(CCollider * pSrc, CCollider * pDest, float fTime)
{
	//OutputDebugString(TEXT("충돌Stay\n"));
}

void CPlayer::HitLeave(CCollider * pSrc, CCollider * pDest, float fTime)
{
	OutputDebugString(TEXT("충돌Leave\n"));
}

void CPlayer::ChangeState(PlayerState tState)
{
	m_eState = tState;
	switch (tState)
	{
	case CPlayer::PlayerState::Idle:
		m_pAnimation->ChangeClip("Idle");
		m_pFSMFunc = &CPlayer::IdleState;
		break;
	case CPlayer::PlayerState::Run:
		m_pAnimation->ChangeClip("Run");
		m_pFSMFunc = &CPlayer::RunState;
		break;
	case CPlayer::PlayerState::Combo1:
		m_pAnimation->ChangeClip("combo1");
		m_pFSMFunc = &CPlayer::Combo1State;
		break;
	case CPlayer::PlayerState::Combo1R:
		m_pAnimation->ChangeClip("combo1R");
		m_pFSMFunc = &CPlayer::Combo1RState;
		break;
	case CPlayer::PlayerState::Combo2:
		m_pAnimation->ChangeClip("combo2");
		m_pFSMFunc = &CPlayer::Combo2State;
		break;
	case CPlayer::PlayerState::Combo2R:
		m_pAnimation->ChangeClip("combo2R");
		m_pFSMFunc = &CPlayer::Combo2RState;
		break;
	case CPlayer::PlayerState::Combo3:
		m_pAnimation->ChangeClip("combo3");
		m_pFSMFunc = &CPlayer::Combo3State;
		break;
	case CPlayer::PlayerState::Combo3R:
		m_pAnimation->ChangeClip("combo3R");
		m_pFSMFunc = &CPlayer::Combo3RState;
		break;
	case CPlayer::PlayerState::Combo4:
		m_pAnimation->ChangeClip("combo4");
		m_pFSMFunc = &CPlayer::Combo4State;
		break;
	case CPlayer::PlayerState::Death:
		m_pAnimation->ChangeClip("Death");
		m_pFSMFunc = &CPlayer::DeathState;
		break;
	default:
		break;
	}
}
