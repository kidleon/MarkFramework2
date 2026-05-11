#include "pch.h"
#include "D3D11HardwareGraphicsLayer.h"


using namespace mark;

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

BOOL __stdcall CreateHardwareGraphicsLayer(
	const RenderSystemCreateDesc& CreateDesc,
	mark::IHardwareGraphicsLayer** ppHardwareGraphicsLayer
)
{
	if (!ppHardwareGraphicsLayer)
		return FALSE;

	D3D11HardwareGraphicsLayer* pHardwareGraphicsLayer = CORE_NEW(D3D11HardwareGraphicsLayer);
	if (!pHardwareGraphicsLayer->Initialize(CreateDesc))
	{
		(*ppHardwareGraphicsLayer) = nullptr;
		pHardwareGraphicsLayer->Release();

		return FALSE;
	}

	*ppHardwareGraphicsLayer = pHardwareGraphicsLayer;

	return TRUE;
}
