#include "pch.h"
#include "D3D11Texture2D.h"


D3D11Texture2D::~D3D11Texture2D() noexcept
{
	CHECK_RELEASE(m_pSRV);
	CHECK_RELEASE(m_pTex2D);
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
		D3D11_POOL_DELETE(this, D3D11Texture2D);
	}
	return NewRefCnt;
}

long D3D11Texture2D::RefCnt()
{
	return m_RefCnt;
}

uint64 D3D11Texture2D::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11Texture2D::GetAssetType() const noexcept
{
	return ASSET_TYPE::TEXTURE_2D;
}

LOAD_STAT D3D11Texture2D::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

uint32 D3D11Texture2D::GetWidth() const noexcept
{
	return m_Width;
}

uint32 D3D11Texture2D::GetHeight() const noexcept
{
	return m_Height;
}

uint32 D3D11Texture2D::GetMipLevels() const noexcept
{
	return m_MipLevels;
}

COLOR_FORMAT D3D11Texture2D::GetFormat() const noexcept
{
	return m_Format;
}

