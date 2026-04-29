#pragma once
#include <cstdint>
#include "render_def.h"


namespace mark
{
	struct RenderDevice
	{
		ResourceHandle (*CreatePrimitiveBuffer)(const PrimitiveBufferCreateDesc& CreateDesc);
		void (*ReleaseHandle)(ResourceHandle Handle);
	};

	struct RenderSystem_D3D11
	{
		bool (*Initialize)(const RenderSystemCreateDesc& CreateDesc);
		void (*Shutdown)();

		RenderDevice* (*GetRenderDevice)();
	};

	struct D3D11_RESOURCE_HANDLE
	{
		union
		{
			struct
			{
				uint64_t TypeIndex : 8;		// 255
				uint64_t PageIndex : 12;	// 4095
				uint64_t SlotIndex : 12;	// 4095
				uint64_t Version : 10;		// 1023
				uint64_t RefCount : 18;		// 262143
				uint64_t Reserved : 4;		// 
			};

			uint64_t ID;
		};
	};
}
