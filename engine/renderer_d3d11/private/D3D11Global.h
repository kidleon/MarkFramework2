#ifndef __D3D11_MAIN_H__
#define __D3D11_MAIN_H__


class D3D11RenderDevice;

class D3D11Global
{
public:
	static inline D3D11RenderDevice* GetRenderDevice() noexcept
	{
		return s_pRenderDevice;
	}

	static D3D11RenderDevice* s_pRenderDevice;

};


#endif // __D3D11_MAIN_H__
