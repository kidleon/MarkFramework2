#ifndef __D3D11_TEXTURE1D_H__
#define __D3D11_TEXTURE1D_H__

#include "ITexture1D.h"


struct D3D11Texture1DImpl;

class D3D11Texture1D final : public ITexture1D
{
public:
	uint32 GetWidth() const noexcept final;
	uint32 GetMipLevels() const noexcept final;
	COLOR_FORMAT GetFormat() const noexcept final;

	void SetData(
		uint32 Width, 
		uint32 MipLevels, 
		COLOR_FORMAT Format, 
		ID3D11Texture1D* pTexture1D, 
		ID3D11ShaderResourceView* pSRV
	) noexcept;

	__FORCEINLINE uint32 INL_GetWidth() const { return m_Width; }
	__FORCEINLINE uint32 INL_GetMipLevels() const { return m_MipLevels; }
	__FORCEINLINE COLOR_FORMAT INL_GetFormat() const { return m_Format; }
	__FORCEINLINE ID3D11Texture1D* INL_GetD3D11Texture1D() const { return m_pD3D11Texture; }
	__FORCEINLINE ID3D11ShaderResourceView* INL_GetD3D11SRV() const { return m_pD3D11SRV; }

private:
	D3D11Texture1D() = delete;
	virtual ~D3D11Texture1D() noexcept;
	void OnDestroy() noexcept final;

private:
	uint32 m_Width = 0;
	uint32 m_MipLevels = 0;
	COLOR_FORMAT m_Format = COLOR_FORMAT::UNKNOWN;

	uint32 PADDING = 0;

	ID3D11Texture1D* m_pD3D11Texture = nullptr;
	ID3D11ShaderResourceView* m_pD3D11SRV = nullptr;

};


#endif // __D3D11_TEXTURE1D_H__
