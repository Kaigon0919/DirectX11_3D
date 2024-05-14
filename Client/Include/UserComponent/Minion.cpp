#include "Minion.h"
#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "GameObject.h"
#include "Component/Renderer.h"
#include "Component/Transform.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderOBB.h"
#include "Component/Material.h"
CMinion::CMinion(): m_pAnimation(nullptr), m_pNavigation(nullptr)
{
}

CMinion::CMinion(const CMinion & minion) : CUserComponent(minion)
{
	m_pAnimation = nullptr;
	m_pNavigation = nullptr;
}

CMinion::~CMinion()
{
	SAFE_RELEASE(m_pNavigation);
	SAFE_RELEASE(m_pAnimation);
}

void CMinion::Start()
{
	SAFE_RELEASE(m_pAnimation);
	m_pAnimation = m_pObject->FindComponentFromType<CAnimation>(CT_ANIMATION);

	m_pAnimation->ChangeClip("Atk01");

	SAFE_RELEASE(m_pNavigation);
	m_pNavigation = m_pObject->FindComponentFromType<CNavigation>(CT_NAVIGATION);


	CColliderSphere*	pTrace = m_pObject->FindComponentFromTag<CColliderSphere>("Trace");

	pTrace->SetCallback<CMinion>(CCS_BEGIN, this, &CMinion::Trace);
	pTrace->SetCallback<CMinion>(CCS_LEAVE, this, &CMinion::TraceExit);
	pTrace->SetScaleEnable(false);
	SAFE_RELEASE(pTrace);
}

bool CMinion::Init()
{
	m_pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);
	m_pTransform->SetLocalRot(0.f, -90.f, 0.f);

	CRenderer*	pRenderer = m_pObject->AddComponent<CRenderer>("MinionRenderer");
	pRenderer->SetMesh("Minion", TEXT("UndeadCommander.msh"),	Vector3::Axis[AXIS_Z]);
	SAFE_RELEASE(pRenderer);

	CMaterial*	pMaterial = m_pObject->FindComponentFromType<CMaterial>(CT_MATERIAL);
	SAFE_RELEASE(pMaterial);

	CColliderOBB*	pBody = m_pObject->AddComponent<CColliderOBB>("Body");
	pBody->SetProfile("Enemy");
	pBody->SetChannel("Enemy");
	pBody->SetOBBInfo(Vector3::Zero, Vector3(0.5f, 0.5f, 0.5f));
	SAFE_RELEASE(pBody);

	CColliderSphere*	pTrace = m_pObject->AddComponent<CColliderSphere>("Trace");
	pTrace->SetProfile("Trace");
	pTrace->SetChannel("Trace");
	pTrace->SetSphereInfo(Vector3::Zero, 15.0f);
	pTrace->SetScaleEnable(false);
	SAFE_RELEASE(pTrace);

	m_pAnimation = m_pObject->FindComponentFromType<CAnimation>(CT_ANIMATION);
	if (!m_pAnimation)
	{
		m_pAnimation = m_pObject->AddComponent<CAnimation>("MinionAnimation");

		m_pAnimation->LoadBone("UndeadCommander.bne");
		m_pAnimation->Load("UndeadCommander.anm");
	}

	CNavigation*	pNav = m_pObject->AddComponent<CNavigation>("Nav");
	SAFE_RELEASE(pNav);

	return true;
}

int CMinion::Input(float fTime)
{
	return 0;
}

int CMinion::Update(float fTime)
{
	return 0;
}

int CMinion::LateUpdate(float fTime)
{
	return 0;
}

int CMinion::Collision(float fTime)
{
	return 0;
}

int CMinion::PrevRender(float fTime)
{
	return 0;
}

int CMinion::Render(float fTime)
{
	return 0;
}

CMinion * CMinion::Clone() const
{
	return new CMinion(*this);
}

void CMinion::Trace(CCollider * pSrc, CCollider * pDest, float fTime)
{
	if (pDest->GetTag() == "PlayerBody")
	{
		m_pNavigation->SetAIFindPath(true);
		m_pNavigation->SetTarget(pDest);
	}
}

void CMinion::TraceExit(CCollider * pSrc, CCollider * pDest, float fTime)
{
	if (pDest->GetTag() == "PlayerBody")
	{
		m_pNavigation->SetAIFindPath(false);
		m_pNavigation->SetTarget(nullptr);
	}
}
