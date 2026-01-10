#pragma once
#include "TArray.h"
using namespace mark;

#include "D3D11RenderQueue.h"


struct D3D11_RENDER_FRAME
{
	static constexpr size_t MAX_RQ_GROUPS = 32;

	size_t NumRQs = 0;
	D3D11_RENDER_QUEUE_GROUP RQs[MAX_RQ_GROUPS];

	void Reset();
	void SortByCameraOrder();
};