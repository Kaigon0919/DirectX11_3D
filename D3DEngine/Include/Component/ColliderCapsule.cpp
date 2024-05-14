#include "ColliderCapsule.h"

ENGINE_USING

CColliderCapsule::CColliderCapsule()
{
	SetTag("Capsule");
	m_eCollType = CT_CAPSULE;
}


CColliderCapsule::CColliderCapsule(const CColliderCapsule & com)	:
	CCollider(com)
{
}

CColliderCapsule::~CColliderCapsule()
{
}

void CColliderCapsule::Start()
{
	CCollider::Start();
}

bool CColliderCapsule::Init()
{
	return false;
}

int CColliderCapsule::Input(float fTime)
{
	return 0;
}

int CColliderCapsule::Update(float fTime)
{
	return 0;
}

int CColliderCapsule::LateUpdate(float fTime)
{
	return 0;
}

int CColliderCapsule::Collision(float fTime)
{
	return 0;
}

int CColliderCapsule::PrevRender(float fTime)
{
	return 0;
}

int CColliderCapsule::Render(float fTime)
{
	return 0;
}

CColliderCapsule * CColliderCapsule::Clone() const
{
	return nullptr;
}

bool CColliderCapsule::Collision(CCollider * pDest)
{
	return false;
}
