#include "pch.h"
#include "RenderSystem.h"
#include "SurfaceMaterial.h"


namespace mark
{
	typedef BOOL(__stdcall* PFN_CREATE_HARDWARE_GRAPHICS_LAYER)(
		const RenderSystemCreateDesc& CreateDesc,
		IHardwareGraphicsLayer** ppHardwareGraphicsLayer
	);

	RenderSystem::~RenderSystem() noexcept
	{
		Shutdown();
	}

	void RenderSystem::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void RenderSystem::Release()
	{
		int64_t newRefCount = m_RefCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
		if (newRefCount == 0)
		{
			CORE_DELETE(RenderSystem, this);
		}
	}

	bool RenderSystem::Initialize(const RenderSystemCreateDesc& CreateDesc)
	{
#if defined(__TARGET_OS_WINDOWS)
		if (CreateDesc.PreferredGraphicsAPI == GRAPHICS_API::D3D11)
		{
			if (!PlugIn_CreateHardwareGraphicsLayer_D3D11(CreateDesc))
			{
				SYS_LOG_ERR("Failed to initialize D3D11 hardware graphics layer.");
				return false;
			}
		}
#endif // #if defined(__TARGET_OS_WINDOWS)

		if (!m_SurfaceMaterialPool.Initialize())
		{
			SYS_LOG_ERR("Failed to initialize SurfaceMaterial memory block pool.");
			return false;
		}

		return true;
	}

	void RenderSystem::Shutdown()
	{
		m_SurfaceMaterialPool.Shutdown();

		CHECK_RELEASE(m_pHardwareGraphicsLayer);

		if (m_hHardwareGraphicsLayer)
		{
			FreeLibrary(m_hHardwareGraphicsLayer);
			m_hHardwareGraphicsLayer = nullptr;
		}
	}

	IGPUBuffer* RenderSystem::CreateGPUBuffer(const GPUBufferCreateDesc& CreateDesc)
	{
		return m_pHardwareGraphicsLayer->CreateGPUBuffer(CreateDesc);
	}

	IShaderProgram* RenderSystem::CreateShaderProgram(const ShaderProgramCreateDesc& CreateDesc)
	{
		IShaderProgram* pShaderProgram = m_pHardwareGraphicsLayer->CreateShaderProgram(CreateDesc);

		if (!pShaderProgram)
			return nullptr;

		return pShaderProgram;
	}

	IShaderProgram* RenderSystem::GetShaderProgram(SHADER_TYPE ShaderType, const char* szShaderName)
	{
		return m_pHardwareGraphicsLayer->QueryShaderProgram(ShaderType, szShaderName);
	}

	ISurfaceMaterial* RenderSystem::CreateSurfaceMaterial()
	{
		// SurfaceMaterial 생성 로직
		SurfaceMaterial* pSurfaceMaterial = m_SurfaceMaterialPool.Acquire();
		return static_cast<ISurfaceMaterial*>(pSurfaceMaterial);
	}

	bool RenderSystem::PlugIn_CreateHardwareGraphicsLayer_D3D11(const RenderSystemCreateDesc& CreateDesc)
	{
		// D3D11 하드웨어 그래픽스 레이어 생성 로직
#if defined(__TARGET_OS_WINDOWS)
		if (!CreateDesc.WindowHandle)
		{
			LOG_ERR("Invalid window handle provided in EngineCreateDesc.");
			return false;
		}

		LOG("Initializing Mark3D with Direct3D 11...");

		// D3D11 초기화 코드 작성
#if defined(_DEBUG)
		HMODULE hModule = LoadLibraryA("./HardwareGraphicsLayer_D3D11d.dll");
#else 
		HMODULE hModule = LoadLibraryA("./HardwareGraphicsLayer_D3D11.dll");
#endif // defined(_DEBUG)

		if (!hModule)
		{
			LOG_ERR("Failed to load HardwareGraphicsLayer_D3D11.dll.");
			return false;
		}

		m_hHardwareGraphicsLayer = hModule;

		PFN_CREATE_HARDWARE_GRAPHICS_LAYER pfnCreateHardwareGraphicsLayer = (PFN_CREATE_HARDWARE_GRAPHICS_LAYER)GetProcAddress(
			hModule,
			"CreateHardwareGraphicsLayer"
		);

		if (!pfnCreateHardwareGraphicsLayer(CreateDesc, &m_pHardwareGraphicsLayer))
		{
			LOG_ERR("Failed to create D3D11 Hardware Graphics Layer.");
			return false;
		}

		return true;
		

#endif // #if defined(__TARGET_OS_WINDOWS)
	}
}
