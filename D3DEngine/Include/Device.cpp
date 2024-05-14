#include "Device.h"

ENGINE_USING

DEFINITION_SINGLE(CDevice)

CDevice::CDevice()	:
	m_pDevice(nullptr),
	m_pContext(nullptr),
	m_pSwapChain(nullptr),
	m_pTargetView(nullptr),
	m_pDepthView(nullptr),
	m_p2DTarget(nullptr),
	m_p2DFactory(nullptr)
{
}

CDevice::~CDevice()
{
	SAFE_RELEASE(m_p2DTarget);
	SAFE_RELEASE(m_p2DFactory);

	SAFE_RELEASE(m_pDepthView);
	SAFE_RELEASE(m_pTargetView);
	SAFE_RELEASE(m_pSwapChain);

	m_pContext->ClearState();

	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

ID3D11Device * CDevice::GetDevice() const
{
	return m_pDevice;
}

ID3D11DeviceContext * CDevice::GetContext() const
{
	return m_pContext;
}

IDXGISwapChain * CDevice::GetSwapChain() const
{
	return m_pSwapChain;
}

Resolution CDevice::GetResolution() const
{
	return m_tRS;
}

ID2D1RenderTarget * CDevice::Get2DRenderTarget() const
{
	return m_p2DTarget;
}

Vector2 CDevice::GetWindowToViewportRatio() const
{
	RECT	rcWindow = {};
	GetClientRect(m_hWnd, &rcWindow);

	Vector2	vSize;
	vSize.x = float(rcWindow.right - rcWindow.left);
	vSize.y = float(rcWindow.bottom - rcWindow.top);

	Vector2	vViewport;
	vViewport.x = (float)m_tRS.iWidth;
	vViewport.y = (float)m_tRS.iHeight;

	return vViewport / vSize;
}

bool CDevice::Init(HWND hWnd, unsigned int iWidth, 
	unsigned int iHeight, bool bWindowMode)
{
	m_hWnd = hWnd;
	unsigned int	iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	m_tRS.iWidth = iWidth;
	m_tRS.iHeight = iHeight;

	iFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL	eFLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL	eFLevel1 = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC	tSwapDesc = {};

	tSwapDesc.BufferDesc.Width = iWidth;
	tSwapDesc.BufferDesc.Height = iHeight;
	tSwapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tSwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	tSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapDesc.BufferDesc.RefreshRate.Numerator = 1;
	tSwapDesc.BufferDesc.RefreshRate.Denominator = 60;
	tSwapDesc.BufferCount = 1;
	tSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapDesc.OutputWindow = hWnd;
	tSwapDesc.SampleDesc.Count = 8;
	tSwapDesc.SampleDesc.Quality = 0;
	tSwapDesc.Windowed = bWindowMode;
	tSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, &eFLevel,
		1, D3D11_SDK_VERSION, &tSwapDesc,
		&m_pSwapChain, &m_pDevice, &eFLevel1, &m_pContext)))
		return false;

	ID3D11Texture2D*	pBuffer = nullptr;

	// 아래처럼 COM 객체를 얻어오면 레퍼런스 카운트가 증가한다.
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);

	m_pDevice->CreateRenderTargetView(pBuffer, nullptr, &m_pTargetView);

	SAFE_RELEASE(pBuffer);

	// 깊이 뷰를 만든다.
	D3D11_TEXTURE2D_DESC	tDepthBuf = {};
	tDepthBuf.Width = iWidth;
	tDepthBuf.Height = iHeight;
	tDepthBuf.ArraySize = 1;
	tDepthBuf.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tDepthBuf.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDepthBuf.Usage = D3D11_USAGE_DEFAULT;
	tDepthBuf.MipLevels = 1;
	tDepthBuf.SampleDesc.Count = 8;
	tDepthBuf.SampleDesc.Quality = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&tDepthBuf, nullptr, &pBuffer)))
		return false;

	m_pDevice->CreateDepthStencilView(pBuffer, nullptr, &m_pDepthView);

	SAFE_RELEASE(pBuffer);

	// 만들어준 타겟뷰와 뎁스뷰를 렌더링 파이프라인에
	// 묶어준다.
	m_pContext->OMSetRenderTargets(1, &m_pTargetView,
		m_pDepthView);

	D3D11_VIEWPORT	tVP = {};

	tVP.Width = (float)iWidth;
	tVP.Height = (float)iHeight;
	tVP.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &tVP);

	// D2D Factory를 생성한다.
	D2D1_FACTORY_OPTIONS	tOption = {};
	tOption.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, tOption,
		&m_p2DFactory)))
		return false;

	// DirectX11 BackBuffer 타겟을 이용하여 2D 렌더링 타겟을 설정해준다.
	IDXGISurface*	pBackSurface = nullptr;

	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackSurface));

	// 2D 렌더타겟을 생성하기 위한 옵션 설정
	D2D1_RENDER_TARGET_PROPERTIES	props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_HARDWARE,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	if (FAILED(m_p2DFactory->CreateDxgiSurfaceRenderTarget(pBackSurface, props,
		&m_p2DTarget)))
		return false;

	SAFE_RELEASE(pBackSurface);

	return true;
}

void CDevice::Clear(float fClearColor[4])
{
	m_pContext->ClearRenderTargetView(m_pTargetView,
		fClearColor);
	m_pContext->ClearDepthStencilView(m_pDepthView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.f, 0);
}

void CDevice::Present()
{
	m_pSwapChain->Present(0, 0);
}
