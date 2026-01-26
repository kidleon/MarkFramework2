#include "pch.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderTarget.h"
using Microsoft::WRL::ComPtr;


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

	m_pRenderTargetView->AddRef();
	m_pDepthStencilTexture->AddRef();
	m_pDepthStencilView->AddRef();

	m_pBackBuffer_RT = D3D11_POOL_NEW(D3D11RenderTarget)(
		Width,
		Height,
		COLOR_FORMAT::R8G8B8A8_UNORM,
		Width,
		Height,
		COLOR_FORMAT::D24_UNORM_S8_UINT,
		pBackBuffer,
		m_pRenderTargetView,
		m_pDepthStencilTexture,
		m_pDepthStencilView
	);

	return TRUE;
}

void D3D11RenderDevice::DestroyDevice() noexcept
{
	if (m_pImmediateContext)
	{
		m_pImmediateContext->ClearState();
		m_pImmediateContext->Flush();
	}

	CHECK_RELEASE(m_pBackBuffer_RT);

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

BOOL D3D11RenderDevice::CreateInputLayout(const D3D11_INPUTLAYOUT_DESC& Desc, ID3D11InputLayout** ppOut)
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[MAX_VERTEX_FORMAT] = {};
	
	size_t AlignedOffset = 0;
	UINT32 TextureCoordCount = 0;
	for (UINT32 i = 0; i < Desc.NumVertexFormat; ++i)
	{
		InputElementDesc[i].InputSlot = 0;
		InputElementDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		switch (Desc.VertexFormats[i])
		{
			case VERTEX_FORMAT::POSITION:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "POSITION";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT3);
			} break;

			case VERTEX_FORMAT::NORMAL:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "NORMAL";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT3);
			} break;

			case VERTEX_FORMAT::TEXCOORD:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "TEXCOORD";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT2);
				TextureCoordCount++;
			} break;

			case VERTEX_FORMAT::COLOR:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "COLOR";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TANGENT:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "TANGENT";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT3);
			} break;

			case VERTEX_FORMAT::BINORMAL:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "BINORMAL";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT3);
			} break;

			case VERTEX_FORMAT::BONE:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "BONE";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(UINT4);
			} break;

			case VERTEX_FORMAT::WEIGHT:
			{
				InputElementDesc[i].SemanticIndex = 0;
				InputElementDesc[i].SemanticName = "WEIGHT";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD1:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD1";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD2:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD2";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD3:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD3";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD4:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD4";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD5:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD5";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD6:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD6";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD7:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD7";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;

			case VERTEX_FORMAT::TEXCOORD8:
			{
				InputElementDesc[i].SemanticIndex = TextureCoordCount++;
				InputElementDesc[i].SemanticName = "TEXCOORD8";
				InputElementDesc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				InputElementDesc[i].AlignedByteOffset = static_cast<UINT>(AlignedOffset);
				AlignedOffset += sizeof(FLOAT4);
			} break;
		}
	}

	size_t s = Desc.pShaderBlob->GetBufferSize();
	ID3D11InputLayout* pD3D11InputLayout = nullptr;
	HRESULT hr = m_pD3D11Device->CreateInputLayout(
		InputElementDesc,
		Desc.NumVertexFormat,
		Desc.pShaderBlob->GetBufferPointer(),
		Desc.pShaderBlob->GetBufferSize(),
		&pD3D11InputLayout
	);

	if (FAILED(hr))
		return FALSE;

	*ppOut = pD3D11InputLayout;
	
	return TRUE;
}

BOOL D3D11RenderDevice::CreateRasterizerState(const RS_RASTERIZER_STATE& RasterizerState, ID3D11RasterizerState** ppOut)
{
	D3D11_RASTERIZER_DESC Desc = {};

	Desc.FillMode = D3D11_IMPL_FILL_MODE[(int)RasterizerState.FillMode]; // 채우기 모드 설정
	Desc.CullMode = D3D11_IMPL_CULL_MODE[(int)RasterizerState.CullMode]; // 컬링 모드 설정
	Desc.FrontCounterClockwise = RasterizerState.IsFrontCounterClockwise();
	Desc.DepthBias = RasterizerState.DepthBias;
	Desc.DepthBiasClamp = RasterizerState.DepthBiasClamp;
	Desc.SlopeScaledDepthBias = RasterizerState.SlopeScaledDepthBias;
	Desc.DepthClipEnable = RasterizerState.IsDepthClipEnabled();
	Desc.ScissorEnable = RasterizerState.IsScissorEnabled();
	Desc.MultisampleEnable = RasterizerState.IsMultisampleEnabled();
	Desc.AntialiasedLineEnable = RasterizerState.IsAntialiasedLineEnabled();

	ID3D11RasterizerState* pD3D11RasterizerState = nullptr;
	if (FAILED(m_pD3D11Device->CreateRasterizerState(&Desc, &pD3D11RasterizerState)))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateRasterizerState - Failed to create rasterizer state.");
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pD3D11RasterizerState;

	return TRUE;
}

BOOL D3D11RenderDevice::CreateBlendState(const RS_BLEND_STATE& BlendState, ID3D11BlendState** ppOut)
{
	D3D11_BLEND_DESC Desc = {};

	Desc.AlphaToCoverageEnable = BlendState.AlphaToCoverageEnable;
	Desc.IndependentBlendEnable = BlendState.IndependentBlendEnable;

	for (INT32 i = 0; i < BlendState.NumBlendTargets; ++i)
	{
		const RS_BLEND_TARGET& BlendTarget = BlendState.BlendTarget[i];

		Desc.RenderTarget[i].BlendEnable = BlendTarget.BlendEnable;
		Desc.RenderTarget[i].SrcBlend = D3D11_IMPL_BLEND_FACTOR[(int)BlendTarget.SrcBlend];
		Desc.RenderTarget[i].DestBlend = D3D11_IMPL_BLEND_FACTOR[(int)BlendTarget.DestBlend];
		Desc.RenderTarget[i].BlendOp = D3D11_IMPL_BLEND_OP[(int)BlendTarget.BlendOp];
		Desc.RenderTarget[i].SrcBlendAlpha = D3D11_IMPL_BLEND_FACTOR[(int)BlendTarget.SrcBlendAlpha];
		Desc.RenderTarget[i].DestBlendAlpha = D3D11_IMPL_BLEND_FACTOR[(int)BlendTarget.DestBlendAlpha];
		Desc.RenderTarget[i].BlendOpAlpha = D3D11_IMPL_BLEND_OP[(int)BlendTarget.BlendOpAlpha];
		Desc.RenderTarget[i].RenderTargetWriteMask = BlendTarget.RenderTargetWriteMask;
	}

	ID3D11BlendState* pD3D11BlendState = nullptr;
	if (FAILED(m_pD3D11Device->CreateBlendState(&Desc, &pD3D11BlendState)))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateBlendState - Failed to create blend state.");
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pD3D11BlendState;

	return TRUE;
}

BOOL D3D11RenderDevice::CreateDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState, ID3D11DepthStencilState** ppOut)
{
	D3D11_DEPTH_STENCIL_DESC Desc = {};

	Desc.DepthEnable = DepthStencilState.DepthEnable;
	Desc.DepthWriteMask = D3D11_IMPL_DEPTH_WRITE_MASK[(int)DepthStencilState.DepthWriteMask];
	Desc.DepthFunc = D3D11_IMPL_COMPARISON_FUNC[(int)DepthStencilState.DepthFunc];
	Desc.StencilEnable = DepthStencilState.StencilEnable;
	Desc.StencilReadMask = DepthStencilState.StencilReadMask;
	Desc.StencilWriteMask = DepthStencilState.StencilWriteMask;
	Desc.FrontFace.StencilFailOp = D3D11_IMPL_STENCIL_OP[(int)DepthStencilState.FrontFace.StencilFailOp];
	Desc.FrontFace.StencilDepthFailOp = D3D11_IMPL_STENCIL_OP[(int)DepthStencilState.FrontFace.StencilDepthFailOp];
	Desc.FrontFace.StencilPassOp = D3D11_IMPL_STENCIL_OP[(int)DepthStencilState.FrontFace.StencilPassOp];
	Desc.FrontFace.StencilFunc = D3D11_IMPL_COMPARISON_FUNC[(int)DepthStencilState.FrontFace.StencilFunc];
	Desc.BackFace.StencilFailOp = D3D11_IMPL_STENCIL_OP[(int)DepthStencilState.BackFace.StencilFailOp];
	Desc.BackFace.StencilDepthFailOp = D3D11_IMPL_STENCIL_OP[(int)DepthStencilState.BackFace.StencilDepthFailOp];
	Desc.BackFace.StencilPassOp = D3D11_IMPL_STENCIL_OP[(int)DepthStencilState.BackFace.StencilPassOp];
	Desc.BackFace.StencilFunc = D3D11_IMPL_COMPARISON_FUNC[(int)DepthStencilState.BackFace.StencilFunc];

	ID3D11DepthStencilState* pD3D11DepthStencilState = nullptr;
	if (FAILED(m_pD3D11Device->CreateDepthStencilState(&Desc, &pD3D11DepthStencilState)))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateDepthStencilState - Failed to create depth-stencil state.");
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pD3D11DepthStencilState;

	return TRUE;
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
			ComPtr<IDXGIDebug> dxgiDebug;
			if (SUCCEEDED(dxgiGetDebugInterface(IID_PPV_ARGS(&dxgiDebug))))
			{
				OutputDebugStringW(L"\n========== Live Object Report ==========\n");

				// DXGI_DEBUG_ALL: 모든 DXGI/D3D 객체 보고
				// DXGI_DEBUG_RLO_ALL: 상세 정보 포함
				dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

				OutputDebugStringW(L"=========================================\n\n");
			}
		}
	}
}
