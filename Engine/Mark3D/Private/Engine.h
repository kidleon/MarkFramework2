#pragma once
#include "Mark3D.h"


namespace mark
{
	interface IMesh;

	class Engine final : public IMark3D
	{
		static Engine* s_pInstance;

	public:
		Engine();

		void AddRef();
		void Release();

		bool Initialize(const EngineCreateDesc& CreateDesc);
		void Shutdown();

		bool GetRenderSystemInterface(IRenderSystem** ppOut);

		IMesh* CreateMesh(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount);
		IMesh* LoadMesh(const char* szFilePath);
		IMesh* LoadMeshAsync(const char* szFilePath);

		inline static Engine& Get() noexcept { return *s_pInstance; }
		inline static Engine* GetPtr() noexcept { return s_pInstance; }
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
