#ifndef __D3D11_TEXTURE1D_H__
#define __D3D11_TEXTURE1D_H__

#include "AssetDef.h"


class D3D11Texture1D final : public ITexture1D
{
public:
	D3D11Texture1D() = default;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual uint64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// ITexture1D interface
	virtual uint32 GetWidth() const noexcept final;
	virtual uint32 GetMipLevels() const noexcept final;
	virtual COLOR_FORMAT GetFormat() const noexcept final;

	__FORCEINLINE uint32 INL_GetWidth() const noexcept { return m_Width; }
	__FORCEINLINE uint32 INL_GetMipLevels() const noexcept { return m_MipLevels; }
	__FORCEINLINE COLOR_FORMAT INL_GetFormat() const noexcept { return m_Format; }

	__FORCEINLINE ID3D11ShaderResourceView* INL_GetSRV() const noexcept { return m_pSRV; }
	__FORCEINLINE ID3D11Texture1D* INL_GetD3D11Texture1D() const noexcept { return m_pTex1D; }

	__FORCEINLINE void INL_SetColorFormat(COLOR_FORMAT Format) noexcept { m_Format = Format; }
	__FORCEINLINE void INL_SetWidth(uint32 Width) noexcept { m_Width = Width; }
	__FORCEINLINE void INL_SetMipLevels(uint32 MipLevels) noexcept { m_MipLevels = MipLevels; }
	__FORCEINLINE void INL_SetSRV(ID3D11ShaderResourceView* pSRV) noexcept { m_pSRV = pSRV; }
	__FORCEINLINE void INL_SetD3D11Texture1D(ID3D11Texture1D* pTex1D) noexcept { m_pTex1D = pTex1D; }

	__FORCEINLINE void INL_SetLoadStat(LOAD_STAT LoadStat) noexcept 
	{ 
		interlock_store_ul((volatile unsigned long*)&m_LoadStat, static_cast<unsigned long>(LoadStat), MEMORY_ORDER_RELAXED);
	}

private:
	
	virtual ~D3D11Texture1D() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	uint64 m_ID = 0;
	volatile LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	COLOR_FORMAT m_Format = COLOR_FORMAT::UNKNOWN;

	uint32 m_Width = 0;
	uint32 m_MipLevels = 0;

	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11Texture1D* m_pTex1D = nullptr;

};



#endif // __D3D11_TEXTURE1D_H__
