#include "AnimationFrame.h"
#include "../Resource/Texture.h"
#include "../Resource/ResourcesManager.h"
#include "Transform.h"
#include "Material.h"
#include "../GameObject.h"
#include "../Render/ShaderManager.h"

ENGINE_USING

CAnimationFrame::CAnimationFrame()
{
	m_eComType = CT_ANIMATIONFRAME;
	m_pCurClip = nullptr;
	m_pDefaultClip = nullptr;
	m_pMaterial = nullptr;
}


CAnimationFrame::CAnimationFrame(const CAnimationFrame & com)
{
	unordered_map<string, PAnimationFrameClip>::const_iterator	iter;
	unordered_map<string, PAnimationFrameClip>::const_iterator	iterEnd = com.m_mapClip.end();

	for (iter = com.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PAnimationFrameClip	pClip = new AnimationFrameClip;

		*pClip = *iter->second;

		if (pClip->pTexture)
			pClip->pTexture->AddRef();

		if (iter->first == com.m_pCurClip->strName)
			m_pCurClip = pClip;

		if (iter->first == com.m_pDefaultClip->strName)
			m_pDefaultClip = pClip;

		m_mapClip.insert(make_pair(iter->first, pClip));
	}

	m_pMaterial = nullptr;
}

CAnimationFrame::~CAnimationFrame()
{
	SAFE_RELEASE(m_pMaterial);

	unordered_map<string, PAnimationFrameClip>::iterator	iter;
	unordered_map<string, PAnimationFrameClip>::iterator	iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pTexture);
		SAFE_DELETE(iter->second);
	}

	m_mapClip.clear();
}

bool CAnimationFrame::CreateClip(const string & strName, 
	ANIMATION_OPTION eOption,
	const Vector2& vTextureSize, const vector<TextureCoord>& vecCoord,
	float fLimitTime, int iRegister, const string & strTexName, 
	const TCHAR * pFileName, const string & strPathName)
{
	PAnimationFrameClip	pClip = FindClip(strName);

	if (pClip)
		return false;

	pClip = new AnimationFrameClip;

	pClip->strName = strName;
	pClip->eType = AFT_ATLAS;
	pClip->eOption = eOption;
	pClip->iFrame = 0;
	pClip->fPlayTime = 0.f;
	pClip->vTextureSize = vTextureSize;
	pClip->vecCoord = vecCoord;
	pClip->fPlayLimitTime = fLimitTime;
	pClip->fFrameTime = fLimitTime / vecCoord.size();
	pClip->iRegister = iRegister;
	GET_SINGLE(CResourcesManager)->LoadTexture(strTexName,
		pFileName, strPathName);
	pClip->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexName);

	if (!m_pCurClip)
		m_pCurClip = pClip;

	if (!m_pDefaultClip)
		m_pDefaultClip = pClip;

	m_mapClip.insert(make_pair(strName, pClip));

	return true;
}

bool CAnimationFrame::CreateClip(const string & strName, 
	ANIMATION_OPTION eOption,
	const Vector2& vTextureSize, const vector<TextureCoord>& vecCoord,
	float fLimitTime, int iRegister, const string & strTexName,
	const vector<TCHAR*>& vecFileName, const string & strPathName)
{
	PAnimationFrameClip	pClip = FindClip(strName);

	if (pClip)
		return false;

	pClip = new AnimationFrameClip;

	pClip->strName = strName;
	pClip->eType = AFT_FRAME;
	pClip->eOption = eOption;
	pClip->iFrame = 0;
	pClip->fPlayTime = 0.f;
	pClip->vTextureSize = vTextureSize;
	pClip->vecCoord = vecCoord;
	pClip->fPlayLimitTime = fLimitTime;
	pClip->fFrameTime = fLimitTime / vecCoord.size();
	pClip->iRegister = iRegister;
	GET_SINGLE(CResourcesManager)->LoadTexture(strTexName,
		vecFileName, strPathName);
	pClip->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexName);

	if (!m_pCurClip)
		m_pCurClip = pClip;

	if (!m_pDefaultClip)
		m_pDefaultClip = pClip;

	m_mapClip.insert(make_pair(strName, pClip));

	return true;
}

void CAnimationFrame::SetCurrentClip(const string & strName)
{
	PAnimationFrameClip	pClip = FindClip(strName);

	if (!pClip)
		return;

	m_pCurClip = pClip;

	m_pCurClip->iFrame = 0;
	m_pCurClip->fPlayTime = 0.f;
}

void CAnimationFrame::SetDefaultClip(const string & strName)
{
	PAnimationFrameClip	pClip = FindClip(strName);

	if (!pClip)
		return;

	m_pDefaultClip = pClip;
}

void CAnimationFrame::ChangeClip(const string & strName)
{
	if (m_pCurClip->strName == strName)
		return;

	string	strPrevName = m_pCurClip->pTexture->GetTag();

	SetCurrentClip(strName);

	m_pMaterial->ChangeTextureSet(0, 0, m_pCurClip->iRegister,
		strPrevName, m_pCurClip->pTexture);
}

PAnimationFrameClip CAnimationFrame::FindClip(const string & strName)
{
	unordered_map<string, PAnimationFrameClip>::iterator	iter = m_mapClip.find(strName);

	if (iter == m_mapClip.end())
		return nullptr;

	return iter->second;
}

void CAnimationFrame::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);

	if (!m_pMaterial)
		m_pMaterial = m_pObject->AddComponent<CMaterial>("Material");

	m_pMaterial->DeleteTextureSet(0, 0);

	// CurrentClip의 Texture를 Material에 지정한다.
	m_pMaterial->AddTextureSet(0, 0, m_pCurClip->iRegister, m_pCurClip->pTexture);
}

bool CAnimationFrame::Init()
{
	return true;
}

int CAnimationFrame::Input(float fTime)
{
	return 0;
}

int CAnimationFrame::Update(float fTime)
{
	m_pCurClip->fPlayTime += fTime;

	while (m_pCurClip->fPlayTime >= m_pCurClip->fFrameTime)
	{
		m_pCurClip->fPlayTime -= m_pCurClip->fFrameTime;

		++m_pCurClip->iFrame;

		if (m_pCurClip->iFrame == m_pCurClip->vecCoord.size())
		{
			m_pCurClip->iFrame = 0;
			
			switch (m_pCurClip->eOption)
			{
			case AO_ONCE_DESTROY:
				m_pObject->Active(false);
				break;
			}
		}
	}

	// 상수버퍼를 채워준다.
	m_tCBuffer.iAnimationFrameType = m_pCurClip->eType;
	m_tCBuffer.iAnimationOption = m_pCurClip->eOption;
	m_tCBuffer.iFrame = m_pCurClip->iFrame;
	m_tCBuffer.vTextureSize = m_pCurClip->vTextureSize;
	m_tCBuffer.vStart = m_pCurClip->vecCoord[m_pCurClip->iFrame].vStart;
	m_tCBuffer.vEnd = m_pCurClip->vecCoord[m_pCurClip->iFrame].vEnd;

	return 0;
}

int CAnimationFrame::LateUpdate(float fTime)
{
	return 0;
}

int CAnimationFrame::Collision(float fTime)
{
	return 0;
}

int CAnimationFrame::PrevRender(float fTime)
{
	GET_SINGLE(CShaderManager)->UpdateCBuffer("AnimationFrame",
		&m_tCBuffer);

	return 0;
}

int CAnimationFrame::Render(float fTime)
{
	return 0;
}

CAnimationFrame * CAnimationFrame::Clone() const
{
	return new CAnimationFrame(*this);
}
