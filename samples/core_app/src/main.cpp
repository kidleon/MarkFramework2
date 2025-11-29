#include "pch.h"
#include <compress.h>
#include <MemoryManager.h>
#include <hash_table.h>


int test_hashtable(); // 해시 테이블 테스트 함수 선언
int test_compress(); // 테스트용 함수 선언
int test_memorymgr(); // 메모리 매니저 테스트 함수 선언


int main()
{
#if defined(__TARGET_OS_WINDOWS)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // __TARGET_OS_WINDOWS

	test_hashtable(); // 해시 테이블 테스트 함수 호출

	if(!test_compress()) // 테스트 함수 호출
		printf("Compression and Decompression test passed.\n");
	else
		printf("Compression and Decompression test failed.\n");

	if (!test_memorymgr()) // 메모리 매니저 테스트 함수 호출
		printf("Memory Manager test passed.\n");
	else
		printf("Memory Manager test failed.\n");

    return 0;
}


// 해시 테이블 테스트용 엔티티 구조체
struct hash_entity_t
{
	UINT32 key;
	char value[32];
	HASH_NODE node;
};

// 해시 테이블 테스트 함수 정의
int test_hashtable()
{
	HASH_TABLE* hash_table = create_hash_table(128); // 버킷 크기 128로 해시 테이블 생성
	
	// 해시 테이블 테스트 코드 작성
	hash_entity_t* entity = (hash_entity_t*)MARK_ALLOC_SYSCALL(sizeof(hash_entity_t));
	memset(entity, 0, sizeof(hash_entity_t));

	entity->key = 1;
	entity->node.data = entity;
	entity->value[0] = 1;
	entity->value[1] = 2;
	entity->value[2] = 3;
	entity->value[3] = 4;

	insert_hash_node(hash_table, entity->key, &entity->node);

	hash_entity_t* entity2 = (hash_entity_t*)MARK_ALLOC_SYSCALL(sizeof(hash_entity_t));
	memset(entity2, 0, sizeof(hash_entity_t));

	entity2->key = 129;
	entity2->node.data = entity2;
	entity2->value[0] = 1;
	entity2->value[1] = 2;
	entity2->value[2] = 3;
	entity2->value[3] = 4;

	insert_hash_node(hash_table, entity2->key, &entity2->node);

	hash_entity_t* find_entity = (hash_entity_t*)query_hash_node(hash_table, 129);
	if (find_entity)
	{
		printf("Found entity with key %u: value = [%d, %d, %d, %d]\n",
			find_entity->key,
			(int)find_entity->value[0],
			(int)find_entity->value[1],
			(int)find_entity->value[2],
			(int)find_entity->value[3]);
	}
	else
	{
		printf("Entity with key 1 not found.\n");
	}

	delete_hash_node(hash_table, 1);
	delete_hash_node(hash_table, 129);

	MARK_FREE_SYSCALL(entity);
	MARK_FREE_SYSCALL(entity2);

	delete_hash_table(hash_table);

	return 0;
}

static void ReportMemoryLeaks(
	const char* type,
	const char* file,
	int line,
	const char* func,
	size_t size
)
{
	printf("Memory Leak Detected: Type=%s, File=%s, Line=%d, Function=%s, Size=%zu bytes\n",
			type, file, line, func, size);
}

// 메모리 매니저 테스트 함수 정의
int test_memorymgr()
{
	mark::MemoryManager::Initialize(
		MEM_SIZE(10 * 1024 * 1024),
		TRUE,
		ReportMemoryLeaks
	); // 10MB 임시 메모리 풀 초기화

	// 메모리 할당 테스트
	void* ptr = MARK_ALLOC_SYSCALL(256); // 256바이트 할당
	if (ptr == nullptr) {
		printf("Memory allocation failed.\n");
		return 1;
	}

	mark::MemoryStats memStats = {};
	mark::MemoryManager::GetMemoryStats(&memStats); // 메모리 통계 정보 가져오기
	
	printf("---------------------------------------------\n");
	printf("Memory Stats:\n");
	printf("  Syscall Alloc Count: %llu\n", memStats.sys_alloc_count);
	printf("  Syscall Alloc Size: %llu bytes\n", memStats.sys_alloc_size);
	printf("  Pool Alloc Count: %llu\n", memStats.pool_alloc_count);
	printf("  Pool Alloc Size: %llu bytes\n", memStats.pool_alloc_size);
	printf("  Temp Peak Alloc Count: %llu bytes\n", memStats.peak_temp_count);
	printf("  Temp Peak Alloc Size: %llu bytes\n", memStats.peak_temp_size);
	printf("---------------------------------------------\n");
	mark::MemoryManager::ReportMemoryLeaks(); // 메모리 사용 통계 보고

	MARK_FREE_SYSCALL(ptr); // 메모리 해제

	mark::MemoryManager::Shutdown(); // 메모리 매니저 종료

	return 0;
}

// 테스트용 압축 및 해제 함수
int test_compress()
{
	char source[] = "This is a test string for compression and decompression using LZ4 algorithm.";
	size_t source_size = sizeof(source);
	size_t max_compressed_size = compress_size_lz4(source_size);
	char* compressed_data = new char[max_compressed_size];
	size_t compressed_size = compress_lz4(source, source_size, compressed_data, max_compressed_size);
	if (compressed_size == 0) {
		printf("Compression failed.\n");
		delete[] compressed_data;
		return 1;
	}

	char* decompressed_data = new char[source_size];
	size_t decompressed_size = decompress_lz4(compressed_data, compressed_size, decompressed_data, source_size);
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