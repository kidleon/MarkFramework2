#include "pch.h"
#include "D3D11Texture2DProxy.h"
#include "D3D11Texture2D.h"
#include "D3D11RenderDef.h"


D3D11Texture2DProxy::D3D11Texture2DProxy(D3D11Texture2D* pTexture1D)
	: m_pD3D11Texture2D(pTexture2D)
{
}

D3D11Texture2DProxy::~D3D11Texture2DProxy() noexcept
{
	m_pD3D11Texture2D = nullptr;
}

void D3D11Texture2DProxy::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11Texture2DProxy);
}

uint32 D3D11Texture2DProxy::GetWidth() const noexcept
{
	return m_pD3D11Texture2D->INL_GetWidth();
}

uint32 D3D11Texture2DProxy::GetHeight() const noexcept
{
	return m_pD3D11Texture2D->INL_GetHeight();
}

uint32 D3D11Texture2DProxy::GetMipLevels() const noexcept
{
	return m_pD3D11Texture2D->INL_GetMipLevels();
}

COLOR_FORMAT D3D11Texture2DProxy::GetColorFormat() const noexcept
{
	return static_cast<COLOR_FORMAT>(m_pD3D11Texture2D->INL_GetFormat());
}

void* D3D11Texture2DProxy::GetNativeTexture() const noexcept
{
	return static_cast<void*>(m_pD3D11Texture2D);
}

