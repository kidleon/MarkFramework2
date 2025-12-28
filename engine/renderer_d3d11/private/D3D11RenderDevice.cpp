#include "pch.h"
#include "D3D11RenderDevice.h"
#include "idgen.h"
#include "fnv.h"
#include "D3D11ShaderCompile.h"
#include "D3D11Shader.h"
#include "D3D11ShaderParamTable.h"
#include "D3D11InputLayoutCache.h"
#include "D3D11InputLayout.h"
#include "D3D11ConstantBufferPool.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11ShaderCache.h"
#include "D3D11Texture1D.h"
#include "D3D11Texture2D.h"
#include "D3D11RenderTarget.h"
#include "D3D11Global.h"
#include "D3D11RenderState.h"
#include "D3D11RenderStateCache.h"
#include "D3D11RenderCommandPool.h"


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

	pBackBuffer->AddRef();
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

	
	pBackBuffer->Release();

	m_pInputLayoutCache = D3D11_NEW(D3D11InputLayoutCache)();
	m_pInputLayoutCache->Init();

	m_pConstantBufferPool = D3D11_NEW(D3D11ConstantBufferPool)(this);
	if (!m_pConstantBufferPool->Init())
	{
		SYS_LOG_E("D3D11RenderDevice::CreateDevice: Failed to initialize constant buffer pool");
		return FALSE;
	}

	m_pShaderCache = D3D11_NEW(D3D11ShaderCache)();
	m_pShaderCache->Init();

	m_pRenderStateCache = D3D11_NEW(D3D11RenderStateCache)();
	m_pRenderStateCache->Init();

	m_pRenderCommandPool = D3D11_NEW(D3D11RenderCommandPool)();
	if (!m_pRenderCommandPool->Init(128))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateDevice: Failed to initialize render command pool");
		return FALSE;
	}

	D3D11Global::s_pRenderDevice = this;

	return TRUE;
}

void D3D11RenderDevice::DestroyDevice() noexcept
{
	if (m_pRenderCommandPool)
	{
		D3D11_DELETE(m_pRenderCommandPool, D3D11RenderCommandPool);
		m_pRenderCommandPool = nullptr;
	}
	
	if (m_pRenderStateCache)
	{
		D3D11_DELETE(m_pRenderStateCache, D3D11RenderStateCache);
		m_pRenderStateCache = nullptr;
	}

	if (m_pInputLayoutCache)
	{
		D3D11_DELETE(m_pInputLayoutCache, D3D11InputLayoutCache);
		m_pInputLayoutCache = nullptr;
	}

	if (m_pConstantBufferPool)
	{
		D3D11_DELETE(m_pConstantBufferPool, D3D11ConstantBufferPool);
		m_pConstantBufferPool = nullptr;
	}

	if (m_pShaderCache)
	{
		D3D11_DELETE(m_pShaderCache, D3D11ShaderCache);
		m_pShaderCache = nullptr;
	}

	CHECK_RELEASE(m_pBackBuffer_RT);

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

BOOL D3D11RenderDevice::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, ID3D11Buffer** ppOut)
{
	if (!m_pD3D11Device || !pDesc || !ppOut)
		return FALSE;

	HRESULT hr = m_pD3D11Device->CreateBuffer(
		pDesc,
		nullptr,
		ppOut
	);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL D3D11RenderDevice::CreateConstantBuffer(size_t BufferSize, D3D11ConstantBuffer** ppCB)
{
	D3D11ConstantBuffer* pCB = m_pConstantBufferPool->Allocate(BufferSize);
	if (!pCB)
	{
		*ppCB = nullptr;
		return FALSE;
	}

	*ppCB = pCB;

	return TRUE;
}

void D3D11RenderDevice::ReleaseConstantBuffer(D3D11ConstantBuffer** ppCB)
{
	if (!ppCB || !*ppCB)
		return;

	m_pConstantBufferPool->Release(*ppCB);
	*ppCB = nullptr;
}

BOOL D3D11RenderDevice::GetOrCreateShader(
	const D3D11_SHADER_COMPILE_DESC* pDesc,
	D3D11Shader** ppShader
)
{
	if (!pDesc || !ppShader)
		return FALSE;

	NameHash nameHash(pDesc->szShaderName);

	// FIND CACHE
	D3D11Shader* pShader = m_pShaderCache->Get(nameHash);
	if (pShader)
	{
		*ppShader = pShader;
		return TRUE;
	}

	D3D11_SHADER_COMPILE_RESULT CompileResult = {};

	if (!D3D11CompileShader(pDesc, &CompileResult))
	{
		SYS_LOG_E("D3D11RenderDevice::CreateShader: Shader compilation failed for %s", pDesc->szShaderName);
		return FALSE;
	}

	D3D11ShaderParamTable* pParamTable = D3D11_POOL_NEW(D3D11ShaderParamTable);
	pParamTable->Build(CompileResult.pShaderParams, CompileResult.NumShaderParams);

	if (pDesc->ShaderType == SHADER_TYPE::VERTEX)
	{
		// 입력 레이아웃 생성
		D3D11_INPUTLAYOUT_DESC ILDesc = {};
		ILDesc.NumVertexFormat = CompileResult.NumVertexFormat;
		ILDesc.VertexFormat = CompileResult.m_BufferFormat;
		for (UINT32 i = 0; i < CompileResult.NumVertexFormat; ++i)
		{
			ILDesc.VertexFormats[i] = (VERTEX_FORMAT)CompileResult.VertexFormats[i];
		}

		ILDesc.pShaderBlob = CompileResult.pShaderBlob;

		D3D11InputLayout* pInputLayout = nullptr;
		if (!GetOrCreateInputLayout(&ILDesc, CompileResult.NumVertexFormat, &pInputLayout))
		{
			SYS_LOG_E("D3D11RenderDevice::CreateShader: Failed to create input layout for shader %s", pDesc->szShaderName);
			return FALSE;
		}

		ID3D11VertexShader* pVS = nullptr;
		m_pD3D11Device->CreateVertexShader(
			CompileResult.pShaderBlob->GetBufferPointer(),
			CompileResult.pShaderBlob->GetBufferSize(),
			nullptr,
			&pVS
		);

		interlock_increment_l((LONG*)&m_VertexShaderIndex, MEMORY_ORDER_RELAXED);
		if (m_VertexShaderIndex > MAX_SHADER_INDEX)
		{
			SYS_LOG_E("D3D11RenderDevice::CreateShader: Exceeded maximum number of vertex shaders");
			CHECK_RELEASE(pVS);
			return FALSE;
		}

		pShader = D3D11_POOL_NEW(D3D11Shader)(
			m_VertexShaderIndex,
			nameHash,
			pParamTable,
			pVS
		);

		m_pShaderCache->Add(nameHash, pShader);
	}
	else if(pDesc->ShaderType == SHADER_TYPE::PIXEL)
	{
		ID3D11PixelShader* pPS = nullptr;
		m_pD3D11Device->CreatePixelShader(
			CompileResult.pShaderBlob->GetBufferPointer(),
			CompileResult.pShaderBlob->GetBufferSize(),
			nullptr,
			&pPS
		);

		interlock_increment_l((LONG*)&m_PixelShaderIndex, MEMORY_ORDER_RELAXED);
		if (m_PixelShaderIndex > MAX_SHADER_INDEX)
		{
			SYS_LOG_E("D3D11RenderDevice::CreateShader: Exceeded maximum number of pixel shaders");
			CHECK_RELEASE(pPS);
			return FALSE;
		}

		pShader = D3D11_POOL_NEW(D3D11Shader)(
			m_PixelShaderIndex,
			nameHash,
			pParamTable,
			pPS
		);

		m_pShaderCache->Add(nameHash, pShader);
	}
	else if (pDesc->ShaderType == SHADER_TYPE::COMPUTE)
	{
		ID3D11ComputeShader* pCS = nullptr;
		m_pD3D11Device->CreateComputeShader(
			CompileResult.pShaderBlob->GetBufferPointer(),
			CompileResult.pShaderBlob->GetBufferSize(),
			nullptr,
			&pCS
		);

		interlock_increment_l((LONG*)&m_ComputeShaderIndex, MEMORY_ORDER_RELAXED);
		if (m_ComputeShaderIndex > MAX_SHADER_INDEX)
		{
			SYS_LOG_E("D3D11RenderDevice::CreateShader: Exceeded maximum number of compute shaders");
			CHECK_RELEASE(pCS);
			return FALSE;
		}

		pShader = D3D11_POOL_NEW(D3D11Shader)(
			m_ComputeShaderIndex, 
			nameHash,
			pParamTable,
			pCS);
		m_pShaderCache->Add(nameHash, pShader);
	}
	else
	{
		SYS_LOG_E("D3D11RenderDevice::CreateShader: Unsupported shader type for shader %s", pDesc->szShaderName);
		return FALSE;
	}

	return TRUE;
}

BOOL D3D11RenderDevice::GetOrCreateInputLayout(
	const D3D11_INPUTLAYOUT_DESC* pDesc,
	UINT NumElements,
	D3D11InputLayout** ppIL
)
{
	if (!pDesc || !ppIL)
		return FALSE;

	// FIND CACHE
	D3D11InputLayout* pInputLayout = m_pInputLayoutCache->GetInputLayout(pDesc->VertexFormat);
	if (pInputLayout)
	{
		*ppIL = pInputLayout;
		return TRUE;
	}

	D3D11_INPUT_ELEMENT_DESC InputElementDescs[MAX_VERTEX_FORMAT] = {};

	VERTEX_FORMAT_INDEX VertexFormatIndices[MAX_VERTEX_FORMAT] = {};
	VERTEX_FORMAT VertexFormats[MAX_VERTEX_FORMAT] = {};

	UINT32 MIN_NUM_VERTEX_FORMAT = T_MIN(pDesc->NumVertexFormat, MAX_VERTEX_FORMAT);

	UINT32 VertexFormat = 0;

	for (UINT32 i = 0; i < MIN_NUM_VERTEX_FORMAT; ++i)
	{
		D3D11_INPUT_ELEMENT_DESC& InputElementDesc = InputElementDescs[i];

		InputElementDesc.SemanticIndex = i;
		InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		VertexFormats[i] = pDesc->VertexFormats[i];

		switch (pDesc->VertexFormats[i])
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
		pDesc->pShaderBlob->GetBufferPointer(),
		pDesc->pShaderBlob->GetBufferSize(),
		&pD3D11InputLayout
	);

	if (FAILED(hr))
		return FALSE;

	D3D11InputLayout* pIL = D3D11_POOL_NEW(D3D11InputLayout)(
		VertexFormat,
		pD3D11InputLayout
	);

	*ppIL = pIL;

	// ADD CACHE
	m_pInputLayoutCache->AddInputLayout(VertexFormat, pIL);

	return TRUE;
}

BOOL D3D11RenderDevice::FillTexture1D(
	const D3D11_TEXTURE1D_CREATE_DESC* pDesc,
	D3D11Texture1D* pTexture1D
)
{
	D3D11_TEXTURE1D_DESC desc = {};

	if (pDesc->Usage == RESOURCE_USAGE::STATIC)
	{
		desc.Usage = (pDesc->InitialSize > 0) ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else if (pDesc->Usage == RESOURCE_USAGE::DYNAMIC)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (pDesc->Usage == RESOURCE_USAGE::RW_BUFFER)
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		SYS_LOG_E("D3D11RenderDevice::CreateTexture1D: Unsupported RESOURCE_USAGE value");
		return FALSE;
	}

	desc.Width = pDesc->Width;
	desc.MipLevels = pDesc->MipLevels;
	desc.ArraySize = 1;
	desc.Format = (DXGI_FORMAT)pDesc->Format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = (pDesc->MipLevels != 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	ID3D11Texture1D* pD3D11Tex1D = nullptr;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = pDesc->pInitialData;
	data.SysMemPitch = desc.Width * (UINT32)GetPixelFormatSize(pDesc->Format);

	D3D11_SUBRESOURCE_DATA* pData = desc.Usage == D3D11_USAGE_IMMUTABLE ? &data : nullptr;

	HRESULT hr = m_pD3D11Device->CreateTexture1D(
		&desc,
		pData,
		&pD3D11Tex1D
	);

	if (FAILED(hr))
		return FALSE;

	// Shader Resource View 
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = (DXGI_FORMAT)pDesc->Format;;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	SRVDesc.Texture1D.MostDetailedMip = 0;
	SRVDesc.Texture1D.MipLevels = pDesc->MipLevels;

	ID3D11ShaderResourceView* pSRV = nullptr;
	hr = m_pD3D11Device->CreateShaderResourceView(
		pD3D11Tex1D,
		&SRVDesc,
		&pSRV
	);

	if (FAILED(hr))
	{
		pTexture1D->Release();
		return FALSE;
	}

	pTexture1D->SetData(
		desc.Width,
		desc.MipLevels,
		pDesc->Format,
		pD3D11Tex1D,
		pSRV
	);

	return TRUE;
}

BOOL D3D11RenderDevice::FillTexture2D(
	const D3D11_TEXTURE2D_CREATE_DESC* pDesc,
	D3D11Texture2D* pTexture2D
)
{
	D3D11_TEXTURE2D_DESC desc = {};

	if (pDesc->Usage == RESOURCE_USAGE::STATIC)
	{
		desc.Usage = D3D11_USAGE_DEFAULT; 
		desc.CPUAccessFlags = 0;
	}
	else if(pDesc->Usage == RESOURCE_USAGE::FIXED)
	{
		if (!pDesc->pInitialData)
			SYS_LOG_W("D3D11RenderDevice::CreateTexture2D: FIXED usage requires initial data");
		
		desc.Usage = (!pDesc->pInitialData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	else if (pDesc->Usage == RESOURCE_USAGE::DYNAMIC)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (pDesc->Usage == RESOURCE_USAGE::RW_BUFFER)
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		SYS_LOG_E("D3D11RenderDevice::CreateTexture2D: Unsupported RESOURCE_USAGE value");
		return FALSE;
	}

	desc.Width = pDesc->Width;
	desc.Height = pDesc->Height;
	desc.MipLevels = pDesc->MipLevels;
	desc.ArraySize = 1;
	desc.Format = (DXGI_FORMAT)pDesc->Format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = (pDesc->MipLevels != 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	ID3D11Texture2D* pD3D11Tex2D = nullptr;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = pDesc->pInitialData;
	data.SysMemPitch = desc.Width * (UINT32)GetPixelFormatSize(pDesc->Format);

	D3D11_SUBRESOURCE_DATA* pData = (desc.Usage == D3D11_USAGE_IMMUTABLE) ? &data : nullptr;

	HRESULT hr = m_pD3D11Device->CreateTexture2D(
		&desc,
		pData,
		&pD3D11Tex2D
	);

	if (FAILED(hr))
	{
		return FALSE;
	}

	// Shader Resource View 
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = (DXGI_FORMAT)pDesc->Format;;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture1D.MostDetailedMip = 0;
	SRVDesc.Texture1D.MipLevels = pDesc->MipLevels;

	ID3D11ShaderResourceView* pSRV = nullptr;
	hr = m_pD3D11Device->CreateShaderResourceView(
		pD3D11Tex2D,
		&SRVDesc,
		&pSRV
	);

	if (FAILED(hr))
	{
		pD3D11Tex2D->Release();
		return FALSE;
	}

	pTexture2D->SetData(
		desc.Width,
		desc.Height,
		desc.MipLevels,
		pDesc->Format,
		pD3D11Tex2D,
		pSRV
	);

	return TRUE;
}

BOOL D3D11RenderDevice::CreateRenderTarget(
	const D3D11_RENDERTARGET_CREATE_DESC* pDesc,
	D3D11RenderTarget** ppRT
)
{
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	


	return TRUE;
}

BOOL D3D11RenderDevice::GetOrCreateSamplerState(
	const RS_SAMPLER_STATE& Desc,
	D3D11SamplerState** ppOut
)
{
	uint64 Hash = fnv64_c(&Desc, sizeof(RS_SAMPLER_STATE));
	return GetOrCreateSamplerState(Hash, Desc, ppOut);
}

BOOL D3D11RenderDevice::GetOrCreateSamplerState(
	uint64 Hash,
	const RS_SAMPLER_STATE& Desc,
	D3D11SamplerState** ppOut
)
{
	D3D11SamplerState* pSamplerState = nullptr;
	if (0 < Hash && m_pRenderStateCache->TryGet(Hash, &pSamplerState))
	{
		*ppOut = pSamplerState;
		return TRUE;
	}

	D3D11_SAMPLER_DESC SamplerDesc = {};

	SamplerDesc.Filter = __D3D11ConvSamplerFilter(
		Desc.MinFilter, 
		Desc.MagFilter, 
		Desc.MipFilter, 
		Desc.MaxAnisotropy
	);

	__D3D11ConvBorderColor(
		Desc.BorderColor, 
		&Desc.CustomBorderColor, 
		SamplerDesc.BorderColor
	);

	SamplerDesc.MaxAnisotropy = Desc.MaxAnisotropy;
	SamplerDesc.MipLODBias = Desc.MipLODBias;

	SamplerDesc.AddressU = D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int32)Desc.AddressU];
	SamplerDesc.AddressV = D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int32)Desc.AddressV];
	SamplerDesc.AddressW = D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int32)Desc.AddressW];
	SamplerDesc.ComparisonFunc = D3D11_IMPL_COMPARISON_FUNC[(int32)Desc.ComparisonFunc];

	SamplerDesc.MinLOD = Desc.MinLOD;
	SamplerDesc.MaxLOD = Desc.MaxLOD;

	ID3D11SamplerState* pD3D11SamplerState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateSamplerState(
		&SamplerDesc,
		&pD3D11SamplerState
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateSamplerState: CreateSamplerState failed");
		(*ppOut) = nullptr;
		return FALSE;
	}

	interlock_increment_l((LONG*)&m_SamplerStateIndex, MEMORY_ORDER_RELAXED);
	if (m_SamplerStateIndex > MAX_STATE_INDEX)
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateSamplerState: Exceeded maximum number of sampler states");
		CHECK_RELEASE(pD3D11SamplerState);
		return FALSE;
	}

	pSamplerState = D3D11_POOL_NEW(D3D11SamplerState)();
	pSamplerState->pD3D11SamplerState = pD3D11SamplerState;
	pSamplerState->Hash = fnv64_c(&Desc, sizeof(RS_SAMPLER_STATE));
	pSamplerState->StateIndex = m_SamplerStateIndex;

	m_pRenderStateCache->Add(pSamplerState->Hash, pSamplerState);

	return TRUE;
}

BOOL D3D11RenderDevice::GetOrCreateBlendState(const RS_BLEND_STATE& Desc, D3D11BlendState** ppOut)
{
	uint64 Hash = fnv64_c(&Desc, sizeof(RS_BLEND_STATE));
	return GetOrCreateBlendState(Hash, Desc, ppOut);
}

BOOL D3D11RenderDevice::GetOrCreateBlendState(
	uint64 Hash,
	const RS_BLEND_STATE& Desc,
	D3D11BlendState** ppOut
)
{
	D3D11BlendState* pBlendState = nullptr;
	if (0 < Hash && m_pRenderStateCache->TryGet(Hash, &pBlendState))
	{
		*ppOut = pBlendState;
		return TRUE;
	}

	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = Desc.AlphaToCoverageEnable;
	BlendDesc.IndependentBlendEnable = Desc.IndependentBlendEnable;

	int32 NumBlendTargets = T_MIN(Desc.NumBlendTargets, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	for (int i = 0; i < NumBlendTargets; ++i)
	{
		BlendDesc.RenderTarget[i].BlendEnable = Desc.BlendTarget[i].BlendEnable;
		BlendDesc.RenderTarget[i].SrcBlend = D3D11_IMPL_BLEND_FACTOR[(int32)Desc.BlendTarget[i].SrcBlend];
		BlendDesc.RenderTarget[i].DestBlend = D3D11_IMPL_BLEND_FACTOR[(int32)Desc.BlendTarget[i].DestBlend];
		BlendDesc.RenderTarget[i].BlendOp = D3D11_IMPL_BLEND_OP[(int32)Desc.BlendTarget[i].BlendOp];
		BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_IMPL_BLEND_FACTOR[(int32)Desc.BlendTarget[i].SrcBlendAlpha];
		BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_IMPL_BLEND_FACTOR[(int32)Desc.BlendTarget[i].DestBlendAlpha];
		BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_IMPL_BLEND_OP[(int32)Desc.BlendTarget[i].BlendOpAlpha];
		BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_IMPL_COLOR_WRITE_MASK[(int32)Desc.BlendTarget[i].RenderTargetWriteMask];
	}

	ID3D11BlendState* pD3D11BlendState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateBlendState(
		&BlendDesc,
		&pD3D11BlendState
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateBlendState: CreateBlendState failed");
		(*ppOut) = nullptr;
		return FALSE;
	}

	interlock_increment_l((LONG*)&m_BlendStateIndex, MEMORY_ORDER_RELAXED);
	if (m_BlendStateIndex > MAX_STATE_INDEX)
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateBlendState: Exceeded maximum number of blend states");
		CHECK_RELEASE(pD3D11BlendState);
		return FALSE;
	}

	pBlendState = D3D11_POOL_NEW(D3D11BlendState)();
	pBlendState->pD3D11BlendState = pD3D11BlendState;
	pBlendState->Hash = fnv64_c(&Desc, sizeof(RS_BLEND_STATE));
	pBlendState->StateIndex = m_BlendStateIndex;

	m_pRenderStateCache->Add(pBlendState->Hash, pBlendState);

	*ppOut = pBlendState;

	return TRUE;
}

BOOL D3D11RenderDevice::GetOrCreateRasterizerState(
	const RS_RASTERIZER_STATE& Desc,
	D3D11RasterizerState** ppOut
)
{
	uint64 Hash = fnv64_c(&Desc, sizeof(RS_RASTERIZER_STATE));
	return GetOrCreateRasterizerState(Hash, Desc, ppOut);
}

BOOL D3D11RenderDevice::GetOrCreateRasterizerState(
	uint64 Hash,
	const RS_RASTERIZER_STATE& Desc,
	D3D11RasterizerState** ppOut
)
{
	D3D11RasterizerState* pRasterizerState = nullptr;
	if (0 < Hash && m_pRenderStateCache->TryGet(Hash, &pRasterizerState))
	{
		*ppOut = pRasterizerState;
		return TRUE;
	}

	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.FillMode = D3D11_IMPL_FILL_MODE[(int32)Desc.FillMode];
	RasterizerDesc.CullMode = D3D11_IMPL_CULL_MODE[(int32)Desc.CullMode];
	RasterizerDesc.FrontCounterClockwise = Desc.IsAntialiasedLineEnabled();
	RasterizerDesc.DepthBias = Desc.DepthBias;
	RasterizerDesc.DepthBiasClamp = Desc.DepthBiasClamp;
	RasterizerDesc.SlopeScaledDepthBias = Desc.SlopeScaledDepthBias;
	RasterizerDesc.DepthClipEnable = Desc.IsDepthClipEnabled();
	RasterizerDesc.MultisampleEnable = Desc.IsMultisampleEnabled();
	RasterizerDesc.AntialiasedLineEnable = Desc.IsAntialiasedLineEnabled();

	ID3D11RasterizerState* pD3D11RasterizerState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateRasterizerState(
		&RasterizerDesc,
		&pD3D11RasterizerState
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateRasterizerState: CreateRasterizerState failed");
		(*ppOut) = nullptr;
		return FALSE;
	}

	interlock_increment_l((LONG*)&m_RasterizerStateIndex, MEMORY_ORDER_RELAXED);
	if (m_RasterizerStateIndex > MAX_STATE_INDEX)
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateRasterizerState: Exceeded maximum number of rasterizer states");
		CHECK_RELEASE(pD3D11RasterizerState);
		return FALSE;
	}

	pRasterizerState = D3D11_POOL_NEW(D3D11RasterizerState)();
	pRasterizerState->pD3D11RasterizerState = pD3D11RasterizerState;
	pRasterizerState->Hash = fnv64_c(&Desc, sizeof(RS_RASTERIZER_STATE));
	pRasterizerState->StateIndex = m_RasterizerStateIndex;

	m_pRenderStateCache->Add(pRasterizerState->Hash, pRasterizerState);

	*ppOut = pRasterizerState;

	return TRUE;
}

BOOL D3D11RenderDevice::GetOrCreateDepthStencilState(
	const RS_DEPTH_STENCIL_STATE& Desc,
	D3D11DepthStencilState** ppOut
)
{
	uint64 Hash = fnv64_c(&Desc, sizeof(RS_DEPTH_STENCIL_STATE));
	return GetOrCreateDepthStencilState(Hash, Desc, ppOut);
}

BOOL D3D11RenderDevice::GetOrCreateDepthStencilState(
	uint64 Hash,
	const RS_DEPTH_STENCIL_STATE& Desc,
	D3D11DepthStencilState** ppOut
)
{
	D3D11DepthStencilState* pDepthStencilState = nullptr;
	if (0 < Hash && m_pRenderStateCache->TryGet(Hash, &pDepthStencilState))
	{
		*ppOut = pDepthStencilState;
		return TRUE;
	}

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = Desc.DepthEnable;
	DepthStencilDesc.DepthWriteMask = Desc.DepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_IMPL_COMPARISON_FUNC[(int32)Desc.DepthFunc];
	DepthStencilDesc.StencilEnable = Desc.StencilEnable;
	DepthStencilDesc.StencilReadMask = Desc.StencilReadMask;
	DepthStencilDesc.StencilWriteMask = Desc.StencilWriteMask;
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_IMPL_STENCIL_OP[(int32)Desc.FrontFace.StencilFailOp];
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_IMPL_STENCIL_OP[(int32)Desc.FrontFace.StencilDepthFailOp];
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_IMPL_STENCIL_OP[(int32)Desc.FrontFace.StencilPassOp];
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_IMPL_COMPARISON_FUNC[(int32)Desc.FrontFace.StencilFunc];
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_IMPL_STENCIL_OP[(int32)Desc.BackFace.StencilFailOp];
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_IMPL_STENCIL_OP[(int32)Desc.BackFace.StencilDepthFailOp];
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_IMPL_STENCIL_OP[(int32)Desc.BackFace.StencilPassOp];
	DepthStencilDesc.BackFace.StencilFunc = D3D11_IMPL_COMPARISON_FUNC[(int32)Desc.BackFace.StencilFunc];

	ID3D11DepthStencilState* pD3D11DepthStencilState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateDepthStencilState(
		&DepthStencilDesc,
		&pD3D11DepthStencilState
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateDepthStencilState: CreateDepthStencilState failed");
		(*ppOut) = nullptr;
		return FALSE;
	}

	interlock_increment_l((LONG*)&m_DepthStencilStateIndex, MEMORY_ORDER_RELAXED);
	if (m_DepthStencilStateIndex > MAX_STATE_INDEX)
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateDepthStencilState: Exceeded maximum number of depth stencil states");
		CHECK_RELEASE(pD3D11DepthStencilState);
		return FALSE;
	}

	pDepthStencilState = D3D11_POOL_NEW(D3D11DepthStencilState)();

	pDepthStencilState->pD3D11DepthStencilState = pD3D11DepthStencilState;
	pDepthStencilState->Hash = fnv64_c(&Desc, sizeof(RS_DEPTH_STENCIL_STATE));
	pDepthStencilState->StateIndex = m_DepthStencilStateIndex;

	m_pRenderStateCache->Add(pDepthStencilState->Hash, pDepthStencilState);

	*ppOut = pDepthStencilState;

	return TRUE;
}

BOOL D3D11RenderDevice::GetOrCreateRenderPipelineState(
	const RS_BLEND_STATE& BlendDesc,
	const RS_RASTERIZER_STATE& RasterizerDesc,
	const RS_DEPTH_STENCIL_STATE& DepthStencilDesc,
	D3D11RenderPipelineState** ppOut
)
{
	uint64 BlendHash = fnv64_c(&BlendDesc, sizeof(RS_DEPTH_STENCIL_STATE));
	uint64 RasterizerHash = fnv64_c(&RasterizerDesc, sizeof(RS_RASTERIZER_STATE));
	uint64 DepthStencilHash = fnv64_c(&DepthStencilDesc, sizeof(RS_DEPTH_STENCIL_STATE));

	return GetOrCreateRenderPipelineState(
		BlendHash,
		RasterizerHash,
		DepthStencilHash,
		BlendDesc,
		RasterizerDesc,
		DepthStencilDesc,
		ppOut
	);
}


BOOL D3D11RenderDevice::GetOrCreateRenderPipelineState(
	uint64 BlendHash,
	uint64 RasterizerHash,
	uint64 DepthStencilHash,
	const RS_BLEND_STATE& BlendDesc,
	const RS_RASTERIZER_STATE& RasterizerDesc,
	const RS_DEPTH_STENCIL_STATE& DepthStencilDesc,
	D3D11RenderPipelineState** ppOut
)
{
	D3D11BlendState* pBlendState = nullptr;
	if (!GetOrCreateBlendState(BlendHash, BlendDesc, &pBlendState))
		return FALSE;

	D3D11RasterizerState* pRasterizerState = nullptr;
	if (!GetOrCreateRasterizerState(RasterizerHash, RasterizerDesc, &pRasterizerState))
	{
		pBlendState->Release();
		return FALSE;
	}

	D3D11DepthStencilState* pDepthStencilState = nullptr;
	if (!GetOrCreateDepthStencilState(DepthStencilHash, DepthStencilDesc, &pDepthStencilState))
	{
		pBlendState->Release();
		pRasterizerState->Release();
		return FALSE;
	}

	RENDER_PIPELINE_STATE_KEY PiplineStateKey = {};
	PiplineStateKey.BlendStateIndex = pBlendState->StateIndex;
	PiplineStateKey.RasterizerStateIndex = pRasterizerState->StateIndex;
	PiplineStateKey.DepthStencilStateIndex = pDepthStencilState->StateIndex;

	if (m_pRenderStateCache->TryGet(PiplineStateKey.RenderStateID, ppOut))
		return TRUE;

	D3D11RenderPipelineState* pPipelineState = D3D11_POOL_NEW(D3D11RenderPipelineState)();
	pPipelineState->pBlendState = pBlendState;
	pPipelineState->pRasterizerState = pRasterizerState;
	pPipelineState->pDepthStencilState = pDepthStencilState;
	pPipelineState->Hash = PiplineStateKey.RenderStateID;

	interlock_increment_l((LONG*)&m_RenderPipelineStateIndex, MEMORY_ORDER_RELAXED);
	if (m_RenderPipelineStateIndex > MAX_STATE_INDEX)
	{
		SYS_LOG_E("D3D11RenderDevice::GetOrCreateRenderPipelineState: Exceeded maximum number of depth stencil states");
		pPipelineState->Release();
		(*ppOut) = nullptr;

		return FALSE;
	}

	pPipelineState->StateIndex = m_RenderPipelineStateIndex;

	m_pRenderStateCache->Add(pPipelineState->Hash, pPipelineState);
	*ppOut = pPipelineState;

	return TRUE;
}

/*
BOOL D3D11RenderDevice::CreateVertexShader(const D3D11_SHADER_COMPILE_DESC& Desc, D3D11VertexShader** ppOut)
{
	D3D11_SHADER_COMPILE_RESULT compileResult = {};

	if (!D3D11CompileShader(Desc, compileResult))
	{
		(*ppOut) = nullptr;
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
		(*ppOut) = nullptr;

		return FALSE;
	}

	D3D11ShaderParams* pShaderProp = MARK_POOL_NEW(D3D11ShaderParams)(
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

	uint32 id = idgen_getid(GLOBAL_VARS::ID_GEN_HANDLE);

	D3D11VertexShader* pVS = MARK_POOL_NEW(D3D11VertexShader)();
	pVS->pVS = pVertexShader;
	pVS->pShaderParams = pShaderProp;
	pVS->pInputLayout = pInputLayout;

	*ppOut = pVS;

	compileResult.pShaderBlob->Release();

	return TRUE;
}

BOOL D3D11RenderDevice::CreatePixelShader(const D3D11_SHADER_COMPILE_DESC& Desc, D3D11PixelShader** ppOut)
{
	D3D11_SHADER_COMPILE_RESULT compileResult = {};

	if (!D3D11CompileShader(Desc, compileResult))
	{
		(*ppOut) = nullptr;
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
		(*ppOut) = nullptr;

		return FALSE;
	}

	D3D11ShaderParams* pShaderProp = MARK_POOL_NEW(D3D11ShaderParams)(
		compileResult.pShaderParams,
		compileResult.NumShaderParams
	);

	D3D11PixelShader* pPS = MARK_POOL_NEW(D3D11PixelShader)();
	pPS->pPS = pPixelShader;
	pPS->pShaderParams = pShaderProp;

	*ppOut = pPS;

	compileResult.pShaderBlob->Release();

	return TRUE;
}

BOOL D3D11RenderDevice::CreateInputLayout(const D3D11_INPUTLAYOUT_DESC& Desc, D3D11InputLayout** ppOut)
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

	D3D11InputLayout* pInputLayout = MARK_POOL_NEW(D3D11InputLayout)();
	pInputLayout->pIL = pD3D11InputLayout;
	pInputLayout->NumVertexFormat = Desc.NumVertexFormat;
	memcpy(pInputLayout->VertexFormats, VertexFormats, sizeof(VERTEX_FORMAT)* MAX_VERTEX_FORMAT);
	memcpy(pInputLayout->VertexFormatIndices, VertexFormatIndices, sizeof(VERTEX_FORMAT_INDEX)* MAX_VERTEX_FORMAT);

	(*ppOut) = pInputLayout;

	return TRUE;
}
*/
/*
BOOL D3D11RenderDevice::CreateSamplerState(const RS_SAMPLER_STATE& Desc, D3D11SamplerState** ppOut)
{
	uint32 Hash = fnv1_c(&Desc, sizeof(RS_SAMPLER_STATE));

	// 추후 이곳에 캐싱 로직 추가 가능

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_IMPL_FILTER[(int)Desc.Filter]; 
	samplerDesc.AddressU = D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int)Desc.AddressU];
	samplerDesc.AddressV = D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int)Desc.AddressV];
	samplerDesc.AddressW = D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int)Desc.AddressW];

	samplerDesc.MipLODBias = (FLOAT)Desc.MipLODBias;
	samplerDesc.MaxAnisotropy = Desc.MaxAnisotropy;
	samplerDesc.ComparisonFunc = D3D11_IMPL_COMPARISON_FUNC[(int)Desc.ComparisonFunc];
	samplerDesc.BorderColor[0] = Desc.BorderColor[0];
	samplerDesc.BorderColor[1] = Desc.BorderColor[1];
	samplerDesc.BorderColor[2] = Desc.BorderColor[2];
	samplerDesc.BorderColor[3] = Desc.BorderColor[3];
	samplerDesc.MinLOD = Desc.MinLOD;
	samplerDesc.MaxLOD = Desc.MaxLOD;

	ID3D11SamplerState* pD3D11SamplerState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateSamplerState(
		&samplerDesc,
		&pD3D11SamplerState
	);

	if (FAILED(hr))
		return FALSE;

	D3D11SamplerState* pSamplerState = MARK_POOL_NEW(D3D11SamplerState)();
	pSamplerState->pSamplerState = pD3D11SamplerState;
	pSamplerState->State = Desc;
	pSamplerState->Hash = Hash;

	(*ppOut) = pSamplerState;

	// 이곳에 캐싱 로직 추가 가능

	return TRUE;
}

BOOL D3D11RenderDevice::CreateBlendState(const RS_BLEND_STATE& Desc, D3D11BlendState** ppOut)
{
	uint32 Hash = fnv1_c(&Desc, sizeof(RS_BLEND_STATE));

	// 추후 이곳에 캐싱 로직 추가 가능
	D3D11_BLEND_DESC blendDesc = {};
	for (int i = 0; i < Desc.NumBlendTargets; ++i)
	{
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;

		D3D11_RENDER_TARGET_BLEND_DESC& rtbd = blendDesc.RenderTarget[i];
		rtbd.BlendEnable = Desc.BlendTarget[i].BlendEnable;
		rtbd.SrcBlend = D3D11_IMPL_BLEND_FACTOR[(int)Desc.BlendTarget[i].SrcBlend];
		rtbd.DestBlend = D3D11_IMPL_BLEND_FACTOR[(int)Desc.BlendTarget[i].DestBlend];
		rtbd.BlendOp = D3D11_IMPL_BLEND_OP[(int)Desc.BlendTarget[i].BlendOp];
		rtbd.SrcBlendAlpha = D3D11_IMPL_BLEND_FACTOR[(int)Desc.BlendTarget[i].SrcBlendAlpha];
		rtbd.DestBlendAlpha = D3D11_IMPL_BLEND_FACTOR[(int)Desc.BlendTarget[i].DestBlendAlpha];
		rtbd.BlendOpAlpha = D3D11_IMPL_BLEND_OP[(int)Desc.BlendTarget[i].BlendOpAlpha];
		rtbd.RenderTargetWriteMask = Desc.BlendTarget[i].RenderTargetWriteMask;
	}

	ID3D11BlendState* pD3D11BlendState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateBlendState(
		&blendDesc,
		&pD3D11BlendState
	);

	if (FAILED(hr))
		return FALSE;

	D3D11BlendState* pBlendState = MARK_POOL_NEW(D3D11BlendState)();
	pBlendState->pBlendState = pD3D11BlendState;
	pBlendState->State = Desc;
	pBlendState->Hash = Hash;

	(*ppOut) = pBlendState;

	// 이곳에 캐싱 로직 추가 가능

	return TRUE;
}

BOOL D3D11RenderDevice::CreateRasterizerState(const RS_RASTERIZER_STATE& Desc, D3D11RasterizerState** ppOut)
{
	uint32 Hash = fnv1_c(&Desc, sizeof(RS_RASTERIZER_STATE));

	// 추후 이곳에 캐싱 로직 추가 가능
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_IMPL_FILL_MODE[(int)Desc.FillMode];
	rasterizerDesc.CullMode = D3D11_IMPL_CULL_MODE[(int)Desc.CullMode];
	rasterizerDesc.FrontCounterClockwise = Desc.FrontCounterClockwise;
	rasterizerDesc.DepthBias = Desc.DepthBias;
	rasterizerDesc.DepthBiasClamp = Desc.DepthBiasClamp;
	rasterizerDesc.SlopeScaledDepthBias = Desc.SlopeScaledDepthBias;
	rasterizerDesc.DepthClipEnable = Desc.DepthClipEnable;
	rasterizerDesc.ScissorEnable = Desc.ScissorEnable;
	rasterizerDesc.MultisampleEnable = Desc.MultisampleEnable;
	rasterizerDesc.AntialiasedLineEnable = Desc.AntialiasedLineEnable;
	//rasterizerDesc.ForcedSampleCount = Desc.ForcedSampleCount;
	//rasterizerDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	ID3D11RasterizerState* pD3D11RasterizerState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateRasterizerState(
		&rasterizerDesc,
		&pD3D11RasterizerState
	);

	if (FAILED(hr))
		return FALSE;

	D3D11RasterizerState* pRasterizerState = MARK_POOL_NEW(D3D11RasterizerState)();
	pRasterizerState->pRasterizerState = pD3D11RasterizerState;
	pRasterizerState->State = Desc;
	pRasterizerState->Hash = Hash;

	(*ppOut) = pRasterizerState;

	// 이곳에 캐싱 로직 추가 가능

	return TRUE;
}

BOOL D3D11RenderDevice::CreateDepthStencilState(const RS_DEPTH_STENCIL_STATE& Desc, D3D11DepthStencilState** ppOut)
{
	uint32 Hash = fnv1_c(&Desc, sizeof(RS_DEPTH_STENCIL_STATE));

	// 추후 이곳에 캐싱 로직 추가 가능

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = Desc.DepthEnable;
	depthStencilDesc.DepthWriteMask = D3D11_IMPL_DEPTH_WRITE_MASK[(int)Desc.DepthWriteMask];
	depthStencilDesc.DepthFunc = D3D11_IMPL_COMPARISON_FUNC[(int)Desc.DepthFunc];
	depthStencilDesc.StencilEnable = Desc.StencilEnable;
	depthStencilDesc.StencilReadMask = Desc.StencilReadMask;
	depthStencilDesc.StencilWriteMask = Desc.StencilWriteMask;

	// 앞면 스텐실 연산
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_IMPL_STENCIL_OP[(int)Desc.FrontFaceStencilFailOp];
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_IMPL_STENCIL_OP[(int)Desc.FrontFaceStencilDepthFailOp];
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_IMPL_STENCIL_OP[(int)Desc.FrontFaceStencilPassOp];
	depthStencilDesc.FrontFace.StencilFunc = D3D11_IMPL_COMPARISON_FUNC[(int)Desc.FrontFaceStencilFunc];

	// 뒷면 스텐실 연산
	depthStencilDesc.BackFace.StencilFailOp = D3D11_IMPL_STENCIL_OP[(int)Desc.BackFaceStencilFailOp];
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_IMPL_STENCIL_OP[(int)Desc.BackFaceStencilDepthFailOp];
	depthStencilDesc.BackFace.StencilPassOp = D3D11_IMPL_STENCIL_OP[(int)Desc.BackFaceStencilPassOp];
	depthStencilDesc.BackFace.StencilFunc = D3D11_IMPL_COMPARISON_FUNC[(int)Desc.BackFaceStencilFunc];

	ID3D11DepthStencilState* pD3D11DepthStencilState = nullptr;
	HRESULT hr = m_pD3D11Device->CreateDepthStencilState(
		&depthStencilDesc,
		&pD3D11DepthStencilState
	);

	if (FAILED(hr))
		return FALSE;

	D3D11DepthStencilState* pDepthStencilState = MARK_POOL_NEW(D3D11DepthStencilState)();
	pDepthStencilState->pDepthStencilState = pD3D11DepthStencilState;
	pDepthStencilState->State = Desc;
	pDepthStencilState->Hash = Hash;

	(*ppOut) = pDepthStencilState;

	// 이곳에 캐싱 로직 추가 가능

	return TRUE;
}
*/
