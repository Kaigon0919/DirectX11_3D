#include "TestMonster.h"
#include "GameObject.h"
#include "Component/Renderer.h"
#include "Component/Transform.h"

CTestMonster::CTestMonster() : m_pAnimation(nullptr)
{
}

CTestMonster::CTestMonster(const CTestMonster & player)
{
}

CTestMonster::~CTestMonster()
{
	SAFE_RELEASE(m_pAnimation);
}

void CTestMonster::Start()
{
}

bool CTestMonster::Init()
{
	m_pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);
	m_pTransform->SetWorldPos(30.0f, 0.5f, 30.0f);
	CRenderer*	pRenderer = m_pObject->AddComponent<CRenderer>("TestmonsterRenderer");
	pRenderer->SetMesh("TestMonster", TEXT("UndeadBeast.msh"), Vector3::Axis[AXIS_Z]);
	SAFE_RELEASE(pRenderer);

	m_pAnimation = m_pObject->FindComponentFromType<CAnimation>(CT_ANIMATION);
	if (!m_pAnimation)
	{
		m_pAnimation = m_pObject->AddComponent<CAnimation>("TestMonsterAnimation");

		m_pAnimation->LoadBone("UndeadBeast.bne");
		m_pAnimation->Load("UndeadBeast.anm");
	}
	return true;
}

int CTestMonster::Input(float fTime)
{
	return 0;
}

int CTestMonster::Update(float fTime)
{
	return 0;
}

int CTestMonster::LateUpdate(float fTime)
{
	return 0;
}

int CTestMonster::Collision(float fTime)
{
	return 0;
}

int CTestMonster::PrevRender(float fTime)
{
	return 0;
}

int CTestMonster::Render(float fTime)
{
	return 0;
}

CTestMonster * CTestMonster::Clone() const
{
	return nullptr;
}
