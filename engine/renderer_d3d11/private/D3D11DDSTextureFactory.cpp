#include "pch.h"
#include "D3D11DDSTextureFactory.h"
#include "D3D11TextureLoader_DDS.h"


D3D11DDSTextureFactory::D3D11DDSTextureFactory(ID3D11Device* pDevice)
	: m_pDevice(pDevice)
{
	if (m_pDevice)
		m_pDevice->AddRef();
}

D3D11DDSTextureFactory::~D3D11DDSTextureFactory() noexcept
{
	CHECK_RELEASE(m_pDevice);
}

long D3D11DDSTextureFactory::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11DDSTextureFactory::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_DELETE(this, D3D11DDSTextureFactory);
	}
	return NewRefCnt;
}

long D3D11DDSTextureFactory::RefCnt()
{
	return m_RefCnt;
}


BOOL D3D11DDSTextureFactory::CreateTextureDDS(
	const char* szFilePath,
	const void* pData,
	size_t DataSize,
	ITexture2D** ppOut
)
{
	HRESULT hr = CreateDDSTextureFromMemory(
		m_pDevice,
		szFilePath,
		pData,
		DataSize,
		(ID3D11ShaderResourceView**)ppOut,
		false
	);
	
	return SUCCEEDED(hr);
}