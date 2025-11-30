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
	* @brief 템플릿 시스템 메모리 할당 함수
	* @param size 할당할 메모리 크기
	* @param file 할당 요청이 발생한 소스 파일 이름
	* @param line 할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 할당 요청이 발생한 함수 이름
	* @return 할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* talloc_syscall(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 풀 메모리 할당 함수
	* @param size 할당할 메모리 크기
	* @param file 할당 요청이 발생한 소스 파일 이름
	* @param line 할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 할당 요청이 발생한 함수 이름
	* @return 할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* talloc_pool(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 임시 메모리 할당 함수
	* @param size 할당할 메모리 크기
	* @param file 할당 요청이 발생한 소스 파일 이름
	* @param line 할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 할당 요청이 발생한 함수 이름
	* @return 할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* talloc_temp(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 시스템 메모리 재할당 함수
	* @param ptr 재할당할 메모리의 기존 포인터
	* @param old_size 재할당 전 메모리 크기
	* @param new_size 재할당할 메모리 크기
	* @param file 재할당 요청이 발생한 소스 파일 이름
	* @param line 재할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 재할당 요청이 발생한 함수 이름
	* @return 재할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* trealloc_syscall(
		void* ptr,
		MEM_SIZE old_size,
		MEM_SIZE new_size,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 풀 메모리 재할당 함수
	* @param ptr 재할당할 메모리의 기존 포인터
	* @param old_size 재할당 전 메모리 크기	
	* @param new_size 재할당할 메모리 크기
	* @param file 재할당 요청이 발생한 소스 파일 이름
	* @param line 재할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 재할당 요청이 발생한 함수 이름
	* @return 재할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* trealloc_pool(
		void* ptr,
		MEM_SIZE old_size,
		MEM_SIZE new_size,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 임시 메모리 재할당 함수
	* @param ptr 재할당할 메모리의 기존 포인터
	* @param new_size 재할당할 메모리 크기
	* @param file 재할당 요청이 발생한 소스 파일 이름
	* @param line 재할당 요청이 발생한 소스 파일의 라인 번호
	* @param func 재할당 요청이 발생한 함수 이름
	* @return 재할당된 메모리의 포인터, 실패시 nullptr
	*/
	MARKENGINE_C_API void* trealloc_temp(
		void* ptr,
		MEM_SIZE new_size,
		const char* file,
		int line,
		const char* func
	);

	/**
	* @brief 템플릿 시스템 메모리 해제 함수
	* @param ptr 해제할 메모리의 포인터
	* @return 없음
	*/
	MARKENGINE_C_API void tfree_syscall(
		void* ptr
	);

	/**
	* @brief 템플릿 풀 메모리 해제 함수
	* @param ptr 해제할 메모리의 포인터
	* @return 없음
	*/
	MARKENGINE_C_API void tfree_pool(
		void* ptr
	);

	/**
	* @brief 템플릿 임시 메모리 해제 함수
	* @param ptr 해제할 메모리의 포인터
	* @return 없음
	*/
	MARKENGINE_C_API void tfree_temp(
		void* ptr
	);


	/**
	* TAlloc_Syscall
	* @brief 템플릿 기반의 메모리 할당기. 시스템 콜, 풀, 임시 메모리 할당을 지원합니다. 
	* constexpr를 사용하여 컴파일 타임에 할당 유형을 결정합니다.
	* @tparam _AllocType 할당 유형 (SYSCALL, POOL, TEMP)
	*/
	template<ALLOC_TYPE _AllocType>
	struct TAlloc_Syscall
	{
		static inline void* alloc(MEM_SIZE size, const char* file, int line, const char* func)
		{
			if constexpr(ALLOC_TYPE::SYSCALL == _AllocType)
			{
				return mark::talloc(size, file, line, func);
			}
			else if constexpr(ALLOC_TYPE::POOL == _AllocType)
			{
				return mark::talloc(size, file, line, func);
			}
			else if constexpr(ALLOC_TYPE::TEMP = _AllocType)
			{
				return mark::talloc(size, file, line, func);
			}

			static_assert(false, "Unsupported Alloc Type");
		}

		static inline void* realloc(void* ptr, MEM_SIZE old_size, MEM_SIZE new_size, const char* file, int line, const char* func)
		{
			if constexpr(ALLOC_TYPE::SYSCALL == _AllocType)
			{
				return mark::trealloc(ptr, old_size, new_size, file, line, func);
			}
			else if constexpr(ALLOC_TYPE::POOL == _AllocType)
			{
				return mark::trealloc(ptr, old_size, new_size, file, line, func);
			}
			else if constexpr(ALLOC_TYPE::TEMP = _AllocType)
			{
				return mark::trealloc(ptr, new_size, file, line, func);
			}

			static_assert(false, "Unsupported Alloc Type");
		}

		static inline void free(void* ptr)
		{
			if constexpr(ALLOC_TYPE::SYSCALL == _AllocType)
			{
				mark::tfree_syscall(ptr);
			}
			else if constexpr(ALLOC_TYPE::POOL == _AllocType)
			{
				mark::tfree_pool(ptr);
			}
			else if constexpr(ALLOC_TYPE::TEMP = _AllocType)
			{
				mark::tfree_temp(ptr);
			}

			static_assert(false, "Unsupported Alloc Type");
		}
	};
}

#	define GENERIC_ALLOC(size, alloc_type) mark::TAlloc_Syscall<alloc_type>::alloc(size, __FILE__, __LINE__, __FUNCTION__)
#	define GENERIC_REALLOC(ptr, old_size, new_size, alloc_type) mark::TAlloc_Syscall<alloc_type>::realloc(ptr, old_size, new_size, __FILE__, __LINE__, __FUNCTION__)
#	define GENERIC_FREE(ptr, alloc_type) mark::TAlloc_Syscall<alloc_type>::free(ptr)

#endif // __GENERIC_ALLOCATOR_H__
