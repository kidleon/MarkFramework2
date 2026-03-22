#pragma once

#include "AssetDef.h"


class D3D11Texture2D final : public ITexture2D
{
public:
	D3D11Texture2D() = default;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual uint64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

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

	__FORCEINLINE void INL_SetColorFormat(COLOR_FORMAT Format) noexcept { m_Format = Format; }
	__FORCEINLINE void INL_SetWidth(uint32 Width) noexcept { m_Width = Width; }
	__FORCEINLINE void INL_SetHeight(uint32 Height) noexcept { m_Height = Height; }
	__FORCEINLINE void INL_SetMipLevels(uint32 MipLevels) noexcept { m_MipLevels = MipLevels; }
	__FORCEINLINE void INL_SetSRV(ID3D11ShaderResourceView* pSRV) noexcept { m_pSRV = pSRV; }
	__FORCEINLINE void INL_SetD3D11Texture2D(ID3D11Texture2D* pTex2D) noexcept { m_pTex2D = pTex2D; }

	__FORCEINLINE void INL_SetLoadStat(LOAD_STAT LoadStat) noexcept
	{
		interlock_store_ul((volatile unsigned long*)&m_LoadStat, static_cast<unsigned long>(LoadStat), MEMORY_ORDER_RELAXED);
	}

private:
	virtual ~D3D11Texture2D() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	uint64 m_ID = 0;
	volatile LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	COLOR_FORMAT m_Format = COLOR_FORMAT::UNKNOWN;

	uint32 m_Width = 0;
	uint32 m_Height = 0;
	uint32 m_MipLevels = 0;

	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11Texture2D* m_pTex2D = nullptr;

};
