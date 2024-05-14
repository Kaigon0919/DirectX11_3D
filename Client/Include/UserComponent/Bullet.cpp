#include "Bullet.h"
#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "GameObject.h"
#include "Component/Renderer.h"
#include "Component/Transform.h"
#include "Component/ColliderSphere.h"

CBullet::CBullet()
{
}

CBullet::CBullet(const CBullet & bullet)	:
	CUserComponent(bullet)
{
	m_fDist = bullet.m_fDist;
	m_fSpeed = bullet.m_fSpeed;
}


CBullet::~CBullet()
{
}

void CBullet::Start()
{
}

bool CBullet::Init()
{
	CRenderer*	pRenderer = m_pObject->AddComponent<CRenderer>("BulletRenderer");

	pRenderer->SetMesh("Pyramid");

	SAFE_RELEASE(pRenderer);

	CColliderSphere*	pBody = m_pObject->AddComponent<CColliderSphere>("Body");

	pBody->SetChannel("EnemyAttack");
	pBody->SetSphereInfo(Vector3::Zero, 1.f);

	SAFE_RELEASE(pBody);

	m_fSpeed = 2.f;
	m_fDist = 3.f;

	return true;
}

int CBullet::Input(float fTime)
{
	return 0;
}

int CBullet::Update(float fTime)
{
	m_pTransform->Move(AXIS_Y, m_fSpeed, fTime);

	m_fDist -= m_fSpeed * fTime;

	if (m_fDist <= 0.f)
	{
		m_pObject->Active(false);
	}

	return 0;
}

int CBullet::LateUpdate(float fTime)
{
	return 0;
}

int CBullet::Collision(float fTime)
{
	return 0;
}

int CBullet::PrevRender(float fTime)
{
	return 0;
}

int CBullet::Render(float fTime)
{
	return 0;
}

CBullet * CBullet::Clone() const
{
	return new CBullet(*this);
}
