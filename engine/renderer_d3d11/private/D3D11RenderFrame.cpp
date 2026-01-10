#include "pch.h"
#include "D3D11RenderFrame.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderQueue.h"



void D3D11_RENDER_FRAME::Reset()
{
	for (size_t i = 0; i < NumRQs; ++i)
	{
		RQs[i].Reset();
	}
}

void D3D11_RENDER_FRAME::SortByCameraOrder()
{
	std::sort(
		RQs,
		RQs + NumRQs,
		[](const D3D11_RENDER_QUEUE_GROUP& a, const D3D11_RENDER_QUEUE_GROUP& b) noexcept
		{
			const D3D11RenderCamera* pCameraA = a.INL_GetRenderCamera();
			const D3D11RenderCamera* pCameraB = b.INL_GetRenderCamera();
			
			if (pCameraA && pCameraB)
			{
				return pCameraA->INL_GetCameraOrder() < pCameraB->INL_GetCameraOrder();
			}
			else if (pCameraA && !pCameraB)
			{
				return true;
			}
			else if (!pCameraA && pCameraB)
			{
				return false;
			}
			else
			{
				return false;
			}
		}
	);
	
}