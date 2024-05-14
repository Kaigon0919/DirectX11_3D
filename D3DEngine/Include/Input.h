#pragma once

#include "Engine.h"

ENGINE_BEGIN

#define	DIK_LBUTTON	0xfd
#define	DIK_RBUTTON	0xfe
#define	DIK_MBUTTON	0xff

typedef struct ENGINE_DLL _tagKeyInfo
{
	bool	bPress;
	bool	bPush;
	bool	bRelease;

	_tagKeyInfo()
	{
		bPress = false;
		bPush = false;
		bRelease = false;
	}
}KeyInfo, *PKeyInfo;

typedef struct ENGINE_DLL _tagKeyScale
{
	unsigned char	cKey;
	float			fScale;
}KeyScale, *PKeyScale;

typedef struct ENGINE_DLL _tagBindAxis
{
	string		strName;
	bool		bFunctionBind;
	function<void(float, float)>	func;
	list<PKeyScale>	KeyList;
	bool		bGlobal;

	_tagBindAxis()
	{
		bFunctionBind = false;
		bGlobal = false;
	}

	~_tagBindAxis()
	{
		Safe_Delete_VecList(KeyList);
	}
}BindAxisInfo, *PBindAxisInfo;

typedef struct ENGINE_DLL _tagActionKeyInfo
{
	MOUSE_BUTTON	eMouseType;
	unsigned char	cKey;
	bool			bSKey[SKEY_END];
	KeyInfo			tKeyInfo;

	_tagActionKeyInfo()
	{
		cKey = 0;
		memset(bSKey, 0, sizeof(bool) * SKEY_END);
		eMouseType = MB_NONE;
	}
}ActionKeyInfo, *PActionKeyInfo;

typedef struct ENGINE_DLL _tagBindAction
{
	string		strName;
	bool		bFunctionBind[KS_END];
	function<void(float)>	func[KS_END];
	list<PActionKeyInfo>	KeyList;
	bool					bGlobal;

	_tagBindAction()
	{
		memset(bFunctionBind, 0, sizeof(bool) * KS_END);
		bGlobal = false;
	}
}BindActionInfo, *PBindActionInfo;

class ENGINE_DLL CInput
{
private:
	IDirectInput8*	m_pInput;
	IDirectInputDevice8*	m_pKeyboard;
	IDirectInputDevice8*	m_pMouseDevice;
	IDirectInputDevice8*	m_pJoyStick;
	unsigned char			m_Key[256];
	unsigned char			m_AddKeyList[256];
	int						m_iAddKeySize;
	KeyInfo					m_tKeyInfo[256];
	short					m_sWheel;
	DIMOUSESTATE2			m_tMouseState;
	KeyInfo					m_tMouseInfo[8];
	DIJOYSTATE2				m_tJoyStickState;

	Vector2					m_vWidnowMousePos;
	Vector2					m_vWidnowMouseMove;
	Vector2					m_vViewportMousePos;
	Vector2					m_vViewportMouseMove;
	class CGameObject*		m_pMouseObj;
	class CColliderRay*		m_pMouseRay;

public:
	void SetWheel(short sWheel);
	short GetWheelDir()	const;
	bool MousePress(MOUSE_BUTTON eMouseButton);
	bool MousePush(MOUSE_BUTTON eMouseButton);
	bool MouseRelease(MOUSE_BUTTON eMouseButton);

	Vector2 GetMouseWindowPos()	const;
	Vector2 GetMouseWindowMove()	const;
	Vector2 GetMouseViewportPos()	const;
	Vector2 GetMouseViewportMove()	const;
	class CColliderRay* GetMouseRay()	const;
	void ChangeMouseScene(class CScene* pScene);
private:
	unordered_map<string, PBindAxisInfo>	m_mapAxis;
	unordered_map<string, PBindActionInfo>	m_mapAction;

public:
	bool Init(HINSTANCE hInst, HWND hWnd);
	void Update(float fTime);
	void Render(float fTime);
	PBindAxisInfo FindAxis(const string& strName);
	PBindActionInfo FindAction(const string& strName);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	bool ReadJoyStick();
private:
	void UpdateKeyboard(float fTime);
	void UpdateMouse(float fTime);
	void UpdateAxis(float fTime);
	void UpdateAction(float fTime);
public:
	void AddAxisKey(const string& strName, unsigned char cKey, float fScale);
	void AddActionKey(const string& strName, unsigned char cKey, int iSKey = 0, MOUSE_BUTTON eMouseButton = MB_NONE);

public:
	void AxisGlobal(const string& strName, bool bGlobal)
	{
		PBindAxisInfo	pBind = FindAxis(strName);

		if (!pBind)
			return;

		pBind->bGlobal = bGlobal;
	}

	void BindAxis(const string& strName,
		void(*pFunc)(float, float))
	{
		PBindAxisInfo	pBind = FindAxis(strName);

		if (!pBind)
		{
			pBind = new _tagBindAxis;

			pBind->strName = strName;
			m_mapAxis.insert(make_pair(strName, pBind));
		}

		pBind->func = bind(pFunc, placeholders::_1, placeholders::_2);
		pBind->bFunctionBind = true;
	}

	template <typename T>
	void BindAxis(const string& strName,
		T* pObj, void(T::*pFunc)(float, float))
	{
		PBindAxisInfo	pBind = FindAxis(strName);

		if (!pBind)
		{
			pBind = new _tagBindAxis;

			pBind->strName = strName;
			m_mapAxis.insert(make_pair(strName, pBind));
		}

		pBind->func = bind(pFunc, pObj, placeholders::_1, placeholders::_2);
		pBind->bFunctionBind = true;
	}

public:
	void ActionGlobal(const string& strName, bool bGlobal)
	{
		PBindActionInfo	pBind = FindAction(strName);

		if (!pBind)
			return;

		pBind->bGlobal = bGlobal;
	}

	void BindAction(const string& strName,
		KEY_STATE eKeyState, void(*pFunc)(float))
	{
		PBindActionInfo	pBind = FindAction(strName);

		if (!pBind)
		{
			pBind = new _tagBindAction;

			pBind->strName = strName;
			m_mapAction.insert(make_pair(strName, pBind));
		}

		pBind->func[eKeyState] = bind(pFunc, placeholders::_1);
		pBind->bFunctionBind[eKeyState] = true;
	}

	template <typename T>
	void BindAction(const string& strName,
		KEY_STATE eKeyState, T* pObj, void(T::*pFunc)(float))
	{
		PBindActionInfo	pBind = FindAction(strName);

		if (!pBind)
		{
			pBind = new _tagBindAction;

			pBind->strName = strName;
			m_mapAction.insert(make_pair(strName, pBind));
		}

		pBind->func[eKeyState] = bind(pFunc, pObj, placeholders::_1);
		pBind->bFunctionBind[eKeyState] = true;
	}


	DECLARE_SINGLE(CInput)
};

ENGINE_END
