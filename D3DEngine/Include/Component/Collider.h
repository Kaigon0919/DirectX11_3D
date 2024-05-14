#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CCollider :
	public CComponent
{
	friend class CGameObject;

protected:
	CCollider();
	CCollider(const CCollider& com);
	virtual ~CCollider() = 0;

#ifdef _DEBUG
protected:
	class CMesh*		m_pMesh;
	class CShader*		m_pShader;
	ID3D11InputLayout*	m_pLayout;
	class CRenderState*	m_pWireFrame;
	Vector4				m_vColor;
	TransformCBuffer	m_tCBuffer;
#endif // _DEBUG

protected:
	COLLIDER_TYPE		m_eCollType;
	Vector3				m_vSectionMin;
	Vector3				m_vSectionMax;
	struct _tagCollisionProfile*	m_pProfile;
	unsigned int		m_iChannelIndex;
	unsigned int*		m_pPrevNumber;
	unsigned int		m_iPrevSize;
	unsigned int		m_iPrevCapacity;
	unsigned int*		m_pCollisionList;
	unsigned int		m_iCollisionSize;
	unsigned int		m_iCollisionCapacity;
	function<void(CCollider*, CCollider*, float)>	m_Callback[CCS_END];
	bool				m_bCallback[CCS_END];
	bool				m_bPick;
	bool				m_bAddCollisionManager;

public:
	COLLIDER_TYPE GetColliderType()	const;
	Vector3 GetSectionMin()	const;
	Vector3 GetSectionMax()	const;
	struct _tagCollisionProfile* GetCollisionProfile()	const;
	unsigned int GetCollisionChannelIndex()	const;
	bool GetPickEnable()	const;

public:
	void PickEnable();
	void EnableCollisionManager(bool bCollisionManager);
	void SetChannel(const string& strChannel);
	void SetProfile(const string& strProfile);

public:
	void SetSerialNumber(unsigned int iSerialNumber);
	void ClearState();
	void AddSerialNumber(unsigned int iSerialNumber);
	void DeleteSerialNumber(unsigned int iSerialNumber);
	void AddCollisionList(unsigned int iSerialNumber);
	void DeleteCollisionList(unsigned int iSerialNumber);
	void ClearCollisionList();
	bool CheckCollisionList(unsigned int iSerialNumber);
	bool CheckCollisionList();
	void Call(COLLISION_CALLBACK_STATE eState, CCollider* pDest, float fTime);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CCollider* Clone()	const;
	virtual bool Collision(CCollider* pDest) = 0;


public:
	bool CollisionSphereToSphere(const SphereInfo& tSrc, const SphereInfo& tDest);
	bool CollisionSpeherToOBB(const SphereInfo& tShere, const OBBInfo& tOBB);
	bool CollisionSphereToRay(const SphereInfo& tSrc, const RayInfo& tDest);
	bool CollisionAABBToRay(const AABBInfo& tAABB, const RayInfo& tRay);
	bool CollisionOBBToOBB(const OBBInfo& tSrc,	const OBBInfo& tDest);
	bool CollisionOBBToRay(const OBBInfo& tOBB, const RayInfo& tRay);
public:
	void SetCallback(COLLISION_CALLBACK_STATE eState, void(*pFunc)(CCollider*, CCollider*, float));

	template <typename T>
	void SetCallback(COLLISION_CALLBACK_STATE eState, T* pObj, void(T::*pFunc)(CCollider*, CCollider*, float))
	{
		m_bCallback[eState] = true;
		m_Callback[eState] = bind(pFunc, pObj, placeholders::_1, placeholders::_2, placeholders::_3);
	}
};

ENGINE_END
