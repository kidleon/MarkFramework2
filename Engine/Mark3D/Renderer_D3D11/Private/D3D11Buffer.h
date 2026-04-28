#pragma once
#include <d3d11.h>


namespace mark
{
	struct ResourceId
	{
		union
		{
			struct
			{
				uint32_t PageIndex : 16;
				uint32_t SlotIndex : 16;
			};
			uint32_t Id;
		};
	};

	struct D3D11Buffer
	{
		ResourceId ResourceId;
		uint16_t Type;
		uint16_t Usage;

		ID3D11Buffer* pBuffer = nullptr;
		uint32_t Capacity = 0;
		uint32_t UsedBytes = 0;
	};

	
}
