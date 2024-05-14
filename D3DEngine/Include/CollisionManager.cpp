#include "CollisionManager.h"
#include "GameObject.h"
#include "Input.h"
#include "Component/ColliderRay.h"
#include "Core.h"
#include "Scene/SceneManager.h"
#include <algorithm>

using namespace std;

ENGINE_USING

DEFINITION_SINGLE(CCollisionManager)

CCollisionManager::CCollisionManager()
{
	m_pSection = nullptr;
	m_pPickSection = nullptr;
	m_pUISection = nullptr;
	m_pPrevMousePick = nullptr;
	m_pPrevMouseCollider = nullptr;

	m_pMouseCollisionList = new ColliderList;

	m_iSerialNumber = 0;

	m_iProfileSize = 0;
	m_iProfileCapacity = 16;
	m_pProfileList = new CollisionProfile[m_iProfileCapacity];

	m_iChannelSize = 0;
	m_pChannelList = new CollisionChannel[MAX_COLLISION_CHANNEL];

	m_iValidSize = 0;
	m_iValidCapacity = 100;

	m_pValidNumber = new unsigned int[m_iValidCapacity];
	memset(m_pValidNumber, 0, sizeof(unsigned int) * m_iValidCapacity);
}


CCollisionManager::~CCollisionManager()
{
	SAFE_DELETE(m_pMouseCollisionList);
	SAFE_DELETE_ARRAY(m_pValidNumber);
	SAFE_DELETE_ARRAY(m_pChannelList);
	SAFE_DELETE_ARRAY(m_pProfileList);
	SAFE_DELETE(m_pSection);
	SAFE_DELETE(m_pPickSection);
	SAFE_DELETE(m_pUISection);
}

unsigned int CCollisionManager::GetSerialNumber()
{
	if (m_iValidSize == 0)
	{
		if (m_iSerialNumber == m_tColliderInfo.iCapacity)
		{
			m_tColliderInfo.Resize();
		}

		unsigned int iNumber = m_iSerialNumber;
		++m_iSerialNumber;
		return iNumber;
	}

	--m_iValidSize;

	return m_pValidNumber[m_iValidSize];
}

void CCollisionManager::AddValidSerialNumber(unsigned int iNumber)
{
	if (m_iValidSize == m_iValidCapacity)
	{
		m_iValidCapacity *= 2;

		unsigned int*	pNumber = new unsigned int[m_iValidCapacity];

		memcpy(pNumber, m_pValidNumber, sizeof(unsigned int) * m_iValidSize);

		SAFE_DELETE_ARRAY(m_pValidNumber);

		m_pValidNumber = pNumber;
	}

	m_pValidNumber[m_iValidSize] = iNumber;
	++m_iValidSize;

	m_tColliderInfo.pColliderList[iNumber] = nullptr;
	--m_tColliderInfo.iSize;
}

void CCollisionManager::AddPrevCollider(unsigned int iSrc,
	unsigned int iDest)
{
	unsigned int	iSrcIdx = iSrc / 32;
	unsigned int	iDestIdx = iDest / 32;

	unsigned int	iSrcBit = 31 - (iSrc % 32);
	unsigned int	iDestBit = 31 - (iDest % 32);

	unsigned int	iSrcValue = 1 << iSrcBit;
	unsigned int	iDestValue = 1 << iDestBit;

	m_tColliderInfo.pAdjMatrix[iSrc][iDestIdx] |= iDestValue;
	m_tColliderInfo.pAdjMatrix[iDest][iSrcIdx] |= iSrcValue;
}

void CCollisionManager::DeletePrevCollider(unsigned int iSrc, unsigned int iDest)
{
	unsigned int	iSrcIdx = iSrc / 32;
	unsigned int	iDestIdx = iDest / 32;

	unsigned int	iSrcBit = 31 - (iSrc % 32);
	unsigned int	iDestBit = 31 - (iDest % 32);

	unsigned int	iSrcValue = 1 << iSrcBit;
	unsigned int	iDestValue = 1 << iDestBit;

	if (m_tColliderInfo.pAdjMatrix[iSrc][iDestIdx] & iDestValue)
		m_tColliderInfo.pAdjMatrix[iSrc][iDestIdx] ^= iDestValue;

	if (m_tColliderInfo.pAdjMatrix[iDest][iSrcIdx] & iSrcValue)
		m_tColliderInfo.pAdjMatrix[iDest][iSrcIdx] ^= iSrcValue;
}

bool CCollisionManager::CheckPrevCollider(unsigned int iSrc, unsigned int iDest)
{
	unsigned int	iDestIdx = iDest / 32;

	unsigned int	iDestBit = 31 - (iDest % 32);

	unsigned int	iDestValue = 1 << iDestBit;

	if (m_tColliderInfo.pAdjMatrix[iSrc][iDestIdx] & iDestValue)
		return true;

	return false;
}

void CCollisionManager::CreateSection(int iNumX, int iNumY, int iNumZ, const Vector3 & vCellSize, const Vector3& vPos, const Vector3& vPivot)
{
	SAFE_DELETE(m_pSection);

	m_pSection = new CollisionSection;

	m_pSection->iNumX = iNumX;
	m_pSection->iNumY = iNumY;
	m_pSection->iNumZ = iNumZ;
	m_pSection->vCellSize = vCellSize;

	m_pSection->pSectionList = new ColliderList[iNumX * iNumY * iNumZ];

	m_pSection->vLength = m_pSection->vCellSize * Vector3(iNumX, iNumY, iNumZ);

	m_vPos = vPos;
	m_vPivot = vPivot;

	m_pSection->vMin = m_vPos - m_vPivot * m_pSection->vLength;
	m_pSection->vMax = m_vPos + (Vector3(1.f, 1.f, 1.f) - m_vPivot) * m_pSection->vLength;

	if (GET_SINGLE(CCore)->GetEditMode())
	{
		SAFE_DELETE(m_pPickSection);
		m_pPickSection = new CollisionSection;

		m_pPickSection->iNumX = iNumX;
		m_pPickSection->iNumY = iNumY;
		m_pPickSection->iNumZ = iNumZ;
		m_pPickSection->vCellSize = vCellSize;

		m_pPickSection->pSectionList = new ColliderList[iNumX * iNumY * iNumZ];
		m_pPickSection->vLength = m_pPickSection->vCellSize * Vector3(iNumX, iNumY, iNumZ);
		m_pPickSection->vMin = m_vPos - m_vPivot * m_pPickSection->vLength;
		m_pPickSection->vMax = m_vPos + (Vector3(1.f, 1.f, 1.f) - m_vPivot) * m_pPickSection->vLength;
	}
}

bool CCollisionManager::AddChannel(const string & strName,
	COLLISION_CHANNEL_STATE eState)
{
	if (m_iChannelSize == MAX_COLLISION_CHANNEL)
		return false;

	m_pChannelList[m_iChannelSize].strName = strName;
	m_pChannelList[m_iChannelSize].eState = eState;

	// 현재 생성되어 있는 모든 Profile에 추가된 채널 정보를 추가한다.
	for (unsigned int i = 0; i < m_iProfileSize; ++i)
	{
		m_pProfileList[i].pChannelList[m_iChannelSize].strName = strName;
		m_pProfileList[i].pChannelList[m_iChannelSize].eState = eState;
		++m_pProfileList[i].iChannelSize;
	}

	++m_iChannelSize;

	return true;
}

bool CCollisionManager::DeleteChannel(const string & strName)
{
	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		if (m_pChannelList[i].strName == strName)
		{
			--m_iChannelSize;

			for (unsigned int j = i; j < m_iChannelSize; ++j)
			{
				m_pChannelList[j] = m_pChannelList[j + 1];
			}

			for (unsigned int j = 0; j < m_iProfileSize; ++j)
			{
				--m_pProfileList[j].iChannelSize;
				for (unsigned int k = i; k < m_iChannelSize; ++k)
				{
					m_pProfileList[j].pChannelList[k] = m_pProfileList[j].pChannelList[k + 1];
				}
			}

			break;
		}
	}

	return true;
}

bool CCollisionManager::CreateProfile(const string & strName)
{
	if (m_iProfileSize == m_iProfileCapacity)
	{
		m_iProfileCapacity *= 2;

		PCollisionProfile	pProfileList = new CollisionProfile[m_iProfileCapacity];

		for (unsigned int i = 0; i < m_iProfileSize; ++i)
		{
			pProfileList[i].strName = m_pProfileList[i].strName;

			for (unsigned int j = 0; j < m_pProfileList[i].iChannelSize; ++j)
			{
				pProfileList[i].pChannelList[j] = m_pProfileList[i].pChannelList[j];
			}

			pProfileList[i].iChannelSize = m_pProfileList[i].iChannelSize;
		}

		SAFE_DELETE_ARRAY(m_pProfileList);

		m_pProfileList = pProfileList;
	}

	m_pProfileList[m_iProfileSize].strName = strName;
	m_pProfileList[m_iProfileSize].iChannelSize = m_iChannelSize;

	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		m_pProfileList[m_iProfileSize].pChannelList[i] =
			m_pChannelList[i];
	}

	++m_iProfileSize;

	return true;
}

bool CCollisionManager::SetProfileChannelState(
	const string & strProfile, const string & strChannel,
	COLLISION_CHANNEL_STATE eState)
{
	for (unsigned int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfileList[i].strName == strProfile)
		{
			for (unsigned int j = 0; j < m_iChannelSize; ++j)
			{
				if (m_pProfileList[i].pChannelList[j].strName == strChannel)
				{
					m_pProfileList[i].pChannelList[j].eState = eState;
					return true;
				}
			}
		}
	}

	return false;
}

PCollisionProfile CCollisionManager::FindProfile(const string & strName)
{
	for (unsigned int i = 0; i < m_iProfileSize; ++i)
	{
		if (m_pProfileList[i].strName == strName)
			return &m_pProfileList[i];
	}

	return nullptr;
}

unsigned int CCollisionManager::GetChannelIndex(const string & strName)
{
	for (unsigned int i = 0; i < m_iChannelSize; ++i)
	{
		if (m_pChannelList[i].strName == strName)
			return i;
	}

	return UINT_MAX;
}

void CCollisionManager::AddCollider(CGameObject * pObj)
{
	if (!pObj->IsActive())
		return;

	const list<CCollider*>*	pColliderList = pObj->GetColliderList();

	list<CCollider*>::const_iterator	iter;
	list<CCollider*>::const_iterator	iterEnd = pColliderList->end();

	if (pColliderList->empty())
		return;

	else if (!pObj->IsEnable())
	{
		for (iter = pColliderList->begin(); iter != iterEnd; ++iter)
		{
			(*iter)->ClearState();
		}

		return;
	}

	for (iter = pColliderList->begin(); iter != iterEnd; ++iter)
	{
		CCollider*	pCollider = *iter;

		if (!pCollider->IsEnable())
		{
			pCollider->ClearState();
			continue;
		}

		else if (pCollider->GetPickEnable())
		{
			pCollider->ClearState();
			continue;
		}

		// 충돌체의 Min, Max 값을 얻어온다.
		Vector3	vCollMin = pCollider->GetSectionMin();
		Vector3	vCollMax = pCollider->GetSectionMax();

		// 전체 영역의 Min값을 제거해서 0, 0, 0으로 만들어준다.
		vCollMin -= m_pSection->vMin;
		vCollMax -= m_pSection->vMin;

		// 1, 1, 1 공간으로 변환한다.
		vCollMin /= m_pSection->vCellSize;
		vCollMax /= m_pSection->vCellSize;

		int	iStartX = -1, iStartY = -1, iStartZ = -1;
		int	iEndX = -1, iEndY = -1, iEndZ = -1;

		iStartX = (int)vCollMin.x;
		iStartY = (int)vCollMin.y;
		iStartZ = (int)vCollMin.z;

		iEndX = (int)vCollMax.x;
		iEndY = (int)vCollMax.y;
		iEndZ = (int)vCollMax.z;

		if (iStartX >= m_pSection->iNumX)
			continue;

		else if (iStartY >= m_pSection->iNumY)
			continue;

		else if (iStartZ >= m_pSection->iNumZ)
			continue;

		iStartX = iStartX < 0 ? 0 : iStartX;
		iStartY = iStartY < 0 ? 0 : iStartY;
		iStartZ = iStartZ < 0 ? 0 : iStartZ;

		if (iEndX < 0)
			continue;

		else if (iEndY < 0)
			continue;

		else if (iEndZ < 0)
			continue;

		iEndX = iEndX >= m_pSection->iNumX ? m_pSection->iNumX - 1 : iEndX;
		iEndY = iEndY >= m_pSection->iNumY ? m_pSection->iNumY - 1 : iEndY;
		iEndZ = iEndZ >= m_pSection->iNumZ ? m_pSection->iNumZ - 1 : iEndZ;

		for (int z = iStartZ; z <= iEndZ; ++z)
		{
			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
						y * m_pSection->iNumX + x;

					PColliderList	pSection = &m_pSection->pSectionList[idx];

					if (pSection->iSize == pSection->iCapacity)
					{
						pSection->iCapacity *= 2;

						CCollider**	pList = new CCollider*[pSection->iCapacity];

						memcpy(pList, pSection->pList, sizeof(CCollider*) * pSection->iSize);

						SAFE_DELETE_ARRAY(pSection->pList);

						pSection->pList = pList;
					}

					pSection->pList[pSection->iSize] = pCollider;
					++pSection->iSize;
				}
			}
		}
	}
}

void CCollisionManager::AddCollider(CCollider * pCollider)
{
	m_tColliderInfo.pColliderList[pCollider->GetSerialNumber()] =
		pCollider;
	++m_tColliderInfo.iSize;
}

CCollider * CCollisionManager::FindCollider(unsigned int iSerialNumber)
{
	return m_tColliderInfo.pColliderList[iSerialNumber];
}

void CCollisionManager::DeleteCollider(unsigned int iSerialNumber)
{
	m_tColliderInfo.pColliderList[iSerialNumber] = nullptr;
}

void CCollisionManager::ComputeSection()
{
	for (unsigned iColl = 0; iColl < m_tColliderInfo.iSize + m_iValidSize; ++iColl)
	{
		CCollider*	pCollider = m_tColliderInfo.pColliderList[iColl];

		if (!pCollider)
			continue;

		if (!pCollider->IsEnable() || !pCollider->IsObjectEnable())
		{
			pCollider->ClearState();
			continue;
		}

		// 충돌체의 Min, Max 값을 얻어온다.
		Vector3	vCollMin = pCollider->GetSectionMin();
		Vector3	vCollMax = pCollider->GetSectionMax();

		// 전체 영역의 Min값을 제거해서 0, 0, 0으로 만들어준다.
		vCollMin -= m_pSection->vMin;
		vCollMax -= m_pSection->vMin;

		// 1, 1, 1 공간으로 변환한다.
		vCollMin /= m_pSection->vCellSize;
		vCollMax /= m_pSection->vCellSize;

		int	iStartX = -1, iStartY = -1, iStartZ = -1;
		int	iEndX = -1, iEndY = -1, iEndZ = -1;

		iStartX = (int)vCollMin.x;
		iStartY = (int)vCollMin.y;
		iStartZ = (int)vCollMin.z;

		iEndX = (int)vCollMax.x;
		iEndY = (int)vCollMax.y;
		iEndZ = (int)vCollMax.z;

		if (iStartX >= m_pSection->iNumX || iStartY >= m_pSection->iNumY ||
			iStartZ >= m_pSection->iNumZ || iEndX < 0 || iEndY < 0 ||
			iEndZ < 0)
		{
			pCollider->ClearState();
			continue;
		}

		iStartX = iStartX < 0 ? 0 : iStartX;
		iStartY = iStartY < 0 ? 0 : iStartY;
		iStartZ = iStartZ < 0 ? 0 : iStartZ;

		iEndX = iEndX >= m_pSection->iNumX ? m_pSection->iNumX - 1 : iEndX;
		iEndY = iEndY >= m_pSection->iNumY ? m_pSection->iNumY - 1 : iEndY;
		iEndZ = iEndZ >= m_pSection->iNumZ ? m_pSection->iNumZ - 1 : iEndZ;

		for (int z = iStartZ; z <= iEndZ; ++z)
		{
			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
						y * m_pSection->iNumX + x;

					PColliderList	pSection = &m_pSection->pSectionList[idx];

					if (pCollider->GetPickEnable())
					{
						if (GET_SINGLE(CCore)->GetEditMode())
							pSection = &m_pPickSection->pSectionList[idx];
						else
							continue;
					}

					if (pSection->iSize == pSection->iCapacity)
					{
						pSection->iCapacity *= 2;

						CCollider**	pList = new CCollider*[pSection->iCapacity];

						memcpy(pList, pSection->pList, sizeof(CCollider*) * pSection->iSize);

						SAFE_DELETE_ARRAY(pSection->pList);

						pSection->pList = pList;
					}

					pSection->pList[pSection->iSize] = pCollider;
					++pSection->iSize;
				}
			}
		}
	}
}

bool CCollisionManager::Init()
{
	CreateSection(5, 5, 5, Vector3(30.f, 30.f, 30.f));

	CreateProfile("WorldStatic");
	CreateProfile("WorldDynamic");

	AddChannel("WorldStatic");
	AddChannel("WorldDynamic");

	SetProfileChannelState("WorldStatic", "WorldStatic", CCS_IGNORE);
	SetProfileChannelState("WorldStatic", "WorldDynamic", CCS_BLOCK);

	SetProfileChannelState("WorldDynamic", "WorldStatic", CCS_BLOCK);
	SetProfileChannelState("WorldDynamic", "WorldDynamic", CCS_BLOCK);

	return true;
}

void CCollisionManager::Collision(float fTime)
{
	// 충돌 전에 공간을 분리해준다.
	ComputeSection();

	bool	bCollision = CollisionMouseUI(fTime);

	if (!bCollision)
	{
		CollisionMouseWorld(fTime);
	}

	CollisionUI(fTime);
	CollisionWorld(fTime);
	if (m_pPickSection)
	{
		for (int i = 0; i < m_pPickSection->iNumX * m_pPickSection->iNumY * m_pPickSection->iNumZ; ++i)
		{
			m_pPickSection->pSectionList[i].Clear();
		}
	}
}

void CCollisionManager::Render(float fTime)
{
	for (unsigned iColl = 0; iColl < m_tColliderInfo.iSize + m_iValidSize;)
	{
		CCollider*	pCollider = m_tColliderInfo.pColliderList[iColl];

		if (!pCollider)
		{
			++iColl;
			continue;
		}

		if (!pCollider->IsEnable())
		{
			++iColl;
			continue;
		}

		else if (!pCollider->IsActive())
		{
			++iColl;
			continue;
		}

		pCollider->PrevRender(fTime);
		pCollider->Render(fTime);
		++iColl;
	}
}

bool CCollisionManager::CollisionMouseUI(float fTime)
{
	return false;
}

bool CCollisionManager::CollisionMouseWorld(float fTime)
{
	// 마우스 광선을 얻어온다.
	CColliderRay*	pMouseRay = GET_SINGLE(CInput)->GetMouseRay();

	RayInfo	tRayInfo = pMouseRay->GetInfo();

	Vector3	vCollMin, vCollMax;
	Vector3	vPos1, vPos2;
	vPos1 = tRayInfo.vOrigin;
	vPos2 = tRayInfo.vOrigin + tRayInfo.vDir * 1500.f;

	vCollMin.x = vPos1.x < vPos2.x ? vPos1.x : vPos2.x;
	vCollMin.y = vPos1.y < vPos2.y ? vPos1.y : vPos2.y;
	vCollMin.z = vPos1.z < vPos2.z ? vPos1.z : vPos2.z;

	vCollMax.x = vPos1.x > vPos2.x ? vPos1.x : vPos2.x;
	vCollMax.y = vPos1.y > vPos2.y ? vPos1.y : vPos2.y;
	vCollMax.z = vPos1.z > vPos2.z ? vPos1.z : vPos2.z;

	vCollMin /= m_pSection->vCellSize;
	vCollMax /= m_pSection->vCellSize;

	int	iStartX = -1, iStartY = -1, iStartZ = -1;
	int	iEndX = -1, iEndY = -1, iEndZ = -1;

	iStartX = (int)vCollMin.x;
	iStartY = (int)vCollMin.y;
	iStartZ = (int)vCollMin.z;

	iEndX = (int)vCollMax.x;
	iEndY = (int)vCollMax.y;
	iEndZ = (int)vCollMax.z;

	iStartX = iStartX < 0 ? 0 : iStartX;
	iStartY = iStartY < 0 ? 0 : iStartY;
	iStartZ = iStartZ < 0 ? 0 : iStartZ;

	iEndX = iEndX >= m_pSection->iNumX ? m_pSection->iNumX - 1 : iEndX;
	iEndY = iEndY >= m_pSection->iNumY ? m_pSection->iNumY - 1 : iEndY;
	iEndZ = iEndZ >= m_pSection->iNumZ ? m_pSection->iNumZ - 1 : iEndZ;

	bool	bEditPick = false;

	if (GET_SINGLE(CCore)->GetEditMode())
	{
		for (int z = iStartZ; z <= iEndZ; ++z)
		{
			for (int y = iStartY; y <= iEndY; ++y)
			{
				for (int x = iStartX; x <= iEndX; ++x)
				{
					int	idx = z * (m_pPickSection->iNumX * m_pPickSection->iNumY) +
						y * m_pPickSection->iNumX + x;

					PColliderList	pSection = &m_pPickSection->pSectionList[idx];

					for (int i = 0; i < pSection->iSize; ++i)
					{
						m_pMouseCollisionList->Add(pSection->pList[i]);
					}
				}
			}
		}

		// 추가된 충돌체들을 거리 순으로 오름차순 정렬한다.
		qsort(m_pMouseCollisionList->pList, m_pMouseCollisionList->iSize, sizeof(CCollider*), CCollisionManager::SortZ);

		for (int i = 0; i < m_pMouseCollisionList->iSize; ++i)
		{
			CCollider*	pDest = m_pMouseCollisionList->pList[i];

			if (pMouseRay->Collision(pDest))
			{
				if (m_pPrevMousePick && m_pPrevMousePick != pDest)
				{
					pMouseRay->DeleteSerialNumber(m_pPrevMousePick->GetSerialNumber());
					m_pPrevMousePick->DeleteSerialNumber(pMouseRay->GetSerialNumber());

					DeletePrevCollider(pMouseRay->GetSerialNumber(), m_pPrevMousePick->GetSerialNumber());
				}
				// 각 충돌체에 서로 충돌된 충돌체라고 등록해준다.
				// 현재 영역에서 충돌되었다고 등록을 해준다.
				// 이 리스트는 다음 영역에서 충돌체크시에 이전 영역에서
				// 충돌되었다면 빠져나가기 위함이다.
				// 왜냐하면 지금 서로 다른 영역에 걸쳐있을때 앞에 체크한
				// 영역에서 만약 충돌이 되었다면 이 충돌체는 계속
				// 충돌상태라고 표현이 되어버리므로 이전 영역에서
				// 충돌이 되었는지를 판단해야 한다.
				pMouseRay->AddCollisionList(pDest->GetSerialNumber());
				pDest->AddCollisionList(pMouseRay->GetSerialNumber());

				// 이전에 충돌되었는지를 판단한다.
				// 처음 충돌될 경우
				if (!CheckPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// 충돌 매트릭스에 이전충돌목록으로
					// 등록해준다.
					AddPrevCollider(pMouseRay->GetSerialNumber(),
						pDest->GetSerialNumber());

					// 각 충돌체에 이전 충돌목록으로 등록한다.
					pMouseRay->AddSerialNumber(pDest->GetSerialNumber());
					pDest->AddSerialNumber(pMouseRay->GetSerialNumber());

					// 처음 충돌되었으므로 처음 충돌되었을때 호출할
					// 콜백을 처리한다.
					pMouseRay->Call(CCS_BEGIN, pDest, fTime);
					pDest->Call(CCS_BEGIN, pMouseRay, fTime);
				}

				// 이전 충돌목록에 있을 경우 계속 충돌상태로
				// 처리한다.
				else
				{
					// 콜백을 처리한다.
					pMouseRay->Call(CCS_STAY, pDest, fTime);
					pDest->Call(CCS_STAY, pMouseRay, fTime);
				}

				bEditPick = true;
				m_pPrevMousePick = pDest;
				break;
			}

			// 충돌이 안된 상태일 경우
			else
			{
				// 이전 충돌목록에 서로 존재할 경우 충돌 되다가
				// 떨어진다는 것이다.
				if (CheckPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber()))
				{
					// 이전 충돌목록에서 제거해준다.
					DeletePrevCollider(pMouseRay->GetSerialNumber(),
						pDest->GetSerialNumber());

					pMouseRay->DeleteSerialNumber(pDest->GetSerialNumber());
					pDest->DeleteSerialNumber(pMouseRay->GetSerialNumber());

					// 콜백을 처리한다.
					pMouseRay->Call(CCS_LEAVE, pDest, fTime);
					pDest->Call(CCS_LEAVE, pMouseRay, fTime);
				}
			}
		}

		m_pMouseCollisionList->Clear();
	}

	if (!bEditPick)
	{
		if (m_pPrevMousePick)
		{
			pMouseRay->DeleteSerialNumber(m_pPrevMousePick->GetSerialNumber());
			m_pPrevMousePick->DeleteSerialNumber(pMouseRay->GetSerialNumber());

			DeletePrevCollider(pMouseRay->GetSerialNumber(), m_pPrevMousePick->GetSerialNumber());
		}

		m_pPrevMousePick = nullptr;
	}

	for (int z = iStartZ; z <= iEndZ; ++z)
	{
		for (int y = iStartY; y <= iEndY; ++y)
		{
			for (int x = iStartX; x <= iEndX; ++x)
			{
				int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) +
					y * m_pSection->iNumX + x;

				PColliderList	pSection = &m_pSection->pSectionList[idx];

				for (int i = 0; i < pSection->iSize; ++i)
				{
					m_pMouseCollisionList->Add(pSection->pList[i]);
				}
			}
		}
	}

	// 추가된 충돌체들을 거리 순으로 오름차순 정렬한다.
	qsort(m_pMouseCollisionList->pList, m_pMouseCollisionList->iSize, sizeof(CCollider*), CCollisionManager::SortZ);

	for (int i = 0; i < m_pMouseCollisionList->iSize; ++i)
	{
		CCollider*	pDest = m_pMouseCollisionList->pList[i];

		if (pMouseRay->Collision(pDest))
		{
			if (m_pPrevMouseCollider && m_pPrevMouseCollider != pDest)
			{
				pMouseRay->DeleteSerialNumber(m_pPrevMouseCollider->GetSerialNumber());
				m_pPrevMouseCollider->DeleteSerialNumber(pMouseRay->GetSerialNumber());

				DeletePrevCollider(pMouseRay->GetSerialNumber(), m_pPrevMouseCollider->GetSerialNumber());
			}
			// 각 충돌체에 서로 충돌된 충돌체라고 등록해준다.
			// 현재 영역에서 충돌되었다고 등록을 해준다.
			// 이 리스트는 다음 영역에서 충돌체크시에 이전 영역에서
			// 충돌되었다면 빠져나가기 위함이다.
			// 왜냐하면 지금 서로 다른 영역에 걸쳐있을때 앞에 체크한
			// 영역에서 만약 충돌이 되었다면 이 충돌체는 계속
			// 충돌상태라고 표현이 되어버리므로 이전 영역에서
			// 충돌이 되었는지를 판단해야 한다.
			pMouseRay->AddCollisionList(pDest->GetSerialNumber());
			pDest->AddCollisionList(pMouseRay->GetSerialNumber());

			// 이전에 충돌되었는지를 판단한다.
			// 처음 충돌될 경우
			if (!CheckPrevCollider(pMouseRay->GetSerialNumber(),
				pDest->GetSerialNumber()))
			{
				// 충돌 매트릭스에 이전충돌목록으로
				// 등록해준다.
				AddPrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber());

				// 각 충돌체에 이전 충돌목록으로 등록한다.
				pMouseRay->AddSerialNumber(pDest->GetSerialNumber());
				pDest->AddSerialNumber(pMouseRay->GetSerialNumber());

				// 처음 충돌되었으므로 처음 충돌되었을때 호출할
				// 콜백을 처리한다.
				pMouseRay->Call(CCS_BEGIN, pDest, fTime);
				pDest->Call(CCS_BEGIN, pMouseRay, fTime);
			}

			// 이전 충돌목록에 있을 경우 계속 충돌상태로
			// 처리한다.
			else
			{
				// 콜백을 처리한다.
				pMouseRay->Call(CCS_STAY, pDest, fTime);
				pDest->Call(CCS_STAY, pMouseRay, fTime);
			}
			m_pPrevMouseCollider = pDest;
			m_pMouseCollisionList->Clear();
			SAFE_RELEASE(pMouseRay);
			return true;
		}

		// 충돌이 안된 상태일 경우
		else
		{
			// 이전 충돌목록에 서로 존재할 경우 충돌 되다가
			// 떨어진다는 것이다.
			if (CheckPrevCollider(pMouseRay->GetSerialNumber(),
				pDest->GetSerialNumber()))
			{
				// 이전 충돌목록에서 제거해준다.
				DeletePrevCollider(pMouseRay->GetSerialNumber(),
					pDest->GetSerialNumber());

				pMouseRay->DeleteSerialNumber(pDest->GetSerialNumber());
				pDest->DeleteSerialNumber(pMouseRay->GetSerialNumber());

				// 콜백을 처리한다.
				pMouseRay->Call(CCS_LEAVE, pDest, fTime);
				pDest->Call(CCS_LEAVE, pMouseRay, fTime);
			}
		}
	}

	if (m_pPrevMouseCollider)
	{
		pMouseRay->DeleteSerialNumber(m_pPrevMouseCollider->GetSerialNumber());
		m_pPrevMouseCollider->DeleteSerialNumber(pMouseRay->GetSerialNumber());

		DeletePrevCollider(pMouseRay->GetSerialNumber(), m_pPrevMouseCollider->GetSerialNumber());
		m_pPrevMouseCollider = nullptr;
	}

	m_pMouseCollisionList->Clear();

	SAFE_RELEASE(pMouseRay);

	return false;
}

bool CCollisionManager::CollisionUI(float fTime)
{
	return false;
}

bool CCollisionManager::CollisionWorld(float fTime)
{
	for (unsigned int z = 0; z < (unsigned int)m_pSection->iNumZ; ++z)
	{
		for (unsigned int y = 0; y < (unsigned int)m_pSection->iNumY; ++y)
		{
			for (unsigned int x = 0; x < (unsigned int)m_pSection->iNumX; ++x)
			{
				unsigned int	idx = z * (m_pSection->iNumX * m_pSection->iNumY) + 	y * m_pSection->iNumX + x;

				PColliderList	pSection = &m_pSection->pSectionList[idx];

				if (pSection->iSize <= 1)
				{
					for (unsigned int i = 0; i < (unsigned int)pSection->iSize; ++i)
					{
						if (pSection->pList[i]->CheckCollisionList())
							pSection->pList[i]->ClearState();
					}
					pSection->iSize = 0;
					continue;
				}

				for (unsigned int i = 0; i < (unsigned int)pSection->iSize - 1; ++i)
				{
					CCollider*	pSrc = pSection->pList[i];
					for (unsigned int j = i + 1; j < (unsigned int)pSection->iSize; ++j)
					{
						CCollider*	pDest = pSection->pList[j];

						// 현재 다른 섹션에서 충돌되었는지를 판단한다.
						if (pSrc->CheckCollisionList(pDest->GetSerialNumber()))
							continue;

						// 두 충돌체가 사용하는 Profile을 얻어온다.
						PCollisionProfile	pSrcProfile = pSrc->GetCollisionProfile();
						PCollisionProfile	pDestProfile = pDest->GetCollisionProfile();

						PCollisionChannel	pSrcChannel = &pSrcProfile->pChannelList[pDest->GetCollisionChannelIndex()];
						PCollisionChannel	pDestChannel = &pDestProfile->pChannelList[pSrc->GetCollisionChannelIndex()];

						if (pSrcChannel->eState != CCS_IGNORE ||
							pDestChannel->eState != CCS_IGNORE)
						{
							if (pSrc->Collision(pDest))
							{
								// 각 충돌체에 서로 충돌된 충돌체라고 등록해준다.
								// 현재 영역에서 충돌되었다고 등록을 해준다.
								// 이 리스트는 다음 영역에서 충돌체크시에 이전 영역에서
								// 충돌되었다면 빠져나가기 위함이다.
								// 왜냐하면 지금 서로 다른 영역에 걸쳐있을때 앞에 체크한
								// 영역에서 만약 충돌이 되었다면 이 충돌체는 계속
								// 충돌상태라고 표현이 되어버리므로 이전 영역에서
								// 충돌이 되었는지를 판단해야 한다.
								pSrc->AddCollisionList(pDest->GetSerialNumber());
								pDest->AddCollisionList(pSrc->GetSerialNumber());

								// 이전에 충돌되었는지를 판단한다.
								// 처음 충돌될 경우
								if (!CheckPrevCollider(pSrc->GetSerialNumber(),
									pDest->GetSerialNumber()))
								{
									// 충돌 매트릭스에 이전충돌목록으로
									// 등록해준다.
									AddPrevCollider(pSrc->GetSerialNumber(),
										pDest->GetSerialNumber());

									// 각 충돌체에 이전 충돌목록으로 등록한다.
									pSrc->AddSerialNumber(pDest->GetSerialNumber());
									pDest->AddSerialNumber(pSrc->GetSerialNumber());

									// 처음 충돌되었으므로 처음 충돌되었을때 호출할
									// 콜백을 처리한다.
									if (pSrcChannel->eState != CCS_IGNORE)
										pSrc->Call(CCS_BEGIN, pDest, fTime);

									if (pDestChannel->eState != CCS_IGNORE)
										pDest->Call(CCS_BEGIN, pSrc, fTime);
								}

								// 이전 충돌목록에 있을 경우 계속 충돌상태로
								// 처리한다.
								else
								{
									// 콜백을 처리한다.
									if (pSrcChannel->eState != CCS_IGNORE)
										pSrc->Call(CCS_STAY, pDest, fTime);

									if (pDestChannel->eState != CCS_IGNORE)
										pDest->Call(CCS_STAY, pSrc, fTime);
								}
							}

							// 충돌이 안된 상태일 경우
							else
							{
								// 이전 충돌목록에 서로 존재할 경우 충돌 되다가
								// 떨어진다는 것이다.
								if (CheckPrevCollider(pSrc->GetSerialNumber(),
									pDest->GetSerialNumber()))
								{
									// 이전 충돌목록에서 제거해준다.
									DeletePrevCollider(pSrc->GetSerialNumber(),
										pDest->GetSerialNumber());

									pSrc->DeleteSerialNumber(pDest->GetSerialNumber());
									pDest->DeleteSerialNumber(pSrc->GetSerialNumber());

									// 콜백을 처리한다.
									if (pSrcChannel->eState != CCS_IGNORE)
										pSrc->Call(CCS_LEAVE, pDest, fTime);

									if (pDestChannel->eState != CCS_IGNORE)
										pDest->Call(CCS_LEAVE, pSrc, fTime);
								}
							}
						}

						else
						{
							if (CheckPrevCollider(pSrc->GetSerialNumber(),
								pDest->GetSerialNumber()))
							{
								// 이전 충돌목록에서 제거해준다.
								DeletePrevCollider(pSrc->GetSerialNumber(),
									pDest->GetSerialNumber());

								pSrc->DeleteSerialNumber(pDest->GetSerialNumber());
								pDest->DeleteSerialNumber(pSrc->GetSerialNumber());

								// 콜백을 처리한다.
								if (pSrcChannel->eState != CCS_IGNORE)
									pSrc->Call(CCS_LEAVE, pDest, fTime);

								if (pDestChannel->eState != CCS_IGNORE)
									pDest->Call(CCS_LEAVE, pSrc, fTime);
							}
						}
					}
				}

				pSection->iSize = 0;
			}
		}
	}

	return true;
}

bool CCollisionManager::Collision(CCollider * pSrc, CCollider * pDest)
{
	return false;
}

int CCollisionManager::SortZ(const void * pSrc, const void * pDest)
{
	CCollider*	pSrcColl = *((CCollider**)pSrc);
	CCollider*	pDestColl = *((CCollider**)pDest);

	Vector3	vSrcMin = pSrcColl->GetSectionMin();
	Vector3	vDestMin = pDestColl->GetSectionMin();
	Vector3	vSrcMax = pSrcColl->GetSectionMax();
	Vector3	vDestMax = pDestColl->GetSectionMax();

	Vector3	vSrcCenter = (vSrcMin + vSrcMax) / 2.f;
	Vector3	vDestCenter = (vDestMin + vDestMax) / 2.f;

	Vector3	vCameraPos = GET_SINGLE(CSceneManager)->GetMainCameraPos();

	float	fSrcDist = vSrcCenter.Distance(vCameraPos);
	float	fDestDist = vDestCenter.Distance(vCameraPos);

	if (fSrcDist > fDestDist)
		return 1;

	else if (fSrcDist < fDestDist)
		return -1;

	return 0;
}
