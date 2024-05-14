#include "Core.h"
#include "Device.h"
#include "Resource/ResourcesManager.h"
#include "Render/RenderManager.h"
#include "PathManager.h"
#include "Render/ShaderManager.h"
#include "Render/Shader.h"
#include "Resource/Mesh.h"
#include "Scene/SceneManager.h"
#include "TimerManager.h"
#include "Timer.h"
#include "Input.h"
#include "CollisionManager.h"
#include "Navigation/NavigationManager.h"
#include "FontManager.h"
#include "SoundManager.h"

ENGINE_USING

DEFINITION_SINGLE(CCore)

bool CCore::m_bLoop = true;

CCore::CCore()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(21871);

#ifdef _DEBUG
	int * a = new int;
#endif

	m_bEditMode = false;

	memset(m_fClearColor, 0, sizeof(float) * 4);
}

CCore::~CCore()
{
	DESTROY_SINGLE(CSceneManager);
	DESTROY_SINGLE(CSoundManager);
	DESTROY_SINGLE(CFontManager);
	DESTROY_SINGLE(CNavigationManager);
	DESTROY_SINGLE(CInput);
	DESTROY_SINGLE(CCollisionManager);
	DESTROY_SINGLE(CTimerManager);
	DESTROY_SINGLE(CRenderManager);
	DESTROY_SINGLE(CResourcesManager);
	DESTROY_SINGLE(CPathManager);

	DESTROY_SINGLE(CDevice);
}

HINSTANCE CCore::GetWindowInstance() const
{
	return m_hInst;
}

HWND CCore::GetWindowHandle() const
{
	return m_hWnd;
}

bool CCore::GetEditMode() const
{
	return m_bEditMode;
}

void CCore::SetClearColor(unsigned char r, unsigned char g,
	unsigned char b, unsigned char a)
{
	m_fClearColor[0] = r / 255.f;
	m_fClearColor[1] = g / 255.f;
	m_fClearColor[2] = b / 255.f;
	m_fClearColor[3] = a / 255.f;
}

void CCore::OnEditMode()
{
	m_bEditMode = true;
}

bool CCore::Init(HINSTANCE hInst, const TCHAR * pTitle, const TCHAR * pClass,
	int iWidth, int iHeight, int iIconID, int iSmallIcon, bool bEditMode, bool bWindowMode)
{
	m_hInst = hInst;
	m_tRS.iWidth = iWidth;
	m_tRS.iHeight = iHeight;

	Register(pClass, iIconID, iSmallIcon);
	CreateWnd(pTitle, pClass);

	return Init(m_hInst, m_hWnd, iWidth, iHeight, bEditMode, bWindowMode);
}

bool CCore::Init(HINSTANCE hInst, HWND hWnd, int iWidth, int iHeight, bool bEditMode, bool bWindowMode)
{
	m_hInst = hInst;
	m_hWnd = hWnd;
	m_bEditMode = bEditMode;

	if (!GET_SINGLE(CDevice)->Init(hWnd, iWidth, iHeight, bWindowMode))
		return false;

	// 경로관리자 초기화
	if (!GET_SINGLE(CPathManager)->Init())
		return false;
	// Font 관리자 초기화
	if (!GET_SINGLE(CFontManager)->Init())
		return false;
	// Sound 관리자 초기화
	if (!GET_SINGLE(CSoundManager)->Init())
		return false;

	// 입력 관리자 초기화
	if (!GET_SINGLE(CInput)->Init(m_hInst, m_hWnd))
		return false;

	// 리소스 관리자 초기화
	if (!GET_SINGLE(CResourcesManager)->Init())
		return false;

	// 렌더링 관리자 초기화 
	if (!GET_SINGLE(CRenderManager)->Init())
		return false;

	// 시간관리자 초기화
	if (!GET_SINGLE(CTimerManager)->Init())
		return false;

	// 충돌관리자 초기화
	if (!GET_SINGLE(CCollisionManager)->Init())
		return false;

	// 내비게이션 관리자 초기화
	if (!GET_SINGLE(CNavigationManager)->Init())
		return false;

	// 장면관리자 초기화
	if (!GET_SINGLE(CSceneManager)->Init())
		return false;

	return true;
}

int CCore::Run()
{
	MSG msg;

	// 기본 메시지 루프입니다.
	while (m_bLoop)
	{
		// PeekMessage : 메세지가 없을때는 FALSE를 반환하면서 바로 빠져나온다.
		// 메세지가 있을 경우 TRUE를 반환하게 된다.
		// 이 메세지를 이용하면 윈도우의 데드타임을 이용해서 게임을 제작할 수 있다.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			// 실제 게임 구현부분은 여기에 들어와야 한다.
			Logic();
		}
	}

	return (int)msg.wParam;
}

void CCore::Logic()
{
	CTimer*	pTimer = GET_SINGLE(CTimerManager)->FindTimer("MainThread");

	pTimer->Update();

	float	fTime = pTimer->GetTime();

	GET_SINGLE(CSoundManager)->Update(fTime);

	if (Input(fTime) == SC_NEXT)
		return;

	if (Update(fTime) == SC_NEXT)
		return;

	if (LateUpdate(fTime) == SC_NEXT)
		return;
	Collision(fTime);
	Render(fTime);
}

int CCore::Input(float fTime)
{
	//GET_SINGLE(CSceneManager)->Input(fTime);
	GET_SINGLE(CInput)->Update(fTime);

	return SC_NONE;
}

int CCore::Update(float fTime)
{
	int sc =  GET_SINGLE(CSceneManager)->Update(fTime);

	return sc;
}

int CCore::LateUpdate(float fTime)
{
	int sc = GET_SINGLE(CSceneManager)->LateUpdate(fTime);

	return sc; 
}

int CCore::Collision(float fTime)
{
	GET_SINGLE(CCollisionManager)->Collision(fTime);
	//GET_SINGLE(CSceneManager)->Collision(fTime);

	return SC_NONE;
}

int CCore::Render(float fTime)
{
	GET_SINGLE(CDevice)->Clear(m_fClearColor);

	int sc = GET_SINGLE(CSceneManager)->Render(fTime);
	GET_SINGLE(CRenderManager)->ComputeInstancing();
	GET_SINGLE(CRenderManager)->Render(fTime);
	GET_SINGLE(CInput)->Render(fTime);

	GET_SINGLE(CDevice)->Present();

	return sc;
}

void CCore::Register(const TCHAR * pClass, int iIconID, int iSmallIconID)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CCore::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(iIconID));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_MY180629);
	wcex.lpszClassName = pClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(iSmallIconID));

	RegisterClassEx(&wcex);
}

void CCore::CreateWnd(const TCHAR * pTitle, const TCHAR * pClass)
{
	m_hWnd = CreateWindow(pClass, pTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, m_tRS.iWidth, m_tRS.iHeight, nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd)
		return;

	RECT		rc = { 0, 0, m_tRS.iWidth, m_tRS.iHeight };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
		GET_SINGLE(CInput)->SetWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		break;
	case WM_DESTROY:
		m_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
