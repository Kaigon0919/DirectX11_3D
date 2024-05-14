#include "Frustum.h"

ENGINE_USING

CFrustum::CFrustum()
{
	m_vPos[0] = Vector3(-1.f, 1.f, 0.f);
	m_vPos[1] = Vector3(1.f, 1.f, 0.f);
	m_vPos[2] = Vector3(-1.f, -1.f, 0.f);
	m_vPos[3] = Vector3(1.f, -1.f, 0.f);

	m_vPos[4] = Vector3(-1.f, 1.f, 1.f);
	m_vPos[5] = Vector3(1.f, 1.f, 1.f);
	m_vPos[6] = Vector3(-1.f, -1.f, 1.f);
	m_vPos[7] = Vector3(1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::Update(const Matrix & matInvVP)
{
	Vector3	vPos[8];

	for (int i = 0; i < 8; ++i)
	{
		vPos[i] = m_vPos[i].TransformCoord(matInvVP);
	}

	// Left
	m_vPlane[FD_LEFT] = XMPlaneFromPoints(vPos[4].Convert(),
		vPos[0].Convert(), vPos[2].Convert());

	// Right
	m_vPlane[FD_RIGHT] = XMPlaneFromPoints(vPos[1].Convert(),
		vPos[5].Convert(), vPos[7].Convert());

	// Top
	m_vPlane[FD_TOP] = XMPlaneFromPoints(vPos[4].Convert(),
		vPos[5].Convert(), vPos[1].Convert());

	// Bottom
	m_vPlane[FD_BOTTOM] = XMPlaneFromPoints(vPos[2].Convert(),
		vPos[3].Convert(), vPos[7].Convert());

	// Near
	m_vPlane[FD_NEAR] = XMPlaneFromPoints(vPos[0].Convert(),
		vPos[1].Convert(), vPos[3].Convert());

	// Far
	m_vPlane[FD_FAR] = XMPlaneFromPoints(vPos[5].Convert(),
		vPos[4].Convert(), vPos[6].Convert());
}

bool CFrustum::FrustumInPoint(const Vector3 & vPos)
{
	for (int i = 0; i < FD_END; ++i)
	{
		float fDist = XMVectorGetX(XMPlaneDotCoord(m_vPlane[i].Convert(),
			vPos.Convert()));

		if (fDist > 0.f)
			return false;
	}

	return true;
}

bool CFrustum::FrustumInSphere(const Vector3 & vCenter, float fRadius)
{
	for (int i = 0; i < FD_END; ++i)
	{
		float fDist = XMVectorGetX(XMPlaneDotCoord(m_vPlane[i].Convert(),
			vCenter.Convert()));

		if (fDist > fRadius)
			return false;
	}

	return true;
}