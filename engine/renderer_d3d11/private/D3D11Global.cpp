#include "pch.h"
#include "D3D11Global.h"


HANDLE D3D11Global::s_hIDGen = nullptr;
D3D11RenderDevice* D3D11Global::s_pRenderDevice = nullptr;


void D3D11Global::Init() noexcept
{
	if (!s_hIDGen)
		s_hIDGen = idgen_create(0x01, 0xFFFFAAAAu);
}

void D3D11Global::Shutdown() noexcept
{
	if (s_hIDGen)
	{
		idgen_destroy(s_hIDGen);
		s_hIDGen = nullptr;
	}
}