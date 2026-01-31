#include "pch.h"
#include "D3D11RenderFrame.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderQueue.h"
#include "stack_pool.h"


void D3D11_RENDER_FRAME::Init()
{
	IndexerStackPool = stackpool_create(1048 * 1024, FALSE); // 1MB
	CommandStackPool = stackpool_create(2096 * 1024, FALSE); // 2MB
	init_linked_list(&ResourceCommandQueue);
	RenderSortIndexer.Init();
}

void D3D11_RENDER_FRAME::Destroy()
{
	if (IndexerStackPool)
	{
		stackpool_destroy(IndexerStackPool);
		IndexerStackPool = nullptr;
	}

	if (CommandStackPool)
	{
		stackpool_destroy(CommandStackPool);
		CommandStackPool = nullptr;
	}

	RenderSortIndexer.Destroy();
}

void D3D11_RENDER_FRAME::Reset()
{
	for (size_t i = 0; i < NumRQs; ++i)
		RQs[i].Reset();
	NumRQs = 0;

	ResourceCommandQueue.head = nullptr;
	ResourceCommandQueue.tail = nullptr;
	ResourceCommandQueue.size = 0;

	stackpool_reset(IndexerStackPool);
	stackpool_reset(CommandStackPool);

	RenderSortIndexer.Reset();
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