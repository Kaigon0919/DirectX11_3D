#pragma once
#include "Component.h"

ENGINE_BEGIN

// ���� ����
enum TEXT_ALIGN_H
{
	TAH_LEFT,
	TAH_CENTER,
	TAH_RIGHT
};

// ���� ����
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
	ID2D1SolidColorBrush* m_pShadowColor; // �׸��ڿ�

										  // �ؽ�Ʈ ������ (YDJ)
	Vector3 m_vOffset;

	// ���� Ÿ��
	ID2D1RenderTarget* m_2DTarget;

	// �׸��� ��뿩��
	bool	m_bShadow;

	// �ؽ�Ʈ
	TCHAR* m_pText;

	// �ؽ�Ʈ �� ����
	int m_iMaxCount;

	// ��Ʈ ��
	TCHAR* m_pFont;

	// ��Ʈ ũ��
	float m_fSize;

	// ���� ���� ����
	TEXT_ALIGN_H m_eAlignH;
	// ���� ���� ����
	TEXT_ALIGN_V m_eAlignV;

	// ���� Ÿ��(2D, 3D, UI) 
	TEXT_RENDER_TYPE m_eRenderType;

	// ���� ���� ����
	bool m_bAlpha;
	// �׸��� ���� ���� ����
	bool m_bAlphaShadow;

	// ����
	float m_fOpacity;

	// �׸��� ����
	float m_fShadowOpacity;

	// ��Ʈ ����
	Vector4 m_vColor;
	// �׸��� ����
	Vector4 m_vShadowColor;

	// �׸��� ������
	Vector3 m_vShadowOffset;

	// ��Ʈ�� �׷��� ����
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