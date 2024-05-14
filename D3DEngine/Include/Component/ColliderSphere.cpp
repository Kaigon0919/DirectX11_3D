#include "ColliderSphere.h"

#ifdef _DEBUG
#include "../Resource/Mesh.h"
#include "../Resource/ResourcesManager.h"
#include "../Scene/Scene.h"
#include "Camera.h"
#endif // _DEBUG

#include "Transform.h"
#include"ColliderOBB.h"
#include "ColliderRay.h"

ENGINE_USING

CColliderSphere::CColliderSphere()
{
	SetTag("Sphere");
	m_eCollType = CT_SPHERE;
	m_bScaleEnable = true;

#ifdef _DEBUG
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("Sky");
#endif // _DEBUG

}


CColliderSphere::CColliderSphere(const CColliderSphere & com)	:
	CCollider(com)
{
	m_tRelative = com.m_tRelative;
	m_tInfo = com.m_tInfo;	
	m_bScaleEnable = com.m_bScaleEnable;
}

CColliderSphere::~CColliderSphere()
{
}

void CColliderSphere::SetSphereInfo(const Vector3 & vCenter, float fRadius)
{
	m_tRelative.vCenter = vCenter;
	m_tRelative.fRadius = fRadius;
	m_tInfo.fRadius = fRadius;
}

void CColliderSphere::SetScaleEnable(bool bEnable)
{
	m_bScaleEnable = bEnable;
}

SphereInfo CColliderSphere::GetInfo() const
{
	return m_tInfo;
}

void CColliderSphere::Start()
{
	CCollider::Start();
}

bool CColliderSphere::Init()
{
	return true;
}

int CColliderSphere::Input(float fTime)
{
	return 0;
}

int CColliderSphere::Update(float fTime)
{
	return 0;
}

int CColliderSphere::LateUpdate(float fTime)
{
	if (m_bScaleEnable)
	{
		Vector3	vScale = m_pTransform->GetWorldScale();
		m_tInfo.vCenter = m_tRelative.vCenter * vScale + m_pTransform->GetWorldPos();

		float	fScale = vScale.x;

		fScale = fScale < vScale.y ? vScale.y : fScale;
		fScale = fScale < vScale.z ? vScale.z : fScale;

		m_tInfo.fRadius = m_tRelative.fRadius * fScale;
	}
	else
	{
		m_tInfo.vCenter = m_tRelative.vCenter + m_pTransform->GetWorldPos();
		m_tInfo.fRadius = m_tRelative.fRadius;
	}

	m_vSectionMin = m_tInfo.vCenter - m_tInfo.fRadius;
	m_vSectionMax = m_tInfo.vCenter + m_tInfo.fRadius;

	return 0;
}

int CColliderSphere::Collision(float fTime)
{
	return 0;
}

int CColliderSphere::PrevRender(float fTime)
{
	return 0;
}

int CColliderSphere::Render(float fTime)
{
#ifdef _DEBUG
	CCamera*	pCamera = m_pScene->GetMainCamera();

	Matrix	matScale, matTranslate, matWorld;

	matScale.Scaling(m_tInfo.fRadius, m_tInfo.fRadius, m_tInfo.fRadius);
	matTranslate.Translation(m_tInfo.vCenter);

	matWorld = matScale * matTranslate;

	m_tCBuffer.matWorldRot.Identity();
	m_tCBuffer.matWVRot = m_tCBuffer.matWorldRot * pCamera->GetViewMatrix();
	m_tCBuffer.matWorld = matWorld;
	m_tCBuffer.matView = pCamera->GetViewMatrix();
	m_tCBuffer.matProj = pCamera->GetProjMatrix();
	m_tCBuffer.matInvProj = m_tCBuffer.matProj;
	m_tCBuffer.matInvProj.Inverse();
	m_tCBuffer.matWV = m_tCBuffer.matWorld * m_tCBuffer.matView;
	m_tCBuffer.matWVP = m_tCBuffer.matWV * m_tCBuffer.matProj;

	m_tCBuffer.matWorldRot.Transpose();
	m_tCBuffer.matWVRot.Transpose();
	m_tCBuffer.matWorld.Transpose();
	m_tCBuffer.matView.Transpose();
	m_tCBuffer.matProj.Transpose();
	m_tCBuffer.matInvProj.Transpose();
	m_tCBuffer.matWV.Transpose();
	m_tCBuffer.matWVP.Transpose();

	SAFE_RELEASE(pCamera);
#endif // _DEBUG


	CCollider::Render(fTime);

	return 0;
}

CColliderSphere * CColliderSphere::Clone() const
{
	return new CColliderSphere(*this);
}
bool CColliderSphere::Collision(CCollider * pDest)
{
	switch (pDest->GetColliderType())
	{
	case CT_SPHERE:
		return CollisionSphereToSphere(m_tInfo, ((CColliderSphere*)pDest)->GetInfo());
	case CT_CAPSULE:
		return false;
	case CT_OBB:
		return CollisionSpeherToOBB(m_tInfo, ((CColliderOBB*)pDest)->GetInfo());
	case CT_POINT:
		return false;
	case CT_RAY:
		return 	CollisionSphereToRay(m_tInfo, ((CColliderRay*)pDest)->GetInfo());;
	case CT_AABB:
		return false;
	}

	return false;
}
