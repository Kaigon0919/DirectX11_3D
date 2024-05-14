#pragma once

#include "../Engine.h"

ENGINE_BEGIN

enum FRUSTUM_DIR
{
	FD_LEFT,
	FD_RIGHT,
	FD_TOP,
	FD_BOTTOM,
	FD_NEAR,
	FD_FAR,
	FD_END
};

class ENGINE_DLL CFrustum
{
	friend class CCamera;

private:
	CFrustum();
	~CFrustum();

private:
	Vector3		m_vPos[8];
	Vector4		m_vPlane[FD_END];

public:
	void Update(const Matrix& matInvVP);
	bool FrustumInPoint(const Vector3& vPos);
	bool FrustumInSphere(const Vector3& vCenter, float fRadius);
};

ENGINE_END