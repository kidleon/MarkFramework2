#pragma once


namespace mark
{
	/**
	* @brief 데이터 압축 및 압축 해제를 위한 클래스입니다.
	*/
	class MARKENGINE_API compress
	{
	public:
		/**
		* @brief lz4 압축을 수행합니다.
		* @param src 압축할 데이터의 포인터
		* @param src_size 압축할 데이터의 사이즈
		* @param dst 압축된 데이터를 저장할 버퍼의 포인터
		* @param dst_size 압축된 데이터를 저장할 버퍼의 사이즈
		* @return 압축된 데이터의 사이즈를 반환합니다. 실패시 0을 반환합니다.
		*/
		static size_t compress_lz4(
			const void* src,
			size_t src_size,
			void* dst,
			size_t dst_size
		) noexcept;

		/**
		* @brief lz4 압축 해제를 수행합니다.
		* @param src 압축 해제할 데이터의 포인터
		* @param src_size 압축 해제할 데이터의 사이즈
		* @param dst 압축 해제된 데이터를 저장할 버퍼의 포인터
		* @param dst_size 압축 해제된 데이터를 저장할 버퍼의 사이즈
		* @return 압축 해제된 데이터의 사이즈를 반환합니다. 실패시 0을 반환합니다.
		*/
		static size_t uncompress_lz4(
			const void* src,
			size_t src_size,
			void* dst,
			size_t dst_size
		) noexcept;

		/**
		* @brief lz4로 압축된 데이터의 최대 사이즈 예측치를 반환합니다.
		* @param src_size 압축할 데이터의 사이즈
		* @return 압축된 데이터의 최대 사이즈를 반환합니다. 실패시 0을 반환합니다.
		*/
		[[nodiscard]] static size_t compress_size_lz4(size_t src_size) noexcept;
	};

}
