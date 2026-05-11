#pragma once
#include "Mark3D.h"


namespace mark
{
	class RenderSystem;

	class Engine final : public IMark3D
	{
		static Engine* s_pInstance;

	public:
		Engine();

		virtual void AddRef();
		virtual void Release();

		virtual bool Initialize(const EngineCreateDesc& CreateDesc);
		virtual void Shutdown();

		virtual bool GetRenderSystemInterface(IRenderSystem** ppOut);
		virtual bool GetAssetManagerInterface(IAssetManager** ppOut);

		virtual IGPUGeometry* CreateGPUGeometry(const GPUGeometryCreateDesc& CreateDesc);

		inline static Engine& Get() noexcept { return *s_pInstance; }
		inline static Engine* GetPtr() noexcept { return s_pInstance; }

		[[nodiscard]] inline RenderSystem* INL_GetRenderSystem() const noexcept { return m_pRenderSystem; }
		[[nodiscard]] inline IAssetManager* INL_GetAssetManager() const noexcept { return m_pAssetManager; }

	private:
		virtual ~Engine() noexcept;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		BOOL m_Initialized = FALSE;

#if defined(__TARGET_OS_WINDOWS)
		HMODULE m_HardwareGraphicsLayerHandle = nullptr;
		HWND m_WindowHandle = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)

		RenderSystem* m_pRenderSystem = nullptr;
		IAssetManager* m_pAssetManager = nullptr;

	};
}
