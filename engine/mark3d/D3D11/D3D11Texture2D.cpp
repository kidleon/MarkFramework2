#include "pch.h"
#include "D3D11Texture2D.h"


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

long D3D11Texture2D::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11Texture2D::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		MARK_POOL_DELETE(this, D3D11Texture2D);
	}
	return NewRefCnt;
}

long D3D11Texture2D::RefCnt()
{
	return m_RefCnt;
}

UINT32 D3D11Texture2D::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11Texture2D::GetAssetType() const noexcept
{
	return ASSET_TYPE::TEXTURE2D;
}

LOAD_STAT D3D11Texture2D::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

uint32 D3D11Texture2D::GetWidth() const noexcept
{
	return INL_GetWidth();
}

uint32 D3D11Texture2D::GetHeight() const noexcept
{
	return INL_GetHeight();
}

uint32 D3D11Texture2D::GetMipLevels() const noexcept
{
	return INL_GetMipLevels();
}

COLOR_FORMAT D3D11Texture2D::GetFormat() const noexcept
{
	return INL_GetFormat();
}

void  D3D11Texture2D::SetData(
	uint32 Width,
	uint32 Height,
	uint32 MipLevels,
	COLOR_FORMAT Format,
	ID3D11Texture2D* pTexture2D,
	ID3D11ShaderResourceView* pSRV
) noexcept
{
	m_Width = Width;
	m_Height = Height;
	m_MipLevels = MipLevels;
	m_Format = Format;
	m_pD3D11Texture = pTexture2D;
	m_pD3D11SRV = pSRV;

	interlock_store_l((long*)&m_LoadStat, (long)LOAD_STAT::LOADED, MEMORY_ORDER_ACQ_REL);
}