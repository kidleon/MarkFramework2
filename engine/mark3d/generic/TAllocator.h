#ifndef __GENERIC_ALLOCATOR_H__
#define __GENERIC_ALLOCATOR_H__


namespace mark
{
	enum class ALLOC_TYPE : unsigned
	{
		SYSCALL,
		POOL,
		TEMP,
	};

	/**
	* @brief 템플릿 메모리 할당 함수
	* @param size 할당할 메모리 크기
	* @param alloc_type 할당 유형
	* @param file 할당 요청이 발생한 소스 파일 이름
	* @param line 할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 할당 요청이 발생한 함수 이름
	* @return 할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* talloc(
		MEM_SIZE size,
		ALLOC_TYPE alloc_type,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 메모리 재할당 함수
	* @param ptr 재할당할 메모리의 기존 포인터
	* @param old_size 재할당 전 메모리 크기
	* @param new_size 재할당할 메모리 크기
	* @param alloc_type 할당 유형
	* @param file 재할당 요청이 발생한 소스 파일 이름
	* @param line 재할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 재할당 요청이 발생한 함수 이름
	* @return 재할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* trealloc(
		void* ptr,
		MEM_SIZE old_size,
		MEM_SIZE new_size,
		ALLOC_TYPE alloc_type,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 메모리 해제 함수
	* @param ptr 해제할 메모리의 포인터
	* @param alloc_type 할당 유형
	* @return 없음
	*/
	MARKENGINE_C_API void tfree(
		void* ptr,
		ALLOC_TYPE alloc_type
	);
}

#	define GENERIC_ALLOC(size, alloc_type) mark::talloc(size, alloc_type, __FILE__, __LINE__, __FUNCTION__)
#	define GENERIC_REALLOC(ptr, old_size, new_size, alloc_type) mark::trealloc(ptr, old_size, new_size, alloc_type, __FILE__, __LINE__, __FUNCTION__)
#	define GENERIC_FREE(ptr, alloc_type) mark::tfree(ptr, alloc_type)

#endif // __GENERIC_ALLOCATOR_H__
