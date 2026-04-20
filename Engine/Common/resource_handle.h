#pragma once
#include <cstdint>

namespace mark
{
	struct resource_handle
	{
		static constexpr uint64_t MAGIC_VALUE = 0xAA;

		union
		{
			struct fields
			{
				uint64_t magic : 8;			// 8비트 매직 넘버 (유효성 검사용)
				uint64_t type : 8;			// 8비트 리소스 타입 (예: 텍스처, 버퍼, 셰이더 등)
				uint64_t page : 16;			// 16비트 페이지 번호 
				uint64_t index : 16;		// 16비트 인덱스 (페이지 내 위치)
				uint64_t version : 16;		// 16비트 버전 (세대 번호)
			} fields;

			uint64_t value;
		};

		resource_handle() : value(0) { fields.magic = MAGIC_VALUE; }

		bool is_valid()  const noexcept { return value != 0 && fields.magic == MAGIC_VALUE; }
		bool is_null()   const noexcept { return value == 0; }
		bool operator==(const resource_handle& o) const noexcept { return value == o.value; }
		bool operator!=(const resource_handle& o) const noexcept { return value != o.value; }
		bool operator< (const resource_handle& o) const noexcept { return value < o.value; }
	};


}
