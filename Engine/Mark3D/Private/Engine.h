#pragma once
#include "Mark3D.h"


namespace mark
{
	class Engine final : public IMark3D
	{
	public:
		void AddRef() final;
		void Release() final;

		bool Initialize(const EngineCreateDesc& CreateDesc) final;
		void Shutdown() final;

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
