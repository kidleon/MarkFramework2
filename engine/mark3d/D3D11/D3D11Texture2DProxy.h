#ifndef __D3D11_TEXTURE2D_PROXY_H__
#define __D3D11_TEXTURE2D_PROXY_H__

#include "ITexture2DProxy.h"


class D3D11Texture2D;

class D3D11Texture2DProxy final : public ITexture2DProxy
{
public:
	D3D11Texture2DProxy(D3D11Texture2D* pTexture2D) noexcept;

	uint32 GetWidth() const noexcept final;
	uint32 GetHeight() const noexcept final;
	uint32 GetMipLevels() const noexcept final;
	COLOR_FORMAT GetColorFormat() const noexcept final;

	void* GetNativeTexture() const noexcept final;

protected:
	virtual ~D3D11Texture2DProxy() noexcept;
	void OnDestroy() final;

private:
	D3D11Texture2D* m_pD3D11Texture2D;

};



#endif // __D3D11_TEXTURE2D_PROXY_H__