#ifndef __D3D11_TEXTURE2D_H__
#define __D3D11_TEXTURE2D_H__

#include "ITexture2D.h"


class D3D11Texture2D final : public ITexture2D
{
public:
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT32 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// ITexture2D interface
	uint32 GetWidth() const noexcept final;
	uint32 GetHeight() const noexcept final;
	uint32 GetMipLevels() const noexcept final;
	COLOR_FORMAT GetFormat() const noexcept final;

	void SetData(
		uint32 Width,
		uint32 Height,
		uint32 MipLevels,
		COLOR_FORMAT Format,
		ID3D11Texture2D* pTexture2D,
		ID3D11ShaderResourceView* pSRV
	) noexcept;

	__FORCEINLINE uint32 INL_GetWidth() const { return m_Width; }
	__FORCEINLINE uint32 INL_GetHeight() const { return m_Height; }
	__FORCEINLINE uint32 INL_GetMipLevels() const { return m_MipLevels; }
	__FORCEINLINE COLOR_FORMAT INL_GetFormat() const { return m_Format; }
	__FORCEINLINE ID3D11Texture2D* INL_GetD3D11Texture2D() const { return m_pD3D11Texture; }
	__FORCEINLINE ID3D11ShaderResourceView* INL_GetD3D11SRV() const { return m_pD3D11SRV; }

private:
	D3D11Texture2D() = delete;
	virtual ~D3D11Texture2D() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	uint32 m_Width = 0;
	uint32 m_Height = 0;
	uint32 m_MipLevels = 0;
	COLOR_FORMAT m_Format = COLOR_FORMAT::UNKNOWN;

	ID3D11Texture2D* m_pD3D11Texture = nullptr;
	ID3D11ShaderResourceView* m_pD3D11SRV = nullptr;
};

#endif // __D3D11_TEXTURE2D_H__
