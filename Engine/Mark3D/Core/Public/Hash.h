#pragma once


namespace mark
{
	/**
	* @brief 해시 함수를 제공하는 클래스입니다.
	*/
	class MARKENGINE_API hash
	{
	public:
		/**
		* @brief 주어진 데이터에 대해 CRC32 해시 값을 계산합니다.
		* @param data 해시를 계산할 데이터의 포인터입니다.
		* @param size 데이터의 크기(바이트 단위)입니다.
		* @param initial_value CRC32 계산을 시작할 초기값입니다. 기본값은 0xFFFFFFFF입니다.
		* @return 계산된 CRC32 해시 값입니다.
		*/
		static uint32_t crc32(const void* data, size_t size, uint32_t initial_value = 0xFFFFFFFFu) noexcept;

		/**
		* @brief 주어진 데이터에 대해 CRC64 해시 값을 계산합니다.
		* @param data 해시를 계산할 데이터의 포인터입니다.
		* @param size 데이터의 크기(바이트 단위)입니다.
		* @param initial_value CRC64 계산을 시작할 초기값입니다. 기본값은 0xFFFFFFFFFFFFFFFF입니다.
		* @return 계산된 CRC64 해시 값입니다.
		*/
		static uint64_t crc64(const void* data, size_t size, uint64_t initial_value = 0xFFFFFFFFFFFFFFFFu) noexcept;

		/**
		* @brief 주어진 데이터에 대해 FNV-1a 32비트 해시 값을 계산합니다.
		* @param data 해시를 계산할 데이터의 포인터입니다.
		* @param size 데이터의 크기(바이트 단위)입니다.
		* @return 계산된 FNV-1a 32비트 해시 값입니다.
		*/
		static uint32_t fnv32(const void* data, size_t size, uint32_t initial_value = 2166136261) noexcept;

		/**
		* @brief 주어진 데이터에 대해 FNV-1a 64비트 해시 값을 계산합니다.
		* @param data 해시를 계산할 데이터의 포인터입니다.
		* @param size 데이터의 크기(바이트 단위)입니다.
		* @return 계산된 FNV-1a 64비트 해시 값입니다.
		*/
		static uint64_t fnv64(const void* data, size_t size, uint64_t initial_value = 14695981039346656037) noexcept;

		/**
		* @brief 주어진 데이터에 대해 MurmurHash3 32비트 해시 값을 계산합니다.
		* @param data 해시를 계산할 데이터의 포인터입니다.
		* @param size 데이터의 크기(바이트 단위)입니다.
		*/
		static bool gen_guid(char* out_guid, size_t buffer_size) noexcept;

	};
}
