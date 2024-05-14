#include "Light.h"
#include "../Render/ShaderManager.h"
#include "Transform.h"
#include "Camera.h"
#include "../Scene/Scene.h"

ENGINE_USING

CLight::CLight()
{
	m_eComType = CT_LIGHT;
}

CLight::CLight(const CLight & com)	:
	CComponent(com)
{
	m_tInfo = com.m_tInfo;
}

CLight::~CLight()
{
}

LightInfo CLight::GetLightInfo() const
{
	return m_tInfo;
}

void CLight::SetLightType(LIGHT_TYPE eType)
{
	m_tInfo.iType = eType;
}

void CLight::SetLightColor(const Vector4 & vDif, const Vector4 & vAmb, 
	const Vector4 & vSpc)
{
	m_tInfo.vDif = vDif;
	m_tInfo.vAmb = vAmb;
	m_tInfo.vSpc = vSpc;
}

void CLight::SetLightDistance(float fDist)
{
	m_tInfo.fDistance = fDist;
}

void CLight::SetLightAngle(float fInAngle, float fOutAngle)
{
	m_fInAngle = fInAngle;
	m_fOutAngle = fOutAngle;

	m_tInfo.fInAngle = cosf(DegreeToRadian(fInAngle) * 0.5f);
	m_tInfo.fOutAngle = cosf(DegreeToRadian(fOutAngle) * 0.5f);
}

void CLight::Start()
{
}

bool CLight::Init()
{
	return true;
}

int CLight::Input(float fTime)
{
	return 0;
}

int CLight::Update(float fTime)
{
	switch (m_tInfo.iType)
	{
	case LT_POINT:
		m_pTransform->SetWorldScale(m_tInfo.fDistance,
			m_tInfo.fDistance, m_tInfo.fDistance);
		break;
	case LT_SPOT:
		break;
	}

	return 0;
}

int CLight::LateUpdate(float fTime)
{
	m_tInfo.vPos = m_pTransform->GetWorldPos();
	m_tInfo.vDir = m_pTransform->GetWorldAxis(AXIS_Z);

	// 조명 위치를 뷰공간으로 변환한다.
	CCamera*	pCamera = m_pScene->GetMainCamera();

	m_tInfo.vPos = m_tInfo.vPos.TransformCoord(pCamera->GetViewMatrix());
	m_tInfo.vDir = m_tInfo.vDir.TransformNormal(pCamera->GetViewMatrix());

	if (m_tInfo.iType == LT_DIR)
		m_tInfo.vDir *= -1.f;

	m_tInfo.vDir.Normalize();

	SAFE_RELEASE(pCamera);


	return 0;
}

int CLight::Collision(float fTime)
{
	return 0;
}

int CLight::PrevRender(float fTime)
{
	return 0;
}

int CLight::Render(float fTime)
{
	return 0;
}

CLight * CLight::Clone() const
{
	return new CLight(*this);
}

void CLight::SetShader()
{
	m_pTransform->PrevRender(0.f);
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Light", &m_tInfo);
}
