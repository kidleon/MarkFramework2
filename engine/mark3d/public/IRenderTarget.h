#ifndef __I_RENDER_TARGET_H__
#define __I_RENDER_TARGET_H__

#include "IAsset.h"


interface IRenderTarget : public IAsset
{
public:
	virtual UINT32 GetColorWidth() const noexcept = 0;
	virtual UINT32 GetColorHeight() const noexcept = 0;
	virtual COLOR_FORMAT GetColorFormat() const noexcept = 0;

	virtual BOOL HasDepthStencil() const noexcept = 0;
	virtual UINT32 GetDepthWidth() const noexcept = 0;
	virtual UINT32 GetDepthHeight() const noexcept = 0;
	virtual COLOR_FORMAT GetDepthFormat() const noexcept = 0;

};


#endif // __I_RENDER_TARGET_H__
