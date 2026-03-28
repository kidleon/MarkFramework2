#include "pch.h"
#include "D3D11RenderSystem.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderTarget.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11ConstantBufferAllocator.h"
#include "D3D11BlobAllocator.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgramCache.h"
#include "D3D11ShaderProgramCompile.h"
#include "D3D11InputLayoutCache.h"
#include "D3D11SurfaceMaterialBlockPool.h"
#include "D3D11SurfaceMaterial.h"
#include "D3D11SurfaceMaterialBlock.h"
#include "D3D11RenderStateCache.h"
#include "D3D11RenderCommand.h"
#include "D3D11BufferPool.h"
#include "D3D11Texture1D.h"
#include "D3D11Texture2D.h"
#include "D3D11TextureUtils.h"
#include "D3D11TextureLoader_DDS.h"
#include "D3D11Model.h"

#include "temp_pool.h"
//#include "D3D11DDSTextureFactory.h"



void MemoryReporter(
	const char* type,
	const char* file,
	int line,
	const char* func,
	size_t size
)
{
	char szTemp[512] = { '\0' };
	sprintf(szTemp, "Memory Report: Type = %s, Size = %zu bytes, Location = %s(%d) %s\n",
		type,
		size,
		file,
		line,
		func
	);
	OutputDebugStringA(szTemp);
}


D3D11RenderSystem::~D3D11RenderSystem() noexcept
{
	Shutdown();
}

long D3D11RenderSystem::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11RenderSystem::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		delete this;
	}
	return NewRefCnt;
}

long D3D11RenderSystem::RefCnt()
{
	return m_RefCnt;
}

BOOL D3D11RenderSystem::Init(
	HWND hWnd,
	uint32 ScreenWidth,
	uint32 ScreenHeight,
	BOOL Fullscreen
)
{
	D3D11Heap_Init(1024 * 1024 * 10, MemoryReporter);
	D3D11_COMMON::Init();

	BOOL DebugDevice = FALSE;

#if defined(_DEBUG) || defined(DEBUG)
	DebugDevice = TRUE;
#endif // defined(_DEBUG)

	m_pRenderDevice = D3D11_NEW(D3D11RenderDevice)();

	if (!m_pRenderDevice->CreateDevice(hWnd, ScreenWidth, ScreenHeight, DebugDevice))
	{
		SYS_LOG_E("D3D11RenderSystem::Initialize: Failed to create D3D11RenderDevice");
		D3D11_DELETE(m_pRenderDevice, D3D11RenderDevice);

		return FALSE;
	}

	m_pBlobAllocator = D3D11_NEW(D3D11BlobAllocator)();
	m_pBlobAllocator->Init();

	m_pCBAllocator = D3D11_NEW(D3D11ConstantBufferAllocator)();
	m_pCBAllocator->Init(m_pRenderDevice);

	m_pInputLayoutCache = D3D11_NEW(D3D11InputLayoutCache)();
	m_pInputLayoutCache->Init();

	m_pShaderProgramCache = D3D11_NEW(D3D11ShaderProgramCache)();
	m_pShaderProgramCache->Init();

	m_pRenderStateCache = D3D11_NEW(D3D11RenderStateCache)();
	m_pRenderStateCache->Init();

	m_pSurfaceMaterialBlockPool = D3D11_NEW(D3D11SurfaceMaterialBlockPool)();
	m_pSurfaceMaterialBlockPool->Init(512);

	m_pRenderCommandExecutor = D3D11_NEW(D3D11RenderCommandExecutor)(m_pRenderDevice);
	
	m_pBufferPool = D3D11_NEW(D3D11BufferPool)(m_pRenderDevice->INL_GetD3D11Device());

	return TRUE;
}

void D3D11RenderSystem::Shutdown()
{
	if(m_pBufferPool)
	{
		D3D11_DELETE(m_pBufferPool, D3D11BufferPool);
		m_pBufferPool = nullptr;
	}

	if (m_pRenderCommandExecutor)
	{
		D3D11_DELETE(m_pRenderCommandExecutor, D3D11RenderCommandExecutor);
		m_pRenderCommandExecutor = nullptr;
	}

	if (m_pSurfaceMaterialBlockPool)
	{
		D3D11_DELETE(m_pSurfaceMaterialBlockPool, D3D11SurfaceMaterialBlockPool);
		m_pSurfaceMaterialBlockPool = nullptr;
	}

	if (m_pRenderStateCache)
	{
		D3D11_DELETE(m_pRenderStateCache, D3D11RenderStateCache);
		m_pRenderStateCache = nullptr;
	}

	if (m_pShaderProgramCache)
	{
		D3D11_DELETE(m_pShaderProgramCache, D3D11ShaderProgramCache);
		m_pShaderProgramCache = nullptr;
	}

	if (m_pInputLayoutCache)
	{
		D3D11_DELETE(m_pInputLayoutCache, D3D11InputLayoutCache);
		m_pInputLayoutCache = nullptr;
	}

	if (m_pRenderContext)
	{
		m_pRenderContext->Release();
		m_pRenderContext = nullptr;
	}

	if (m_pBlobAllocator)
	{
		D3D11_DELETE(m_pBlobAllocator, D3D11BlobAllocator);
		m_pBlobAllocator = nullptr;
	}

	if (m_pCBAllocator)
	{
		D3D11_DELETE(m_pCBAllocator, D3D11ConstantBufferAllocator);
		m_pCBAllocator = nullptr;
	}

	if (m_pRenderDevice)
	{
		D3D11_DELETE(m_pRenderDevice, D3D11RenderDevice);
		m_pRenderDevice = nullptr;
	}
	
	D3D11_COMMON::Shutdown();

	D3D11Heap_ReportLeaks();
	D3D11Heap_Shutdown();
}

const RENDER_SETTINGS& D3D11RenderSystem::GetRenderSettings() const noexcept
{
	return D3D11_COMMON::GetRenderSettings();
}

void D3D11RenderSystem::SetRenderSettings(const RENDER_SETTINGS& Settings) noexcept
{
	D3D11_COMMON::SetRenderSettings(Settings);
}

BOOL D3D11RenderSystem::CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& Desc, IPrimitiveBuffer** ppOut)
{
	ID3D11Device* pDevice = m_pRenderDevice->INL_GetD3D11Device();

	D3D11PrimitiveBuffer* pPrimitiveBuffer = D3D11_POOL_NEW(D3D11PrimitiveBuffer);
	pPrimitiveBuffer->Create(
		Desc.VertexFormat,
		Desc.MaxVertexCount,
		Desc.MaxIndexCount,
		Desc.UsageVB,
		Desc.UsageIB
	);

	*ppOut = static_cast<IPrimitiveBuffer*>(pPrimitiveBuffer);

	return TRUE;
}

BOOL D3D11RenderSystem::CreateRenderCamera(
	const RENDERCAMERA_CREATE_DESC& Desc,
	IRenderCamera** ppOut
)
{
	D3D11RenderTarget* pRenderTarget = m_pRenderDevice->INL_GetBackBuffer_RenderTarget();
	pRenderTarget->AddRef();

	D3D11RenderCamera* pRenderCamera = D3D11_POOL_NEW(D3D11RenderCamera)(
		Desc.CameraMode,
		pRenderTarget
	);

	pRenderCamera->SetCameraMode(Desc.CameraMode);
	if (Desc.CameraMode == CAMERA_MODE::PERSPECTIVE)
	{
		pRenderCamera->SetPerspective(
			Desc.FOVY,
			Desc.AspectRatio,
			Desc.NearZ,
			Desc.FarZ
		);
	}
	else if (Desc.CameraMode == CAMERA_MODE::ORTHO)
	{
		pRenderCamera->SetOrtho(
			Desc.OrthoWidth,
			Desc.OrthoHeight,
			Desc.NearZ,
			Desc.FarZ
		);
	}

	pRenderCamera->SetView(
		FLOAT3{ 0.0f, 0.0f, 0.0f },
		FLOAT3{ 0.0f, 0.0f, -1.0f },
		FLOAT3{ 0.0f, 1.0f, 0.0f }
	);

	pRenderCamera->SetClearTarget(
		Desc.ClearFlags,
		Desc.ClearColor,
		Desc.Depth,
		Desc.Stencil
	);

	pRenderCamera->SetCameraOrder(Desc.CameraOrder);

	*ppOut = pRenderCamera;

	return TRUE;
}

BOOL D3D11RenderSystem::CreateModel(const MODEL_CREATE_DESC& Desc, IModel** ppOut)
{
	PRIMITIVEBUFFER_CREATE_DESC PrimitiveBufferDesc = {};
	PrimitiveBufferDesc.VertexFormat = Desc.VertexFormat;
	PrimitiveBufferDesc.MaxVertexCount = Desc.MaxVertexCount;
	PrimitiveBufferDesc.MaxIndexCount = Desc.MaxIndexCount;
	PrimitiveBufferDesc.UsageVB = BUFFER_USAGE::DYNAMIC;
	PrimitiveBufferDesc.UsageIB = BUFFER_USAGE::DYNAMIC;

	IPrimitiveBuffer* pPrimitiveBuffer = nullptr;

	if (!CreatePrimitiveBuffer(PrimitiveBufferDesc, &pPrimitiveBuffer))
	{
		SYS_LOG_E("D3D11RenderSystem::CreateModel: Failed to create primitive buffer for model");
		*ppOut = nullptr;
		return FALSE;
	}

	D3D11Model* pModel = D3D11_POOL_NEW(D3D11Model)(0, Desc.VertexFormat, pPrimitiveBuffer);
	*ppOut = static_cast<IModel*>(pModel);

	return TRUE;
}

BOOL D3D11RenderSystem::CreateSurfaceMaterial(ISurfaceMaterial** ppOut)
{
	D3D11_SURFACE_MATERIAL_BLOCK* pBlock = m_pSurfaceMaterialBlockPool->Acquire();
	if (!pBlock)
	{
		SYS_LOG_E("D3D11RenderSystem::CreateSurfaceMaterial: Failed to acquire surface material block");
		*ppOut = nullptr;
		return FALSE;
	}

	pBlock->Reset();

	D3D11SurfaceMaterial* pSurfaceMaterial = D3D11_POOL_NEW(D3D11SurfaceMaterial)(pBlock);

	*ppOut = pSurfaceMaterial;

	return TRUE;
}

BOOL D3D11RenderSystem::CreateTexture1D(ITexture1D** ppOut)
{
	D3D11Texture1D* pTexture = D3D11_POOL_NEW(D3D11Texture1D);
	*ppOut = static_cast<ITexture1D*>(pTexture);
	return TRUE;
}

BOOL D3D11RenderSystem::CreateTexture2D(ITexture2D** ppOut)
{
	D3D11Texture2D* pTexture = D3D11_POOL_NEW(D3D11Texture2D);
	*ppOut = static_cast<ITexture2D*>(pTexture);
	return TRUE;
}

BOOL D3D11RenderSystem::CreateTexture1D(const TEXTURE1D_CREATE_DESC& Desc, ITexture1D* pTexture)
{
	switch (Desc.FileFormat)
	{
		case TEXTURE_FILE_FORMAT::DDS:
		{
			ID3D11Texture1D* pTex1D = nullptr;
			ID3D11ShaderResourceView* pSRV = nullptr;

			HRESULT hr = CreateDDSTexture1DFromMemory(
				m_pRenderDevice->INL_GetD3D11Device(),
				Desc.pData,
				Desc.DataSize,
				&pTex1D,
				&pSRV,
				Desc.sRGB
			);

			if (FAILED(hr))
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture1D: Failed to create DDS texture from memory, HRESULT = 0x%X", hr);
				return FALSE;
			}

			D3D11Texture1D* pTextureImpl = static_cast<D3D11Texture1D*>(pTexture);
			pTextureImpl->INL_SetColorFormat(Desc.Format);
			pTextureImpl->INL_SetWidth(Desc.Width);
			pTextureImpl->INL_SetMipLevels(Desc.MipLevels);
			pTextureImpl->INL_SetD3D11Texture1D(pTex1D);
			pTextureImpl->INL_SetSRV(pSRV);
			pTextureImpl->INL_SetLoadStat(LOAD_STAT::LOADED);
		}
		break;

		case TEXTURE_FILE_FORMAT::PNG:
		case TEXTURE_FILE_FORMAT::JPEG:
		case TEXTURE_FILE_FORMAT::BMP:
		case TEXTURE_FILE_FORMAT::TGA:
		{
			if (!Desc.hTempHeap)
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture1D: Temporary heap handle is null for non-DDS texture");
				return FALSE;
			}

			ID3D11Texture1D* pTex1D = nullptr;
			ID3D11ShaderResourceView* pSRV = nullptr;

			if (!CreateTexture1DFromSTBI(
				Desc.hTempHeap,
				m_pRenderDevice->INL_GetD3D11Device(),
				Desc,
				&pTex1D, // ppTex1D (1D 텍스처는 지원하지 않음)
				&pSRV  // ppSRV (1D 텍스처는 지원하지 않음)
			))
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture1D: Failed to create texture from STBI for non-DDS texture");
				return FALSE;
			}

			D3D11Texture1D* pTextureImpl = static_cast<D3D11Texture1D*>(pTexture);
			pTextureImpl->INL_SetColorFormat(Desc.Format);
			pTextureImpl->INL_SetWidth(Desc.Width);
			pTextureImpl->INL_SetMipLevels(Desc.MipLevels);
			pTextureImpl->INL_SetD3D11Texture1D(pTex1D);
			pTextureImpl->INL_SetSRV(pSRV);
			pTextureImpl->INL_SetLoadStat(LOAD_STAT::LOADED);
		}
		break;

		case TEXTURE_FILE_FORMAT::UNKNOWN:
		{
			if (Desc.Width <= 1 || Desc.Format == COLOR_FORMAT::UNKNOWN)
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture1D: Invalid parameters for unknown texture format");
				return FALSE;
			}

			// 파일 포맷이 알려지지 않은 경우, 임시로 빈 텍스처를 생성하여 반환하며 Mip데이터는 생성하지 않는다.
			const DXGI_FORMAT dxgiFormat = D3D11_IMPL_COLOR_FORMAT[(uint32)Desc.Format];
			const size_t pixelSize = D3D11_IMPL_COLOR_FORMAT_SIZE[(uint32)Desc.Format];

			D3D11_SUBRESOURCE_DATA SubResourceData = {};

			// Mip0: stb_image 원본 그대로 (width * 4 bytes, height = 1 고정)
			uint8_t* pTempBuffer = (uint8_t*)temppool_alloc(Desc.hTempHeap, static_cast<size_t>(Desc.Width * pixelSize));
			memset(pTempBuffer, 0, Desc.Width * pixelSize);

			SubResourceData.pSysMem = pTempBuffer;
			SubResourceData.SysMemPitch = static_cast<UINT>(Desc.Width * pixelSize); // 1D는 사실상 의미 없지만 명시
			SubResourceData.SysMemSlicePitch = 0;

			D3D11_TEXTURE1D_DESC TexDesc = {};
			TexDesc.Width = Desc.Width;
			TexDesc.MipLevels = 1;
			TexDesc.ArraySize = 1;
			TexDesc.Format = dxgiFormat;
			TexDesc.Usage = D3D11_USAGE_IMMUTABLE;
			TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			TexDesc.CPUAccessFlags = 0;
			TexDesc.MiscFlags = 0;

			ID3D11Texture1D* pTex1D = nullptr;
			HRESULT hr = m_pRenderDevice->INL_GetD3D11Device()->CreateTexture1D(
				&TexDesc,
				&SubResourceData,
				&pTex1D
			);

			if (FAILED(hr))
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture1D: Failed to create texture from memory for unknown format, HRESULT = 0x%X", hr);
				return FALSE;
			}

			ID3D11ShaderResourceView* pSRV = nullptr;

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = dxgiFormat;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			SRVDesc.Texture1D.MostDetailedMip = 0;
			SRVDesc.Texture1D.MipLevels = 1;

			hr = m_pRenderDevice->INL_GetD3D11Device()->CreateShaderResourceView(pTex1D, &SRVDesc, &pSRV);
			if (FAILED(hr))
			{
				SYS_LOG_E("CreateTexture1DFromSTBI: CreateShaderResourceView failed, HRESULT = 0x%X", hr);
				CHECK_RELEASE(pTex1D);
				return FALSE;
			}

			D3D11Texture1D* pTextureImpl = static_cast<D3D11Texture1D*>(pTexture);
			pTextureImpl->INL_SetColorFormat(Desc.Format);
			pTextureImpl->INL_SetWidth(Desc.Width);
			pTextureImpl->INL_SetMipLevels(1);
			pTextureImpl->INL_SetD3D11Texture1D(pTex1D);
			pTextureImpl->INL_SetSRV(pSRV);
			pTextureImpl->INL_SetLoadStat(LOAD_STAT::LOADED);
		}
		break;
	}
	
	return TRUE;
}

BOOL D3D11RenderSystem::CreateTexture2D(const TEXTURE2D_CREATE_DESC& Desc, ITexture2D* pTexture)
{
	switch (Desc.FileFormat)
	{
		case TEXTURE_FILE_FORMAT::DDS:
		{
			ID3D11Texture2D* pTex2D = nullptr;
			ID3D11ShaderResourceView* pSRV = nullptr;

			HRESULT hr = CreateDDSTexture2DFromMemory(
				m_pRenderDevice->INL_GetD3D11Device(),
				Desc.pData,
				Desc.DataSize,
				&pTex2D,
				&pSRV,
				false
			);

			if (FAILED(hr))
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture1D: Failed to create DDS texture from memory, HRESULT = 0x%X", hr);
				return FALSE;
			}

			D3D11Texture2D* pTextureImpl = static_cast<D3D11Texture2D*>(pTexture);
			pTextureImpl->INL_SetColorFormat(Desc.Format);
			pTextureImpl->INL_SetWidth(Desc.Width);
			pTextureImpl->INL_SetHeight(Desc.Height);
			pTextureImpl->INL_SetMipLevels(Desc.MipLevels);
			pTextureImpl->INL_SetD3D11Texture2D(pTex2D);
			pTextureImpl->INL_SetSRV(pSRV);
			pTextureImpl->INL_SetLoadStat(LOAD_STAT::LOADED);
		}
		break;

		case TEXTURE_FILE_FORMAT::PNG:
		case TEXTURE_FILE_FORMAT::JPEG:
		case TEXTURE_FILE_FORMAT::BMP:
		case TEXTURE_FILE_FORMAT::TGA:
		{
			if (!Desc.hTempHeap)
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture2D: Temporary heap handle is null for non-DDS texture");
				return FALSE;
			}

			ID3D11Texture2D* pTex2D = nullptr;
			ID3D11ShaderResourceView* pSRV = nullptr;

			if (!CreateTexture2DFromSTBI(
				Desc.hTempHeap,
				m_pRenderDevice->INL_GetD3D11Device(),
				Desc,
				&pTex2D,
				&pSRV
			))
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture2D: Failed to create texture from STBI for non-DDS texture");
				return FALSE;
			}

			D3D11Texture2D* pTextureImpl = static_cast<D3D11Texture2D*>(pTexture);
			pTextureImpl->INL_SetColorFormat(Desc.Format);
			pTextureImpl->INL_SetWidth(Desc.Width);
			pTextureImpl->INL_SetHeight(Desc.Height);
			pTextureImpl->INL_SetMipLevels(Desc.MipLevels);
			pTextureImpl->INL_SetD3D11Texture2D(pTex2D);
			pTextureImpl->INL_SetSRV(pSRV);
			pTextureImpl->INL_SetLoadStat(LOAD_STAT::LOADED);
		}
		break; 

		case TEXTURE_FILE_FORMAT::UNKNOWN:
		{
			// 파일 포맷이 알려지지 않은 경우, 임시로 빈 텍스처를 생성하여 반환하며 Mip데이터는 생성하지 않는다.
			const DXGI_FORMAT dxgiFormat = D3D11_IMPL_COLOR_FORMAT[(uint32)Desc.Format];
			const size_t pixelSize = D3D11_IMPL_COLOR_FORMAT_SIZE[(uint32)Desc.Format];

			D3D11_SUBRESOURCE_DATA SubResourceData = {};

			uint8_t* pTempBuffer = (uint8_t*)temppool_alloc(Desc.hTempHeap, static_cast<size_t>(Desc.Width * Desc.Height * pixelSize));
			memset(pTempBuffer, 0, Desc.Width * Desc.Height * pixelSize);

			SubResourceData.pSysMem = pTempBuffer;
			SubResourceData.SysMemPitch = static_cast<UINT>(Desc.Width * pixelSize);
			SubResourceData.SysMemSlicePitch = 0;

			D3D11_TEXTURE2D_DESC TexDesc = {};
			TexDesc.Width = Desc.Width;
			TexDesc.Height = Desc.Height;
			TexDesc.MipLevels = 1;
			TexDesc.ArraySize = 1;
			TexDesc.Format = dxgiFormat;
			TexDesc.Usage = D3D11_USAGE_IMMUTABLE;
			TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			TexDesc.CPUAccessFlags = 0;
			TexDesc.MiscFlags = 0;

			ID3D11Texture2D* pTex2D = nullptr;

			HRESULT hr = m_pRenderDevice->INL_GetD3D11Device()->CreateTexture2D(
				&TexDesc,
				&SubResourceData,
				&pTex2D
			);

			if (FAILED(hr))
			{
				SYS_LOG_E("D3D11RenderSystem::CreateTexture2D: Failed to create texture from memory for unknown format, HRESULT = 0x%X", hr);
				return FALSE;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = dxgiFormat;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture1D.MostDetailedMip = 0;
			SRVDesc.Texture1D.MipLevels = 1;

			ID3D11ShaderResourceView* pSRV = nullptr;

			hr = m_pRenderDevice->INL_GetD3D11Device()->CreateShaderResourceView(pTex2D, &SRVDesc, &pSRV);
			if (FAILED(hr))
			{
				SYS_LOG_E("CreateTexture2DFromSTBI: CreateShaderResourceView failed, HRESULT = 0x%X", hr);
				CHECK_RELEASE(pTex2D);
				return FALSE;
			}

			D3D11Texture2D* pTextureImpl = static_cast<D3D11Texture2D*>(pTexture);
			pTextureImpl->INL_SetColorFormat(Desc.Format);
			pTextureImpl->INL_SetWidth(Desc.Width);
			pTextureImpl->INL_SetHeight(Desc.Height);
			pTextureImpl->INL_SetMipLevels(1);
			pTextureImpl->INL_SetD3D11Texture2D(pTex2D);
			pTextureImpl->INL_SetSRV(pSRV);
			pTextureImpl->INL_SetLoadStat(LOAD_STAT::LOADED);
		}
		break;

	}

	return TRUE;
}

__FORCEINLINE int compare_strings(const void* a, const void* b) 
{
	return strcmp((const char*)a, (const char*)b);
}

BOOL D3D11RenderSystem::GetOrCreateShaderProgram(const SHADER_PROGRAM_CREATE_DESC& Desc, IShaderProgram** ppOut)
{
	static volatile unsigned long s_ShaderProgramIndex_VS = 0;
	static volatile unsigned long s_ShaderProgramIndex_PS = 0;

	if (Desc.ShaderName.empty())
	{
		SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: ShaderName is empty");
		(*ppOut) = nullptr;
		return FALSE;
	}

	uint32 ShaderDefinesHash = fnv1(Desc.szShaderDefines, MAX_SHADER_DEFINE * MAX_SHADER_DEFINE_LENGTH, 0);

	D3D11ShaderProgram* pSP = m_pShaderProgramCache->Find_RS(Desc.ShaderType, Desc.ShaderName, ShaderDefinesHash);
	if (pSP)
	{
		(*ppOut) = pSP;
		return TRUE;
	}

	if (!Desc.pShaderSource)
	{
		SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: Shader source is null for shader '%u'", (uint32)Desc.ShaderName);
		(*ppOut) = nullptr;
		return FALSE;
	}

	// COMPILE SHADER
	D3D11_SHADER_COMPILE_DESC CompileDesc = {};
	D3D11_SHADER_COMPILE_RESULT CompileResult = {};

	CompileDesc.ShaderName = Desc.ShaderName;
	CompileDesc.ShaderType = Desc.ShaderType;
	fstrlcpy(CompileDesc.szEntryPoint, Desc.pEntryPoint, 32);
	fstrlcpy(CompileDesc.szShaderModel, Desc.pTargetProfile, 32);
	
	UINT32 NumDefines = 0;
	for (size_t i = 0; i < MAX_SHADER_DEFINE; i++)
	{
		if (Desc.szShaderDefines[i][0] == '\0')
			break;

		fstrlcpy(CompileDesc.szShaderDefines[i], Desc.szShaderDefines[i], MAX_SHADER_DEFINE_LENGTH);
		NumDefines++;
	}

	/*
	* 추후 정렬이 필요할 수도 있으므로 남겨둠
	if (NumDefines)
		qsort(CompileDesc.szShaderDefines, NumDefines, MAX_SHADER_DEFINE_LENGTH, compare_strings);
	*/

	CompileDesc.NumDefines = NumDefines;
	CompileDesc.pBuffer = Desc.pShaderSource;
	CompileDesc.BufferSize = static_cast<UINT32>(Desc.ShaderSourceSize);

#if defined(_DEBUG) || defined(DEBUG)
	CompileDesc.Debug = TRUE;
#else
	CompileDesc.Debug = FALSE;
#endif // defined(_DEBUG)

	BOOL CompileOK = D3D11CompileShaderProgram(
		CompileDesc,
		CompileResult
	);

	if (!CompileOK)
	{
		SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: Failed to compile shader program '%s'", Desc.szDebugName);
		(*ppOut) = nullptr;
		return FALSE;
	}

	if (Desc.ShaderType == SHADER_TYPE::VERTEX)
	{
		// 입력 레이아웃 생성
		ID3D11InputLayout* pIL = D3D11InputLayoutCache::Get()->Find_RS(CompileResult.VertexFormat);
		if (!pIL)
		{
			D3D11_INPUTLAYOUT_DESC InputLayoutDesc = {};
			InputLayoutDesc.NumVertexFormat = CompileResult.NumVertexFormat;

			memcpy(
				InputLayoutDesc.VertexFormats,
				CompileResult.VertexFormats,
				sizeof(VERTEX_FORMAT) * CompileResult.NumVertexFormat
			);

			InputLayoutDesc.VertexFormat = CompileResult.VertexFormat;
			InputLayoutDesc.pShaderBlob = CompileResult.pShaderBlob;

			if (!m_pRenderDevice->CreateInputLayout(InputLayoutDesc, &pIL))
			{
				SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: Failed to create input layout for shader '%s'", Desc.szDebugName);
				CHECK_RELEASE(CompileResult.pShaderBlob);
				(*ppOut) = nullptr;
				return FALSE;
			}

			D3D11InputLayoutCache::Get()->Register(CompileResult.VertexFormat, pIL);
		}

		ID3D11Device* pDevice = m_pRenderDevice->INL_GetD3D11Device();

		ID3D11VertexShader* pVS = nullptr;

		if (FAILED(pDevice->CreateVertexShader(
			CompileResult.pShaderBlob->GetBufferPointer(),
			CompileResult.pShaderBlob->GetBufferSize(),
			nullptr,
			&pVS
		)))
		{
			SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: Failed to create vertex shader for shader '%s'", Desc.szDebugName);
			CHECK_RELEASE(CompileResult.pShaderBlob);
			(*ppOut) = nullptr;
			return FALSE;
		}

		interlock_increment_ul(&s_ShaderProgramIndex_VS, MEMORY_ORDER_RELAXED);

		if (MAX_VERTEX_SHADER_INDEX <= s_ShaderProgramIndex_VS)
		{
			SYS_LOG_W("D3D11RenderSystem::GetOrCreateShaderProgram: Vertex shader index exceeded the maximum limit");
		}

		D3D11ShaderProgram* pShaderProgram = D3D11_POOL_NEW(D3D11ShaderProgram)(
			(UINT32)s_ShaderProgramIndex_VS,
			Desc.ShaderName,
			ShaderDefinesHash,
			CompileResult.VertexFormat,
			pVS
		);

		CHECK_RELEASE(CompileResult.pShaderBlob);

		*ppOut = static_cast<IShaderProgram*>(pShaderProgram);
		pShaderProgram->AddRef();

		m_pShaderProgramCache->Register(pShaderProgram);
	}
	else if(Desc.ShaderType == SHADER_TYPE::PIXEL)
	{
		ID3D11PixelShader* pPS = nullptr;

		ID3D11Device* pDevice = m_pRenderDevice->INL_GetD3D11Device();
		if (FAILED(pDevice->CreatePixelShader(
			CompileResult.pShaderBlob->GetBufferPointer(),
			CompileResult.pShaderBlob->GetBufferSize(),
			nullptr,
			&pPS
		)))
		{
			SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: Failed to create pixel shader for shader '%s'", Desc.szDebugName);
			if (CompileResult.pShaderBlob)
				CompileResult.pShaderBlob->Release();
			(*ppOut) = nullptr;
			return FALSE;
		}

		interlock_increment_ul(&s_ShaderProgramIndex_PS, MEMORY_ORDER_RELAXED);

		if (MAX_PIXEL_SHADER_INDEX <= s_ShaderProgramIndex_PS)
		{
			SYS_LOG_W("D3D11RenderSystem::GetOrCreateShaderProgram: Pixel shader index exceeded the maximum limit");
		}

		D3D11ShaderProgram* pShaderProgram = D3D11_POOL_NEW(D3D11ShaderProgram)(
			(UINT32)s_ShaderProgramIndex_PS,
			Desc.ShaderName,
			ShaderDefinesHash,
			pPS
		);

		CHECK_RELEASE(CompileResult.pShaderBlob);

		*ppOut = static_cast<IShaderProgram*>(pShaderProgram);
		pShaderProgram->AddRef();

		m_pShaderProgramCache->Register(pShaderProgram);
	}
	else
	{
		SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: Unsupported shader type for shader '%s'", Desc.szDebugName);
		CHECK_RELEASE(CompileResult.pShaderBlob);

		(*ppOut) = nullptr;

		return FALSE;
	}

	return TRUE;
}

/*
BOOL D3D11RenderSystem::GetOrCreateDDSTextureFactory(IDDSTextureFactory** ppOut)
{
	if (m_pDDSTextureFactory == nullptr)
		m_pDDSTextureFactory = D3D11_NEW(D3D11DDSTextureFactory)(m_pRenderDevice->INL_GetD3D11Device());

	m_pDDSTextureFactory->AddRef();
	*ppOut = m_pDDSTextureFactory;

	return TRUE;
}
*/

BOOL D3D11RenderSystem::GetOrCreateRenderContext(IRenderContext** ppContext)
{
	if (m_pRenderContext == nullptr)
	{
		m_pRenderContext = D3D11_NEW(D3D11RenderContext)();
		m_pRenderContext->Init();
	}

	m_pRenderContext->AddRef();
	*ppContext = m_pRenderContext;

	return TRUE;
}

void D3D11RenderSystem::Update()
{
	if (m_pRenderCommandExecutor)
	{
		m_pRenderCommandExecutor->Execute();
	}
}
