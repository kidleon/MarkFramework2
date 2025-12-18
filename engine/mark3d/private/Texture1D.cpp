#include "pch.h"
#include "Texture1D.h"
#include "GlobalVars.h"

#include "idgen.h"
#include "ITexture1DProxy.h"


Texture1D::Texture1D(UINT32 ID)
	: m_pTexture1DProxy(nullptr)
{
	m_ID = ID;
	m_LoadStat = LOAD_STAT::NOT_LOADED;
}

Texture1D::~Texture1D() noexcept
{
	idgen_release(GLOBAL_VARS::ID_GEN_HANDLE, m_ID);
	m_ID = 0;

	if (m_pTexture1DProxy)
	{
		m_pTexture1DProxy->Release();
		m_pTexture1DProxy = nullptr;
	}
}

uint32 Texture1D::GetWidth() const noexcept
{
	return m_pTexture1DProxy->GetWidth();
}

uint32 Texture1D::GetMipLevels() const noexcept
{
	return m_pTexture1DProxy->GetMipLevels();
}

COLOR_FORMAT Texture1D::GetFormat() const noexcept
{
	return m_pTexture1DProxy->GetColorFormat();
}

void Texture1D::SetTexture1DProxy(ITexture1DProxy* pTexture1DProxy) noexcept
{
	m_pTexture1DProxy = pTexture1DProxy;
	m_LoadStat = LOAD_STAT::LOADED;
}