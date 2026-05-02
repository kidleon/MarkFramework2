#pragma once


namespace mark
{
	class D3D11RenderDevice;
	class PagedPointerTable;

	class D3D11RenderSystem final : public IRenderSystem
	{
	public:
		virtual ~D3D11RenderSystem() noexcept;

		void AddRef();
		void Release();

		bool Initialize(const RenderSystemCreateDesc& desc);
		void Shutdown();

		IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& desc);

	private:
		mutable std::atomic<int64_t> m_RefCount = 1;
		D3D11RenderDevice* m_pRenderDevice = nullptr;

	};
}
