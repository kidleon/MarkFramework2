#include "pch.h"
#include "RenderSystemFactory.h"

/*
#if defined(__TARGET_OS_WINDOWS)
HMODULE g_hModule = nullptr;
#endif // defined(__TARGET_OS_WINDOWS)

BOOL __stdcall CreateAndInitRenderModule(
	const RENDERER_CREATE_DESC& CreateDesc,
	IRenderSystem** ppRenderSystem
)
{
	if(CreateDesc.RenderAPI == RENDER_API::D3D11)
	{
#if defined(__TARGET_OS_WINDOWS)
		if (g_hModule) return FALSE;

		typedef BOOL(__stdcall* PFN_CREATE_RENDERER_D3D11)(
			HWND,
			uint32,
			uint32,
			BOOL,
			IRenderSystem**
		);

#if defined(_DEBUG)
		g_hModule = LoadLibraryA(".\\renderer_d3d11_d.dll");
#else 
		g_hModule = LoadLibraryA(".\\renderer_d3d11.dll");
#endif // defined(_DEBUG)
		
		if(!g_hModule)
			return FALSE;

		PFN_CREATE_RENDERER_D3D11 pfnCreateRendererD3D11 = (PFN_CREATE_RENDERER_D3D11)GetProcAddress(g_hModule, "CreateRendererD3D11");
		if (!pfnCreateRendererD3D11)
		{
			FreeLibrary(g_hModule);
			return FALSE;
		}

		if (!pfnCreateRendererD3D11(
			CreateDesc.hWnd,
			CreateDesc.ScreenWidth,
			CreateDesc.ScreenHeight,
			CreateDesc.Fullscreen,
			ppRenderSystem
		))
		{
			FreeLibrary(g_hModule);
			return FALSE;
		}
#else
		return FALSE;
#endif // defined(_TARGET_OS_WINDOWS)
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void __stdcall CleanupRenderModule()
{
#if defined(__TARGET_OS_WINDOWS)
	if (g_hModule)
	{
		FreeLibrary(g_hModule);
		g_hModule = nullptr;
	}
#endif // defined(__TARGET_OS_WINDOWS)
}
*/
