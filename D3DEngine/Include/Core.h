#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CCore
{
private:
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	Resolution	m_tRS;
	static bool	m_bLoop;
	float		m_fClearColor[4];
	bool		m_bEditMode;

public:
	HINSTANCE GetWindowInstance()	const;
	HWND GetWindowHandle()	const;
	bool GetEditMode()	const;

public:
	void SetClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void OnEditMode();

public:
	bool Init(HINSTANCE hInst, const TCHAR* pTitle, const TCHAR* pClass, int iWidth, int iHeight, int iIconID, int iSmallIcon, bool bEditMode = false, bool bWindowMode = true);
	bool Init(HINSTANCE hInst, HWND hWnd, int iWidth, int iHeight, bool bEditMode = false, bool bWindowMode = true);
	int Run();
	void Logic();

private:
	int Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	int Collision(float fTime);
	int Render(float fTime);

private:
	void Register(const TCHAR* pClass, int iIconID, int iSmallIconID);
	void CreateWnd(const TCHAR* pTitle, const TCHAR* pClass);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_SINGLE(CCore)
};

ENGINE_END
