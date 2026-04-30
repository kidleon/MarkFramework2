#pragma once


namespace mark
{
	class D3D11RenderDevice;

	class D3D11RenderSystem final : public IRenderSystem
	{
	public:
		~D3D11RenderSystem() noexcept;

		void AddRef() final;
		void Release() final;

		bool Initialize(const RenderSystemCreateDesc& desc) final;
		void Shutdown() final;

		PrimitiveBufferHandle CreatePrimitiveBuffer(const PrimitiveBufferCreateDesc& desc) final;

	private:
		mutable std::atomic<int64_t> m_RefCount = 1;
		D3D11RenderDevice* m_pRenderDevice = nullptr;

	};
}
