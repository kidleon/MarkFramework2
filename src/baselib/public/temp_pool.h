#ifndef __TEMP_POOL_H__
#define __TEMP_POOL_H__


/**
 * @brief Temp Pool을 생성합니다. Temp Pool은 고정된 크기의 블록을 관리하는데 사용됩니다.
 * @param pool_size 미리 할당할 전체 풀 사이즈
 * @param thread_safe 멀티 스레드 환경에서 안전하게 동작하도록 설정합니다.
 * @return HANDLE 생성된 Temp Pool의 핸들을 반환합니다. 실패시 NULL을 반환합니다.
 */
MARKENGINE_C_API HANDLE create_temp_pool(
	size_t pool_size,
	BOOL thread_safe
);

/**
 * @brief Temp Pool을 파괴합니다.
 * @param pool_handle Temp Pool의 핸들입니다.
 */
MARKENGINE_C_API void destroy_temp_pool(
	HANDLE pool_handle
);

/**
 * @brief Temp Pool에서 블록을 할당합니다.
 * @param pool_handle Temp Pool의 핸들입니다.
 * @return void* 할당된 블록의 포인터를 반환합니다. 할당에 실패할 경우 NULL을 반환합니다.
 */
MARKENGINE_C_API void* temp_pool_alloc(
	HANDLE pool_handle,
	size_t alloc_size
);


/**
 * @brief Temp Pool을 초기화 합니다.
 * @param pool_handle Temp Pool의 핸들입니다.
 */ 
MARKENGINE_C_API void temp_pool_clear(
	HANDLE pool_handle
);




#endif // __TEMP_POOL_H__
