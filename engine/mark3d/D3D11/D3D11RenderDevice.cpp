#include "pch.h"
#include "D3D11RenderDef.h"
#include "D3D11RenderDevice.h"


D3D11RenderDevice::D3D11RenderDevice()
{
}

D3D11RenderDevice::~D3D11RenderDevice() noexcept
{
	DestroyDevice();
}

BOOL D3D11RenderDevice::CreateDevice(HWND hWnd, uint32 Width, uint32 Height, BOOL DebugDevice)
{
	UINT32 CreateDeviceFlags = 0;
	if (DebugDevice)
		CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT32 NumFeatureLevels = 4;

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		m_DriverType,
		nullptr,
		CreateDeviceFlags,
		FeatureLevels,
		NumFeatureLevels,
		D3D11_SDK_VERSION,
		&m_pD3D11Device,
		&m_FeatureLevel,
		&m_pImmediateContext
	);

	if (E_INVALIDARG == hr)
	{
		// DirectX 11.0 플랫폼은 D3D_FEATURE_LEVEL_11_1을 인식하지 못하므로 이를 제외하고 다시 시도해야 합니다.
		hr = D3D11CreateDevice(
			nullptr,
			m_DriverType,
			nullptr,
			CreateDeviceFlags,
			&FeatureLevels[1],
			NumFeatureLevels - 1,
			D3D11_SDK_VERSION,
			&m_pD3D11Device,
			&m_FeatureLevel,
			&m_pImmediateContext
		);
	}

	if (FAILED(hr))
		return FALSE;

	// DXGI 팩토리 가져오기
	IDXGIFactory1* pDxgiFactory = nullptr;
	{
		IDXGIDevice* pDxgiDevice = nullptr;
		hr = m_pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* pAdapter = nullptr;
			hr = pDxgiDevice->GetAdapter(&pAdapter);
			if (SUCCEEDED(hr))
			{
				hr = pAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
				pAdapter->Release();
			}
			pDxgiDevice->Release();
		}
	}

	if (FAILED(hr))
		return FALSE;

	IDXGIFactory2* pDxgiFactory2 = nullptr;
	hr = pDxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&pDxgiFactory2));
	if (pDxgiFactory2)
	{
		// DirectX 11.1 시스템
		hr = m_pD3D11Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_pD3D11Device1));
		if (SUCCEEDED(hr))
		{
			(void)m_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_pImmediateContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = Width;
		sd.Height = Height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = pDxgiFactory2->CreateSwapChainForHwnd(m_pD3D11Device, hWnd, &sd, nullptr, nullptr, &m_pSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = m_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_pSwapChain));
		}

		pDxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 시스템
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width = Width;
		sd.BufferDesc.Height = Height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = pDxgiFactory->CreateSwapChain(m_pD3D11Device, &sd, &m_pSwapChain);
	}

	pDxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	pDxgiFactory->Release();

	if (FAILED(hr))
		return FALSE;

	m_hWnd = hWnd;

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return FALSE;

	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);

	if (FAILED(hr))
		return FALSE;

	// 깊이-스텐실 텍스처 생성
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = Width;
	descDepth.Height = Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pD3D11Device->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencilTexture);
	if (FAILED(hr))
		return FALSE;

	// 깊이-스텐실 뷰 생성
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pD3D11Device->CreateDepthStencilView(m_pDepthStencilTexture, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		return FALSE;

	//pBackBuffer->AddRef();
	//m_pRenderTargetView->AddRef();
	//m_pDepthStencilTexture->AddRef();
	//m_pDepthStencilView->AddRef();
	
	pBackBuffer->Release();

	return TRUE;
}

void D3D11RenderDevice::DestroyDevice() noexcept
{
	CHECK_RELEASE(m_pDepthStencilView);
	CHECK_RELEASE(m_pDepthStencilTexture);
	CHECK_RELEASE(m_pRenderTargetView);

	CHECK_RELEASE(m_pSwapChain1);
	CHECK_RELEASE(m_pSwapChain);

	CHECK_RELEASE(m_pImmediateContext1);
	CHECK_RELEASE(m_pImmediateContext);

	CHECK_RELEASE(m_pD3D11Device1);
	CHECK_RELEASE(m_pD3D11Device);
}