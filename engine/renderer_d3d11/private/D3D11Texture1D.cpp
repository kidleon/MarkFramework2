#include "pch.h"
#include "D3D11Texture1D.h"


D3D11Texture1D::~D3D11Texture1D() noexcept
{
	CHECK_RELEASE(m_pSRV);
	CHECK_RELEASE(m_pTex1D);
}

long D3D11Texture1D::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11Texture1D::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11Texture1D);
	}
	return NewRefCnt;
}

long D3D11Texture1D::RefCnt()
{
	return m_RefCnt;
}

uint64 D3D11Texture1D::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11Texture1D::GetAssetType() const noexcept
{
	return ASSET_TYPE::TEXTURE_1D;
}

LOAD_STAT D3D11Texture1D::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

TEXTURE_TYPE D3D11Texture1D::GetTextureType() const noexcept
{
	return TEXTURE_TYPE::TEX_1D;
}

uint32 D3D11Texture1D::GetWidth() const noexcept
{
	return m_Width;
}

uint32 D3D11Texture1D::GetMipLevels() const noexcept
{
	return m_MipLevels;
}

COLOR_FORMAT D3D11Texture1D::GetFormat() const noexcept
{
	return m_Format;
}
