#include "pch.h"
#include "D3D11Texture2D.h"


D3D11Texture2D::D3D11Texture2D(
	uint32 Width,
	uint32 Height,
	uint32 MipLevels,
	DXGI_FORMAT Format,
	ID3D11Texture2D* pTexture,
	ID3D11ShaderResourceView* pSRV
)
	: m_pD3D11Texture(pTexture)
	, m_pD3D11SRV(pSRV)
	, m_Width(Width)
	, m_Height(Height)
	, m_MipLevels(MipLevels)
	, m_Format(Format)
{
}

D3D11Texture2D::~D3D11Texture2D() noexcept
{
	if (m_pD3D11SRV)
	{
		m_pD3D11SRV->Release();
		m_pD3D11SRV = nullptr;
	}

	if (m_pD3D11Texture)
	{
		m_pD3D11Texture->Release();
		m_pD3D11Texture = nullptr;
	}
}