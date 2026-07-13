#pragma once
#include "TMemoryBlockPool.h"


namespace mark
{
	class ShaderProgramCache;
	class SurfaceMaterialPool;
	class SurfaceMaterial;

	class RenderSystem final : public IRenderSystem
	{
	public:
		virtual void AddRef();
		virtual void Release();

		bool Initialize(const RenderSystemCreateDesc& CreateDesc);

		virtual IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& CreateDesc);
		virtual IShaderProgram* CreateShaderProgram(const ShaderProgramCreateDesc& CreateDesc);
		virtual IShaderProgram* GetShaderProgram(SHADER_TYPE ShaderType, const char* szShaderName);
		virtual ISurfaceMaterial* CreateSurfaceMaterial();

	private:
		virtual ~RenderSystem() noexcept;
		void Shutdown();

		bool PlugIn_CreateHardwareGraphicsLayer_D3D11(const RenderSystemCreateDesc& CreateDesc);

	private:
		std::atomic<int64_t> m_RefCount{ 1 };
		IHardwareGraphicsLayer* m_pHardwareGraphicsLayer = nullptr;
		ShaderProgramCache* m_pShaderProgramCache = nullptr;
		SurfaceMaterialPool* m_pSurfaceMaterialPool = nullptr;

		TMemoryBlockPool<SurfaceMaterial> m_SurfaceMaterialPool;

#if defined(__TARGET_OS_WINDOWS)
		HMODULE m_hHardwareGraphicsLayer = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)
	};
}
