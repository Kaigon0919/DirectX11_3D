#include "Text.h"
#include "../Device.h"
#include "../FontManager.h"
#include "../Scene/Scene.h"
#include "Camera.h"
#include "Transform.h"
#include "../GameObject.h"

ENGINE_USING

CText::CText() : m_vOffset(0.f, 0.f, 0.f),
	// 랜더 타겟
	// RENDERTARGET2D은 GET_SINGLE(CDevice)->Get2DRenderTarget()과 같다
	m_2DTarget(RENDERTARGET2D),
	// 그림자 설정
	m_bShadow(false)
{
	m_eComType = CT_TEXT;
	SetTag("Text");

	// 폰트명 설정
	m_pFont = new TCHAR[64];
	memset(m_pFont, 0, sizeof(TCHAR) * 64);
	lstrcpy(m_pFont, TEXT("돋움"));

	// 폰트 사이즈
	m_fSize = 20.f;

	// 가로 정렬
	m_eAlignH = TAH_LEFT;

	// 세로 정렬
	m_eAlignV = TAV_MID;

	// 텍스트 타입 (UI)
	m_eRenderType = TRT_UI;

	// 알파 값 적용
	m_bAlpha = false;

	// 그림자 알파 값 적용
	m_bAlphaShadow = false;

	// 투명도
	m_fOpacity = 1.f;

	// 그림자 투명도
	m_fShadowOpacity = 1.f;

	// 폰트 색상
	m_vColor = Vector4::White;

	// 그림자 색상
	m_vShadowColor = Vector4::Black;

	// 텍스트 포맷
	m_pTextFormat = GET_SINGLE(CFontManager)->FindTextFormat("Dotum");

	// 텍스트 레이아웃
	m_pTextLayout = GET_SINGLE(CFontManager)->CreateTextLayout(TEXT(""), m_pTextFormat, 100.f, 100.f);

	// 폰트 컬러 설정
	m_pColor = GET_SINGLE(CFontManager)->CreateColor(m_vColor);
	// 폰트 그림자 설정
	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(m_vShadowColor);

	// 텍스트 글자수 256자
	m_iMaxCount = 256;

	m_pText = new TCHAR[m_iMaxCount];

	memset(m_pText, 0, sizeof(TCHAR) * m_iMaxCount);

	// 기본 
	lstrcpy(m_pText, TEXT("Text"));

	// 초기화 (텍스트 오브젝트 에러를 방지)
	m_tRenderArea = {};
}

CText::CText(const CText & com) :
	CComponent(com)
{
	*this = com;
	m_iRefCount = 1;

	// 텍스트를 삽입할 TCHAR 변수 초기화
	m_pText = new TCHAR[m_iMaxCount];
	memcpy(m_pText, com.m_pText, sizeof(TCHAR) * m_iMaxCount);

	m_pFont = new TCHAR[64];
	memset(m_pFont, 0, sizeof(TCHAR) * 64);
	lstrcpy(m_pFont, com.m_pFont);
	m_pTextLayout = nullptr;
	CreateTextLayout();

	// 초기화 (텍스트 오브젝트 에러를 방지)
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

// 글자 수를 늘리는 함수
void CText::SetTextCount(int iMaxCount)
{
	// 글자 수가 변경되었는데, 기존 글자보다 짧은 경우
	if (m_iMaxCount >= iMaxCount)
	{
		return;
	}

	// 새로이 할당
	TCHAR*	pText = new TCHAR[iMaxCount];
	// 초기화
	memset(pText, 0, sizeof(TCHAR) * iMaxCount);

	// 기존 값을 새로 복사한 변수에 복사
	memcpy(pText, m_pText, m_iMaxCount);
	// 카운트 값 복사
	m_iMaxCount = iMaxCount;

	// 기존 값 해제
	SAFE_DELETE_ARRAY(m_pText);
	// 기존 값에 복사한 값 삽입
	m_pText = pText;
}
void CText::SetText(const TCHAR *pText)
{
	int iCount = lstrlen(pText);

	// 기존 텍스트보다 더 긴 텍스트가 들어온 경우
	if (m_iMaxCount < iCount)
	{
		m_iMaxCount = iCount;

		// 기존 값을 삭제
		SAFE_DELETE_ARRAY(m_pText);

		// 길이만큼 새로이 할당
		m_pText = new TCHAR[m_iMaxCount];
	}

	memset(m_pText, 0, sizeof(TCHAR) * m_iMaxCount);
	lstrcpy(m_pText, pText);
}

// 텍스트 이어붙이기
void CText::AddText(const TCHAR *pText)
{
	int iCount = lstrlen(pText);
	int iCurCount = lstrlen(m_pText);

	// 이어붙일 텍스트 길이보다 현재 MaxCount가 더 작은경우
	if (m_iMaxCount < iCount + iCurCount)
	{
		// 이어붙인 텍스트 길이 * 2 만큼 늘려준다.
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
	// 벡터로 가지는 컬러 값 (0 ~ 1)
	m_vShadowColor = Vector4(r, g, b, a);
	// 포인터로 가지는 컬러 값
	m_pShadowColor = GET_SINGLE(CFontManager)->CreateColor(r, g, b, a);
}

void CText::SetShadowColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	// 벡터로 가지는 그림자 컬러 값 (0 ~ 1)
	m_vShadowColor = Vector4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	// 포인터로 가지는 그림자 컬러 값
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
	// RESOLUTION => 좌하단 0, 0 / 우상단 1, 1
	// RectF => 좌, 상, 우, 하 매개변수

	// D2D1 에서는 y 좌표가 반대이므로 2, 4번째 매개변수를 반대로 집어넣어주어야 한다.
	m_tRenderArea = D2D1::RectF(l, _RESOLUTION.iHeight - b,
		r, _RESOLUTION.iHeight - t);

	CreateTextLayout();
}

void CText::CreateTextLayout()
{
	// 현재 설정된 텍스트 폰트가 존재하지 않은 경우
	if (!m_pTextFormat)
	{
		return;
	}

	// 기존의 텍스트 레이아웃 해제
	SAFE_RELEASE(m_pTextLayout);
	// 텍스트 레이아웃 생성
	// 1. 텍스트
	// 2. 텍스트 포맷
	// 3. 가로 너비
	// 4. 세로 너비
	m_pTextLayout = GET_SINGLE(CFontManager)->CreateTextLayout(
		m_pText, m_pTextFormat, m_tRenderArea.right - m_tRenderArea.left,
		m_tRenderArea.bottom - m_tRenderArea.top);

	// 텍스트 길이에 대한 설정 (0번째부터 해당 텍스트 전체 길이까지)
	DWRITE_TEXT_RANGE	tRange;
	tRange.startPosition = 0;
	tRange.length = lstrlen(m_pText);

	// 레이아웃에 텍스트길이 및 텍스트 사이즈 지정
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
	// 그리기 호출은 BeginDraw 호출과 EndDraw 호출 사이에서만 발행 될 수 있습니다.
	m_2DTarget->BeginDraw();

	Vector3 vPos = m_pTransform->GetWorldPos();

	float fHeight = m_tRenderArea.bottom - m_tRenderArea.top;

	// UI가 아닌 월드카메라에 영향받는 타입인 경우
	// 월드 카메라값을 적용해주어야 한다.
	if (m_eRenderType != TRT_UI)
	{
		// 월드 카메라 Transform을 얻어온다.
		CTransform* pCameraTr = m_pScene->GetMainCameraTransform();

		// 해당 오브젝트는 카메라 값만큼 빼서 클라이언트 영역 내에 존재하는것처럼 위장시킨다.
		vPos -= pCameraTr->GetWorldPos();
		SAFE_RELEASE(pCameraTr);
	}

	// 폰트에 그림자가 존재하는 경우
	if (m_bShadow)
	{
		// 그림자는 보통 현재 폰트의 오프셋 값만큼 떨어져서
		// 마치 그림자처럼 보이게끔 하는것이므로...
		Vector3 vShadowPos = vPos + m_vShadowOffset;

		// 그림자에 알파값이 존재하는 경우
		if (m_bAlphaShadow)
		{
			// 해당 투명도 만큼 설정해준다 (0 ~ 1)
			m_pShadowColor->SetOpacity(m_fShadowOpacity);
		}
		else
		{
			// 불투명
			m_pShadowColor->SetOpacity(1.f);
		}

		// 그림자가 현재 폰트보다 위로 가도록
		// 해당 구문이 없으면 현재 폰트보다 아래로 갈 것이다.
		vShadowPos.y = _RESOLUTION.iHeight - vShadowPos.y - fHeight;
		// 그림자를 그린다.
		// 1. 그림자 출력 위치
		// 2. 그림자 레이아웃
		// 3. 그림자 컬러
		m_2DTarget->DrawTextLayout(D2D1::Point2F(vShadowPos.x, vShadowPos.y), m_pTextLayout, m_pShadowColor);
	}

	// 폰트에 알파가 존재하는 경우
	if (m_bAlpha)
	{
		// 알파값 적용
		m_pColor->SetOpacity(m_fOpacity);
	}
	else
	{
		// 불투명
		m_pColor->SetOpacity(1.f);
	}
	vPos.y = _RESOLUTION.iHeight - vPos.y - fHeight;

	// 텍스트를 그린다.
	// 1. 텍스트 출력 위치
	// 2. 텍스트 레이아웃
	// 3. 텍스트 컬러
	m_2DTarget->DrawTextLayout(D2D1::Point2F(vPos.x, vPos.y), m_pTextLayout, m_pColor);

	m_2DTarget->EndDraw();

	return 0;
}

CText * CText::Clone() const
{
	return new CText(*this);
}