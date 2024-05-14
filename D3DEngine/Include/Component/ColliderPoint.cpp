#include "ColliderPoint.h"

ENGINE_USING

CColliderPoint::CColliderPoint()
{
	SetTag("Point");
	m_eCollType = CT_POINT;
}


CColliderPoint::CColliderPoint(const CColliderPoint & com)	:
	CCollider(com)
{
}

CColliderPoint::~CColliderPoint()
{
}

void CColliderPoint::Start()
{
	CCollider::Start();
}

bool CColliderPoint::Init()
{
	return false;
}

int CColliderPoint::Input(float fTime)
{
	return 0;
}

int CColliderPoint::Update(float fTime)
{
	return 0;
}

int CColliderPoint::LateUpdate(float fTime)
{
	return 0;
}

int CColliderPoint::Collision(float fTime)
{
	return 0;
}

int CColliderPoint::PrevRender(float fTime)
{
	return 0;
}

int CColliderPoint::Render(float fTime)
{
	return 0;
}

CColliderPoint * CColliderPoint::Clone() const
{
	return nullptr;
}

bool CColliderPoint::Collision(CCollider * pDest)
{
	return false;
}
