#include "pch.h"
#include "RenderSystem.h"


#ifdef __TARGET_OS_WINDOWS
#   if defined(__MARK3D_RENDERSYSTEM_D3D11__)
#	    pragma comment(lib, "d3d11.lib")
#	    pragma comment(lib, "d3dcompiler.lib")
#	    pragma comment(lib, "dxguid.lib")
#   endif // defined(__MARK3D_RENDERSYSTEM_D3D11__)
#endif // __TARGET_OS_WINDOWS


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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

BOOL __stdcall CreateMark3D(
    HWND hWnd,
    uint32 ScreenWidth,
    uint32 ScreenHeight,
    int32 MinLogLevel,
    BOOL Fullscreen,
    IRenderSystem** ppRenderSystem)
{
    if (!ppRenderSystem)
        return FALSE;


    RenderSystem* pRenderSystem = new RenderSystem();
    if (!pRenderSystem->Initialize(
        hWnd,
        ScreenWidth,
        ScreenHeight,
        MinLogLevel,
        Fullscreen
    ))
    {
        return FALSE;
    }

    *ppRenderSystem = pRenderSystem;


    return TRUE;
}
