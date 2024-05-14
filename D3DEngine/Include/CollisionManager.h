#pragma once

#include "Component/Collider.h"

ENGINE_BEGIN

#define	MAX_COLLISION_CHANNEL	32

typedef struct ENGINE_DLL _tagColliderInfo
{
	CCollider**			pColliderList;
	unsigned int**		pAdjMatrix;
	unsigned int		iSize;
	unsigned int		iCapacity;

	void Resize()
	{
		iCapacity *= 2;

		CCollider** pList = new CCollider*[iCapacity];

		memcpy(pList, pColliderList, sizeof(CCollider*) * iSize);

		SAFE_DELETE_ARRAY(pColliderList);

		pColliderList = pList;

		unsigned int** pMatrix = new unsigned int*[iCapacity];

		unsigned int	iWidth = iCapacity / 32;
		for (unsigned int i = 0; i < iCapacity; ++i)
		{
			pMatrix[i] = new unsigned int[iWidth];
			memset(pMatrix[i], 0, sizeof(unsigned int) * iWidth);
			memcpy(pMatrix[i], pAdjMatrix[i], sizeof(unsigned int) * (iWidth / 2));
			SAFE_DELETE_ARRAY(pAdjMatrix[i]);
		}

		SAFE_DELETE_ARRAY(pAdjMatrix);
		pAdjMatrix = pMatrix;
	}

	_tagColliderInfo()
	{
		iSize = 0;
		iCapacity = 4096;
		pColliderList = new CCollider*[iCapacity];
		pAdjMatrix = new unsigned int*[iCapacity];

		unsigned int	iWidth = iCapacity / 32;
		for (unsigned int i = 0; i < iCapacity; ++i)
		{
			pAdjMatrix[i] = new unsigned int[iWidth];
			memset(pAdjMatrix[i], 0, sizeof(unsigned int) * iWidth);
		}
	}
	
	~_tagColliderInfo()
	{
		SAFE_DELETE_ARRAY(pColliderList);
		for (unsigned int i = 0; i < iCapacity; ++i)
		{
			SAFE_DELETE_ARRAY(pAdjMatrix[i]);
		}
		SAFE_DELETE_ARRAY(pAdjMatrix);
	}
}ColliderInfo, *PColliderInfo;

typedef struct ENGINE_DLL _tagColliderList
{
	CCollider**	pList;
	int		iSize;
	int		iCapacity;

	_tagColliderList()
	{
		iSize = 0;
		iCapacity = 100;
		pList = new CCollider*[iCapacity];
		memset(pList, 0, sizeof(CCollider*) * iCapacity);
	}

	~_tagColliderList()
	{
		SAFE_DELETE_ARRAY(pList);
	}
	void Add(class CCollider* pCollider)
	{
		if (iSize == iCapacity)
		{
			iCapacity *= 2;

			CCollider**	pNewList = new CCollider*[iCapacity];
			memset(pNewList, 0, sizeof(CCollider*) * iCapacity);
			memcpy(pNewList, pList, sizeof(CCollider*) * iSize);

			SAFE_DELETE_ARRAY(pList);
			pList = pNewList;
		}

		pList[iSize] = pCollider;
		++iSize;
	}

	void Clear()
	{
		iSize = 0;
	}
}ColliderList, *PColliderList;

typedef struct ENGINE_DLL _tagCollisionChannel
{
	string		strName;
	COLLISION_CHANNEL_STATE	eState;

	_tagCollisionChannel()
	{
		strName = "";
		eState = CCS_IGNORE;
	}
}CollisionChannel, *PCollisionChannel;

typedef struct ENGINE_DLL _tagCollisionProfile
{
	string	strName;
	PCollisionChannel	pChannelList;
	unsigned int		iChannelSize;

	PCollisionChannel FindChannel(const string& strName)
	{
		for (unsigned int i = 0; i < iChannelSize; ++i)
		{
			if (pChannelList[i].strName == strName)
				return &pChannelList[i];
		}

		return nullptr;
	}

	_tagCollisionProfile()
	{
		strName = "";
		iChannelSize = 0;
		pChannelList = new CollisionChannel[MAX_COLLISION_CHANNEL];
	}

	~_tagCollisionProfile()
	{
		SAFE_DELETE_ARRAY(pChannelList);
	}
}CollisionProfile, *PCollisionProfile;

typedef struct ENGINE_DLL _tagCollisionSection
{
	PColliderList	pSectionList;
	int		iNumX;
	int		iNumY;
	int		iNumZ;
	Vector3	vMin;
	Vector3	vMax;
	Vector3	vCellSize;
	Vector3	vLength;

	_tagCollisionSection() :
		iNumX(1),
		iNumY(1),
		iNumZ(1),
		pSectionList(nullptr)
	{
	}

	~_tagCollisionSection()
	{
		SAFE_DELETE_ARRAY(pSectionList);
	}
}CollisionSection, *PCollisionSection;

class ENGINE_DLL CCollisionManager
{
private:
	ColliderInfo	m_tColliderInfo;
	unsigned int	m_iSerialNumber;

	unsigned int*	m_pValidNumber;
	unsigned int	m_iValidSize;
	unsigned int	m_iValidCapacity;

private:
	PCollisionSection	m_pSection;
	PCollisionSection	m_pPickSection;
	PCollisionSection	m_pUISection;
	PColliderList		m_pMouseCollisionList;
	Vector3				m_vPos;
	Vector3				m_vPivot;
	class CCollider*	m_pPrevMousePick;
	class CCollider*	m_pPrevMouseCollider;

private:
	PCollisionProfile	m_pProfileList;
	unsigned int		m_iProfileSize;
	unsigned int		m_iProfileCapacity;
	
	PCollisionChannel	m_pChannelList;
	unsigned int		m_iChannelSize;

public:
	unsigned int GetSerialNumber();
	void AddValidSerialNumber(unsigned int iNumber);
	void AddPrevCollider(unsigned int iSrc, unsigned int iDest);
	void DeletePrevCollider(unsigned int iSrc, unsigned int iDest);
	bool CheckPrevCollider(unsigned int iSrc, unsigned int iDest);

public:
	void CreateSection(int iNumX, int iNumY, int iNumZ,
		const Vector3& vCellSize, const Vector3& vPos = Vector3::Zero,
		const Vector3& vPivot = Vector3::Zero);
	bool AddChannel(const string& strName, COLLISION_CHANNEL_STATE eState = CCS_IGNORE);
	bool DeleteChannel(const string& strName);
	bool CreateProfile(const string& strName);
	bool SetProfileChannelState(const string& strProfile,
		const string& strChannel, COLLISION_CHANNEL_STATE eState);
	PCollisionProfile FindProfile(const string& strName);
	unsigned int GetChannelIndex(const string& strName);
	void AddCollider(class CGameObject* pObj);
	void AddCollider(class CCollider* pCollider);
	class CCollider* FindCollider(unsigned int iSerialNumber);
	void DeleteCollider(unsigned int iSerialNumber);
	void ComputeSection();

public:
	bool Init();
	void Collision(float fTime);
	void Render(float fTime);
private:
	bool CollisionMouseUI(float fTime);
	bool CollisionMouseWorld(float fTime);
	bool CollisionUI(float fTime);
	bool CollisionWorld(float fTime);
	bool Collision(class CCollider* pSrc, class CCollider* pDest);

public:
	static int SortZ(const void* pSrc, const void* pDest);
	DECLARE_SINGLE(CCollisionManager)
};

ENGINE_END
