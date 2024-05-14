#include "Navigation.h"
#include "Transform.h"
#include "../Navigation/NavigationManager.h"
#include "../Navigation/NavigationMesh.h"
#include "../GameObject.h"

#include<iostream>
ENGINE_USING

CNavigation::CNavigation() : m_bOnCellYPosition(true), m_bMouseMove(false), m_bFind(false),m_pTarget(nullptr), m_fTargetDetectTime(0.f) , m_fTargetDetectWaitTime(1.0f)
{
	m_eComType = CT_NAVIGATION;
}

CNavigation::CNavigation(const CNavigation & com) :	CComponent(com)
{
	m_bOnCellYPosition = com.m_bOnCellYPosition;
	m_bMouseMove = com.m_bMouseMove;
	m_bFind = com.m_bFind;
	m_pTarget = nullptr;
	m_fTargetDetectTime = 0.f;
}

CNavigation::~CNavigation()
{
	SAFE_RELEASE(m_pTarget);
}

void CNavigation::SetCellYPosition(bool bEnable)
{
	m_bOnCellYPosition = bEnable;
}

void CNavigation::SetMouseMove(bool bEnable)
{
	m_bMouseMove = bEnable;
}

void CNavigation::SetAIFindPath(bool bEnable)
{
	m_bFind = bEnable;

	if (!bEnable)
	{
		m_PathList.clear();
		m_fTargetDetectTime = 0.f;
	}

	else if (m_pTarget)
		m_vTargetPos = m_pTarget->GetWorldPos();
}

void CNavigation::SetTarget(CGameObject * pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = pTarget->GetTransform();
	m_vTargetPos = m_pTarget->GetWorldPos();
	m_fTargetDetectTime = 0.f;
}

void CNavigation::SetTarget(CComponent * pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = pTarget->GetTransform();
	m_vTargetPos = m_pTarget->GetWorldPos();
	m_fTargetDetectTime = 0.f;
}

void CNavigation::SetTarget(nullptr_t pNull)
{
	SAFE_RELEASE(m_pTarget);
}

void CNavigation::SetWaitTime(float fTime)
{
	m_fTargetDetectWaitTime = fTime;
}

void CNavigation::Start()
{
}

bool CNavigation::Init()
{
	return true;
}

int CNavigation::Input(float fTime)
{
	return 0;
}

int CNavigation::Update(float fTime)
{
	if (m_bFind)
	{
		if (m_pTarget)
		{
			m_fTargetDetectTime += fTime;

			if (m_fTargetDetectTime >= m_fTargetDetectWaitTime)
			{
				m_fTargetDetectTime -= m_fTargetDetectWaitTime;
				Vector3	vTargetPos = m_pTarget->GetWorldPos();

				if (m_vTargetPos != vTargetPos)
				{
					m_vTargetPos = vTargetPos;
					m_PathList.clear();
					m_vWay = Vector3::Zero;
				}
			}
		}
		// 만약 경로가 비어있다면 길을 탐색한다.
		if (m_PathList.empty())
		{
			CNavigationMesh*	pNavMesh = GET_SINGLE(CNavigationManager)->FindNavMesh(m_pScene, m_pTransform->GetWorldPos());
			if (!pNavMesh)
				return -1;
			pNavMesh->FindPath(m_pTransform->GetWorldPos(), m_pTarget->GetWorldPos());
			m_PathList = pNavMesh->GetPathList();
			m_vWay = m_PathList.front();
			m_PathList.pop_front();

			SAFE_RELEASE(pNavMesh);


		}

		Vector3	vDir = m_vWay - m_pTransform->GetWorldPos();
		vDir.Normalize();

		float	fDist = m_vWay.Distance(m_pTransform->GetWorldPos());

		float	fSpeed = 3.f * fTime;

		if (fDist < fSpeed)
		{
			fSpeed = fDist;

			if (!m_PathList.empty())
			{
				m_vWay = m_PathList.front();
				m_PathList.pop_front();
			}
			else
			{
				m_vWay = Vector3::Zero;
				m_bFind = false;
			}
		}

		m_pTransform->Move(vDir, 3.f, fTime);
	}

	return 0;
}

int CNavigation::LateUpdate(float fTime)
{
	if (m_bOnCellYPosition)
	{
		Vector3	vPos = m_pTransform->GetWorldPos();
		CNavigationMesh* pNavMesh = GET_SINGLE(CNavigationManager)->FindNavMesh(m_pScene, vPos);

		if (pNavMesh)
		{
			if (pNavMesh->GetCellEnable(vPos))
			{
				vPos.y = pNavMesh->GetY(vPos);

				m_pTransform->SetWorldPos(vPos);
			}

			else if (!m_bMouseMove && !m_bFind) 
			{
				m_pTransform->MoveBack();
			}

			SAFE_RELEASE(pNavMesh);
		}
	}

	return 0;
}

int CNavigation::Collision(float fTime)
{
	return 0;
}

int CNavigation::PrevRender(float fTime)
{
	return 0;
}

int CNavigation::Render(float fTime)
{
	return 0;
}

CNavigation * CNavigation::Clone() const
{
	return new CNavigation(*this);
}