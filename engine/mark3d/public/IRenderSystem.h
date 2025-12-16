#ifndef __IRENDER_SYSTEM_H__
#define __IRENDER_SYSTEM_H__


interface ITexture1D;
interface ITexture2D;
interface ISurfaceMaterial;

interface IRenderSystem : IUNKNOWN_IMPL
{
public:
	virtual BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	) = 0;

	virtual void Shutdown() = 0;

	virtual BOOL CreateMaterial(ISurfaceMaterial** ppMaterial) = 0;
	virtual void ReleaseMaterial(ISurfaceMaterial* pMaterial) = 0;

	virtual BOOL CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture) = 0;
	virtual BOOL CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture) = 0;

};

#endif // __IRENDER_SYSTEM_H__