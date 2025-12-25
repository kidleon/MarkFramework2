#ifndef __VIRTUAL_FRFELIST_H__
#define __VIRTUAL_FRFELIST_H__

/**
* @brief 자유 목록 가상 할당자
* @note 해당 freelist는 buddy 할당자 기반으로 동작합니다.
* @note size는 내부에서 2의 제곱수로 올림 처리됩니다.
* @note 실제로 메모리가 할당되지는 않으며, offset 값만 관리합니다.
*/

/**
* @brief 자유 목록 핸들 생성
* @param size 할당자에서 관리할 전체 크기
* @param min_block_size 최소 블록 크기
* @return 생성된 핸들, 실패 시 NULL 반환
*/
MARK_BASELIB_C_API HANDLE vfreelist_create(
	size_t size, 
	size_t min_block_size
);

/**
* @brief 자유 목록 핸들 파괴
* @param hFreelist 파괴할 핸들
* @return 없음
*/
MARK_BASELIB_C_API void vfreelist_destroy(
	HANDLE hFreelist
);

/**
* @brief 자유 목록에서 메모리 블록 할당
* @param hFreelist 할당자 핸들
* @param size 할당할 블록 크기
*/
MARK_BASELIB_C_API uint32 vfreelist_alloc(
	HANDLE hFreelist,
	size_t size
);

/**
* @brief 자유 목록에 메모리 블록 해제
* @param hFreelist 할당자 핸들
* @param offset 해제할 블록의 오프셋
* @return 없음
*/
MARK_BASELIB_C_API void vfreelist_free(
	HANDLE hFreelist,
	uint64 offset
);


#endif // __VIRTUAL_FRFELIST_H__
