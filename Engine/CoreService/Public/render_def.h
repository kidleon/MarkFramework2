#pragma once
#include <cstdint>
#include "unknown.h"


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
	using PrimitiveBufferHandle = ResourceHandle;


	struct IRenderSystem : public Unknown
	{
		virtual bool Initialize(const RenderSystemCreateDesc& desc) = 0;
		virtual void Shutdown() = 0;

		virtual PrimitiveBufferHandle CreatePrimitiveBuffer(const PrimitiveBufferCreateDesc& desc) = 0;

	};
}
