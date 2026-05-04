#include "pch.h"
#include "D3D11RenderDevice.h"
#include "D3D11BufferPool.h"
#include "D3D11ShaderProgram.h"


namespace mark
{
	D3D11RenderDevice* D3D11RenderDevice::s_pInstance = nullptr;

	D3D11RenderDevice::D3D11RenderDevice()
	{
		if (!s_pInstance)
			s_pInstance = this;
	}

	D3D11RenderDevice::~D3D11RenderDevice() noexcept
	{
		DestroyDevice();

		if (s_pInstance == this)
			s_pInstance = nullptr;
	}

	bool D3D11RenderDevice::CreateDevice(HWND hWnd, uint32_t Width, uint32_t Height, bool DebugDevice)
	{
		uint32_t CreateDeviceFlags = 0;
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

		uint32_t NumFeatureLevels = 4;

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
		if (SUCCEEDED(hr))
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
		pDxgiFactory2->Release();
		pDxgiFactory->Release();

		m_hWnd = hWnd;

		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
		if (FAILED(hr))
			return FALSE;

		hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
		pBackBuffer->Release();

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

		m_pD3D11BufferPool = CORE_NEW(D3D11BufferPool)(m_pD3D11Device);

		return TRUE;
	}

	void D3D11RenderDevice::DestroyDevice() noexcept
	{
		CORE_DELETE(D3D11BufferPool, m_pD3D11BufferPool);

		if (m_pImmediateContext)
		{
			m_pImmediateContext->ClearState();
			m_pImmediateContext->Flush();
		}

		//CHECK_RELEASE(m_pBackBuffer_RT);

		CHECK_RELEASE(m_pDepthStencilView);
		CHECK_RELEASE(m_pDepthStencilTexture);
		CHECK_RELEASE(m_pRenderTargetView);

		CHECK_RELEASE(m_pSwapChain);
		CHECK_RELEASE(m_pSwapChain1);

		CHECK_RELEASE(m_pImmediateContext1);
		CHECK_RELEASE(m_pImmediateContext);

		CHECK_RELEASE(m_pD3D11Device1);
		CHECK_RELEASE(m_pD3D11Device);

		ReportLiveObjects();
	}

	void D3D11RenderDevice::ReportLiveObjects() noexcept
	{
		typedef HRESULT(WINAPI* DXGIGetDebugInterfaceFunc)(REFIID, void**);

		HMODULE hModule = GetModuleHandleW(L"dxgidebug.dll");
		if (!hModule)
		{
			hModule = LoadLibraryW(L"dxgidebug.dll");
		}

		if (hModule)
		{
			auto dxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterfaceFunc>(
				GetProcAddress(hModule, "DXGIGetDebugInterface")
				);

			if (dxgiGetDebugInterface)
			{
				IDXGIDebug* dxgiDebug = nullptr;
				if (SUCCEEDED(dxgiGetDebugInterface(IID_PPV_ARGS(&dxgiDebug))))
				{
					OutputDebugStringW(L"\n========== Live Object Report ==========\n");

					// DXGI_DEBUG_ALL: 모든 DXGI/D3D 객체 보고
					// DXGI_DEBUG_RLO_ALL: 상세 정보 포함
					dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

					OutputDebugStringW(L"=========================================\n\n");

					dxgiDebug->Release();
				}
			}

			FreeLibrary(hModule);
			hModule = nullptr;
		}
	}

	//---------------------------------------------------------------------
	// Resource Methods

	ID3D11Buffer* D3D11RenderDevice::CreateBuffer(const GPUBufferCreateDesc& CreateDesc)
	{
		ID3D11Buffer* pD3D11Buffer = m_pD3D11BufferPool->Acquire(
			CreateDesc.Type,
			CreateDesc.Usage,
			CreateDesc.BufferSize
		);

		if (pD3D11Buffer)
			return pD3D11Buffer;

		D3D11_BUFFER_DESC BuffDesc = {};
		BuffDesc.ByteWidth = (UINT)CreateDesc.BufferSize;
		BuffDesc.BindFlags = D3D11_IMPL_BUFFER_BIND_FLAGS[(int)CreateDesc.Type];
		BuffDesc.Usage = D3D11_IMPL_BUFFER_USAGE[(int)CreateDesc.Usage];
		BuffDesc.CPUAccessFlags = (CreateDesc.Usage == BUFFER_USAGE::DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;

		pD3D11Buffer = nullptr;
		if (FAILED(m_pD3D11Device->CreateBuffer(&BuffDesc, nullptr, &pD3D11Buffer)))
		{
			SYS_LOG_ERR_F("Failed to create D3D11 buffer (Type: {}, Usage: {}, Size: {})", (int)CreateDesc.Type, (int)CreateDesc.Usage, CreateDesc.BufferSize);
			return nullptr;
		}

		return pD3D11Buffer;
	}

	void D3D11RenderDevice::ReleaseBuffer(ID3D11Buffer* pBuffer)
	{
		if (!pBuffer) [[unlikely]]
			return;

		if (m_pD3D11BufferPool->Release(pBuffer))
			return;

		pBuffer->Release();
	}

	D3D11ShaderProgram* D3D11RenderDevice::CompileShaderProgram(const ShaderProgramCreateDesc& CreateDesc)
	{

		return nullptr;
	}
}
