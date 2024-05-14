#include"Arm.h"

#include"../GameObject.h"
#include"Transform.h"
#include"../Input.h"

ENGINE_USING
CArm::CArm()
{
	m_eComType = CT_ARM;
	m_pTarget = nullptr;
	m_bMouseEnable = false;
}

CArm::CArm(const CArm & com) :
	CComponent(com)
{
	m_iRefCount = 1;
	m_pTarget = nullptr;
	m_bMouseEnable = com.m_bMouseEnable;
}

CArm::~CArm()
{
	SAFE_RELEASE(m_pTarget);
}

void CArm::SetTarget(CGameObject * pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = pTarget->GetTransform();

	if (m_vDist.Length() == 0.f)
		m_vDist = m_pTarget->GetWorldPos() - m_pTransform->GetWorldPos();

	Vector3 vView = m_pTransform->GetWorldAxis(AXIS_Z);
	Vector3 vDir = m_vDist;
	vView.Normalize();
	vDir.Normalize();
	
	if (m_vDist.z < 0.0f)
		m_vDist.z *= -1.f;


	RotateTarget(vView, vDir);
}

void CArm::SetTarget(CComponent * pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = pTarget->GetTransform();

	if (m_vDist.Length() == 0.f)
		m_vDist = m_pTarget->GetWorldPos() - m_pTransform->GetWorldPos();

	Vector3 vView = m_pTransform->GetWorldAxis(AXIS_Z);
	Vector3 vDir = m_vDist;
	vView.Normalize();
	vDir.Normalize();

	if (m_vDist.z < 0.0f)
		m_vDist.z *= -1.f;

	RotateTarget(vView, vDir);
}

void CArm::SetDist(const Vector3 & vDist)
{
	m_vDist = vDist;
}

void CArm::MouseEnable(bool bMouseEnable)
{
	m_bMouseEnable = bMouseEnable;
}

bool CArm::GetMouseEnable() const
{
	return m_bMouseEnable;
}

void CArm::Start()
{
}

bool CArm::Init()
{
	return true;
}

int CArm::Input(float fTime)
{
	return 0;
}
int CArm::Update(float fTime)
{

	if (m_bMouseEnable)
	{
		short sWheel = GET_SINGLE(CInput)->GetWheelDir();
		if (sWheel != 0)
		{
			//m_pTransform->Move(AXIS_Z, 20.f * sWheel, fTime);
			m_vDist.z -= 20.f * sWheel* fTime;
		}
		if (GET_SINGLE(CInput)->MousePush(MB_MBUTTON))
		{
			Vector2	vMove = GET_SINGLE(CInput)->GetMouseViewportMove();

			if (vMove.x != 0.f)
			{
				m_pTransform->RotationY(vMove.x);
			}

			if (vMove.y != 0.f)
			{
				m_pTransform->RotationX(vMove.y);
			}

		}
	}
	else
	{
		Vector2	vMove = GET_SINGLE(CInput)->GetMouseViewportMove();

		if (vMove.x != 0.f)
		{
			m_pTransform->RotationY(vMove.x);
		}

		if (vMove.y != 0.f)
		{
			m_pTransform->RotationX(vMove.y);
		}
	}

	return 0;
}

int CArm::LateUpdate(float fTime)
{
	//if (m_pTarget)
	//{
	//	// 타겟이 움직일 경우 따라서 움직인다.
	//	Vector3	vMove = m_pTarget->GetMove();

	//	if (vMove.Length() != 0.f)
	//	{
	//		m_pTransform->Move(vMove);
	//	}
	//}
	if (m_pTarget)
	{
		float	fDist = m_vDist.z;
		fDist = fDist < 0.0f ? 0.0f : fDist;
		m_pTransform->SetWorldPos(m_pTarget->GetWorldPos() + m_pTransform->GetWorldAxis(AXIS_Z) * -fDist);
	}
	return 0;
}

int CArm::Collision(float fTime)
{
	return 0;
}

int CArm::PrevRender(float fTime)
{
	return 0;
}

int CArm::Render(float fTime)
{
	return 0;
}

CArm * CArm::Clone() const
{
	return new CArm(*this);
}

void CArm::RotateTarget(const Vector3 & vView, const Vector3 & vDir)
{
	Vector3 vTarget = Vector3::Zero;

	Vector3 vViewTarget = vView;
	Vector3 vView2 = vView;
	Vector3 vDir2 = vDir;


	// y축 처리.
	vView2[AXIS_Y] = 0;
	vDir2[AXIS_Y] = 0;

	Matrix matRot;

	if (vView2 == Vector3::Zero || vDir2 == Vector3::Zero)
	{}
	else
	{
		vView2.Normalize();
		vDir2.Normalize();

		float fAngle = vView2.Angle(vDir2);
		vTarget[AXIS_Y] = fAngle;
		
		matRot.RotationY(fAngle);
		vViewTarget = vViewTarget.TransformNormal(matRot);
	}

	// X축 처리.
	vView2 = vViewTarget;
	vDir2 = vDir;

	vView2[AXIS_X] = 0;
	vDir2[AXIS_X] = 0;
	matRot.Identity();


	if (vView2 == Vector3::Zero || vDir2 == Vector3::Zero)
	{ }
	else
	{
		vView2.Normalize();
		vDir2.Normalize();

		float fAngle = vView2.Angle(vDir2);
		vTarget[AXIS_X] = fAngle;

		matRot.RotationX(fAngle);
		vViewTarget.TransformNormal(matRot);
	}
	m_pTransform->Rotation(vTarget);

}
