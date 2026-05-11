#pragma once


namespace mark
{
	class D3D11ShaderProgram;

	class D3D11RenderDevice
	{
		static D3D11RenderDevice* s_pInstance;

	public:
		D3D11RenderDevice();
		~D3D11RenderDevice() noexcept;

		bool CreateDevice(HWND hWnd, uint32_t Width, uint32_t Height, bool DebugDevice);

		ID3D11Buffer* CreateBuffer(const GPUBufferCreateDesc& CreateDesc);

		D3D11ShaderProgram* CompileShaderProgram(const ShaderProgramCreateDesc& CreateDesc);

		[[nodiscard]] inline ID3D11Device* INL_GetD3D11Device() const noexcept { return m_pD3D11Device; }
		[[nodiscard]] inline ID3D11DeviceContext* INL_GetD3D11Context() const noexcept { return m_pImmediateContext; }
		[[nodiscard]] inline IDXGISwapChain* INL_GetSwapChain() const noexcept { return m_pSwapChain; }
		[[nodiscard]] inline ID3D11RenderTargetView* INL_GetBackBuffer_RenderTargetView() const noexcept { return m_pRenderTargetView; }
		[[nodiscard]] inline ID3D11DepthStencilView* INL_GetBackBuffer_DepthStencilView() const noexcept { return m_pDepthStencilView; }
		[[nodiscard]] inline ID3D11Texture2D* INL_GetBackBuffer_DepthStencilTexture() const noexcept { return m_pDepthStencilTexture; }

		static D3D11RenderDevice& Get() noexcept { return *s_pInstance; }

	private:
		void DestroyDevice() noexcept;
		void ReportLiveObjects() noexcept;

	private:
		HWND m_hWnd = nullptr;
		D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
		D3D_FEATURE_LEVEL m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

		ID3D11Device* m_pD3D11Device = nullptr;
		ID3D11Device1* m_pD3D11Device1 = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;
		ID3D11DeviceContext1* m_pImmediateContext1 = nullptr;
		IDXGISwapChain* m_pSwapChain = nullptr;
		IDXGISwapChain1* m_pSwapChain1 = nullptr;

		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		
	};
}
