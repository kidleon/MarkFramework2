#pragma once


namespace mark
{
	class D3D11RenderDevice;

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

	private:
		mutable std::atomic<int64_t> m_RefCount{ 1 };
		D3D11RenderDevice* m_pRenderDevice = nullptr;

	};
}
