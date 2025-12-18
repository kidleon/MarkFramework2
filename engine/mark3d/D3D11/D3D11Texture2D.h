#ifndef __D3D11_TEXTURE2D_H__
#define __D3D11_TEXTURE2D_H__

class D3D11Texture2D
{
public:
	explicit D3D11Texture2D(
		uint32 Width,
		uint32 Height,
		uint32 MipLevels,
		DXGI_FORMAT Format,
		ID3D11Texture2D* pTexture,
		ID3D11ShaderResourceView* pSRV
	);

	virtual ~D3D11Texture2D() noexcept;

	__FORCEINLINE uint32 INL_GetWidth() const { return m_Width; }
	__FORCEINLINE uint32 INL_GetHeight() const { return m_Height; }
	__FORCEINLINE uint32 INL_GetMipLevels() const { return m_MipLevels; }
	__FORCEINLINE DXGI_FORMAT INL_GetFormat() const { return m_Format; }
	__FORCEINLINE ID3D11Texture2D* INL_GetD3D11Texture2D() const { return m_pD3D11Texture; }
	__FORCEINLINE ID3D11ShaderResourceView* INL_GetD3D11SRV() const { return m_pD3D11SRV; }

private:
	D3D11Texture2D() = delete;

private:
	uint32 m_Width;
	uint32 m_Height;
	uint32 m_MipLevels;
	DXGI_FORMAT m_Format;

	ID3D11Texture2D* m_pD3D11Texture;
	ID3D11ShaderResourceView* m_pD3D11SRV;
};

#endif // __D3D11_TEXTURE2D_H__
