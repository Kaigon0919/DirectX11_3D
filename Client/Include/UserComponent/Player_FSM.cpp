#include"Player.h"
#include "Input.h"
#include "Component/Transform.h"

void CPlayer::IdleState(float fTime)
{
	if (GET_SINGLE(CInput)->MousePress(MB_LBUTTON))
		ChangeState(PlayerState::Combo1);

	if (m_vMoveScale != Vector3::Zero)
	{
		ChangeState(PlayerState::Run);
	}

}

void CPlayer::RunState(float fTime)
{
	if (GET_SINGLE(CInput)->MousePress(MB_LBUTTON))
		ChangeState(PlayerState::Combo1);

	if (m_vMoveScale == Vector3::Zero)
	{
		ChangeState(PlayerState::Idle);
		return;
	}

	Vector3 vCamView = Vector3::Zero;
	Vector3 vPlayerView = m_pTransform->GetWorldAxis(AXIS_Z);

	// 카메라 기준, 앞 뒤로 움직일시 처리.
	if (m_vMoveScale.z != 0.0f)
	{
		Vector3 vAxisZ = m_pCameraTr->GetWorldAxis(AXIS_Z);
		if (m_vMoveScale.z < 0)
		{
			vAxisZ *= -1.f;
			m_vMoveScale.z *= -1.f;
		}
		vCamView += vAxisZ;
	}

	// 카메라 기준, 좌 우로 움직일시 처리.
	if (m_vMoveScale.x != 0.0f)
	{
		Vector3 vAxisX = m_pCameraTr->GetWorldAxis(AXIS_X);
		if (m_vMoveScale.x < 0)
		{
			vAxisX *= -1.f;
			m_vMoveScale.x *= -1.f;
		}
		vCamView += vAxisX;
	}

	// Y값을 죽인다.
	vCamView.y = vPlayerView.y = 0.0f;

	if (vCamView != Vector3::Zero && vPlayerView != Vector3::Zero)
	{
		vCamView.Normalize();
		vPlayerView.Normalize();

		float fDot = vPlayerView.Dot(vCamView);

		// nan값 대비 조건문.
		if (fDot != 1.0f)
		{
			float fAngle = vPlayerView.Angle(vCamView);
			float fDir = vPlayerView.Cross(vCamView).y;
			if (isnan(fAngle) == 0)
			{
				// 왼쪽으로 회전시.
				if (fDir < 0)
					fAngle *= -1.0f;

				m_pTransform->RotationY(fAngle);
			}
			else
				int a = 0;
		}
	}

	m_pTransform->Move(AXIS::AXIS_Z, m_fSpeed, fTime);


}

void CPlayer::Combo1State(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{
		ChangeState(PlayerState::Combo1R);
	}

	if (GET_SINGLE(CInput)->MousePress(MB_LBUTTON) && m_pAnimation->GetAniPercent() > 0.7f)
	{
		ChangeState(PlayerState::Combo2);
	}

}

void CPlayer::Combo1RState(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{

		ChangeState(PlayerState::Idle);
	}
}

void CPlayer::Combo2State(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{
		ChangeState(PlayerState::Combo2R);
	}

	if (GET_SINGLE(CInput)->MousePress(MB_LBUTTON) && m_pAnimation->GetAniPercent() > 0.7f)
	{
		ChangeState(PlayerState::Combo3);
	}
}

void CPlayer::Combo2RState(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{

		ChangeState(PlayerState::Idle);
	}
}

void CPlayer::Combo3State(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{
		ChangeState(PlayerState::Combo3R);

	}

	if (GET_SINGLE(CInput)->MousePress(MB_LBUTTON) && m_pAnimation->GetAniPercent() > 0.7f)
	{
		ChangeState(PlayerState::Combo4);
	}
}

void CPlayer::Combo3RState(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{
		ChangeState(PlayerState::Idle);
	}
}

void CPlayer::Combo4State(float fTime)
{
	if (m_pAnimation->IsAnimationEnd())
	{
		ChangeState(PlayerState::Idle);
	}
}

void CPlayer::DeathState(float fTime)
{
	if (m_pAnimation->GetCurrentClip()->strName == "Death" && m_pAnimation->IsAnimationEnd())
	{
		m_pAnimation->ChangeClip("Deathwait");
	}
}
