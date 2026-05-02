#pragma once
#include "Mark3D.h"


namespace mark
{
	class Engine final : public IMark3D
	{
		static Engine* s_pInstance;

	public:
		Engine();

		void AddRef() final;
		void Release() final;

		bool Initialize(const EngineCreateDesc& CreateDesc) final;
		void Shutdown() final;

		IModel* CreateModel(uint32_t VertexFormats, uint32_t VertexCount, uint32_t IndexCount) final;

		inline static Engine& Get() noexcept { return *s_pInstance; }
		[[nodiscard]] inline IRenderSystem* INL_GetRenderSystem() const noexcept { return m_pRenderSystem; }

	private:
		virtual ~Engine() noexcept;

		bool InitializeD3D11(const EngineCreateDesc& CreateDesc);

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		BOOL m_Initialized = FALSE;

#if defined(__TARGET_OS_WINDOWS)
		HMODULE m_RenderSystemModule = nullptr;
		HWND m_WindowHandle = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)

		IRenderSystem* m_pRenderSystem = nullptr;
	};
}
