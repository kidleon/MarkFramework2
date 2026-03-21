#pragma once

#include "AssetDef.h"


class D3D11Texture2D final : public ITexture2D
{
public:
	D3D11Texture2D(
		uint64 ID,
		COLOR_FORMAT Format,
		uint32 Width,
		uint32 Height,
		uint32 MipLevels,
		ID3D11Texture2D* pTex2D,
		ID3D11ShaderResourceView* pSRV
	);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// ITexture2D interface
	virtual uint32 GetWidth() const noexcept final;
	virtual uint32 GetHeight() const noexcept final;
	virtual uint32 GetMipLevels() const noexcept final;
	virtual COLOR_FORMAT GetFormat() const noexcept final;

	__FORCEINLINE uint32 INL_GetWidth() const noexcept { return m_Width; }	
	__FORCEINLINE uint32 INL_GetHeight() const noexcept { return m_Height; }
	__FORCEINLINE uint32 INL_GetMipLevels() const noexcept { return m_MipLevels; }
	__FORCEINLINE COLOR_FORMAT INL_GetFormat() const noexcept { return m_Format; }

	__FORCEINLINE ID3D11ShaderResourceView* INL_GetSRV() const noexcept { return m_pSRV; }
	__FORCEINLINE ID3D11Texture2D* INL_GetD3D11Texture2D() const noexcept { return m_pTex2D; }

private:
	D3D11Texture2D() = delete;
	virtual ~D3D11Texture2D() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	uint64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	COLOR_FORMAT m_Format = COLOR_FORMAT::UNKNOWN;

	uint32 m_Width = 0;
	uint32 m_Height = 0;
	uint32 m_MipLevels = 0;

	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11Texture2D* m_pTex2D = nullptr;

};
