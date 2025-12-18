#include "pch.h"
#include "D3D11Texture1DProxy.h"
#include "D3D11Texture1D.h"
#include "D3D11RenderDef.h"


D3D11Texture1DProxy::D3D11Texture1DProxy(D3D11Texture1D* pTexture1D)
	: m_pD3D11Texture1D(pTexture1D)
{
}

D3D11Texture1DProxy::~D3D11Texture1DProxy() noexcept
{
	m_pD3D11Texture1D = nullptr;
}

void D3D11Texture1DProxy::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11Texture1DProxy);
}

uint32 D3D11Texture1DProxy::GetWidth() const noexcept
{
	return m_pD3D11Texture1D->INL_GetWidth();
}

uint32 D3D11Texture1DProxy::GetMipLevels() const noexcept
{
	return m_pD3D11Texture1D->INL_GetMipLevels();
}

COLOR_FORMAT D3D11Texture1DProxy::GetColorFormat() const noexcept
{
	return static_cast<COLOR_FORMAT>(m_pD3D11Texture1D->INL_GetFormat());
}

void* D3D11Texture1DProxy::GetNativeTexture() const noexcept
{
	return static_cast<void*>(m_pD3D11Texture1D);
}