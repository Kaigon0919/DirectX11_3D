#pragma once
#include"UI.h"
ENGINE_BEGIN


enum BUTTON_STATE
{
	BS_DISABLE,
	BS_NORMAL,
	BS_MOUSEON,
	BS_CLICK,
	BS_END
};

class ENGINE_DLL CUIButton  :public CUI
{
	friend class CGameObject;

protected:
	CUIButton();
	CUIButton(const CUIButton& com);
	virtual ~CUIButton();

protected:
	BUTTON_STATE	m_ePrevState;
	BUTTON_STATE	m_eState;
	bool			m_bStartTextureChange;
	function<void(float)>	m_Callback[BS_END];
	class CTexture*			m_pTexture[BS_END];
	Vector4					m_vColor[BS_END];
	class CMaterial*		m_pMaterial;
	ButtonCBuffer			m_tButtonCBuffer;
public:
	void SetStateColor(BUTTON_STATE eState, const Vector4& vColor);
	void SetStateTexture(BUTTON_STATE eState, const string& strName, const TCHAR* pFileName, const string& strPathName = TEXTURE_PATH);
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CUIButton* Clone()	const;

public:
	void SetCallback(BUTTON_STATE eState, void(*pFunc)(float));

	template <typename FuncType>
	void SetCallback(BUTTON_STATE eState, FuncType* pObj,
		void (FuncType::*pFunc)(float))
	{
		m_Callback[eState] = bind(pFunc, pObj, placeholders::_1);
	}
};

ENGINE_END