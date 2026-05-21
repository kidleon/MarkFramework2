#include "pch.h"
#include "Engine.h"
#include "core.h"
#include "AssetManager.h"
#include "FileAssetProvider.h"
#include "GPUGeometry.h"
#include "GPUGeometryFactory.h"
#include "RenderSystem.h"


namespace mark
{
	typedef BOOL(__stdcall* PFN_CREATE_HARDWARE_GRAPHICS_LAYER)(
		const RenderSystemCreateDesc& CreateDesc,
		IHardwareGraphicsLayer** ppHardwareGraphicsLayer
	);

	Engine::Engine()
	{
	}

	Engine::~Engine() noexcept
	{
		Shutdown();
	}

	void Engine::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void Engine::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(Engine, this);
		}
	}

	bool Engine::Initialize(const EngineCreateDesc& CreateDesc)
	{
		if (m_Initialized) return true;

		LOG("Mark3D core memory initialized.");

		RenderSystemCreateDesc RenderCreateDesc = {};
		RenderCreateDesc.ScreenWidth = CreateDesc.ScreenWidth;
		RenderCreateDesc.ScreenHeight = CreateDesc.ScreenHeight;
		RenderCreateDesc.PreferredGraphicsAPI = CreateDesc.PreferredGraphicsAPI;

#if defined(_DEBUG)
		RenderCreateDesc.DebugMode = TRUE;
#else 
		RenderCreateDesc.DebugMode = FALSE;
#endif // defined(_DEBUG)
		
#if defined(__TARGET_OS_WINDOWS)
		RenderCreateDesc.WindowHandle = CreateDesc.WindowHandle;
#endif // #if defined(__TARGET_OS_WINDOWS)

		m_pRenderSystem = CORE_NEW(RenderSystem);
		if (!m_pRenderSystem->Initialize(RenderCreateDesc))
		{
			SYS_LOG_ERR("Failed to initialize RenderSystem.");
			m_pRenderSystem->Release();
			m_pRenderSystem = nullptr;
			return false;
		}

		FileAssetProvider* pFileAssetProvider = CORE_NEW(FileAssetProvider)(CreateDesc.szAssetRootPath);
		m_pAssetManager = CORE_NEW(AssetManager)(pFileAssetProvider);

		m_Initialized = TRUE;

		return true;
	}

	void Engine::Shutdown()
	{
		m_Initialized = FALSE;

		CHECK_RELEASE(m_pRenderSystem);
		if (m_HardwareGraphicsLayerHandle)
		{
			FreeLibrary(m_HardwareGraphicsLayerHandle);
			m_HardwareGraphicsLayerHandle = nullptr;
		}

		CHECK_RELEASE(m_pAssetManager);
//		shutdown_core_service();
	}

	bool Engine::GetRenderSystemInterface(IRenderSystem** ppOut)
	{
		if (!ppOut)
			return false;

		(*ppOut) = m_pRenderSystem;

		if (m_pRenderSystem)
			m_pRenderSystem->AddRef();

		return true;
	}

	bool Engine::GetAssetManagerInterface(IAssetManager** ppOut)
	{
		if (!ppOut)
			return false;

		(*ppOut) = m_pAssetManager;

		if (m_pAssetManager)
			m_pAssetManager->AddRef();

		return true;
	}

	IGPUGeometry* Engine::CreateGPUGeometry(const GPUGeometryCreateDesc& CreateDesc)
	{
		GPUGeometry* pGeometry = CORE_NEW(GPUGeometry);
		if (!GPUGeometryFactory::CreateGeometry(m_pRenderSystem, CreateDesc, pGeometry))
		{
			pGeometry->Release();
			return nullptr;
		}

		return pGeometry;
	}
}
