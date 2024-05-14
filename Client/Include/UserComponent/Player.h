#pragma once

#include "Component/UserComponent.h"
#include "Component/Animation.h"
#include "Component/Collider.h"

ENGINE_USING

class CPlayer : public CUserComponent
{
private:
	enum class PlayerState
	{
		Idle,
		Run,
		Combo1,
		Combo1R,
		Combo2,
		Combo2R,
		Combo3,
		Combo3R,
		Combo4,
		Death
	};
	enum class PlayerChangeState
	{
		PCS_OK,
		PCS_SKILL
	};
public:
	CPlayer();
	CPlayer(const CPlayer& player);
	~CPlayer();

private:
	CAnimation*	m_pAnimation;
	CTransform * m_pCameraTr;
private:
	PlayerState m_eState;
	PlayerChangeState m_eChangeState;
	void(CPlayer::*m_pFSMFunc)(float fTime);
	Vector3 m_vMoveScale;
private:
	float m_fSpeed;
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CPlayer* Clone()	const;

public:
	void MoveFront(float fScale, float fTime);
	void MoveRight(float fScale, float fTime);
	void Rotation(float fScale, float fTime);

private:
	void Hit(CCollider* pSrc, CCollider* pDest, float fTime);
	void HitStay(CCollider* pSrc, CCollider* pDest, float fTime);
	void HitLeave(CCollider* pSrc, CCollider* pDest, float fTime);

private:
	void ChangeState(PlayerState tState);
	void IdleState(float fTime);
	void RunState(float fTime);
	void Combo1State(float fTime);
	void Combo1RState(float fTime);
	void Combo2State(float fTime);
	void Combo2RState(float fTime);
	void Combo3State(float fTime);
	void Combo3RState(float fTime);
	void Combo4State(float fTime);
	void DeathState(float fTime);
};

