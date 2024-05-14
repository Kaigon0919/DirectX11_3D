#include "ColliderRay.h"
#include "ColliderSphere.h"
#include"ColliderAABB.h"
#include"ColliderOBB.h"
#include "../Input.h"
#include "../Device.h"
#include "../Scene/Scene.h"
#include "Camera.h"
ENGINE_USING

CColliderRay::CColliderRay()
{
	SetTag("Ray");
	m_eCollType = CT_RAY;
	m_bMouse = false;
}


CColliderRay::CColliderRay(const CColliderRay & com)	:
	CCollider(com)
{
	m_tRelative = com.m_tRelative;
	m_tInfo = com.m_tInfo;
	m_bMouse = com.m_bMouse;
}

CColliderRay::~CColliderRay()
{
}

void CColliderRay::MouseEnable(bool bEnable)
{
	m_bMouse = true;
}

RayInfo CColliderRay::GetInfo() const
{
	return m_tInfo;
}

void CColliderRay::Start()
{
	CCollider::Start();
}

bool CColliderRay::Init()
{
	return true;
}

int CColliderRay::Input(float fTime)
{
	return 0;
}

int CColliderRay::Update(float fTime)
{
	return 0;
}

int CColliderRay::LateUpdate(float fTime)
{
	if (m_bMouse)
	{
		Vector2	vMousePos = GET_SINGLE(CInput)->GetMouseViewportPos();

		Resolution	tRS = _RESOLUTION;

		CCamera*	pCamera = m_pScene->GetMainCamera();

		Matrix	matProj = pCamera->GetProjMatrix();
		Matrix	matView = pCamera->GetViewMatrix();

		SAFE_RELEASE(pCamera);

		matView = matView.Inverse();

		// 마우스 위치를 3차원 공간으로 변환한다.
		m_tInfo.vDir.x = (vMousePos.x / (tRS.iWidth / 2.f) - 1.f) / matProj._11;
		m_tInfo.vDir.y = (vMousePos.y / (tRS.iHeight / -2.f) + 1.f) / matProj._22;
		m_tInfo.vDir.z = 1.f;

		m_tInfo.vDir.Normalize();
		m_tInfo.vOrigin = Vector3::Zero;

		// 뷰의 역행렬을 곱해서 월드공간으로 변환한다.
		m_tInfo.vDir = m_tInfo.vDir.TransformNormal(matView);
		m_tInfo.vDir.Normalize();

		m_tInfo.vOrigin = m_tInfo.vOrigin.TransformCoord(matView);

		Vector3	vEnd = m_tInfo.vOrigin + m_tInfo.vDir * 1000.f;

		m_vSectionMin.x = m_tInfo.vOrigin.x < vEnd.x ? m_tInfo.vOrigin.x : vEnd.x;
		m_vSectionMin.y = m_tInfo.vOrigin.y < vEnd.y ? m_tInfo.vOrigin.y : vEnd.y;
		m_vSectionMin.z = m_tInfo.vOrigin.z < vEnd.z ? m_tInfo.vOrigin.z : vEnd.z;

		m_vSectionMax.x = m_tInfo.vOrigin.x > vEnd.x ? m_tInfo.vOrigin.x : vEnd.x;
		m_vSectionMax.y = m_tInfo.vOrigin.y > vEnd.y ? m_tInfo.vOrigin.y : vEnd.y;
		m_vSectionMax.z = m_tInfo.vOrigin.z > vEnd.z ? m_tInfo.vOrigin.z : vEnd.z;
	}

	else
	{
	}
	return 0;
}

int CColliderRay::Collision(float fTime)
{
	return 0;
}

int CColliderRay::PrevRender(float fTime)
{
	return 0;
}

int CColliderRay::Render(float fTime)
{
	return 0;
}

CColliderRay * CColliderRay::Clone() const
{
	return new CColliderRay(*this);
}
bool CColliderRay::Collision(CCollider * pDest)
{
	switch (pDest->GetColliderType())
	{
	case CT_SPHERE:
		return CollisionSphereToRay(((CColliderSphere*)pDest)->GetInfo(), m_tInfo);
	case CT_CAPSULE:
		return false;
	case CT_OBB:
		//return CollisionOBBToRay(((CColliderOBB*)pDest)->GetInfo(), m_tInfo);
	case CT_POINT:
		return false;
	case CT_RAY:
		return false;
	case CT_AABB:
		return CollisionAABBToRay(((CColliderAABB*)pDest)->GetInfo(), m_tInfo);
	default:
		return false;
	}
	return false;
}
