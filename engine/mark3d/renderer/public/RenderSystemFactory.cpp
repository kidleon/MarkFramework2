#include "pch.h"
#include "RenderSystemFactory.h"


BOOL __stdcall CreateRenderSystem(
	const ENGINE_CREATE_DESC& CreateDesc,
	IRenderSystem** ppRenderSystem
)
{
	if(CreateDesc.RenderAPI == RENDER_API::D3D11)
	{
#if defined(__TARGET_OS_WINDOWS)
		typedef BOOL(__stdcall* PFN_CREATE_RENDERER_D3D11)(
			HWND,
			uint32,
			uint32,
			BOOL,
			IRenderSystem**
		);

		HMODULE hModule = LoadLibraryA("renderer_d3d11.dll");
		if(!hModule)
			return FALSE;

		PFN_CREATE_RENDERER_D3D11 pfnCreateRendererD3D11 = (PFN_CREATE_RENDERER_D3D11)GetProcAddress(hModule, "CreateRendererD3D11");
		if (!pfnCreateRendererD3D11)
		{
			FreeLibrary(hModule);
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
			FreeLibrary(hModule);
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