#include "ColliderAABB.h"
#include "Transform.h"
#ifdef _DEBUG
#include "../Resource/Mesh.h"
#include "../Resource/ResourcesManager.h"
#include "../Scene/Scene.h"
#include "Camera.h"
#endif // _DEBUG

ENGINE_USING

CColliderAABB::CColliderAABB()
{
	SetTag("AABB");
	m_eCollType = CT_AABB;
#ifdef _DEBUG
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("ColliderBox");
#endif // _DEBUG
}


CColliderAABB::CColliderAABB(const CColliderAABB & com)	:
	CCollider(com)
{
}

CColliderAABB::~CColliderAABB()
{
}

void CColliderAABB::Start()
{
	CCollider::Start();
}

bool CColliderAABB::Init()
{
	return true;
}

int CColliderAABB::Input(float fTime)
{
	return 0;
}

int CColliderAABB::Update(float fTime)
{
	return 0;
}

int CColliderAABB::LateUpdate(float fTime)
{
	m_vCenter = m_pTransform->GetWorldPos();

	m_tInfo.vMax = m_vCenter + m_vHalfLength + m_vPivot;
	m_tInfo.vMin = m_vCenter - m_vHalfLength + m_vPivot;
	return 0;
}

int CColliderAABB::Collision(float fTime)
{
	return 0;
}

int CColliderAABB::PrevRender(float fTime)
{
	return 0;
}
int CColliderAABB::Render(float fTime)
{
#ifdef _DEBUG
	CCamera*	pCamera = m_pScene->GetMainCamera();

	Matrix	matScale, matRot, matTranslate, matWorld;

	matScale.Scaling(m_vHalfLength);
	matTranslate.Translation(m_vCenter);

	matWorld = matScale * matTranslate;

	m_tCBuffer.matWorldRot = matRot;
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

CColliderAABB * CColliderAABB::Clone() const
{
	return nullptr;
}

bool CColliderAABB::Collision(CCollider * pDest)
{
	switch (pDest->GetColliderType())
	{
	case CT_SPHERE:
		return false;
	case CT_CAPSULE:
		return false;
	case CT_OBB:
		return false;
	case CT_POINT:
		return false;
	case CT_RAY:
		return false;
	case CT_AABB:
		return false;
	}
	return false;
}

void CColliderAABB::SetAABBInfo(const Vector3 & vPivot, const Vector3 & vHalfLength)
{
	m_vPivot = vPivot;
	m_vHalfLength = vHalfLength;

	m_tInfo.vMax = m_vCenter + vHalfLength + vPivot;
	m_tInfo.vMin = m_vCenter - vHalfLength + vPivot;
}

AABBInfo CColliderAABB::GetInfo() const
{
	return m_tInfo;
}
