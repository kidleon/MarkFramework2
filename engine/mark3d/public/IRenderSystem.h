#ifndef __IRENDER_SYSTEM_H__
#define __IRENDER_SYSTEM_H__


interface IRenderSystem : IUNKNOWN
{
public:
	virtual BOOL Initialize(
		HWND hWnd,
		uint32 width,
		uint32 height,
		BOOL fullscreen
	) = 0;
	virtual void Shutdown() = 0;
};

#endif // __IRENDER_SYSTEM_H__