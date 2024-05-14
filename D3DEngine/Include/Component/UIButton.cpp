#include "UIButton.h"
#include "Renderer.h"
#include "Material.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../Resource/Texture.h"
#include "../Resource/ResourcesManager.h"
#include "../Input.h"

#include "../Render/ShaderManager.h"
ENGINE_USING

CUIButton::CUIButton()
{
	m_pMaterial = nullptr;
	m_eState = BS_NORMAL;
	m_ePrevState = BS_NORMAL;

	m_pTexture[BS_DISABLE] = nullptr;
	m_pTexture[BS_NORMAL] = nullptr;
	m_pTexture[BS_MOUSEON] = nullptr;
	m_pTexture[BS_CLICK] = nullptr;

	m_vColor[BS_DISABLE]	= Vector4::White;
	m_vColor[BS_NORMAL]		= Vector4::White;
	m_vColor[BS_MOUSEON]	= Vector4::White;
	m_vColor[BS_CLICK]		= Vector4::White;

	m_bStartTextureChange = true;
}

CUIButton::CUIButton(const CUIButton & com)
{
	m_eState = com.m_eState;
	m_ePrevState = m_eState;

	for (int i = 0; i < BS_END; ++i)
	{
		m_Callback[i] = nullptr;
		m_pTexture[i] = com.m_pTexture[i];

		if (m_pTexture[i])
			m_pTexture[i]->AddRef();

		m_vColor[i] = com.m_vColor[i];
	}

	m_pMaterial = nullptr;

	m_bStartTextureChange = true;
}


CUIButton::~CUIButton()
{
	for (int i = 0; i < BS_END; ++i)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}

	SAFE_RELEASE(m_pMaterial);
}

void CUIButton::SetStateColor(BUTTON_STATE eState, const Vector4 & vColor)
{
	m_vColor[eState] = vColor;
}

void CUIButton::SetStateTexture(BUTTON_STATE eState, const string & strName, const TCHAR * pFileName, const string & strPathName)
{
	SAFE_RELEASE(m_pTexture[eState]);
	if (!GET_SINGLE(CResourcesManager)->LoadTexture(strName, pFileName, strPathName))
		return;

	m_pTexture[eState] = GET_SINGLE(CResourcesManager)->FindTexture(strName);

	if (m_eState == eState && m_pMaterial)
	{
		m_pMaterial->DeleteTextureSet(0, 0);

		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture[m_eState]);
	}
}

void CUIButton::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);


	if (m_pTexture[m_eState])
	{
		m_pMaterial->DeleteTextureSet(0, 0);

		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture[m_eState]);
	}

}

bool CUIButton::Init()
{
	if (!CUI::Init())
		return false;

	CRenderer*	pRenderer = AddComponent<CRenderer>("ButtonRenderer");

	pRenderer->SetMesh("Rectangle");
	pRenderer->SetShader("ButtonShader");

	pRenderer->SetRenderState("AlphaBlend");

	SAFE_RELEASE(pRenderer);

	CMaterial*	pMaterial = AddComponent<CMaterial>("ButtonMaterial");

	pMaterial->AddTextureSet(0, 0, 0, "ButtonDefaultNormal", TEXT("UIDefault.png"));

	SAFE_RELEASE(pMaterial);

	m_bStartTextureChange = true;

	return true;
}

int CUIButton::Input(float fTime)
{
	return 0;
}

int CUIButton::Update(float fTime)
{
	if (m_eState != BS_DISABLE)
	{
		m_ePrevState = m_eState;
		Vector2	vMousePos = GET_SINGLE(CInput)->GetMouseViewportPos();

		Vector3	vPos = m_pTransform->GetWorldPos();
		Vector3	vScale = m_pTransform->GetWorldScale();

		if (vPos.x <= vMousePos.x && vMousePos.x <= vPos.x + vScale.x &&
			vPos.y <= vMousePos.y && vMousePos.y <= vPos.y + vScale.y)
		{
			if (GET_SINGLE(CInput)->MousePress(MB_LBUTTON))
				m_eState = BS_CLICK;

			else if (GET_SINGLE(CInput)->MousePush(MB_LBUTTON))
			{
				if (m_eState != BS_CLICK)
					m_eState = BS_MOUSEON;
			}
			else if (GET_SINGLE(CInput)->MouseRelease(MB_LBUTTON) && m_eState == BS_CLICK)
			{
				m_Callback[BS_CLICK](fTime);
			}
			else
				m_eState = BS_MOUSEON;
		}

		else
			m_eState = BS_NORMAL;
	}
	if (m_Callback[m_eState] != nullptr && m_eState != BS_CLICK)
		m_Callback[m_eState](fTime);

	return 0;
}

int CUIButton::LateUpdate(float fTime)
{
	return 0;
}

int CUIButton::Collision(float fTime)
{
	return 0;
}

int CUIButton::PrevRender(float fTime)
{
	m_tButtonCBuffer.vColor = m_vColor[m_eState];

	GET_SINGLE(CShaderManager)->UpdateCBuffer("Button",	&m_tButtonCBuffer);

	if (m_ePrevState != m_eState && m_pTexture[m_eState])
	{
		m_pMaterial->DeleteTextureSet(0, 0);

		m_pMaterial->AddTextureSet(0, 0, 0, m_pTexture[m_eState]);
	}

	return 0;
}

int CUIButton::Render(float fTime)
{
	return 0;
}

CUIButton * CUIButton::Clone() const
{
	return new CUIButton(*this);
}

void CUIButton::SetCallback(BUTTON_STATE eState, void(*pFunc)(float))
{
	m_Callback[eState] = bind(pFunc, placeholders::_1);
}
