#ifndef __D3D11_MAIN_H__
#define __D3D11_MAIN_H__


class D3D11RenderDevice;

class D3D11Global
{
public:
	static void Init() noexcept;
	static void Shutdown() noexcept;

	static inline D3D11RenderDevice* GetRenderDevice() noexcept
	{
		return s_pRenderDevice;
	}

	static inline uint32 GetUID() noexcept
	{
		return idgen_getid(s_hIDGen);
	}

	static inline void ReleaseUID(uint32 ID) noexcept
	{
		idgen_release(s_hIDGen, ID);
	}

	static HANDLE s_hIDGen;
	static D3D11RenderDevice* s_pRenderDevice;

};


#endif // __D3D11_MAIN_H__
