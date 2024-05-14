#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL __declspec(align(16)) CTransform :
	public CComponent
{
	friend class CGameObject;

private:
	CTransform();
	CTransform(const CTransform& com);
	~CTransform();

private:
	bool	m_bStatic;
	bool	m_bUpdate;
	bool	m_bUI;
	TransformCBuffer	m_tCBuffer;

public:
	bool GetStatic()	const;
	bool GetUpdate()	const;
	bool GetUI()	const;

public:
	void UIEnable(bool bEnable = true);
private:
	Vector3	m_vLocalScale;
	Vector3	m_vLocalRot;
	Vector3	m_vLocalPos;
	Vector3	m_vLocalRelativeView;
	Vector3	m_vLocalView;

public:
	Vector3 GetLocalScale()	const;
	Vector3 GetLocalRot()	const;
	Vector3 GetLocalPos()	const;
	Vector3 GetLocalView()	const;

public:
	void SetLocalScale(float x, float y, float z);
	void SetLocalScale(const Vector3& vScale);
	void SetLocalRot(float x, float y, float z);
	void SetLocalRot(const Vector3& vRot);
	void SetLocalRotX(float x);
	void SetLocalRotY(float y);
	void SetLocalRotZ(float z);
	void SetLocalPos(float x, float y, float z);
	void SetLocalPos(const Vector3& vPos);
	void SetLocalRelativeView(float x, float y, float z);
	void SetLocalRelativeView(const Vector3& vView);

private:
	Matrix	m_matLocalScale;
	Matrix	m_matLocalRot;
	Matrix	m_matLocalPos;
	Matrix	m_matLocal;

public:
	Matrix GetLocalRotMatrix()	const;
	Matrix GetLocalMatrix()	const;

private:
	Vector3	m_vWorldScale;
	Vector3	m_vWorldRot;
	Vector3	m_vWorldPos;
	Vector3	m_vWorldAxis[AXIS_END];
	Vector3	m_vWorldView;
	Vector3	m_vMove;
	CTransform*	m_pLookAt;
	LOOKAT_AXIS	m_eLookAxis;

public:
	Vector3 GetWorldScale()	const;
	Vector3 GetWorldRot()	const;
	Vector3 GetWorldPos()	const;
	Vector3 GetWorldAxis(AXIS axis)	const;
	Vector3 GetWorldView()	const;
	Vector3 GetMove()	const;

public:
	void SetWorldScale(float x, float y, float z);
	void SetWorldScale(const Vector3& vScale);
	void SetWorldRot(float x, float y, float z);
	void SetWorldRot(const Vector3& vRot);
	void SetWorldRotX(float x);
	void SetWorldRotY(float y);
	void SetWorldRotZ(float z);
	void SetWorldPos(float x, float y, float z);
	void SetWorldPos(const Vector3& vPos);
	void SetLookAtAxis(LOOKAT_AXIS eAxis);

private:
	Matrix	m_matWorldScale;
	Matrix	m_matWorldRot;
	Matrix	m_matWorldPos;
	Matrix	m_matWorldParent;
	Matrix	m_matWorld;
	
public:
	void SetParentMatrix(const Matrix& matParent);

public:
	Matrix GetWorldRotationMatrix()	const;
	Matrix GetWorldMatrix()	const;

public:
	void Move(AXIS eAxis, float fSpeed, float fTime);
	void Move(const Vector3& vDir, float fSpeed, float fTime);
	void MoveBack();
	void Move(const Vector3& vMove);
	void Rotation(const Vector3& vRot, float fTime);
	void Rotation(const Vector3& vRot);
	void RotationX(float fSpeed, float fTime);
	void RotationX(float fSpeed);
	void RotationY(float fSpeed, float fTime);
	void RotationY(float fSpeed);
	void RotationZ(float fSpeed, float fTime);
	void RotationZ(float fSpeed);
	void LookAt(class CGameObject* pObj);
	void LookAt(CComponent* pCom);
	void RemoveLookAt();
	void RotationLookAt();
	void RotationLookAt(Vector3 vLookAt);

private:
	void ComputeAxis();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CTransform* Clone()	const;
};

ENGINE_END
