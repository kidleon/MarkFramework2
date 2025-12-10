#include "pch.h"
#include "D3D11RenderDef.h"
#include "D3D11RenderDevice.h"

#include "Log.h"

#include "D3D11ShaderCompile.h"
#include "D3D11ShaderProp.h"
#include "D3D11Shader.h"
#include "D3D11InputLayout.h"
#include "D3D11InputLayoutCache.h"


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

	//m_pInputLayoutCache = MARK_POOL_NEW(D3D11InputLayoutCache)();
	//m_pInputLayoutCache->Init();

	return TRUE;
}

void D3D11RenderDevice::DestroyDevice() noexcept
{
	/*
	if (m_pInputLayoutCache)
	{
		MARK_POOL_FREE(m_pInputLayoutCache);
		m_pInputLayoutCache = nullptr;
	}
	*/

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

BOOL D3D11RenderDevice::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, ID3D11Buffer** ppBuffer)
{
	if (!m_pD3D11Device || !pDesc || !ppBuffer)
		return FALSE;

	HRESULT hr = m_pD3D11Device->CreateBuffer(
		pDesc,
		nullptr,
		ppBuffer
	);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL D3D11RenderDevice::CreateVertexShader(const D3D11_SHADER_COMPILE_DESC& Desc, D3D11VertexShader** ppVertexShader)
{
	D3D11_SHADER_COMPILE_RESULT compileResult = {};

	if (!D3D11CompileShader(Desc, compileResult))
	{
		(*ppVertexShader) = nullptr;
		SYS_LOG_E("D3D11RenderDevice::CreateVertexShader: Shader compilation failed - %s", Desc.szShaderName);

		return FALSE;
	}

	ID3D11VertexShader* pVertexShader = nullptr;
	HRESULT hr = m_pD3D11Device->CreateVertexShader(
		compileResult.pShaderBlob->GetBufferPointer(),
		compileResult.pShaderBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateVertexShader: CreateVertexShader failed - %s", Desc.szShaderName);

		compileResult.pShaderBlob->Release();

		if (compileResult.pShaderParams)
		{
			MARK_POOL_FREE(compileResult.pShaderParams);
			compileResult.pShaderParams = nullptr;
		}
		(*ppVertexShader) = nullptr;

		return FALSE;
	}

	D3D11ShaderProp* pShaderProp = MARK_POOL_NEW(D3D11ShaderProp)(
		compileResult.pShaderParams,
		compileResult.NumShaderParams
	);

	D3D11_INPUTLAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumVertexFormat = compileResult.NumVertexFormat;
	memcpy(inputLayoutDesc.VertexFormats, compileResult.VertexFormats, sizeof(UINT32) * MAX_VERTEX_FORMAT);

	D3D11InputLayout* pInputLayout = nullptr;
	if (!CreateInputLayout(inputLayoutDesc, &pInputLayout))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateVertexShader: CreateInputLayout failed - %s", Desc.szShaderName);
	}

	*ppVertexShader = MARK_POOL_NEW(D3D11VertexShader)(
		pVertexShader,
		pShaderProp,
		pInputLayout
	);

	compileResult.pShaderBlob->Release();

	return TRUE;
}

BOOL D3D11RenderDevice::CreatePixelShader(const D3D11_SHADER_COMPILE_DESC& Desc, D3D11PixelShader** ppPixelShader)
{
	D3D11_SHADER_COMPILE_RESULT compileResult = {};

	if (!D3D11CompileShader(Desc, compileResult))
	{
		(*ppPixelShader) = nullptr;
		SYS_LOG_E("D3D11RenderDevice::CreatePixelShader: Shader compilation failed - %s", Desc.szShaderName);

		return FALSE;
	}

	ID3D11PixelShader* pPixelShader = nullptr;
	HRESULT hr = m_pD3D11Device->CreatePixelShader(
		compileResult.pShaderBlob->GetBufferPointer(),
		compileResult.pShaderBlob->GetBufferSize(),
		nullptr,
		&pPixelShader
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderDevice::CreatePixelShader: CreatePixelShader failed - %s", Desc.szShaderName);

		compileResult.pShaderBlob->Release();

		if (compileResult.pShaderParams)
		{
			MARK_POOL_FREE(compileResult.pShaderParams);
			compileResult.pShaderParams = nullptr;
		}
		(*ppPixelShader) = nullptr;

		return FALSE;
	}

	D3D11ShaderProp* pShaderProp = MARK_POOL_NEW(D3D11ShaderProp)(
		compileResult.pShaderParams,
		compileResult.NumShaderParams
	);

	(*ppPixelShader) = MARK_POOL_NEW(D3D11PixelShader)(
		pPixelShader,
		pShaderProp
	);

	compileResult.pShaderBlob->Release();

	return TRUE;
}

BOOL D3D11RenderDevice::CreateInputLayout(const D3D11_INPUTLAYOUT_DESC& Desc, D3D11InputLayout** ppInputLayout)
{
	D3D11_INPUT_ELEMENT_DESC InputElementDescs[MAX_VERTEX_FORMAT] = {};

	VERTEX_FORMAT_INDEX VertexFormatIndices[MAX_VERTEX_FORMAT] = {};
	VERTEX_FORMAT VertexFormats[MAX_VERTEX_FORMAT] = {};

	UINT32 MIN_NUM_VERTEX_FORMAT = T_MIN(Desc.NumVertexFormat, MAX_VERTEX_FORMAT);

	UINT32 VertexFormat = 0;

	for (UINT32 i = 0; i < MIN_NUM_VERTEX_FORMAT; ++i)
	{
		D3D11_INPUT_ELEMENT_DESC& InputElementDesc = InputElementDescs[i];

		InputElementDesc.SemanticIndex = i;
		InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		VertexFormats[i] = Desc.VertexFormats[i];

		switch (Desc.VertexFormats[i])
		{
			case VERTEX_FORMAT::POSITION:
			{
				InputElementDesc.SemanticName = "POSITION";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::POSITION;
				VertexFormat |= (UINT32)VERTEX_FORMAT::POSITION;

			} break;

			case VERTEX_FORMAT::NORMAL:
			{
				InputElementDesc.SemanticName = "NORMAL";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::NORMAL;
				VertexFormat |= (UINT32)VERTEX_FORMAT::NORMAL;
			} break;

			case VERTEX_FORMAT::COLOR:
			{
				InputElementDesc.SemanticName = "COLOR";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::COLOR;
				VertexFormat |= (UINT32)VERTEX_FORMAT::COLOR;
			} break;

			case VERTEX_FORMAT::TEXCOORD:
			{
				InputElementDesc.SemanticName = "TEXCOORD";
				InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD;
			} break;

			case VERTEX_FORMAT::BONE:
			{
				InputElementDesc.SemanticName = "BONE";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::BONE;
				VertexFormat |= (UINT32)VERTEX_FORMAT::BONE;
			} break;

			case VERTEX_FORMAT::WEIGHT:
			{
				InputElementDesc.SemanticName = "WEIGHT";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::WEIGHT;
				VertexFormat |= (UINT32)VERTEX_FORMAT::WEIGHT;
			} break;

			case VERTEX_FORMAT::TEXCOORD1:
			{
				InputElementDesc.SemanticName = "TEXCOORD1";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD1;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD1;
			} break;

			case VERTEX_FORMAT::TEXCOORD2:
			{
				InputElementDesc.SemanticName = "TEXCOORD2";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD2;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD2;
			} break;

			case VERTEX_FORMAT::TEXCOORD3:
			{
				InputElementDesc.SemanticName = "TEXCOORD3";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD3;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD3;
			} break;

			case VERTEX_FORMAT::TEXCOORD4:
			{
				InputElementDesc.SemanticName = "TEXCOORD4";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD4;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD4;
			} break;

			case VERTEX_FORMAT::TEXCOORD5:
			{
				InputElementDesc.SemanticName = "TEXCOORD5";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD5;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD5;
			} break;

			case VERTEX_FORMAT::TEXCOORD6:
			{
				InputElementDesc.SemanticName = "TEXCOORD6";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD6;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD6;
			} break;

			case VERTEX_FORMAT::TEXCOORD7:
			{
				InputElementDesc.SemanticName = "TEXCOORD7";
				InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				VertexFormatIndices[i] = VERTEX_FORMAT_INDEX::TEXCOORD7;
				VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD7;
			} break;
		}
	}

	ID3D11InputLayout* pD3D11InputLayout = nullptr;
	HRESULT hr = m_pD3D11Device->CreateInputLayout(
		InputElementDescs,
		MIN_NUM_VERTEX_FORMAT,
		Desc.pShaderBlob->GetBufferPointer(),
		Desc.pShaderBlob->GetBufferSize(),
		&pD3D11InputLayout
	);

	if (FAILED(hr))
		return FALSE;

	D3D11InputLayout* pInputLayout = MARK_POOL_NEW(D3D11InputLayout)(
		Desc.NumVertexFormat,
		VertexFormats,
		VertexFormatIndices,
		pD3D11InputLayout
	);

	(*ppInputLayout) = pInputLayout;

	return TRUE;
}
