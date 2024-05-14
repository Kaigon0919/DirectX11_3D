#pragma once
#include "Component.h"

ENGINE_BEGIN

// 가로 정렬
enum TEXT_ALIGN_H
{
	TAH_LEFT,
	TAH_CENTER,
	TAH_RIGHT
};

// 세로 정렬
enum TEXT_ALIGN_V
{
	TAV_TOP,
	TAV_MID,
	TAV_BOTTOM
};

enum TEXT_RENDER_TYPE
{
	TRT_UI,
	TRT_2D,
	TRT_3D
};

class ENGINE_DLL CText :
	public CComponent
{
	friend class CGameObject;

protected:
	CText();
	CText(const CText& com);
	virtual ~CText();

private:
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextLayout* m_pTextLayout;

	ID2D1SolidColorBrush* m_pColor;
	ID2D1SolidColorBrush* m_pShadowColor; // 그림자용

										  // 텍스트 오프셋 (YDJ)
	Vector3 m_vOffset;

	// 랜더 타겟
	ID2D1RenderTarget* m_2DTarget;

	// 그림자 사용여부
	bool	m_bShadow;

	// 텍스트
	TCHAR* m_pText;

	// 텍스트 총 길이
	int m_iMaxCount;

	// 폰트 명
	TCHAR* m_pFont;

	// 폰트 크기
	float m_fSize;

	// 가로 정렬 기준
	TEXT_ALIGN_H m_eAlignH;
	// 세로 정렬 기준
	TEXT_ALIGN_V m_eAlignV;

	// 랜더 타입(2D, 3D, UI) 
	TEXT_RENDER_TYPE m_eRenderType;

	// 알파 적용 여부
	bool m_bAlpha;
	// 그림자 알파 적용 여부
	bool m_bAlphaShadow;

	// 투명도
	float m_fOpacity;

	// 그림자 투명도
	float m_fShadowOpacity;

	// 폰트 색상
	Vector4 m_vColor;
	// 그림자 색상
	Vector4 m_vShadowColor;

	// 그림자 오프셋
	Vector3 m_vShadowOffset;

	// 폰트를 그려낼 영역
	D2D1_RECT_F m_tRenderArea;

public:
	void SetRenderType(TEXT_RENDER_TYPE eType);
	void SetTextCount(int iMaxCount);
	void SetText(const TCHAR* pText);
	void AddText(const TCHAR* pText);

	void SetFont(const string& strName, const TCHAR* pFontName,	int iWeight, int iStyle, int iStretch, float fSize, const TCHAR* pLocalName);
	void SetFont(const string& strName);
	void SetSize(float fSize);
	void SetAlignH(TEXT_ALIGN_H eAlign);
	void SetAlignV(TEXT_ALIGN_V eAlign);
	void Shadow(bool bShadow);
	void SetOffset(const Vector3& vOffset); // YDJ
	void SetShadowOffset(const Vector3& vShadowOffset);
	void SetShadowColor(float r, float g, float b, float a);
	void SetShadowColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void SetShadowColor(const Vector4& vColor);
	void SetShadowColor(unsigned int iColor);
	void AlphaBlend(bool bAlpha);
	void ShadowAlphaBlend(bool bAlpha);
	void SetOpacity(float fOpacity);
	void SetShadowOpacity(float fOpacity);
	void SetColor(float r, float g, float b, float a);
	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void SetColor(const Vector4& vColor);
	void SetColor(unsigned int iColor);
	void SetRenderArea(float l, float t, float r, float b);

private:
	void CreateTextLayout();

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CText* Clone()	const;
};

ENGINE_END