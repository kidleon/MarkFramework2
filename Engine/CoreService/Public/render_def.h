#pragma once
#include <cstdint>


namespace mark
{
	struct RenderSystemCreateDesc
	{
		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;
	};

	struct PrimitiveBufferCreateDesc
	{
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
	};

	using ResourceHandle = uint64_t;
}
