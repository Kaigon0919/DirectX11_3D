#include "Text.h"
#include "../Device.h"
#include "../FontManager.h"
#include "../Scene/Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "../GameObject.h"

ENGINE_USING

CText::CText() : m_vOffset(0.f, 0.f, 0.f),
	// ���� Ÿ��
	// RENDERTARGET2D�� GET_SINGLE(CDevice)->Get2DRenderTarget()�� ����
	m_2DTarget(RENDERTARGET2D),
	// �׸��� ����
	m_bShadow(false)
{
	m_eComType = CT_TEXT;
	SetTag("Text");

	// ��Ʈ�� ����
	m_pFont = new TCHAR[64];
	memset(m_pFont, 0, sizeof(TCHAR) * 64);
	lstrcpy(m_pFont, TEXT("����"));

	// ��Ʈ ������
	m_fSize = 20.f;

	// ���� ����
	m_eAlignH = TAH_LEFT;

	// ���� ����
	m_eAlignV = TAV_MID;

	// �ؽ�Ʈ Ÿ�� (UI)
	m_eRenderType = TRT_UI;

	// ���� �� ����
	m_bAlpha = false;

	// �׸��� ���� �� ����
	m_bAlphaShadow = false;

	// ����
	m_fOpacity = 1.f;

	// �׸��� ����
	m_fShadowOpacity = 1.f;

	// ��Ʈ ����
	m_vColor = Vector4::White;

	// �׸��� ����
	m_vShadowColor = Vector4::Black;

	// �ؽ�Ʈ ����
	m_pTextFormat = GET_SINGLE(CFontManager)->FindTextFormat("Dotum");

	// �ؽ�Ʈ ���̾ƿ�
	m_pTextLayout = GET_SINGLE(CFontManager)->CreateTextLayout(TEXT(""), m_pTextFormat, 100.f, 100.f);

	// ��Ʈ �÷� ����
	m_pColor = GET_SINGLE(CFontManager)->CreateColor(m_vColor);
	// ��Ʈ �׸��� ����
	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(m_vShadowColor);

	// �ؽ�Ʈ ���ڼ� 256��
	m_iMaxCount = 256;

	m_pText = new TCHAR[m_iMaxCount];

	memset(m_pText, 0, sizeof(TCHAR) * m_iMaxCount);

	// �⺻ 
	lstrcpy(m_pText, TEXT("Text"));

	// �ʱ�ȭ (�ؽ�Ʈ ������Ʈ ������ ����)
	m_tRenderArea = {};
}

CText::CText(const CText & com) :
	CComponent(com)
{
	*this = com;
	m_iRefCount = 1;

	// �ؽ�Ʈ�� ������ TCHAR ���� �ʱ�ȭ
	m_pText = new TCHAR[m_iMaxCount];
	memcpy(m_pText, com.m_pText, sizeof(TCHAR) * m_iMaxCount);

	m_pFont = new TCHAR[64];
	memset(m_pFont, 0, sizeof(TCHAR) * 64);
	lstrcpy(m_pFont, com.m_pFont);
	m_pTextLayout = nullptr;
	CreateTextLayout();

	// �ʱ�ȭ (�ؽ�Ʈ ������Ʈ ������ ����)
	m_tRenderArea = {};
}

CText::~CText()
{
	SAFE_RELEASE(m_pTextLayout);
	SAFE_DELETE_ARRAY(m_pText);
	SAFE_DELETE_ARRAY(m_pFont);
}

void CText::SetRenderType(TEXT_RENDER_TYPE eType)
{
	m_eRenderType = eType;
}

// ���� ���� �ø��� �Լ�
void CText::SetTextCount(int iMaxCount)
{
	// ���� ���� ����Ǿ��µ�, ���� ���ں��� ª�� ���
	if (m_iMaxCount >= iMaxCount)
	{
		return;
	}

	// ������ �Ҵ�
	TCHAR*	pText = new TCHAR[iMaxCount];
	// �ʱ�ȭ
	memset(pText, 0, sizeof(TCHAR) * iMaxCount);

	// ���� ���� ���� ������ ������ ����
	memcpy(pText, m_pText, m_iMaxCount);
	// ī��Ʈ �� ����
	m_iMaxCount = iMaxCount;

	// ���� �� ����
	SAFE_DELETE_ARRAY(m_pText);
	// ���� ���� ������ �� ����
	m_pText = pText;
}
void CText::SetText(const TCHAR *pText)
{
	int iCount = lstrlen(pText);

	// ���� �ؽ�Ʈ���� �� �� �ؽ�Ʈ�� ���� ���
	if (m_iMaxCount < iCount)
	{
		m_iMaxCount = iCount;

		// ���� ���� ����
		SAFE_DELETE_ARRAY(m_pText);

		// ���̸�ŭ ������ �Ҵ�
		m_pText = new TCHAR[m_iMaxCount];
	}

	memset(m_pText, 0, sizeof(TCHAR) * m_iMaxCount);
	lstrcpy(m_pText, pText);
}

// �ؽ�Ʈ �̾���̱�
void CText::AddText(const TCHAR *pText)
{
	int iCount = lstrlen(pText);
	int iCurCount = lstrlen(m_pText);

	// �̾���� �ؽ�Ʈ ���̺��� ���� MaxCount�� �� �������
	if (m_iMaxCount < iCount + iCurCount)
	{
		// �̾���� �ؽ�Ʈ ���� * 2 ��ŭ �÷��ش�.
		int iMaxCount = (iCount + iCurCount) * 2;

		TCHAR* pNewText = new TCHAR[iMaxCount];
		memset(pNewText, 0, sizeof(TCHAR) * iMaxCount);

		lstrcpy(pNewText, m_pText);

		SAFE_DELETE_ARRAY(m_pText);


		m_pText = pNewText;

		m_iMaxCount = iMaxCount;
	}

	lstrcat(m_pText, pText);
}

void CText::SetFont(const string& strName, const TCHAR* pFontName,
	int iWeight, int iStyle, int iStretch, float fSize,
	const TCHAR* pLocalName)
{
	lstrcpy(m_pFont, pFontName);
	m_fSize = fSize;

	GET_SINGLE(CFontManager)->CreateTextFormat(strName, pFontName,
		iWeight, iStyle, iStretch, fSize, pLocalName);

	m_pTextFormat = GET_SINGLE(CFontManager)->FindTextFormat(strName);

	CreateTextLayout();
}
void CText::SetFont(const string & strName)
{
	m_pTextFormat = GET_SINGLE(CFontManager)->FindTextFormat(strName);

	CreateTextLayout();
}

void CText::SetSize(float fSize)
{
	m_fSize = fSize;

	CreateTextLayout();
}

void CText::SetAlignH(TEXT_ALIGN_H eAlign)
{
	m_eAlignH = eAlign;

	if (!m_pTextLayout)
		CreateTextLayout();

	switch (eAlign)
	{
	case TAH_LEFT:
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case TAH_CENTER:
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case TAH_RIGHT:
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	}
}

void CText::SetAlignV(TEXT_ALIGN_V eAlign)
{
	m_eAlignV = eAlign;

	if (!m_pTextLayout)
		CreateTextLayout();

	switch (eAlign)
	{
	case TAV_TOP:
		m_pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TAV_MID:
		m_pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case TAV_BOTTOM:
		m_pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	}
}

void CText::Shadow(bool bShadow)
{
	m_bShadow = bShadow;
}

void CText::SetOffset(const Vector3 & vOffset)
{
	m_vOffset = vOffset;
}

void CText::SetShadowOffset(const Vector3 & vShadowOffset)
{
	m_vShadowOffset = vShadowOffset;
}

void CText::SetShadowColor(float r, float g, float b, float a)
{
	// ���ͷ� ������ �÷� �� (0 ~ 1)
	m_vShadowColor = Vector4(r, g, b, a);
	// �����ͷ� ������ �÷� ��
	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(r, g, b, a);
}

void CText::SetShadowColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	// ���ͷ� ������ �׸��� �÷� �� (0 ~ 1)
	m_vShadowColor = Vector4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	// �����ͷ� ������ �׸��� �÷� ��
	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(r, g, b, a);
}
void CText::SetShadowColor(const Vector4 & vColor)
{
	m_vShadowColor = vColor;

	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(m_vShadowColor);
}
void CText::SetShadowColor(unsigned int iColor)
{
	m_vShadowColor.b = (iColor & 0x000000ff) / 255.f;
	m_vShadowColor.g = ((iColor >> 8) & 0x000000ff) / 255.f;
	m_vShadowColor.r = ((iColor >> 16) & 0x000000ff) / 255.f;
	m_vShadowColor.a = ((iColor >> 24) & 0x000000ff) / 255.f;

	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(iColor);
}

void CText::AlphaBlend(bool bAlpha)
{
	m_bAlpha = bAlpha;
}

void CText::ShadowAlphaBlend(bool bAlpha)
{
	m_bAlphaShadow = bAlpha;
}

void CText::SetOpacity(float fOpacity)
{
	m_fOpacity = fOpacity;
}

void CText::SetShadowOpacity(float fOpacity)
{
	m_fShadowOpacity = fOpacity;
}

void CText::SetColor(float r, float g, float b, float a)
{
	m_vColor = Vector4(r, g, b, a);

	m_pColor = GET_SINGLE(CFontManager)->CreateColor(r, g, b, a);
}

void CText::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_vColor = Vector4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	m_pColor = GET_SINGLE(CFontManager)->CreateColor(r, g, b, a);
}

void CText::SetColor(const Vector4 & vColor)
{
	m_vColor = vColor;

	m_pColor = GET_SINGLE(CFontManager)->CreateColor(m_vColor);
}

void CText::SetColor(unsigned int iColor)
{
	m_vColor.b = (iColor & 0x000000ff) / 255.f;
	m_vColor.g = ((iColor >> 8) & 0x000000ff) / 255.f;
	m_vColor.r = ((iColor >> 16) & 0x000000ff) / 255.f;
	m_vColor.a = ((iColor >> 24) & 0x000000ff) / 255.f;

	m_pColor = GET_SINGLE(CFontManager)->CreateColor(iColor);
}

void CText::SetRenderArea(float l, float t, float r, float b)
{
	// RESOLUTION => ���ϴ� 0, 0 / ���� 1, 1
	// RectF => ��, ��, ��, �� �Ű�����

	// D2D1 ������ y ��ǥ�� �ݴ��̹Ƿ� 2, 4��° �Ű������� �ݴ�� ����־��־�� �Ѵ�.
	m_tRenderArea = D2D1::RectF(l, _RESOLUTION.iHeight - b,
		r, _RESOLUTION.iHeight - t);

	CreateTextLayout();
}

void CText::CreateTextLayout()
{
	// ���� ������ �ؽ�Ʈ ��Ʈ�� �������� ���� ���
	if (!m_pTextFormat)
	{
		return;
	}

	// ������ �ؽ�Ʈ ���̾ƿ� ����
	SAFE_RELEASE(m_pTextLayout);
	// �ؽ�Ʈ ���̾ƿ� ����
	// 1. �ؽ�Ʈ
	// 2. �ؽ�Ʈ ����
	// 3. ���� �ʺ�
	// 4. ���� �ʺ�
	m_pTextLayout = GET_SINGLE(CFontManager)->CreateTextLayout(
		m_pText, m_pTextFormat, m_tRenderArea.right - m_tRenderArea.left,
		m_tRenderArea.bottom - m_tRenderArea.top);

	// �ؽ�Ʈ ���̿� ���� ���� (0��°���� �ش� �ؽ�Ʈ ��ü ���̱���)
	DWRITE_TEXT_RANGE	tRange;
	tRange.startPosition = 0;
	tRange.length = lstrlen(m_pText);

	// ���̾ƿ��� �ؽ�Ʈ���� �� �ؽ�Ʈ ������ ����
	m_pTextLayout->SetFontSize(m_fSize, tRange);
}

void CText::Start()
{
	if (m_eRenderType == TRT_UI)
		m_pObject->SetRenderGroup(RG_UI);
}

bool CText::Init()
{
	return true;
}

int CText::Input(float fTime)
{
	return 0;
}

int CText::Update(float fTime)
{
	return 0;
}

int CText::LateUpdate(float fTime)
{
	return 0;
}

int CText::Collision(float fTime)
{
	return 0;
}

int CText::PrevRender(float fTime)
{
	return 0;
}

int CText::Render(float fTime)
{
	// �׸��� ȣ���� BeginDraw ȣ��� EndDraw ȣ�� ���̿����� ���� �� �� �ֽ��ϴ�.
	m_2DTarget->BeginDraw();

	Vector3 vPos = m_pTransform->GetWorldPos();

	float fHeight = m_tRenderArea.bottom - m_tRenderArea.top;

	// UI�� �ƴ� ����ī�޶� ����޴� Ÿ���� ���
	// ���� ī�޶��� �������־�� �Ѵ�.
	if (m_eRenderType != TRT_UI)
	{
		// ���� ī�޶� Transform�� ���´�.
		CTransform* pCameraTr = m_pScene->GetMainCameraTransform();

		// �ش� ������Ʈ�� ī�޶� ����ŭ ���� Ŭ���̾�Ʈ ���� ���� �����ϴ°�ó�� �����Ų��.
		vPos -= pCameraTr->GetWorldPos();
		SAFE_RELEASE(pCameraTr);
	}

	// ��Ʈ�� �׸��ڰ� �����ϴ� ���
	if (m_bShadow)
	{
		// �׸��ڴ� ���� ���� ��Ʈ�� ������ ����ŭ ��������
		// ��ġ �׸���ó�� ���̰Բ� �ϴ°��̹Ƿ�...
		Vector3 vShadowPos = vPos + m_vShadowOffset;

		// �׸��ڿ� ���İ��� �����ϴ� ���
		if (m_bAlphaShadow)
		{
			// �ش� ���� ��ŭ �������ش� (0 ~ 1)
			m_pShadowColor->SetOpacity(m_fShadowOpacity);
		}
		else
		{
			// ������
			m_pShadowColor->SetOpacity(1.f);
		}

		// �׸��ڰ� ���� ��Ʈ���� ���� ������
		// �ش� ������ ������ ���� ��Ʈ���� �Ʒ��� �� ���̴�.
		vShadowPos.y = _RESOLUTION.iHeight - vShadowPos.y - fHeight;
		// �׸��ڸ� �׸���.
		// 1. �׸��� ��� ��ġ
		// 2. �׸��� ���̾ƿ�
		// 3. �׸��� �÷�
		m_2DTarget->DrawTextLayout(D2D1::Point2F(vShadowPos.x, vShadowPos.y), m_pTextLayout, m_pShadowColor);
	}

	// ��Ʈ�� ���İ� �����ϴ� ���
	if (m_bAlpha)
	{
		// ���İ� ����
		m_pColor->SetOpacity(m_fOpacity);
	}
	else
	{
		// ������
		m_pColor->SetOpacity(1.f);
	}
	vPos.y = _RESOLUTION.iHeight - vPos.y - fHeight;

	// �ؽ�Ʈ�� �׸���.
	// 1. �ؽ�Ʈ ��� ��ġ
	// 2. �ؽ�Ʈ ���̾ƿ�
	// 3. �ؽ�Ʈ �÷�
	m_2DTarget->DrawTextLayout(D2D1::Point2F(vPos.x, vPos.y), m_pTextLayout, m_pColor);

	m_2DTarget->EndDraw();

	return 0;
}

CText * CText::Clone() const
{
	return new CText(*this);
}