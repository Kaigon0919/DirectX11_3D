#pragma once
#include "../Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CComponent :
	public CRef
{
	friend class CGameObject;

protected:
	CComponent();
	CComponent(const CComponent& com);
	virtual ~CComponent() = 0;

protected:
	class CScene*	m_pScene;
	class CLayer*	m_pLayer;
	class CGameObject*	m_pObject;
	class CTransform*	m_pTransform;

public:
	class CScene* GetScene()	const;
	class CLayer* GetLayer()	const;
	class CGameObject* GetGameObject()	const;
	class CTransform* GetTransform()	const;
	bool IsObjectEnable()	const;
	bool IsObjectActive()	const;

public:
	void SetScene(class CScene* pScene);
	void SetLayer(class CLayer* pLayer);
	void SetGameObject(class CGameObject* pObj);

protected:
	COMPONENT_TYPE	m_eComType;

public:
	COMPONENT_TYPE GetComponentType()	const;

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CComponent* Clone()	const;

public:
	void EraseComponentFromTag(const string& strTag);
	void EraseComponentFromType(COMPONENT_TYPE eType);
	void EraseComponent(class CComponent* pCom);

public:
	template <typename T>
	T* FindComponentFromTag(const string& strTag)
	{
		return m_pObject->FindComponentFromTag<T>(strTag);
	}

	template <typename T>
	T* FindComponentFromType(COMPONENT_TYPE eType)
	{
		return m_pObject->FindComponentFromType<T>(eType);
	}

	template <typename T>
	T* AddComponent(const string& strTag)
	{
		return m_pObject->AddComponent<T>(strTag);
	}
};

ENGINE_END
