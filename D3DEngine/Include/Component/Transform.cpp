#include "Transform.h"
#include "../Device.h"
#include "../Render/ShaderManager.h"
#include "../GameObject.h"
#include "../Scene/Scene.h"
#include "Camera.h"

ENGINE_USING

CTransform::CTransform()	:
	m_bStatic(false),
	m_bUpdate(true),
	m_pLookAt(nullptr),
	m_bUI(false)
{
	m_eComType = CT_TRANSFORM;
	m_vLocalRelativeView = Vector3::Axis[AXIS_Z];
	m_vLocalView = m_vLocalRelativeView;
	m_vWorldView = m_vLocalRelativeView;
	m_eLookAxis = LOOK_ALL;
}

CTransform::CTransform(const CTransform & com)	:
	CComponent(com)
{
	*this = com;
	m_iRefCount = 1;
	m_bUpdate = true;
	m_pLookAt = nullptr;
	m_bUI = com.m_bUI;

	m_pLookAt = nullptr;
}

CTransform::~CTransform()
{
	SAFE_RELEASE(m_pLookAt);
}

bool CTransform::GetStatic() const
{
	return m_bStatic;
}

bool CTransform::GetUpdate() const
{
	return m_bUpdate;
}
bool CTransform::GetUI()	const
{
	return m_bUI;
}

void CTransform::UIEnable(bool bEnable)
{
	m_bUI = bEnable;
}
Matrix CTransform::GetLocalRotMatrix()	const
{
	return m_matLocalRot;
}
Matrix CTransform::GetLocalMatrix() const
{
	return m_matLocal;
}

Vector3 CTransform::GetWorldScale() const
{
	return m_vWorldScale;
}

Vector3 CTransform::GetWorldRot() const
{
	return m_vWorldRot;
}

Vector3 CTransform::GetWorldPos() const
{
	return m_vWorldPos;
}

Vector3 CTransform::GetWorldAxis(AXIS axis) const
{
	return m_vWorldAxis[axis];
}

Vector3 CTransform::GetWorldView()	const
{
	return m_vWorldView;
}
Vector3 CTransform::GetMove()	const
{
	return m_vMove;
}
void CTransform::SetWorldScale(float x, float y, float z)
{
	m_vWorldScale.x = x;
	m_vWorldScale.y = y;
	m_vWorldScale.z = z;

	m_matWorldScale.Scaling(m_vWorldScale);

	m_bUpdate = true;
}

void CTransform::SetWorldScale(const Vector3 & vScale)
{
	m_vWorldScale = vScale;

	m_matWorldScale.Scaling(m_vWorldScale);

	m_bUpdate = true;
}

void CTransform::SetWorldRot(float x, float y, float z)
{
	m_vWorldRot.x = x;
	m_vWorldRot.y = y;
	m_vWorldRot.z = z;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::SetWorldRot(const Vector3 & vRot)
{
	m_vWorldRot = vRot;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::SetWorldRotX(float x)
{
	m_vWorldRot.x = x;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::SetWorldRotY(float y)
{
	m_vWorldRot.y = y;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::SetWorldRotZ(float z)
{
	m_vWorldRot.z = z;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::SetWorldPos(float x, float y, float z)
{
	m_vMove += Vector3(x, y, z) - m_vWorldPos;
	m_vWorldPos.x = x;
	m_vWorldPos.y = y;
	m_vWorldPos.z = z;

	m_matWorldPos.Translation(m_vWorldPos);

	m_bUpdate = true;
}

void CTransform::SetWorldPos(const Vector3 & vPos)
{
	m_vMove += vPos - m_vWorldPos;
	m_vWorldPos = vPos;

	m_matWorldPos.Translation(m_vWorldPos);

	m_bUpdate = true;
}

void CTransform::SetLookAtAxis(LOOKAT_AXIS eAxis)
{
	m_eLookAxis = eAxis;
}

void CTransform::SetParentMatrix(const Matrix & matParent)
{
	m_matWorldParent = matParent;
}

Matrix CTransform::GetWorldRotationMatrix()	const
{
	return m_matWorldRot;
}

Matrix CTransform::GetWorldMatrix() const
{
	return m_matWorld;
}

void CTransform::Move(AXIS eAxis, float fSpeed, float fTime)
{
	m_vMove += m_vWorldAxis[eAxis] * fSpeed * fTime;
	m_vWorldPos += m_vWorldAxis[eAxis] * fSpeed * fTime;

	m_matWorldPos.Translation(m_vWorldPos);

	m_bUpdate = true;
}

void CTransform::Move(const Vector3 & vDir, float fSpeed, float fTime)
{
	m_vMove += vDir * fSpeed * fTime;
	m_vWorldPos += vDir * fSpeed * fTime;

	m_matWorldPos.Translation(m_vWorldPos);

	m_bUpdate = true;
}
void CTransform::Move(const Vector3& vMove)
{
	m_vMove += vMove;
	m_vWorldPos += vMove;

	m_matWorldPos.Translation(m_vWorldPos);

	m_bUpdate = true;
}

void CTransform::MoveBack()
{
	m_vWorldPos -= m_vMove;
	m_vMove = Vector3::Zero;

	m_matWorldPos.Translation(m_vWorldPos);

	m_bUpdate = true;

}

void CTransform::Rotation(const Vector3& vRot, float fTime)
{
	m_vWorldRot += vRot * fTime;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::Rotation(const Vector3& vRot)
{
	m_vWorldRot += vRot;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::RotationX(float fSpeed, float fTime)
{
	m_vWorldRot.x += fSpeed * fTime;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::RotationX(float fSpeed)
{
	m_vWorldRot.x += fSpeed;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::RotationY(float fSpeed, float fTime)
{
	m_vWorldRot.y += fSpeed * fTime;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::RotationY(float fSpeed)
{
	m_vWorldRot.y += fSpeed;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::RotationZ(float fSpeed, float fTime)
{
	m_vWorldRot.z += fSpeed * fTime;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::RotationZ(float fSpeed)
{
	m_vWorldRot.z += fSpeed;

	m_matWorldRot.Rotation(m_vWorldRot);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::LookAt(class CGameObject* pObj)
{
	SAFE_RELEASE(m_pLookAt);
	m_pLookAt = pObj->GetTransform();
}

void CTransform::LookAt(CComponent* pCom)
{
	if (pCom->GetComponentType() == CT_TRANSFORM)
	{
		m_pLookAt = (CTransform*)pCom;
		pCom->AddRef();
	}

	else
	{
		m_pLookAt = pCom->GetTransform();
	}
}

void CTransform::RemoveLookAt()
{
	SAFE_RELEASE(m_pLookAt);
}

void CTransform::RotationLookAt()
{
	if (!m_pLookAt)
		return;

	RotationLookAt(m_pLookAt->GetWorldPos());
}

void CTransform::RotationLookAt(Vector3 vLookAt)
{
	// 바라볼 방향을 구한다.
	Vector3	vView = m_vLocalView;
	Vector3	vPos = m_vWorldPos;

	switch (m_eLookAxis)
	{
	case LOOK_X:
		vLookAt.x = 0.f;
		vPos.x = 0.f;
		vView.x = 0.f;
		break;
	case LOOK_Y:
		vLookAt.y = 0.f;
		vPos.y = 0.f;
		vView.y = 0.f;
		break;
	case LOOK_Z:
		vLookAt.z = 0.f;
		vPos.z = 0.f;
		vView.z = 0.f;
		break;
	}

	Vector3	vDir = vLookAt - vPos;
	if (vDir == Vector3::Zero)
		return;
	vDir.Normalize();
	vView.Normalize();
	
	// 뷰와 바라볼 방향을 외적하여 회전축을 구한다.
	Vector3	vRotAxis = vView.Cross(vDir);
	vRotAxis.Normalize();

	// 뷰와 바라볼 방향을 내적하고 acosf를 이용하여 각도를 구한다.
	float	fAngle = vView.Angle(vDir);

	m_matWorldRot.RotationAxis(fAngle, vRotAxis);

	ComputeAxis();

	m_bUpdate = true;
}

void CTransform::ComputeAxis()
{
	for (int i = 0; i < AXIS_END; ++i)
	{
		// 나중에 부모 적용된 행렬로 교체
		m_vWorldAxis[i] = Vector3::Axis[i].TransformNormal(m_matWorldRot);
		m_vWorldAxis[i].Normalize();
	}

	m_vWorldView = m_vLocalView.TransformNormal(m_matWorldRot);
	m_vWorldView.Normalize();
}

void CTransform::Start()
{
}

bool CTransform::Init()
{
	m_vLocalScale = Vector3(1.f, 1.f, 1.f);
	m_vWorldScale = Vector3(1.f, 1.f, 1.f);

	for (int i = 0; i < AXIS_END; ++i)
	{
		m_vWorldAxis[i] = Vector3::Axis[i];
	}

	return true;
}

int CTransform::Input(float fTime)
{
	return 0;
}

int CTransform::Update(float fTime)
{
	return 0;
}

int CTransform::LateUpdate(float fTime)
{
	m_matLocal = m_matLocalScale * m_matLocalRot * m_matLocalPos;

	m_matWorld = m_matWorldScale * m_matWorldRot * m_matWorldPos *	m_matWorldParent;

	return 0;
}

int CTransform::Collision(float fTime)
{
	return 0;
}

int CTransform::PrevRender(float fTime)
{
	CCamera*	pCamera = nullptr;

	pCamera = m_bUI ? m_pScene->GetUICamera() : m_pScene->GetMainCamera();

	if (m_bUpdate || pCamera->GetUpdate())
	{
		m_tCBuffer.matWorldRot = m_matLocalRot * m_matWorldRot;
		m_tCBuffer.matWVRot = m_tCBuffer.matWorldRot * pCamera->GetViewMatrix();
		m_tCBuffer.matWorld = m_matLocal * m_matWorld;
		m_tCBuffer.matView = pCamera->GetViewMatrix();
		m_tCBuffer.matProj = pCamera->GetProjMatrix();
		m_tCBuffer.matInvProj = m_tCBuffer.matProj;
		m_tCBuffer.matInvProj.Inverse();
		m_tCBuffer.matWV = m_tCBuffer.matWorld * m_tCBuffer.matView;
		m_tCBuffer.matWVP = m_tCBuffer.matWV * m_tCBuffer.matProj;
		m_tCBuffer.matVP = m_tCBuffer.matView * m_tCBuffer.matProj;

		m_tCBuffer.matWorldRot.Transpose();
		m_tCBuffer.matWVRot.Transpose();
		m_tCBuffer.matWorld.Transpose();
		m_tCBuffer.matView.Transpose();
		m_tCBuffer.matProj.Transpose();
		m_tCBuffer.matInvProj.Transpose();
		m_tCBuffer.matWV.Transpose();
		m_tCBuffer.matWVP.Transpose();
		m_tCBuffer.matVP.Transpose();
	}

	SAFE_RELEASE(pCamera);

	GET_SINGLE(CShaderManager)->UpdateCBuffer("Transform", &m_tCBuffer);

	return 0;
}

int CTransform::Render(float fTime)
{
	m_vMove = Vector3::Zero;

	m_bUpdate = false;
	return 0;
}

CTransform * CTransform::Clone() const
{
	return new CTransform(*this);
}

Vector3 CTransform::GetLocalScale() const
{
	return m_vLocalScale;
}

Vector3 CTransform::GetLocalRot() const
{
	return m_vLocalRot;
}

Vector3 CTransform::GetLocalPos() const
{
	return m_vLocalPos;
}

Vector3 CTransform::GetLocalView()	const
{
	return m_vLocalView;
}

void CTransform::SetLocalScale(float x, float y, float z)
{
	m_vLocalScale.x = x;
	m_vLocalScale.y = y;
	m_vLocalScale.z = z;

	m_matLocalScale.Scaling(m_vLocalScale);

	m_bUpdate = true;
}

void CTransform::SetLocalScale(const Vector3 & vScale)
{
	m_vLocalScale = vScale;

	m_matLocalScale.Scaling(m_vLocalScale);

	m_bUpdate = true;
}

void CTransform::SetLocalRot(float x, float y, float z)
{
	m_vLocalRot.x = x;
	m_vLocalRot.y = y;
	m_vLocalRot.z = z;

	m_matLocalRot.Rotation(m_vLocalRot);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void CTransform::SetLocalRot(const Vector3 & vRot)
{
	m_vLocalRot = vRot;

	m_matLocalRot.Rotation(m_vLocalRot);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void CTransform::SetLocalRotX(float x)
{
	m_vLocalRot.x = x;

	m_matLocalRot.Rotation(m_vLocalRot);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void CTransform::SetLocalRotY(float y)
{
	m_vLocalRot.y = y;

	m_matLocalRot.Rotation(m_vLocalRot);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void CTransform::SetLocalRotZ(float z)
{
	m_vLocalRot.z = z;

	m_matLocalRot.Rotation(m_vLocalRot);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();

	m_bUpdate = true;
}

void CTransform::SetLocalPos(float x, float y, float z)
{
	m_vLocalPos.x = x;
	m_vLocalPos.y = y;
	m_vLocalPos.z = z;

	m_matLocalPos.Translation(m_vLocalPos);

	m_bUpdate = true;
}

void CTransform::SetLocalPos(const Vector3 & vPos)
{
	m_vLocalPos = vPos;

	m_matLocalPos.Translation(m_vLocalPos);

	m_bUpdate = true;
}

void CTransform::SetLocalRelativeView(float x, float y, float z)
{
	m_vLocalRelativeView = Vector3(x, y, z);

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();
}

void CTransform::SetLocalRelativeView(const Vector3& vView)
{
	m_vLocalRelativeView = vView;

	m_vLocalView = m_vLocalRelativeView.TransformNormal(m_matLocalRot);
	m_vLocalView.Normalize();
}