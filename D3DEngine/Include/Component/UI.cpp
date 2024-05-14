#include "UI.h"

#include "Transform.h"
#include "../GameObject.h"
ENGINE_USING

CUI::CUI()
{
	m_eComType = CT_UI;
}

CUI::CUI(const CUI & other) : CComponent(other)
{
}


CUI::~CUI()
{
}

void CUI::Start()
{
}

bool CUI::Init()
{
	m_pTransform->UIEnable();
	m_pObject->SetRenderGroup(RG_UI);
	return true;
}

int CUI::Input(float fTime)
{
	return 0;
}

int CUI::Update(float fTime)
{
	return 0;
}

int CUI::LateUpdate(float fTime)
{
	return 0;
}

int CUI::Collision(float fTime)
{
	return 0;
}

int CUI::PrevRender(float fTime)
{
	return 0;
}

int CUI::Render(float fTime)
{
	return 0;
}

CUI * CUI::Clone() const
{
	return nullptr;
}
