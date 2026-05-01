#pragma once
#include <cstdint>
#include "unknown.h"


namespace mark
{
	enum class GraphicsAPI : uint8_t
	{
		D3D11,
		D3D12,
		Vulkan,
		Metal,
		GLES3
	};

	struct RenderSystemCreateDesc
	{
		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

#if defined(__TARGET_OS_WINDOWS)
		HWND WindowHandle = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)
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

		[[nodiscard]] virtual PrimitiveBufferHandle CreatePrimitiveBuffer(const PrimitiveBufferCreateDesc& desc) = 0;

	};
}
