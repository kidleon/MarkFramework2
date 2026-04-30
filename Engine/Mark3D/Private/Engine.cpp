#include "pch.h"
#include "Engine.h"
#include "core.h"


namespace mark
{
	typedef BOOL(__stdcall* PFN_CREATE_RENDERER_D3D11)(
		const RenderSystemCreateDesc& CreateDesc,
		IRenderSystem** ppRenderSystem
	);

	Engine::~Engine() noexcept
	{
		Shutdown();
	}

	void Engine::AddRef()
	{
		m_RefCount++;
	}

	void Engine::Release()
	{
		if (--m_RefCount == 0)
		{
			CORE_DELETE(Engine, this);
		}
	}

	bool Engine::Initialize(const EngineCreateDesc& CreateDesc)
	{
		if (m_Initialized) return true;

		initialaize_core_service(
			1024 * 1024 * 32 // 32MB 임시 버퍼
		);

		LOG("Mark3D core memory initialized.");

#if defined(__TARGET_OS_WINDOWS)
		if (!CreateDesc.WindowHandle)
		{
			Shutdown();
			LOG_ERR("Invalid window handle provided in EngineCreateDesc.");
			return false;
		}

		if (CreateDesc.PreferredGraphicsAPI == GraphicsAPI::D3D11)
		{
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
				Shutdown();
				return false;
			}

			m_RenderSystemModule = hModule;
		}
		else
		{
			LOG_ERR("Unsupported graphics API specified in EngineCreateDesc.");
			return false;
		}

		PFN_CREATE_RENDERER_D3D11 pfnCreateRendererD3D11 = (PFN_CREATE_RENDERER_D3D11)GetProcAddress(
			m_RenderSystemModule,
			"CreateRenderSystem"
		);

		if (!pfnCreateRendererD3D11)
		{
			LOG_ERR("Failed to find CreateRenderSystem function in RenderSystem_D3D11.dll.");
			Shutdown();
			return FALSE;
		}

		LOG("Successfully loaded RenderSystem_D3D11.dll and found CreateRenderSystem function.");

		RenderSystemCreateDesc RenderCreateDesc = {};
		RenderCreateDesc.ScreenWidth = CreateDesc.ScreenWidth;
		RenderCreateDesc.ScreenHeight = CreateDesc.ScreenHeight;
		RenderCreateDesc.WindowHandle = CreateDesc.WindowHandle;
		if (!pfnCreateRendererD3D11(RenderCreateDesc, &m_pRenderSystem))
		{
			LOG_ERR("Failed to create D3D11 render system.");
			Shutdown();
			return false;
		}

#endif // #if defined(__TARGET_OS_WINDOWS)

		m_Initialized = TRUE;

		return true;
	}

	void Engine::Shutdown()
	{
		m_Initialized = FALSE;

		if (m_pRenderSystem)
		{
			m_pRenderSystem->Release();
			m_pRenderSystem = nullptr;
		}

		if (m_RenderSystemModule)
		{
			FreeLibrary(m_RenderSystemModule);
			m_RenderSystemModule = nullptr;
		}

		shutdown_core_service();
	}

}
