#pragma once
#include <cstdint>


namespace mark
{
	struct D3D11_RESOURCE_HANDLE
	{
		union
		{
			struct
			{
				uint64_t Magic : 8; // 매직 넘버 (예: 0x11)
				uint64_t Type : 8;
				uint64_t Page : 12;  // 페이지 번호 (0~4095)
				uint64_t Index : 12; // 페이지 내 인덱스 (0~4095)
				uint64_t Version : 8;
				uint64_t RefCnt : 16;
			};

			uint64_t Id;
		};
	};
}
