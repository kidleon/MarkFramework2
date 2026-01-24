#pragma once
#include "TArray.h"
using namespace mark;

#include "D3D11RenderQueue.h"
#include "D3D11ResourceCommand.h"


struct D3D11_RENDER_FRAME
{
	static constexpr size_t MAX_RQ_GROUPS = 32;

	size_t NumRQs = 0;
	D3D11_RENDER_QUEUE_GROUP RQs[MAX_RQ_GROUPS];

	TArray<D3D11_RESOURCE_COMMAND*, TA_POOL> ResourceCommands;

	void Reset();
	void SortByCameraOrder();
};