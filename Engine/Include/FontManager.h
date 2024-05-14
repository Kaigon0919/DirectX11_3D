#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CFontManager
{
private:
	// IDWriteFactory �������̽��� DirectWrite�� �����
	// �ؽ�Ʈ ������� �ϱ� ���� �ݵ�� �ʿ��� �������̽��̴�.
	IDWriteFactory* m_pWriteFactory;
	unordered_map<string, IDWriteTextFormat*> m_mapFont;
	unordered_map<unsigned int, ID2D1SolidColorBrush*> m_mapColor;

public:
	bool Init();
	bool CreateTextFormat(const string& strName, const TCHAR* pFontName, int iWeight, int iStyle, int iStretch, float fSize,	const TCHAR* pLocalName);
	ID2D1SolidColorBrush* CreateColor(float r, float g, float b, float a);
	ID2D1SolidColorBrush* CreateColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	ID2D1SolidColorBrush* CreateColor(const Vector4& vColor);
	ID2D1SolidColorBrush* CreateColor(unsigned int iColor);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* pText, IDWriteTextFormat* pFormat, float fWidth, float fHeight);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* pText, const string& strFontKey, float fWidth, float fHeight);
	IDWriteTextFormat* FindTextFormat(const string& strName);
	ID2D1SolidColorBrush* FindColor(float r, float g, float b, float a);
	ID2D1SolidColorBrush* FindColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	ID2D1SolidColorBrush* FindColor(const Vector4& vColor);
	ID2D1SolidColorBrush* FindColor(unsigned int iColor);
	unsigned int CreateColorKey(float r, float g, float b, float a);
	unsigned int CreateColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	unsigned int CreateColorKey(const Vector4& vColor);


	DECLARE_SINGLE(CFontManager)
};
ENGINE_END

