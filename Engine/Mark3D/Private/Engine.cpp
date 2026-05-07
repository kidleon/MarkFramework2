#include "pch.h"
#include "Engine.h"
#include "core.h"
#include "AssetManager.h"
#include "FileAssetProvider.h"


namespace mark
{
	Engine* Engine::s_pInstance = nullptr;

	typedef BOOL(__stdcall* PFN_CREATE_RENDERER_D3D11)(
		const RenderSystemCreateDesc& CreateDesc,
		IRenderSystem** ppRenderSystem
		);

	Engine::Engine()
	{
		if (!s_pInstance)
			s_pInstance = this;
	}

	Engine::~Engine() noexcept
	{
		Shutdown();

		if (s_pInstance == this)
			s_pInstance = nullptr;
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

		if (CreateDesc.PreferredGraphicsAPI == GraphicsAPI::D3D11)
		{
			if (!InitializeD3D11(CreateDesc))
			{
				LOG_ERR("Failed to initialize D3D11 render system.");
				Shutdown();
				return false;
			}
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
		if (m_RenderSystemModule)
		{
			FreeLibrary(m_RenderSystemModule);
			m_RenderSystemModule = nullptr;
		}

		CHECK_RELEASE(m_pAssetManager);
//		shutdown_core_service();
	}

	bool Engine::InitializeD3D11(const EngineCreateDesc& CreateDesc)
	{
#if defined(__TARGET_OS_WINDOWS)
		if (!CreateDesc.WindowHandle)
		{
			LOG_ERR("Invalid window handle provided in EngineCreateDesc.");
			return false;
		}

		LOG("Initializing Mark3D with Direct3D 11...");

		// D3D11 초기화 코드 작성
#if defined(_DEBUG)
		HMODULE hModule = LoadLibraryA("./RenderSystem_D3D11d.dll");
#else 
		HMODULE hModule = LoadLibraryA("./RenderSystem_D3D11.dll");
#endif // defined(_DEBUG)

		if (!hModule)
		{
			LOG_ERR("Failed to load RenderSystem_D3D11.dll.");
			return false;
		}

		m_RenderSystemModule = hModule;

		PFN_CREATE_RENDERER_D3D11 pfnCreateRendererD3D11 = (PFN_CREATE_RENDERER_D3D11)GetProcAddress(
			m_RenderSystemModule,
			"CreateRenderSystem"
		);

		if (!pfnCreateRendererD3D11)
		{
			LOG_ERR("Failed to find CreateRenderSystem function in RenderSystem_D3D11.dll.");
			return FALSE;
		}

		LOG("Successfully loaded RenderSystem_D3D11.dll and found CreateRenderSystem function.");

		RenderSystemCreateDesc RenderCreateDesc = {};
		RenderCreateDesc.ScreenWidth = CreateDesc.ScreenWidth;
		RenderCreateDesc.ScreenHeight = CreateDesc.ScreenHeight;
		RenderCreateDesc.WindowHandle = CreateDesc.WindowHandle;
		RenderCreateDesc.DebugMode = FALSE; // TODO: CreateDesc에서 DebugMode 설정 추가

#if defined(_DEBUG)
		RenderCreateDesc.DebugMode = TRUE;
#endif // defined(_DEBUG)

		if (!pfnCreateRendererD3D11(RenderCreateDesc, &m_pRenderSystem))
		{
			LOG_ERR("Failed to create D3D11 render system.");
			return false;
		}

#endif // #if defined(__TARGET_OS_WINDOWS)

		return true;
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

	IGPUGeometry* Engine::CreateGeometry(
		IModelAsset* pModelAsset,
		GPU_BUFFER_LAYOUT BufferLayout,
		BOOL HasModelAsset
	)
	{
		return nullptr;
	}
}
