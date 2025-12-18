#ifndef __D3D11_TEXTURE1D_H__
#define __D3D11_TEXTURE1D_H__

#include "Texture1D.h"


class D3D11Texture1D : public Texture1D
{
public:
	explicit D3D11Texture1D(
		uint32 Width,
		uint32 MipLevels,
		DXGI_FORMAT Format,
		ID3D11Texture1D* pTexture, 
		ID3D11ShaderResourceView* pSRV
	);

	virtual ~D3D11Texture1D() noexcept;

	__FORCEINLINE uint32 INL_GetWidth() const { return m_Width; }
	__FORCEINLINE uint32 INL_GetMipLevels() const { return m_MipLevels; }
	__FORCEINLINE DXGI_FORMAT INL_GetFormat() const { return m_Format; }
	__FORCEINLINE ID3D11Texture1D* INL_GetD3D11Texture1D() const { return m_pD3D11Texture; }
	__FORCEINLINE ID3D11ShaderResourceView* INL_GetD3D11SRV() const { return m_pD3D11SRV; }

private:
	D3D11Texture1D() = delete;

private:
	uint32 m_Width;
	uint32 m_MipLevels;
	DXGI_FORMAT m_Format;

	uint32 PADDING;

	ID3D11Texture1D* m_pD3D11Texture;
	ID3D11ShaderResourceView* m_pD3D11SRV;

};


#endif // __D3D11_TEXTURE1D_H__
