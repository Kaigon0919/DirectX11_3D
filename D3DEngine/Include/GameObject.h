#pragma once
#include "Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CGameObject :
	public CRef
{
private:
	CGameObject();
	CGameObject(const CGameObject& obj);
	~CGameObject();

private:
	static unordered_map<class CScene*, unordered_map<string, CGameObject*>>	m_mapPrototype;

public:
	static CGameObject* CreateObject(const string& strTag = "", class CLayer* pLayer = nullptr);
	static CGameObject* CreatePrototype(const string& strTag, class CScene* pScene = nullptr);
	static CGameObject* CreateClone(const string& strPrototype, class CScene* pScene, class CLayer* pLayer = nullptr);
	static void RemovePrototype(class CScene* pScene);
	static void RemovePrototype(class CScene* pScene, const string& strPrototype);

private:
	static CGameObject* FindPrototype(const string& strName,
		class CScene* pScene = nullptr);

private:
	class CScene*	m_pScene;
	class CLayer*	m_pLayer;
	class CTransform*	m_pTransform;
	RENDER_GROUP		m_eRenderGroup;
	float				m_fLifeTime;
	bool				m_bInstancing;
	bool				m_bFrustum;

public:
	class CScene* GetScene()	const;
	class CLayer* GetLayer()	const;
	class CTransform* GetTransform()	const;
	RENDER_GROUP GetRenderGroup()	const;
	float GetLifeTime()	const;
	bool IsInstancing()	const;
	bool IsFrustumCulling()	const;

public:
	void SetScene(class CScene* pScene);
	void SetLayer(class CLayer* pLayer);
	void SetRenderGroup(RENDER_GROUP eGroup);
	void SetLifeTime(float fTime);
	void SetInstancingEnable(bool bEnable);
	void SetFrustumCulling(bool bCulling);
private:
	list<class CComponent*>	m_ComList;
	list<class CComponent*>	m_StartList;
	list<class CCollider*>	m_ColliderList;

public:
	const list<class CCollider*>* GetColliderList()	const;

public:
	void Start();
	bool Init();
	int Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);	
	int PrevRender(float fTime);
	int Render(float fTime);
	CGameObject* Clone()	const;

public:
	bool CheckComponentFromTag(const string& strTag);
	bool CheckComponentFromType(COMPONENT_TYPE eType);
	CComponent* AddComponent(class CComponent* pCom);
	void EraseComponentFromTag(const string& strTag);
	void EraseComponentFromType(COMPONENT_TYPE eType);
	void EraseComponent(class CComponent* pCom);

	template <typename T>
	T* AddComponent(const string& strTag = "")
	{
		T* pCom = new T;

		pCom->SetTag(strTag.c_str());
		pCom->SetScene(m_pScene);
		pCom->SetLayer(m_pLayer);
		pCom->SetGameObject(this);
		pCom->m_pTransform = m_pTransform;

		if (!pCom->Init())
		{
			SAFE_RELEASE(pCom);
			return nullptr;
		}

		return (T*)AddComponent(pCom);
	}

	template <typename T>
	T* FindComponentFromTag(const string& strTag)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComList.end();

		for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetTag() == strTag)
			{
				(*iter)->AddRef();
				return (T*)*iter;
			}
		}

		return nullptr;
	}

	template <typename T>
	T* FindComponentFromType(COMPONENT_TYPE eType)
	{
		list<CComponent*>::iterator	iter;
		list<CComponent*>::iterator	iterEnd = m_ComList.end();

		for (iter = m_ComList.begin(); iter != iterEnd; ++iter)
		{
			if ((*iter)->GetComponentType() == eType)
			{
				(*iter)->AddRef();
				return (T*)*iter;
			}
		}

		return nullptr;
	}
};

ENGINE_END
