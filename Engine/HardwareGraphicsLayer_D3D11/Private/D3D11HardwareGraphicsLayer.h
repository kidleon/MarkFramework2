#pragma once


namespace mark
{
	class D3D11RenderDevice;
	class D3D11GPUBufferPool;
	class D3D11ShaderProgramCache;

	class D3D11HardwareGraphicsLayer final : public IHardwareGraphicsLayer
	{
	public:
		virtual ~D3D11HardwareGraphicsLayer() noexcept;

		void AddRef();
		void Release();

		bool Initialize(const RenderSystemCreateDesc& desc);
		void Shutdown();

		IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& desc);
		IShaderProgram* CreateShaderProgram(const ShaderProgramCreateDesc& desc);
		IShaderProgram* QueryShaderProgram(SHADER_TYPE ShaderType, const char* szShaderName);

		inline D3D11RenderDevice* INL_GetRenderDevice() const noexcept { return m_pRenderDevice; }

	private:
		mutable std::atomic<int64_t> m_RefCount{ 1 };
		D3D11RenderDevice* m_pRenderDevice = nullptr;
		D3D11GPUBufferPool* m_pGPUBufferPool = nullptr;
		D3D11ShaderProgramCache* m_pShaderProgramCache = nullptr;

	};
}
