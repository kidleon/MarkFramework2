#ifndef __I_TEXTURE1D_PROXY_H__
#define __I_TEXTURE1D_PROXY_H__


interface ITexture1DProxy : public IUNKNOWN_IMPL
{
	virtual uint32 GetWidth() const noexcept = 0;
	virtual uint32 GetMipLevels() const noexcept = 0;
	virtual COLOR_FORMAT GetColorFormat() const noexcept = 0;

	virtual void* GetNativeTexture() const noexcept = 0;

};


#endif // __I_TEXTURE1D_PROXY_H__
