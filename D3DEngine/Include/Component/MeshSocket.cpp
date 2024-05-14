#include "MeshSocket.h"
#include "../GameObject.h"
#include "Animation.h"
#include"Transform.h"

ENGINE_USING

#ifdef _DEBUG
#include<iostream>
#endif
CMeshSocket::CMeshSocket() : m_pObjTransform(nullptr), m_pAni(nullptr)
{
}


CMeshSocket::CMeshSocket(const CMeshSocket & ref)
{
	*this = ref;
	m_iRefCount = 1;
	m_pAni= nullptr;
}

CMeshSocket::~CMeshSocket()
{
	SAFE_RELEASE(m_pObjTransform);
	SAFE_RELEASE(m_pAni);
}

void CMeshSocket::Start()
{
}

bool CMeshSocket::Init()
{
	return true;
}

int CMeshSocket::Input(float fTime)
{
	return 0;
}

int CMeshSocket::Update(float fTime)
{
	//Matrix matBone = m_pAni->GetBoneMatrix(m_strBonename);


	return 0;
}

int CMeshSocket::LateUpdate(float fTime)
{
	// 초기 종료 조건.
	if (m_strBonename.empty())
		return 0;
	if (!m_pObjTransform)
		return 0;

	// 애니메이션 컴포넌트를 찾고 없으면 종료.
	if (!m_pAni)
	{
		SAFE_RELEASE(m_pAni);
		m_pAni = FindComponentFromType<CAnimation>(COMPONENT_TYPE::CT_ANIMATION);
		if (!m_pAni)
			return 0;
	}
	Matrix matBone = m_pAni->GetBoneMatrix(m_strBonename) *  m_pTransform->GetLocalMatrix() * m_pTransform->GetWorldMatrix();
	//Matrix matBone = m_pAni->GetBoneMatrix(m_strBonename);
	//Matrix matLocal = m_pTransform->GetLocalMatrix();
	//Matrix matWorld = m_pTransform->GetWorldMatrix();
	Vector3 vPos, vRot, vScale;
	vRot = matBone.GetRotate();
	vScale = matBone.GetScale();
	vPos = matBone.GetPosition();

#ifdef _DEBUG
	std::system("cls");
	std::cout << "x :" << matBone._41 << ",y :"<< matBone._42 << std::endl;
#endif

	m_pObjTransform->SetWorldPos(vPos);
	m_pObjTransform->SetWorldRot(vRot);
	//m_pObjTransform->SetWorldScale(vScale);

	return 0;
}

int CMeshSocket::Collision(float fTime)
{
	return 0;
}

int CMeshSocket::PrevRender(float fTime)
{
	return 0;
}

int CMeshSocket::Render(float fTime)
{
	return 0;
}

CMeshSocket * CMeshSocket::Clone() const
{
	return nullptr;
}

void CMeshSocket::SetSoket(const string strBoneName)
{
	m_strBonename = strBoneName;
}

void CMeshSocket::SetObject(CGameObject * const pObj)
{
	SetObject(pObj->GetTransform());
}

void CMeshSocket::SetObject(CTransform * const pTransform)
{
	SAFE_RELEASE(m_pObjTransform);
	m_pObjTransform = pTransform;
}
