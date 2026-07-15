#include "pch.h"
#include "Engine.h"
#include "core.h"
#include "AssetManager.h"
#include "FileAssetProvider.h"
#include "RenderSystem.h"
#include "PrimitiveBuffer.h"
#include "World.h"
#include "Model.h"
#include "ModelFactory.h"


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

		while (!m_lstWorlds.empty())
		{
			World* pWorld = m_lstWorlds.back();
			m_lstWorlds.pop_back();

			if (pWorld)
			{
				pWorld->BigRip();
				pWorld->Release();
			}
		}

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

	ISurfaceMaterial* Engine::CreateSurfaceMaterial()
	{
		return m_pRenderSystem->CreateSurfaceMaterial();
	}

	IModel* Engine::CreateModel(const ModelCreateDesc& CreateDesc)
	{
		// Implement model creation logic here
		PrimitiveBuffer* pPrimitiveBuffer = CORE_NEW(PrimitiveBuffer);

		if (!pPrimitiveBuffer->Create(
			m_pRenderSystem,
			CreateDesc.VertexFormats,
			CreateDesc.VertexBufferSize,
			CreateDesc.IndexFormat,
			CreateDesc.IndexBufferSize
		))
		{
			CORE_DELETE(PrimitiveBuffer, pPrimitiveBuffer);
			return nullptr;
		}

		return static_cast<IModel*>(CORE_NEW(Model)(pPrimitiveBuffer));
	}

	IModel* Engine::LoadModel(const char* szModelPath, MODEL_LAYOUT Layout)
	{
		if (!szModelPath || !szModelPath[0])
			return nullptr;

		ModelAsset* pModelAsset = m_pAssetManager->LoadModelAsset(szModelPath);
		if (!pModelAsset)
			return nullptr;

		Model* pModel = ModelFactory::CreateModel(m_pRenderSystem, pModelAsset, Layout);
		
		return static_cast<IModel*>(pModel);
	}

	IWorld* Engine::CreateWorld(const char* Name)
	{
		if (!Name || !Name[0])
			return nullptr;

		for (World* pWorld : m_lstWorlds)
		{
			if (pWorld && safe_strcmp(pWorld->INL_GetName(), Name) == 0)
				return nullptr;
		}

		World* pWorld = CORE_NEW(World);
		if (!pWorld)
			return nullptr;

		pWorld->BigBang(Name);
		m_lstWorlds.push_back(pWorld);

		return pWorld;
	}

	void Engine::DestroyWorld(IWorld* pWorld)
	{
		World* pTargetWorld = static_cast<World*>(pWorld);
		if (!pTargetWorld)
			return;

		for (auto it = m_lstWorlds.begin(); it != m_lstWorlds.end(); ++it)
		{
			if (*it == pTargetWorld)
			{
				m_lstWorlds.erase(it);
				pTargetWorld->BigRip();
				pTargetWorld->Release();
				return;
			}
		}
	}
}
