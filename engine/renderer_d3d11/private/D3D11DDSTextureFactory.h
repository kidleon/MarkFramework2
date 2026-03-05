#pragma once
#include <d3d11.h>


class D3D11DDSTextureFactory final : public IDDSTextureFactory
{
public:
	D3D11DDSTextureFactory(ID3D11Device* pDevice);
	virtual ~D3D11DDSTextureFactory() noexcept;

	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	virtual BOOL CreateTextureDDS(
		const char* szFilePath,
		const void* pData,
		size_t DataSize,
		ITexture2D** ppOut
	) final;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	ID3D11Device* m_pDevice;

};
