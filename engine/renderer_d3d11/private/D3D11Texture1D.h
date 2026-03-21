#ifndef __D3D11_TEXTURE1D_H__
#define __D3D11_TEXTURE1D_H__

#include "AssetDef.h"


class D3D11Texture1D final : public ITexture1D
{
public:
	D3D11Texture1D(
		uint64 ID,
		COLOR_FORMAT Format,
		uint32 Width,
		uint32 MipLevels,
		ID3D11Texture1D* pTex1D,
		ID3D11ShaderResourceView* pSRV
	);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// ITexture1D interface
	virtual uint32 GetWidth() const noexcept final;
	virtual uint32 GetMipLevels() const noexcept final;
	virtual COLOR_FORMAT GetFormat() const noexcept final;

	__FORCEINLINE uint32 INL_GetWidth() const noexcept { return m_Width; }
	__FORCEINLINE uint32 INL_GetMipLevels() const noexcept { return m_MipLevels; }
	__FORCEINLINE COLOR_FORMAT INL_GetFormat() const noexcept { return m_Format; }

	__FORCEINLINE ID3D11ShaderResourceView* INL_GetSRV() const noexcept { return m_pSRV; }
	__FORCEINLINE ID3D11Texture1D* INL_GetD3D11Texture1D() const noexcept { return m_pTex1D; }

private:
	D3D11Texture1D() = delete;
	virtual ~D3D11Texture1D() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	uint64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	COLOR_FORMAT m_Format = COLOR_FORMAT::UNKNOWN;

	uint32 m_Width = 0;
	uint32 m_MipLevels = 0;

	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11Texture1D* m_pTex1D = nullptr;

};



#endif // __D3D11_TEXTURE1D_H__
