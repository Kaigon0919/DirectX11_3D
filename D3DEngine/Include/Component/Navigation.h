#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CNavigation :
	public CComponent
{
	friend class CGameObject;

protected:
	CNavigation();
	CNavigation(const CNavigation& com);
	~CNavigation();

private:
	bool				m_bOnCellYPosition;
	bool				m_bMouseMove;
	bool				m_bFind;
	list<Vector3>		m_PathList;
	class CTransform*	m_pTarget;
	Vector3				m_vWay;
	Vector3			m_vTargetPos;
	float			m_fTargetDetectTime;
	float			m_fTargetDetectWaitTime;

public:
	void SetCellYPosition(bool bEnable);
	void SetMouseMove(bool bEnable);
	void SetAIFindPath(bool bEnable);
	void SetTarget(class CGameObject* pTarget);
	void SetTarget(class CComponent* pTarget);
	void SetTarget(nullptr_t pNull);
	void SetWaitTime(float fTime);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CNavigation* Clone()	const;
};

ENGINE_END