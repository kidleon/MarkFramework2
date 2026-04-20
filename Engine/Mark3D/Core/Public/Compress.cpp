#include "pch.h"
#include "Compress.h"
#include "lz4/lz4.h"


namespace mark
{
	size_t compress::compress_lz4(
		const void* src,
		size_t src_size,
		void* dst,
		size_t dst_size
	) noexcept
	{
		if (0 == src_size || 0 == dst_size || nullptr == src || nullptr == dst)
			return 0;

		if (LZ4_MAX_INPUT_SIZE < src_size)
			return 0;

		if (dst_size < LZ4_compressBound((int)src_size))
			return 0;

		int compressed_size = LZ4_compress_default(
			(const char*)src,
			(char*)dst,
			(int)src_size,
			(int)dst_size
		);

		if (compressed_size <= 0)
			return 0;

		// Return the size of the compressed data
		return (size_t)compressed_size;
	}

	size_t compress::uncompress_lz4(
		const void* src,
		size_t src_size,
		void* dst,
		size_t dst_size
	) noexcept
	{
		if (0 == src_size || 0 == dst_size || nullptr == src || nullptr == dst)
			return 0;

		if (LZ4_MAX_INPUT_SIZE < src_size)
			return 0;

		// 용량이 작을 경우 오히려 커지는 경우가 있을 수 있으므로,
		// dst_size가 src_size보다 작은 경우에도 0을 반환하지 않도록 수정
		//if (dst_size < src_size)
		//	return 0;

		int decompressed_size = LZ4_decompress_safe(
			(const char*)src,
			(char*)dst,
			(int)src_size,
			(int)dst_size
		);

		if (decompressed_size < 0)
			return 0;

		// Return the size of the decompressed data
		return (size_t)decompressed_size;
	}

	size_t compress::compress_size_lz4(size_t src_size) noexcept
	{
		if (0 == src_size)
			return 0;

		if (LZ4_MAX_INPUT_SIZE < src_size)
			return 0;

		return (size_t)LZ4_compressBound((int)src_size);
	}
}
