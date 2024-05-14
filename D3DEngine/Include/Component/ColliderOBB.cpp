#include "ColliderOBB.h"

#ifdef _DEBUG
#include "../Resource/Mesh.h"
#include "../Resource/ResourcesManager.h"
#include "../Scene/Scene.h"
#include "Camera.h"
#endif // _DEBUG

#include "Transform.h"

ENGINE_USING

CColliderOBB::CColliderOBB()
{
	SetTag("OBB");
	m_eCollType = CT_OBB;

#ifdef _DEBUG
	m_pMesh = GET_SINGLE(CResourcesManager)->FindMesh("ColliderBox");
#endif // _DEBUG
}


CColliderOBB::CColliderOBB(const CColliderOBB & com)	:
	CCollider(com)
{
	m_tRelative = com.m_tRelative;
	m_tInfo = com.m_tInfo;
}

CColliderOBB::~CColliderOBB()
{
}

void CColliderOBB::SetOBBInfo(const Vector3 & vCenter, 
	const Vector3 & vHalfLength)
{
	m_tRelative.vCenter = vCenter;
	m_tRelative.vHalfLength = vHalfLength;
	m_tInfo.vHalfLength = vHalfLength;
}

OBBInfo CColliderOBB::GetInfo() const
{
	return m_tInfo;
}

void CColliderOBB::Start()
{
	CCollider::Start();
}

bool CColliderOBB::Init()
{
	return true;
}

int CColliderOBB::Input(float fTime)
{
	return 0;
}

int CColliderOBB::Update(float fTime)
{
	return 0;
}

int CColliderOBB::LateUpdate(float fTime)
{
	m_tInfo.vCenter = m_tRelative.vCenter + m_pTransform->GetWorldPos();

	// 축정보를 회전시킨다.
	Matrix	matWorldRot = m_pTransform->GetWorldRotationMatrix();
	for (int i = 0; i < AXIS_END; ++i)
	{
		m_tInfo.vAxis[i] = Vector3::Axis[i].TransformNormal(matWorldRot);
		m_tInfo.vAxis[i].Normalize();
	}

	// 8개의 육면체 점 정보를 만들어낸다.
	Vector3	vPos[8];

	vPos[0] = Vector3(-m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[1] = Vector3(m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[2] = Vector3(-m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[3] = Vector3(m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, -m_tInfo.vHalfLength.z);
	vPos[4] = Vector3(-m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);
	vPos[5] = Vector3(m_tInfo.vHalfLength.x, m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);
	vPos[6] = Vector3(-m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);
	vPos[7] = Vector3(m_tInfo.vHalfLength.x, -m_tInfo.vHalfLength.y, m_tInfo.vHalfLength.z);

	// 8개의 정점정보를 회전시키고 중점을 더해서 월드공간으로
	// 변환해준다.
	for (int i = 0; i < 8; ++i)
	{
		vPos[i] = vPos[i].TransformCoord(matWorldRot);
		vPos[i] += m_tInfo.vCenter;
	}

	// 최소값과 최대값을 구해준다.
	m_vSectionMin = Vector3(100000.f, 100000.f, 100000.f);
	m_vSectionMax = Vector3(-100000.f, -100000.f, -100000.f);

	for (int i = 0; i < 8; ++i)
	{
		if (m_vSectionMin.x > vPos[i].x)
			m_vSectionMin.x = vPos[i].x;

		if (m_vSectionMin.y > vPos[i].y)
			m_vSectionMin.y = vPos[i].y;

		if (m_vSectionMin.z > vPos[i].z)
			m_vSectionMin.z = vPos[i].z;

		if (m_vSectionMax.x < vPos[i].x)
			m_vSectionMax.x = vPos[i].x;

		if (m_vSectionMax.y < vPos[i].y)
			m_vSectionMax.y = vPos[i].y;

		if (m_vSectionMax.z < vPos[i].z)
			m_vSectionMax.z = vPos[i].z;
	}

	return 0;
}

int CColliderOBB::Collision(float fTime)
{
	return 0;
}

int CColliderOBB::PrevRender(float fTime)
{
	return 0;
}

int CColliderOBB::Render(float fTime)
{
#ifdef _DEBUG
	CCamera*	pCamera = m_pScene->GetMainCamera();

	Matrix	matScale, matRot, matTranslate, matWorld;

	for (int i = 0; i < AXIS_END; ++i)
	{
		memcpy(&matRot[i][0], &m_tInfo.vAxis[i], sizeof(Vector3));
	}

	matScale.Scaling(m_tInfo.vHalfLength);
	matTranslate.Translation(m_tInfo.vCenter);

	matWorld = matScale * matRot * matTranslate;

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

CColliderOBB * CColliderOBB::Clone() const
{
	return new CColliderOBB(*this);
}

bool CColliderOBB::Collision(CCollider * pDest)
{
	switch (pDest->GetColliderType())
	{
	case CT_SPHERE:
		//return CollisionSphereToSphere(m_tInfo, ((CColliderSphere*)pDest)->GetInfo());
		return false;
	case CT_CAPSULE:
		return false;
	case CT_OBB:
		return CollisionOBBToOBB(m_tInfo, ((CColliderOBB*)pDest)->GetInfo());
	case CT_POINT:
		return false;
	case CT_RAY:
		return false;
	case CT_AABB:
		return false;
	}

	return false;
}
