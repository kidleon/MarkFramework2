#pragma once


namespace mark
{
	/**
	* @brief 코어 힙 메모리를 생성한다. 이 힙은 시스템메모리, 동기화 풀 메모리, 비동기화 풀 메모리, 임시 버퍼를 관리하는 역할을 한다.
	* Mark3D 외부에서 사용하기 위한 API들이며 Mark3D 내부에서는 PrivateMemory.h의 내부 함수들을 직접 호출하여 사용한다.
	* @param limited_memory_size 힙이 사용할 수 있는 최대 메모리 크기
	* @param sync_pool_count_per_chunk 동기화 풀의 청크당 블록 수
	* @param unsync_pool_count_per_chunk 비동기화 풀의 청크당 블록 수
	* @param sync_pool_max_size_per_block 동기화 풀의 블록당 최대 크기
	* @param unsync_pool_max_size_per_block 비동기화 풀의 블록당 최대 크기
	* @param temp_buffer_size 임시 버퍼의 크기
	* @return 생성된 코어 힙의 핸들
	*/
	[[nodiscard]] MARKENGINE_API HANDLE coreheap_create(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	);

	/**
	* @brief 코어 힙 메모리를 해제한다. 이 함수는 coreheap_create로 생성된 힙을 완전히 해제하고 관련된 모든 리소스를 정리한다.
	* @param heap_handle 해제할 코어 힙의 핸들
	* @return 없음
	*/
	MARKENGINE_API void coreheap_destroy(
		HANDLE heap_handle
	);

#if defined(__MEMORY_TRACKING_ENABLED__)
	[[nodiscard]] MARKENGINE_API void* mark_sys_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] MARKENGINE_API void* mark_spool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] MARKENGINE_API void* mark_upool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] MARKENGINE_API void* mark_temp_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] MARKENGINE_API void* coreheap_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	[[nodiscard]] MARKENGINE_API void* coreheap_spool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	[[nodiscard]] MARKENGINE_API void* coreheap_upool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	[[nodiscard]] MARKENGINE_API void* coreheap_temp_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	[[nodiscard]] MARKENGINE_API HANDLE coreheap_temppool_create(
		HANDLE heap_handle,
		size_t size,
		std::source_location loc
	);

#else
	/**
	* @brief 시스템 메모리를 할당한다. 이 함수는 PrivateMemory의 내부 함수로 직접 할당한다.
	* Mark3D 외부에서 Global한 메모리 할당에 사용한다.
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* mark_sys_alloc(
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 동기화 풀 메모리를 할당한다. 이 함수는 PrivateMemory의 내부 함수로 직접 할당한다.
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* mark_spool_alloc(
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 동기화되지 않는 풀 메모리를 할당한다. 이 함수는 PrivateMemory의 내부 함수로 직접 할당한다.
	* 동기화가 되지 않기 때문에 쓰레드에서 사용해서는 안된다. 동기화 풀보다는 속도가 더 빠르다.
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* mark_upool_alloc(
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 임시 버퍼 메모리를 할당한다. 이 함수는 PrivateMemory의 내부 함수로 직접 할당한다.
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* mark_temp_alloc(
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 메모리를 할당한다. 이 함수는 coreheap_create로 생성된 힙에서 메모리를 할당한다.
	* @param heap_handle 메모리를 할당할 코어 힙의 핸들
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* coreheap_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 동기화 풀 메모리를 할당한다. 이 함수는 coreheap_create로 생성된 힙에서 동기화 풀 메모리를 할당한다.
	* @param heap_handle 메모리를 할당할 코어 힙의 핸들
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* coreheap_spool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 동기화되지 않는 풀 메모리를 할당한다. 이 함수는 coreheap_create로 생성된 힙에서 동기화되지 않는 풀 메모리를 할당한다.
	* @param heap_handle 메모리를 할당할 코어 힙의 핸들
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* coreheap_upool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 임시 버퍼 메모리를 할당한다. 이 함수는 coreheap_create로 생성된 힙에서 임시 버퍼 메모리를 할당한다.
	* @param heap_handle 메모리를 할당할 코어 힙의 핸들
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* coreheap_temp_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 임시 풀을 생성한다. 이 함수는 coreheap_create로 생성된 힙에서 임시 풀을 생성한다.
	* @param heap_handle 임시 풀을 생성할 코어 힙의 핸들
	* @param size 임시 풀의 크기 (바이트 단위)
	* @return 생성된 임시 풀의 핸들, 생성 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API HANDLE coreheap_temppool_create(
		HANDLE heap_handle,
		size_t size
	);
#endif // __MEMORY_TRACKING_ENABLED__

	/**
	* @brief 시스템 메모리를 해제한다. 이 함수는 PrivateMemory의 내부 함수로 직접 해제한다.
	* @param ptr 해제할 메모리의 포인터
	* @param alignment 해제할 메모리의 정렬 (바이트 단위)
	* @return 없음
	*/
	MARKENGINE_API void mark_sys_free(
		void* ptr,
		size_t alignment
	);

	/**
	* @brief 동기화 풀 메모리를 해제한다. 이 함수는 PrivateMemory의 내부 함수로 직접 해제한다.
	* @param ptr 해제할 메모리의 포인터
	* @param alignment 해제할 메모리의 정렬 (바이트 단위)
	* @return 없음
	*/
	MARKENGINE_API void mark_spool_free(
		void* ptr,
		size_t alignment
	);

	/**
	* @brief 동기화되지 않는 풀 메모리를 해제한다. 이 함수는 PrivateMemory의 내부 함수로 직접 해제한다.
	* @param ptr 해제할 메모리의 포인터
	* @param alignment 해제할 메모리의 정렬 (바이트 단위)
	* @return 없음
	*/
	MARKENGINE_API void mark_upool_free(
		void* ptr,
		size_t alignment
	);

	/**
	* @brief 임시 버퍼 메모리를 해제한다. 이 함수는 PrivateMemory의 내부 함수로 직접 해제한다.
	* @param ptr 해제할 메모리의 포인터
	* @param alignment 해제할 메모리의 정렬 (바이트 단위)
	* @return 없음
	*/
	MARKENGINE_API void coreheap_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 동기화 풀 메모리를 해제한다. 이 함수는 coreheap_create로 생성된 힙에서 동기화 풀 메모리를 해제한다.
	* @param heap_handle 메모리를 해제할 코어 힙의 핸들
	* @param ptr 해제할 메모리의 포인터
	* @param alignment 해제할 메모리의 정렬 (바이트 단위)
	* @return 없음
	*/
	MARKENGINE_API void coreheap_spool_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 동기화되지 않는 풀 메모리를 해제한다. 이 함수는 coreheap_create로 생성된 힙에서 동기화되지 않는 풀 메모리를 해제한다.
	* @param heap_handle 메모리를 해제할 코어 힙의 핸들
	* @param ptr 해제할 메모리의 포인터
	* @param alignment 해제할 메모리의 정렬 (바이트 단위)
	* @return 없음
	*/
	MARKENGINE_API void coreheap_upool_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	);

	/**
	* @brief 코어 힙에서 임시 버퍼 메모리를 초기화 한다. Offset을 0으로 세팅한다.
	* @param heap_handle 메모리를 초기화할 코어 힙의 핸들
	* @return 없음
	*/
	MARKENGINE_API void coreheap_temp_reset(
		HANDLE heap_handle
	);

	/**
	* @brief 코어힙으로 부터 생성한 임시 풀에서 메모리를 할당한다. 이 함수는 coreheap_temppool_create로 생성된 임시 풀에서 메모리를 할당한다.
	* @param temppool_handle 메모리를 할당할 임시 풀의 핸들
	* @param bytes 할당할 메모리의 크기 (바이트 단위)
	* @param alignment 할당할 메모리의 정렬 (바이트 단위)
	* @return 할당된 메모리의 포인터, 할당 실패 시 nullptr
	*/
	[[nodiscard]] MARKENGINE_API void* coreheap_temppool_alloc(
		HANDLE temppool_handle,
		size_t bytes,
		size_t alignment
	);

	/**
	* @brief 코어힙으로 부터 생성한 임시 풀에서 메모리를 초기화 한다. Offset을 0으로 세팅한다.
	* @param temppool_handle 메모리를 초기화할 임시 풀의 핸들
	* @return 없음
	*/
	MARKENGINE_API void coreheap_temppool_reset(
		HANDLE temppool_handle
	);

	/**
	* @brief 코어힙으로 부터 생성한 임시 풀을 해제한다. 이 함수는 coreheap_temppool_create로 생성된 임시 풀을 해제한다.
	* @param heap_handle 임시 풀을 해제할 코어 힙의 핸들
	* @param temppool_handle 해제할 임시 풀의 핸들
	* @return 없음
	*/
	MARKENGINE_API void coreheap_temppool_destroy(
		HANDLE heap_handle,
		HANDLE temppool_handle
	);
}
