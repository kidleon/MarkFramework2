#pragma once


namespace mark
{
	class D3D11RenderDevice;

	class D3D11Renderer final : public IRenderer
	{
	public:
		D3D11Renderer();
		virtual ~D3D11Renderer() noexcept;

		bool Initialize(const RENDERER_CREATE_DESC& CreateDesc) final;
		void Shutdown() final;

		ResourceHandle CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& CreateDesc) final;
		void ReleaseHandle(ResourceHandle Handle) final;

	private:
		D3D11RenderDevice* m_pRenderDevice; 
		
	};
}
