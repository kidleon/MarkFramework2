#include "pch.h"
#include "RenderSystemFactory.h"
//#include "D3D11/D3D11RenderSystem.h"


BOOL __stdcall CreateRenderSystemInstance(IRenderSystem** ppOut)
{
	/*
#if defined(__MARK3D_RENDERSYSTEM_D3D11__)
	D3D11RenderSystem* pRenderSystem = new D3D11RenderSystem();
	if (!pRenderSystem)
	{
		SYS_LOG_E("CreateRenderSystem - Failed to create RenderSystem_D3D11 instance.");
		return FALSE;
	}

	*ppOut = pRenderSystem;

	return TRUE;
#else // __MARK3D_RENDERSYSTEM_D3D11__
	SYS_LOG_E("No supported render system is defined.");
	return FALSE;
#endif // __MARK3D_RENDERSYSTEM_D3D11__
*/
	return FALSE;
}