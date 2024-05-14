#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL __declspec(align(16)) CCamera :
	public CComponent
{
	friend class CGameObject;

private:
	CCamera();
	CCamera(const CCamera& camera);
	~CCamera();

private:
	Matrix	m_matView;
	Matrix	m_matProj;
	CAMERA_TYPE	m_eCameraType;
	UINT	m_iWidth;
	UINT	m_iHeight;
	float	m_fAngle;
	float	m_fNear;
	float	m_fFar;
	bool	m_bUpdate;
	class CFrustum*	m_pFrustum;
public:
	void SetCameraInfo(CAMERA_TYPE eType, UINT iWidth, UINT iHeight,
		float fAngle, float fNear, float fFar);
	void SetCameraType(CAMERA_TYPE eType);

public:
	bool GetUpdate()	const;
	Matrix GetViewMatrix()	const;
	Matrix GetProjMatrix()	const;

public:	
	bool FrustumInPoint(const Vector3& vPos);
	bool FrustumInSphere(const Vector3& vCenter, float fRadius);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CCamera* Clone()	const;
};

ENGINE_END
