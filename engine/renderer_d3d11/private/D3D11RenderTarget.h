#ifndef __D3D11_RENDER_TARGET_H__
#define __D3D11_RENDER_TARGET_H__


class D3D11RenderTarget final : public IRenderTarget
{
public:
	explicit D3D11RenderTarget(
		UINT32 ColorWidth,
		UINT32 ColorHeight,
		COLOR_FORMAT ColorFormat,
		ID3D11Texture2D* pD3D11ColorTexture,
		ID3D11RenderTargetView* pD3D11RTV
	) noexcept;

	explicit D3D11RenderTarget(
		UINT32 ColorWidth,
		UINT32 ColorHeight,
		COLOR_FORMAT ColorFormat,
		UINT32 DepthStencilWidth,
		UINT32 DepthStencilHeight,
		COLOR_FORMAT DepthStencilFormat,
		ID3D11Texture2D* pD3D11ColorTexture,
		ID3D11RenderTargetView* pD3D11RTV,
		ID3D11Texture2D* pD3D11DepthStencilTexture,
		ID3D11DepthStencilView* pD3D11DSV
	) noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IRenderTarget interface
	virtual UINT32 GetColorWidth() const noexcept final;
	virtual UINT32 GetColorHeight() const noexcept final;
	virtual COLOR_FORMAT GetColorFormat() const noexcept final;

	virtual BOOL HasDepthStencil() const noexcept final;
	virtual UINT32 GetDepthWidth() const noexcept final;
	virtual UINT32 GetDepthHeight() const noexcept final;
	virtual COLOR_FORMAT GetDepthFormat() const noexcept final;

	__INLINE UINT32 INL_GetColorWidth() const noexcept { return m_ColorWidth; }
	__INLINE UINT32 INL_GetColorHeight() const noexcept { return m_ColorHeight; }
	__INLINE COLOR_FORMAT INL_GetColorFormat() const noexcept { return m_ColorFormat; }

	__INLINE BOOL INL_HasDepthStencil() const noexcept { return m_HasDepthStencil; }
	__INLINE UINT32 INL_GetDepthWidth() const noexcept { return m_DepthStencilWidth; }
	__INLINE UINT32 INL_GetDepthHeight() const noexcept { return m_DepthStencilHeight; }
	__INLINE COLOR_FORMAT INL_GetDepthFormat() const noexcept { return m_DepthStencilFormat; }

	__INLINE ID3D11Texture2D* INL_GetColorTexture() const noexcept { return m_pD3D11ColorTexture; }
	__INLINE ID3D11Texture2D* INL_GetDepthStencilTexture() const noexcept { return m_pD3D11DepthStencilTexutre; }
	__INLINE ID3D11RenderTargetView* INL_GetRTV() const noexcept { return m_pD3D11RTV; }
	__INLINE ID3D11DepthStencilView* INL_GetDSV() const noexcept { return m_pD3D11DSV; }

private:
	virtual ~D3D11RenderTarget() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	UINT32 m_ColorWidth = 0;
	UINT32 m_ColorHeight = 0;
	COLOR_FORMAT m_ColorFormat = COLOR_FORMAT::UNKNOWN;

	BOOL m_HasDepthStencil = FALSE;
	UINT32 m_DepthStencilWidth = 0;
	UINT32 m_DepthStencilHeight = 0;
	COLOR_FORMAT m_DepthStencilFormat = COLOR_FORMAT::UNKNOWN;

	UINT32 PADDING = 0;

	ID3D11Texture2D* m_pD3D11ColorTexture = nullptr;
	ID3D11RenderTargetView* m_pD3D11RTV = nullptr;

	ID3D11Texture2D* m_pD3D11DepthStencilTexutre = nullptr;
	ID3D11DepthStencilView* m_pD3D11DSV = nullptr;

};

#endif // __D3D11_RENDER_TARGET_H__
