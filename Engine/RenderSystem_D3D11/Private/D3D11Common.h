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
				uint64_t Magic : 8;		// 매직 넘버 (예: 0x11)
				uint64_t Type : 8;		// 리소스 타입 (예: 0: 하드웨어 버퍼, 1: 텍스처 등)
				uint64_t Version : 16;	// 리소스 버전 (업데이트 될 때마다 증가)
				uint64_t TableIndex : 32;	// 페이지 테이블 인덱스
			};

			uint64_t Id;
		};
	};

	enum class D3D11_RESOURCE_TYPE : uint32_t
	{
		PRIMITIVE_BUFFER = 0,

		EMAX
	};

	struct D3D11_PTR_TABLE_INDEX
	{
		union
		{
			struct
			{
				uint64_t Version : 16; // 슬롯 버전 (업데이트 될 때마다 증가)
				uint64_t Index : 32; // 슬롯 인덱스 (페이지당 256개 슬롯)
			};
			uint64_t Id;
		};
	};

	//--------------------------------------------------------------
	// D3D11HardwareBufferPool 관련 열거형 및 상수 정의
	enum class POOL_BUFFER_TYPE {
		VERTEX_BUFFER, INDEX_BUFFER, CONSTANT_BUFFER, EMAX
	};

	enum class POOL_BUFFER_USAGE {
		DEFAULT, DYNAMIC, STAGING, EMAX
	};

	enum class POOL_BUFFER_SIZE : uint8_t {
		SIZE_256, SIZE_512, SIZE_1024, SIZE_2048,
		SIZE_4096, SIZE_8192, SIZE_16384, SIZE_32768,
		SIZE_65536, SIZE_131072, SIZE_262144, SIZE_524288,
		SIZE_1048576, SIZE_2097152, EMAX
	};

	constexpr static size_t POOL_BUFFER_SIZES[] = {
		256, 512, 1024, 2048,
		4096, 8192, 16384, 32768,
		65536, 131072, 262144, 524288,
		1048576, 2097152
	};

	constexpr static size_t BUFFER_COUNT_PER_PAGE[] = {
		128, 128, 128, 128,
		64, 64, 64, 64,
		32, 32, 32, 32,
		16, 16
	};

}

