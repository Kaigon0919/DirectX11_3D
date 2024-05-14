#pragma once
#include "../Ref.h"

ENGINE_BEGIN

class ENGINE_DLL CLayer :
	public CRef
{
	friend class CScene;

private:
	CLayer();
	~CLayer();

private:
	class CScene*	m_pScene;
	int		m_iZOrder;
	list<class CGameObject*>	m_ObjList;
	list<class CGameObject*>	m_StartList;

public:
	int GetZOrder()	const;
	void SetZOrder(int iZOrder);
	void Start();

public:
	bool Init();
	int Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	int Render(float fTime);
	void AddObject(class CGameObject* pObj);
};

ENGINE_END
