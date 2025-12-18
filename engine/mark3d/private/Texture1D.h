#ifndef __TEXTURE1D_H__
#define __TEXTURE1D_H__

#include "ITexture1D.h"


class Texture1D : public ITexture1D
{
public:
	Texture1D(UINT32 ID);
	virtual ~Texture1D() noexcept;


private:
#if defined(__MARK3D_RENDERSYSTEM_D3D11__)
	D3D11Texture1D* m_pImpl_D3D11;
#endif // defined(__MARK3D_RENDERSYSTEM_D3D11__)

};


#endif // __TEXTURE1D_H__