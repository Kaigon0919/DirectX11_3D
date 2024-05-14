#include "UserComponent.h"

ENGINE_USING

CUserComponent::CUserComponent()
{
	m_eComType = CT_USERCOMPONENT;
}

CUserComponent::CUserComponent(const CUserComponent & com)	:
	CComponent(com)
{
	m_iRefCount = 1;
}

CUserComponent::~CUserComponent()
{
}

void CUserComponent::Start()
{
}

bool CUserComponent::Init()
{
	return true;
}

int CUserComponent::Input(float fTime)
{
	return 0;
}

int CUserComponent::Update(float fTime)
{
	return 0;
}

int CUserComponent::LateUpdate(float fTime)
{
	return 0;
}

int CUserComponent::Collision(float fTime)
{
	return 0;
}

int CUserComponent::PrevRender(float fTime)
{
	return 0;
}

int CUserComponent::Render(float fTime)
{
	return 0;
}

CUserComponent * CUserComponent::Clone() const
{
	return nullptr;
}
