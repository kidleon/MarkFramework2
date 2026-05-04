#pragma once


namespace mark
{
	class D3D11RenderDevice;
	class D3D11ShaderProgramCache;

	class D3D11RenderSystem final : public IRenderSystem
	{
	public:
		virtual ~D3D11RenderSystem() noexcept;

		void AddRef();
		void Release();

		bool Initialize(const RenderSystemCreateDesc& desc);
		void Shutdown();

		IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& desc);
		IShaderProgram* CreateShaderProgram(const ShaderProgramCreateDesc& desc);
		IShaderProgram* GetShaderProgram(SHADER_TYPE ShaderType, const char* szShaderName);

	private:
		mutable std::atomic<int64_t> m_RefCount = 1;
		D3D11RenderDevice* m_pRenderDevice = nullptr;
		D3D11ShaderProgramCache* m_pShaderProgramCache = nullptr;

	};
}
