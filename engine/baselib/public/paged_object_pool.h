#ifndef __PAGED_OBJECT_POOL_H__
#define __PAGED_OBJECT_POOL_H__

/**
 * @brief Object Pool을 생성합니다. Object Pool은 고정된 사이즈의 객체를 관리하는데 사용됩니다.
 * @param block_size 객체의 크기를 지정합니다.
 * @param block_count 관리할 객체의 개수를 지정합니다.
 * @param thread_safe 멀티 스레드 환경에서 안전하게 동작하도록 설정합니다.
 * @return HANDLE 생성된 Object Pool의 핸들을 반환합니다. 실패시 NULL을 반환합니다.
 */
MARK_BASELIB_C_API HANDLE create_paged_object_pool(
	unsigned char pool_index,
	MEM_SIZE block_size,
	MEM_SIZE block_count_per_page,
	BOOL thread_safe
);

/**
 * @brief Object Pool을 파괴합니다.
 * @param pool_handle Object Pool의 핸들입니다.
 */
MARK_BASELIB_C_API void destroy_paged_object_pool(
	HANDLE pool_handle
);

/**
 * @brief Object Pool에서 객체를 할당합니다.
 * @param pool_handle Object Pool의 핸들입니다.
 * @param file 요청을 한 소스 코드 파일 이름(__FILE__), 사용하지 않을 경우 NULL
 * @param line 요청을 한 소스 코드의 라인 번호(__LINE__), 사용하지 않을 경우 0
 * @return void* 할당된 객체의 포인터를 반환합니다. 할당에 실패할 경우 NULL을 반환합니다.
 */
MARK_BASELIB_C_API void* paged_object_pool_alloc(
	HANDLE pool_handle
);

/**
 * @brief Object Pool에서 객체를 반환합니다.
 * @param pool_handle Object Pool의 핸들입니다.
 * @param ptr 반환할 객체의 포인터입니다. 반드시 할당한 핸들과 매칭되어야 합니다. 매칭이 안되면 힙 손상이 발생할 수 있습니다.
 */
MARK_BASELIB_C_API void paged_object_pool_free(
	HANDLE pool_handle,
	void* ptr
);

/**
 * @brief Object Pool의 총 크기를 반환합니다.
 * @param pool_handle Object Pool의 핸들입니다.
 * @return size_t Object Pool의 총 크기를 바이트 단위로 반환합니다.
 */
MARK_BASELIB_C_API size_t paged_object_pool_get_used_size(
	HANDLE pool_handle
);


#endif // __PAGED_OBJECT_POOL_H__
