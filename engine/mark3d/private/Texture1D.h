#ifndef __TEXTURE1D_H__
#define __TEXTURE1D_H__

#include "ITexture1D.h"


interface ITexture1DProxy;

class Texture1D final : public ITexture1D
{
public:
	Texture1D(UINT32 ID);
	virtual ~Texture1D() noexcept;


	uint32 GetWidth() const noexcept final;

	uint32 GetMipLevels() const noexcept final;

	COLOR_FORMAT GetFormat() const noexcept final;

	void SetTexture1DProxy(ITexture1DProxy* pTexture1DProxy) noexcept;

private:
	ITexture1DProxy* m_pTexture1DProxy;

};


#endif // __TEXTURE1D_H__