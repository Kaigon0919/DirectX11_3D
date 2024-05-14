#include "Camera.h"
#include "Transform.h"
#include "Frustum.h"

ENGINE_USING

CCamera::CCamera() : m_bUpdate(true)
{
	m_eComType = CT_CAMERA;
	m_pFrustum = new CFrustum;
}

CCamera::CCamera(const CCamera & camera)	:
	CComponent(camera)
{
	*this = camera;
	m_iRefCount = 1;
	m_bUpdate = true;
	m_pFrustum = new CFrustum;
}

CCamera::~CCamera()
{
	SAFE_DELETE(m_pFrustum);
}

void CCamera::SetCameraInfo(CAMERA_TYPE eType, UINT iWidth, UINT iHeight, float fAngle, float fNear, float fFar)
{
	m_eCameraType = eType;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_fAngle = fAngle;
	m_fNear = fNear;
	m_fFar = fFar;
	SetCameraType(eType);

}

void CCamera::SetCameraType(CAMERA_TYPE eType)
{
	switch (eType)
	{
	case CT_PERSPECTIVE:
		m_matProj = XMMatrixPerspectiveFovLH(DegreeToRadian(m_fAngle),	m_iWidth / (float)m_iHeight, m_fNear, m_fFar);
		break;
	case CT_ORTHO:
		m_matProj = XMMatrixOrthographicOffCenterLH(0.f, (float)m_iWidth, 0.f, (float)m_iHeight, 0.f, m_fFar);
		break;
	}
}
bool CCamera::GetUpdate()	const
{
	return m_bUpdate;
}

Matrix CCamera::GetViewMatrix() const
{
	return m_matView;
}

Matrix CCamera::GetProjMatrix() const
{
	return m_matProj;
}
bool CCamera::FrustumInPoint(const Vector3& vPos)
{
	return m_pFrustum->FrustumInPoint(vPos);
}

bool CCamera::FrustumInSphere(const Vector3& vCenter, float fRadius)
{
	return m_pFrustum->FrustumInSphere(vCenter, fRadius);
}
void CCamera::Start()
{
}

bool CCamera::Init()
{
	return true;
}

int CCamera::Input(float fTime)
{
	return 0;
}

int CCamera::Update(float fTime)
{
	m_bUpdate = false;
	return 0;
}

int CCamera::LateUpdate(float fTime)
{
	return 0;
}

int CCamera::Collision(float fTime)
{
	return 0;
}

int CCamera::PrevRender(float fTime)
{
	if (m_pTransform->GetUpdate() && !m_pTransform->GetUI()) 
	{
		m_bUpdate = true;
		m_matView.Identity();

		// 11, 12, 13 = Xรเ
		// 21, 22, 23 = Yรเ
		// 31, 32, 33 = Zรเ
		for (int i = 0; i < AXIS_END; ++i)
		{
			memcpy(&m_matView[i][0], &m_pTransform->GetWorldAxis((AXIS)i), sizeof(Vector3));
		}

		m_matView.Transpose();

		for (int i = 0; i < AXIS_END; ++i)
		{
			m_matView[3][i] = -m_pTransform->GetWorldPos().Dot(m_pTransform->GetWorldAxis((AXIS)i));
		}
	}
	Matrix	matVP = m_matView * m_matProj;
	matVP.Inverse();
	m_pFrustum->Update(matVP);

	return 0;
}


int CCamera::Render(float fTime)
{
	return 0;
}

CCamera * CCamera::Clone() const
{
	return new CCamera(*this);
}
