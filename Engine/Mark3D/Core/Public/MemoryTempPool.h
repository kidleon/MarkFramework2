#pragma once


namespace mark
{
	/**
	* @brief  TEMP 메모리 할당자를 생성.
	* @param size TEMP 메모리 할당자가 관리할 총 메모리 크기 (바이트 단위)
	* @return TEMP 메모리 할당자 핸들. 실패시 nullptr 반환
	*/
	MARKENGINE_API HANDLE temppool_create(size_t size);

	/**
	* @brief  TEMP 메모리 할당자를 파괴.
	* @param temp_alloc_handle 파괴할 TEMP 메모리 할당자 핸들.
	*/
	MARKENGINE_API void temppool_destroy(HANDLE temp_alloc_handle);

	/**
	* @brief  TEMP 메모리 할당자에서 메모리를 할당.
	* @param temp_alloc_handle 메모리를 할당할 TEMP 메모리 할당자 핸들.
	* @param size 할당할 메모리 크기 (바이트 단위)
	* @return 할당된 메모리의 포인터. 할당에 실패하면 nullptr 반환.
	*/
	MARKENGINE_API void* temppool_alloc(
		HANDLE temp_alloc_handle,
		size_t size
	);

	/**
	* @brief  TEMP 메모리 할당자에서 할당된 모든 메모리를 리셋
	* @param temp_alloc_handle 리셋할 TEMP 메모리 할당자 핸들.
	*/
	MARKENGINE_API void temppool_reset(HANDLE temp_alloc_handle);
}
