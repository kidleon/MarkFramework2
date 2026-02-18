#include "pch.h"
#include "D3D11RenderTarget.h"


D3D11RenderTarget::D3D11RenderTarget(
	UINT32 ColorWidth,
	UINT32 ColorHeight,
	COLOR_FORMAT ColorFormat,
	ID3D11Texture2D* pD3D11ColorTexture,
	ID3D11RenderTargetView* pD3D11RTV
) noexcept
	: m_ColorWidth(ColorWidth)
	, m_ColorHeight(ColorHeight)
	, m_ColorFormat(ColorFormat)
	, m_HasDepthStencil(FALSE)
	, m_DepthStencilWidth(0)
	, m_DepthStencilHeight(0)
	, m_DepthStencilFormat(COLOR_FORMAT::UNKNOWN)
	, m_pD3D11ColorTexture(pD3D11ColorTexture)
	, m_pD3D11RTV(pD3D11RTV)
	, m_pD3D11DepthStencilTexutre(nullptr)
	, m_pD3D11DSV(nullptr)
{
}

D3D11RenderTarget::D3D11RenderTarget(
	UINT32 ColorWidth,
	UINT32 ColorHeight,
	COLOR_FORMAT ColorFormat,
	UINT32 DepthStencilWidth,
	UINT32 DepthStencilHeight,
	COLOR_FORMAT DepthStencilFormat,
	ID3D11Texture2D* pD3D11ColorTexture,
	ID3D11RenderTargetView* pD3D11RTV,
	ID3D11Texture2D* pD3D11DepthStencilTexture,
	ID3D11DepthStencilView* pD3D11DSV
) noexcept
	: m_ColorWidth(ColorWidth)
	, m_ColorHeight(ColorHeight)
	, m_ColorFormat(ColorFormat)
	, m_HasDepthStencil(pD3D11DepthStencilTexture ? TRUE : FALSE)
	, m_DepthStencilWidth(DepthStencilWidth)
	, m_DepthStencilHeight(DepthStencilHeight)
	, m_DepthStencilFormat(DepthStencilFormat)
	, m_pD3D11ColorTexture(pD3D11ColorTexture)
	, m_pD3D11RTV(pD3D11RTV)
	, m_pD3D11DepthStencilTexutre(pD3D11DepthStencilTexture)
	, m_pD3D11DSV(pD3D11DSV)
{
}

D3D11RenderTarget::~D3D11RenderTarget()
{
	CHECK_RELEASE(m_pD3D11DSV);
	CHECK_RELEASE(m_pD3D11DepthStencilTexutre);
	CHECK_RELEASE(m_pD3D11RTV);
	CHECK_RELEASE(m_pD3D11ColorTexture);
}

long D3D11RenderTarget::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11RenderTarget::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11RenderTarget);
	}
	return NewRefCnt;
}

long D3D11RenderTarget::RefCnt()
{
	return m_RefCnt;
}

UINT32 D3D11RenderTarget::GetColorWidth() const noexcept
{
	return INL_GetColorWidth();
}

UINT32 D3D11RenderTarget::GetColorHeight() const noexcept
{
	return INL_GetColorHeight();
}

COLOR_FORMAT D3D11RenderTarget::GetColorFormat() const noexcept
{
	return INL_GetColorFormat();
}

BOOL D3D11RenderTarget::HasDepthStencil() const noexcept
{
	return INL_HasDepthStencil();
}

UINT32 D3D11RenderTarget::GetDepthWidth() const noexcept
{
	return INL_GetDepthWidth();
}

UINT32 D3D11RenderTarget::GetDepthHeight() const noexcept
{
	return INL_GetDepthHeight();
}

COLOR_FORMAT D3D11RenderTarget::GetDepthFormat() const noexcept
{
	return INL_GetDepthFormat();
}
