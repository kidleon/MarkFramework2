#include "pch.h"
#include "compress.h"
#include "lz4/lz4.h"


size_t compress(
	char* src_data,
	size_t src_size,
	char* dest_buffer,
	size_t dest_size
)
{
	if (src_size == 0 || dest_size == 0 || src_data == NULL || dest_buffer == NULL)
		return 0;

	if (src_size > LZ4_MAX_INPUT_SIZE)
		return 0;

	if (dest_size < LZ4_compressBound((int)src_size))
		return 0;

	int compressed_size = LZ4_compress_default(
		src_data, 
		dest_buffer, 
		(int)src_size,
		(int)dest_size
	);

	if (compressed_size <= 0)
		return 0;

	// Return the size of the compressed data
	return (size_t)compressed_size;
}

size_t decompress(
	char* src_data,
	size_t src_size,
	char* dest_buffer,
	size_t dest_size
)
{
	if (src_size == 0 || dest_size == 0 || src_data == NULL || dest_buffer == NULL)
		return 0;

	if (src_size > LZ4_MAX_INPUT_SIZE)
		return 0;

	if (dest_size < src_size)
		return 0;

	int decompressed_size = LZ4_decompress_safe(
		src_data,
		dest_buffer,
		(int)src_size,
		(int)dest_size
	);

	if (decompressed_size < 0)
		return 0;

	// Return the size of the decompressed data
	return (size_t)decompressed_size;
}

size_t compress_size(
	size_t src_size
)
{
	if (src_size > LZ4_MAX_INPUT_SIZE)
		return 0;

	return LZ4_compressBound((int)src_size);
}
