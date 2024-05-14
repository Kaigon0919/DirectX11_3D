#pragma once
#include"Component.h"

ENGINE_BEGIN
class ENGINE_DLL CArm : public CComponent
{
	friend class CGameObject;

protected:
	CArm();
	CArm(const CArm& com);
	virtual ~CArm();
private:
	class CTransform*	m_pTarget;
	Vector3				m_vDist;
	bool				m_bMouseEnable;

public:
	void SetTarget(class CGameObject* pTarget);
	void SetTarget(CComponent* pTarget);
	void SetDist(const Vector3& vDist);
	void MouseEnable(bool bEnable);
	bool GetMouseEnable()const;
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CArm* Clone()	const;
private:
	void RotateTarget(const Vector3& vView, const Vector3& vDir);
};
ENGINE_END