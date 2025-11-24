#include "pch.h"
#include <compress.h>
#include <MemoryManager.h>


int test_compress(); // 테스트용 함수 선언

int main()
{
#if defined(__TARGET_OS_WINDOWS)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // __TARGET_OS_WINDOWS

	if(!test_compress()) // 테스트 함수 호출
		printf("Compression and Decompression test passed.\n");
	else
		printf("Compression and Decompression test failed.\n");

	mark::MemoryManager::Initialize(10 * 1024 * 1024, TRUE); // 10MB 임시 메모리 풀 초기화


	mark::MemoryManager::Shutdown(); // 메모리 매니저 종료

    return 0;
}


// 테스트용 압축 및 해제 함수
int test_compress()
{
	char source[] = "This is a test string for compression and decompression using LZ4 algorithm.";
	size_t source_size = sizeof(source);
	size_t max_compressed_size = compress_size(source_size);
	char* compressed_data = new char[max_compressed_size];
	size_t compressed_size = compress(source, source_size, compressed_data, max_compressed_size);
	if (compressed_size == 0) {
		printf("Compression failed.\n");
		delete[] compressed_data;
		return 1;
	}

	char* decompressed_data = new char[source_size];
	size_t decompressed_size = decompress(compressed_data, compressed_size, decompressed_data, source_size);
	if (decompressed_size == 0) {
		printf("Decompression failed.\n");
		delete[] compressed_data;
		delete[] decompressed_data;
		return 1;
	}
	printf("Original Size: %zu\n", source_size);
	printf("Compressed Size: %zu\n", compressed_size);
	printf("Decompressed Size: %zu\n", decompressed_size);
	printf("Decompressed Data: %s\n", decompressed_data);
	delete[] compressed_data;
	delete[] decompressed_data;

	return 0;
}