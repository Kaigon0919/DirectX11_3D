#include "Input.h"
#include"Core.h"
#include "Device.h"
#include "GameObject.h"
#include "Component/Renderer.h"
#include "Component/Material.h"
#include "Component/ColliderRay.h"
#include "Component/ColliderPoint.h"

ENGINE_USING

DEFINITION_SINGLE(CInput)

CInput::CInput() :
	m_pInput(nullptr),
	m_pKeyboard(nullptr),
	m_pMouseDevice(nullptr),
	m_pJoyStick(nullptr),
	m_sWheel(0),
	m_iAddKeySize(0)
{
	memset(m_AddKeyList, 0, sizeof(unsigned char) * 256);
	memset(&m_tMouseState, 0, sizeof(m_tMouseState));

	m_pMouseObj = CGameObject::CreateObject("Mouse");

	m_pMouseRay = m_pMouseObj->AddComponent<CColliderRay>("MouseRay");

	m_pMouseRay->PickEnable();
	m_pMouseRay->MouseEnable();
	m_pMouseRay->EnableCollisionManager(false);
}

CInput::~CInput()
{
	SAFE_RELEASE(m_pMouseRay);
	SAFE_RELEASE(m_pMouseObj);

	unordered_map<string, PBindAxisInfo>::iterator	iter;
	unordered_map<string, PBindAxisInfo>::iterator	iterEnd = m_mapAxis.end();
	
	for (iter = m_mapAxis.begin(); iter != iterEnd; ++iter)
	{
		Safe_Delete_VecList(iter->second->KeyList);
		SAFE_DELETE(iter->second);
	}

	unordered_map<string, PBindActionInfo>::iterator	iter1;
	unordered_map<string, PBindActionInfo>::iterator	iter1End = m_mapAction.end();

	for (iter1 = m_mapAction.begin(); iter1 != iter1End; ++iter1)
	{
		Safe_Delete_VecList(iter1->second->KeyList);
		SAFE_DELETE(iter1->second);
	}
	if (m_pJoyStick)
	{
		m_pJoyStick->Unacquire();
		SAFE_RELEASE(m_pJoyStick);
	}
	if (m_pMouseDevice)
	{
		m_pMouseDevice->Unacquire();
		SAFE_RELEASE(m_pMouseDevice);
	}

	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		SAFE_RELEASE(m_pKeyboard);
	}

	SAFE_RELEASE(m_pInput);
}

void CInput::SetWheel(short sWheel)
{
	m_sWheel = sWheel;
}

short CInput::GetWheelDir() const
{
	return m_sWheel;
}

bool CInput::MousePress(MOUSE_BUTTON eMouseButton)
{
	return m_tMouseInfo[eMouseButton].bPress;
}

bool CInput::MousePush(MOUSE_BUTTON eMouseButton)
{
	return m_tMouseInfo[eMouseButton].bPush;
}

bool CInput::MouseRelease(MOUSE_BUTTON eMouseButton)
{
	return m_tMouseInfo[eMouseButton].bRelease;
}

Vector2 CInput::GetMouseWindowPos() const
{
	return m_vWidnowMousePos;
}

Vector2 CInput::GetMouseWindowMove() const
{
	return m_vWidnowMouseMove;
}

Vector2 CInput::GetMouseViewportPos() const
{
	return m_vViewportMousePos;
}

Vector2 CInput::GetMouseViewportMove() const
{
	return m_vViewportMouseMove;
}

CColliderRay * CInput::GetMouseRay() const
{
	if (m_pMouseRay)
		m_pMouseRay->AddRef();
	return m_pMouseRay;
}

void CInput::ChangeMouseScene(CScene * pScene)
{
	m_pMouseObj->SetScene(pScene);
}

bool CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&m_pInput, nullptr)))
		return false;

	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return false;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr)))
		return false;

	if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2)))
		return false;

	//if (FAILED(m_pInput->CreateDevice(GUID_Joystick, &m_pJoyStick, nullptr)))
		//return false;

	//if (FAILED(m_pJoyStick->SetDataFormat(&c_dfDIJoystick)))
	//	return false;

	AddActionKey("MouseRButton", DIK_RBUTTON, 0, MB_RBUTTON);

	return true;
}

void CInput::Update(float fTime)
{
	ReadKeyboard();
	ReadMouse();
	//ReadJoyStick();

	UpdateKeyboard(fTime);
	UpdateMouse(fTime);
	UpdateAxis(fTime);
	UpdateAction(fTime);

	m_pMouseObj->Update(fTime);
	m_pMouseObj->LateUpdate(fTime);
}

void CInput::Render(float fTime)
{
	m_sWheel = 0;
	m_pMouseObj->Render(fTime);
}

PBindAxisInfo CInput::FindAxis(const string & strName)
{
	unordered_map<string, PBindAxisInfo>::iterator	iter = m_mapAxis.find(strName);

	if (iter == m_mapAxis.end())
		return nullptr;

	return iter->second;
}

PBindActionInfo CInput::FindAction(const string & strName)
{
	unordered_map<string, PBindActionInfo>::iterator	iter = m_mapAction.find(strName);

	if (iter == m_mapAction.end())
		return nullptr;

	return iter->second;
}

bool CInput::ReadKeyboard()
{
	HRESULT result = m_pKeyboard->GetDeviceState(256, m_Key);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			if (FAILED(m_pKeyboard->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
				return false;

			m_pKeyboard->Acquire();
		}
		else
			return false;
	}

	return true;
}

bool CInput::ReadMouse()
{
	HRESULT result = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_tMouseState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			if (FAILED(m_pMouseDevice->SetCooperativeLevel(WINDOWHANDLE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
				return false;

			m_pMouseDevice->Acquire();
		}

		else
			return false;
	}
	return true;
}

bool CInput::ReadJoyStick()
{
	return false;
}

void CInput::UpdateKeyboard(float fTime)
{
	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		unsigned char cKey = m_AddKeyList[i];

		if (m_Key[cKey] & 0x80)
		{
			if (!m_tKeyInfo[cKey].bPress && !m_tKeyInfo[cKey].bPush)
			{
				m_tKeyInfo[cKey].bPress = true;
				m_tKeyInfo[cKey].bPush = true;
			}
			else
			{
				m_tKeyInfo[cKey].bPress = false;
			}
		}
		else if (m_tKeyInfo[cKey].bPress || m_tKeyInfo[cKey].bPush)
		{
			m_tKeyInfo[cKey].bPress = false;
			m_tKeyInfo[cKey].bPush = false;
			m_tKeyInfo[cKey].bRelease = true;
		}
		else if (m_tKeyInfo[cKey].bRelease)
			m_tKeyInfo[cKey].bRelease = false;
	}

}

void CInput::UpdateMouse(float fTime)
{
	for (int i = 0; i < 8; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (!m_tMouseInfo[i].bPress && !m_tMouseInfo[i].bPush)
			{
				m_tMouseInfo[i].bPress = true;
				m_tMouseInfo[i].bPush = true;
			}

			else
			{
				m_tMouseInfo[i].bPress = false;
			}
		}

		else if (m_tMouseInfo[i].bPress || m_tMouseInfo[i].bPush)
		{
			m_tMouseInfo[i].bPress = false;
			m_tMouseInfo[i].bPush = false;
			m_tMouseInfo[i].bRelease = true;
		}

		else if (m_tMouseInfo[i].bRelease)
		{
			m_tMouseInfo[i].bRelease = false;
		}
	}
	// 마우스 좌표를 구한다.
	POINT	ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(WINDOWHANDLE, &ptMouse);

	Vector2	vMousePos;
	vMousePos.x = (float)ptMouse.x;
	vMousePos.y = (float)ptMouse.y;

	m_vWidnowMouseMove = vMousePos - m_vWidnowMousePos;
	m_vWidnowMousePos = vMousePos;

	Vector2	vRatio = GET_SINGLE(CDevice)->GetWindowToViewportRatio();

	vMousePos *= vRatio;
	m_vViewportMouseMove = vMousePos - m_vViewportMousePos;
	m_vViewportMousePos = vMousePos;
}

void CInput::UpdateAxis(float fTime)
{
	unordered_map<string, PBindAxisInfo>::iterator	iter;
	unordered_map<string, PBindAxisInfo>::iterator	iterEnd = m_mapAxis.end();

	for (iter = m_mapAxis.begin(); iter != iterEnd; ++iter)
	{
		list<PKeyScale>::iterator	iter1;
		list<PKeyScale>::iterator	iter1End = iter->second->KeyList.end();

		float	fScale = 0.f;
		for (iter1 = iter->second->KeyList.begin(); iter1 != iter1End; ++iter1)
		{
			if (m_Key[(*iter1)->cKey] & 0x80)
			{
				fScale += (*iter1)->fScale;
			}
		}
		if (iter->second->bFunctionBind)
			iter->second->func(fScale, fTime);
	}
}
void CInput::UpdateAction(float fTime)
{
	unordered_map<string, PBindActionInfo>::iterator	iter;
	unordered_map<string, PBindActionInfo>::iterator	iterEnd = m_mapAction.end();

	const unsigned char	cSKey[SKEY_END] = { DIK_LCONTROL, DIK_LALT, DIK_LSHIFT };
	bool	bSKeyState[SKEY_END] = {};

	for (int i = 0; i < SKEY_END; ++i)
	{
		if (m_Key[cSKey[i]] & 0x80)
			bSKeyState[i] = true;

		else
			bSKeyState[i] = false;
	}

	for (iter = m_mapAction.begin(); iter != iterEnd; ++iter)
	{

		list<PActionKeyInfo>::iterator	iter1;
		list<PActionKeyInfo>::iterator	iter1End = iter->second->KeyList.end();

		bool	bFunctionCall[KS_END] = {};

		for (iter1 = iter->second->KeyList.begin(); iter1 != iter1End; ++iter1)
		{
			bool bSKeyEnable = true;

			for (int i = 0; i < SKEY_END; ++i)
			{
				if ((*iter1)->bSKey[i])
				{
					if (!bSKeyState[i])
					{
						bSKeyEnable = false;
						break;
					}
				}

				else
				{
					if (bSKeyState[i])
					{
						bSKeyEnable = false;
						break;
					}
				}
			}

			if ((*iter1)->eMouseType == MB_NONE)
			{

				if (iter->first == "MouseEnable")
					int a = 0;
				if (m_tKeyInfo[(*iter1)->cKey].bPress && bSKeyEnable)
				{
					if (iter->first == "MouseEnable")
						int a = 0;
					(*iter1)->tKeyInfo.bPress = true;
					(*iter1)->tKeyInfo.bPush = true;

					bFunctionCall[KS_PRESS] = true;
				}

				else if (m_tKeyInfo[(*iter1)->cKey].bPush && bSKeyEnable)
				{
					(*iter1)->tKeyInfo.bPush = true;
					(*iter1)->tKeyInfo.bPress = false;
					bFunctionCall[KS_PUSH] = true;
				}

				else if ((*iter1)->tKeyInfo.bPush && (!bSKeyEnable || m_tKeyInfo[(*iter1)->cKey].bRelease))
				{
					(*iter1)->tKeyInfo.bPush = false;
					(*iter1)->tKeyInfo.bRelease = true;
					bFunctionCall[KS_RELEASE] = true;
				}

				else if ((*iter1)->tKeyInfo.bRelease)
					(*iter1)->tKeyInfo.bRelease = false;
			}

			else
			{
			}
		}

		if (bFunctionCall[KS_PRESS] && iter->second->bFunctionBind[KS_PRESS])
			iter->second->func[KS_PRESS](fTime);

		if (bFunctionCall[KS_PUSH] && iter->second->bFunctionBind[KS_PUSH])
			iter->second->func[KS_PUSH](fTime);

		if (bFunctionCall[KS_RELEASE] && iter->second->bFunctionBind[KS_RELEASE])
			iter->second->func[KS_RELEASE](fTime);
	}
}
void CInput::AddAxisKey(const string & strName,
	unsigned char cKey, float fScale)
{
	PBindAxisInfo	pBind = FindAxis(strName);

	if (!pBind)
	{
		pBind = new BindAxisInfo;
		pBind->strName = strName;
		m_mapAxis.insert(make_pair(strName, pBind));
	}

	list<PKeyScale>::iterator	iter;
	list<PKeyScale>::iterator	iterEnd = pBind->KeyList.end();

	for (iter = pBind->KeyList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->cKey == cKey)
			return;
	}

	PKeyScale	pKeyScale = new KeyScale;

	pKeyScale->fScale = fScale;
	pKeyScale->cKey = cKey;

	// 현재 키가 기존이 등록된 키인지를 판단한다.
	bool	bFind = false;

	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		if (m_AddKeyList[i] == cKey)
		{
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		m_AddKeyList[m_iAddKeySize] = cKey;
		++m_iAddKeySize;
	}

	pBind->KeyList.push_back(pKeyScale);
}

void CInput::AddActionKey(const string & strName,
	unsigned char cKey, int iSKey, MOUSE_BUTTON eMouseButton)
{
	PBindActionInfo	pBind = FindAction(strName);

	if (!pBind)
	{
		pBind = new BindActionInfo;
		pBind->strName = strName;
		m_mapAction.insert(make_pair(strName, pBind));
	}

	list<PActionKeyInfo>::iterator	iter;
	list<PActionKeyInfo>::iterator	iterEnd = pBind->KeyList.end();

	for (iter = pBind->KeyList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->cKey == cKey)
			return;
	}

	PActionKeyInfo	pKeyAction = new ActionKeyInfo;

	pKeyAction->cKey = cKey;
	pKeyAction->eMouseType = eMouseButton;

	if (iSKey & SE_CTRL)
		pKeyAction->bSKey[SKEY_CTRL] = true;

	if (iSKey & SE_ALT)
		pKeyAction->bSKey[SKEY_ALT] = true;

	if (iSKey & SE_SHIFT)
		pKeyAction->bSKey[SKEY_SHIFT] = true;

	// 현재 키가 기존이 등록된 키인지를 판단한다.
	bool	bFind = false;

	for (int i = 0; i < m_iAddKeySize; ++i)
	{
		if (m_AddKeyList[i] == cKey)
		{
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		m_AddKeyList[m_iAddKeySize] = cKey;
		++m_iAddKeySize;
	}

	pBind->KeyList.push_back(pKeyAction);
}
