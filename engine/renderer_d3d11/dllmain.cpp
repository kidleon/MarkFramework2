#include "pch.h"
#include "D3D11RenderSystem.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

BOOL APIENTRY DllMain(HMODULE g_hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            OutputDebugStringA("START RENDER_SYSTEM_D3D11 MODULE\n");
        } break;
        case DLL_THREAD_ATTACH:
        {

        }
        break;
        case DLL_THREAD_DETACH:
        {

        }
        break;

        case DLL_PROCESS_DETACH:
        {
            OutputDebugStringA("TERMINATED RENDER_SYSTEM_D3D11 MODULE\n");
        } break;
    }
    return TRUE;
}

BOOL __stdcall CreateRendererD3D11(
    HWND hWnd,
    uint32 ScreenWidth,
    uint32 ScreenHeight,
    BOOL Fullscreen,
    IRenderSystem** ppRenderSystem
)
{
    if (!ppRenderSystem)
        return FALSE;

    D3D11RenderSystem* pRenderSystem = new D3D11RenderSystem();
    if (!pRenderSystem->Initialize(
        hWnd,
        ScreenWidth,
        ScreenHeight,
        Fullscreen
    ))
    {
        return FALSE;
    }

    *ppRenderSystem = pRenderSystem;

    return TRUE;
}
