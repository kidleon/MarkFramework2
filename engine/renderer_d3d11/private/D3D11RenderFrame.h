#pragma once
#include "TArray.h"
using namespace mark;


class D3D11RenderQueue;

struct D3D11_RENDER_FRAME
{
	TArray<D3D11RenderQueue*, TA_POOL> OpaqueRQs;
	TArray<D3D11RenderQueue*, TA_POOL> TransparentRQs;
	/*
	TArray<D3D11RenderQueue*, TA_POOL> ShadowRQs;
	TArray<D3D11RenderQueue*, TA_POOL> SpriteRQs;
	TArray<D3D11RenderQueue*, TA_POOL> UIRQs;
	TArray<D3D11RenderQueue*, TA_POOL> OverrayRQs;
	*/
};