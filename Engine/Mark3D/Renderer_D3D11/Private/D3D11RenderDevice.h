#pragma once


namespace mark
{
	class D3D11RenderDevice
	{
		static D3D11RenderDevice* s_pInstance;

	public:
		D3D11RenderDevice();
		~D3D11RenderDevice() noexcept;

		BOOL CreateDevice(HWND hWnd, uint32_t Width, uint32_t Height, bool DebugDevice);

		static inline D3D11RenderDevice& Get() noexcept
		{
			assert(s_pInstance && "D3D11RenderDevice instance is not created yet.");
			return *s_pInstance;
		}

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
