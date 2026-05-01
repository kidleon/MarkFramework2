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

	enum class BUFFER_USAGE : uint32_t
	{
		DEFAULT,
		IMMUTABLE,
		DYNAMIC,
		STAGING,

		EMAX
	};

	enum class BUFFER_TYPE : uint32_t
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER,

		EMAX
	};

	struct RenderSystemCreateDesc
	{
		uint32_t ScreenWidth;
		uint32_t ScreenHeight;

#if defined(__TARGET_OS_WINDOWS)
		HWND WindowHandle;
#endif // #if defined(__TARGET_OS_WINDOWS)
	};

	struct PrimitiveBufferCreateDesc
	{
		BUFFER_USAGE VertexBufferUsage;
		BUFFER_USAGE IndexBufferUsage;
		uint32_t VertexCount;
		uint32_t IndexCount;
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
