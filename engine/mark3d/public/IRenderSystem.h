#ifndef __IRENDER_SYSTEM_H__
#define __IRENDER_SYSTEM_H__


interface ITexture1D;
interface ITexture2D;
interface ISurfaceMaterial;

interface IRenderSystem : public IUNKNOWN
{
public:
	virtual BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		int32 MinLogLevel,
		BOOL Fullscreen
	) = 0;

	virtual void Shutdown() = 0;

};

#endif // __IRENDER_SYSTEM_H__