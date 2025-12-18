#ifndef __D3D11_TEXTURE1D_PROXY_H__
#define __D3D11_TEXTURE1D_PROXY_H__

#include "ITexture1DProxy.h"


class D3D11Texture1D;

class D3D11Texture1DProxy final : public ITexture1DProxy
{
public:
	D3D11Texture1DProxy(D3D11Texture1D* pTexture1D) noexcept;

	uint32 GetWidth() const noexcept final;
	uint32 GetMipLevels() const noexcept final;
	COLOR_FORMAT GetColorFormat() const noexcept final;

	void* GetNativeTexture() const noexcept final;

protected:
	virtual ~D3D11Texture1DProxy() noexcept;
	void OnDestroy() final;

private:
	D3D11Texture1D* m_pD3D11Texture1D;

};


#endif // __D3D11_TEXTURE1D_PROXY_H__
