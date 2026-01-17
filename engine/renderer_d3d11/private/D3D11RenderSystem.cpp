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
	D3D11Common::Init();

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

	m_pCBAllocator = D3D11_NEW(D3D11ConstantBufferAllocator)();
	m_pCBAllocator->Init(m_pRenderDevice);

	m_pShaderProgramCache = D3D11_NEW(D3D11ShaderProgramCache)();
	m_pShaderProgramCache->Init();

	m_pRenderCommandExecutor = D3D11_NEW(D3D11RenderCommandExecutor)(m_pRenderDevice);

	return TRUE;
}

void D3D11RenderSystem::Shutdown()
{
	if (m_pRenderCommandExecutor)
	{
		D3D11_DELETE(m_pRenderCommandExecutor, D3D11RenderCommandExecutor);
		m_pRenderCommandExecutor = nullptr;
	}

	if (m_pShaderProgramCache)
	{
		D3D11_DELETE(m_pShaderProgramCache, D3D11ShaderProgramCache);
		m_pShaderProgramCache = nullptr;
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
	
	D3D11Common::Shutdown();

	D3D11Heap_ReportLeaks();
	D3D11Heap_Shutdown();
}

const RENDER_SETTINGS& D3D11RenderSystem::GetRenderSettings() const noexcept
{
	return D3D11Common::GetRenderSettings();
}

void D3D11RenderSystem::SetRenderSettings(const RENDER_SETTINGS& Settings) noexcept
{
	D3D11Common::SetRenderSettings(Settings);
}

BOOL D3D11RenderSystem::CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& Desc, IPrimitiveBuffer** ppOut)
{
	D3D11_BUFFER_DESC VertexBufferDesc = {};
	VertexBufferDesc.ByteWidth = static_cast<UINT>(Desc.VertexBufferSize);
	VertexBufferDesc.Usage = D3D11_IMPL_BUFFER_USAGE[(int)Desc.Usage];
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = (Desc.Usage == BUFFER_USAGE::DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;

	D3D11_BUFFER_DESC IndexBufferDesc = {};
	IndexBufferDesc.ByteWidth = static_cast<UINT>(Desc.IndexBufferSize);
	IndexBufferDesc.Usage = D3D11_IMPL_BUFFER_USAGE[(int)Desc.Usage];
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = (Desc.Usage == BUFFER_USAGE::DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;

	ID3D11Device* pDevice = m_pRenderDevice->INL_GetD3D11Device();

	D3D11_SUBRESOURCE_DATA InitialVertexData = {};
	InitialVertexData.pSysMem = Desc.pInitialVertexData;

	ID3D11Buffer* pD3D11_VB = nullptr;
	HRESULT hr = pDevice->CreateBuffer(
		&VertexBufferDesc,
		Desc.IsInitialData ? &InitialVertexData : nullptr,
		&pD3D11_VB
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderSystem::CreatePrimitiveBuffer: Failed to create vertex buffer (HRESULT: 0x%08X)", hr);
		return FALSE;
	}

	D3D11_SUBRESOURCE_DATA InitialIndexData = {};
	InitialIndexData.pSysMem = Desc.pInitialIndexData;
	ID3D11Buffer* pD3D11_IB = nullptr;
	hr = pDevice->CreateBuffer(
		&IndexBufferDesc,
		Desc.IsInitialData ? &InitialIndexData : nullptr,
		&pD3D11_IB
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderSystem::CreatePrimitiveBuffer: Failed to create index buffer (HRESULT: 0x%08X)", hr);
		CHECK_RELEASE(pD3D11_VB);
		return FALSE;
	}

	D3D11PrimitiveBuffer* pPrimitiveBuffer = D3D11_POOL_NEW(D3D11PrimitiveBuffer)(
		Desc.Usage,
		pD3D11_VB,
		pD3D11_IB,
		Desc.VertexBufferSize,
		Desc.IndexBufferSize
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

BOOL D3D11RenderSystem::GetOrCreateShaderProgram(const SHADER_PROGRAM_CREATE_DESC& Desc, IShaderProgram** ppOut)
{
	if (Desc.ShaderName.empty())
	{
		SYS_LOG_E("D3D11RenderSystem::GetOrCreateShaderProgram: ShaderName is empty");
		(*ppOut) = nullptr;
		return FALSE;
	}

	uint32 ShaderDefinesHash = fnv1(Desc.szShaderDefines, MAX_SHADER_DEFINE * MAX_SHADER_DEFINE_LENGTH, 0);

	D3D11ShaderProgram* pSP = m_pShaderProgramCache->Find(Desc.ShaderType, Desc.ShaderName, ShaderDefinesHash);
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
	


	return TRUE;
}

BOOL D3D11RenderSystem::GetOrCreateRenderContext(IRenderContext** ppContext)
{
	if (m_pRenderContext == nullptr)
	{
		m_pRenderContext = D3D11_POOL_NEW(D3D11RenderContext)();
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
