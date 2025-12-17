#ifndef __D3D11_TEXTURE1D_H__
#define __D3D11_TEXTURE1D_H__

#include "Texture1D.h"


class D3D11Texture1D : public Texture1D
{
public:
	D3D11Texture1D();
	virtual ~D3D11Texture1D() noexcept;

	virtual uint32 GetWidth() const noexcept override;
	
	virtual uint32 GetMipLevels() const noexcept override;
};


#endif // __D3D11_TEXTURE1D_H__
