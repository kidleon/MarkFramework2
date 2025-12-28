#ifndef __CORE_HEAP_H__
#define __CORE_HEAP_H__


// Core Heap API
extern "C"
{
	BOOL CoreHeap_Init(
		size_t TempSize,
		void (*pfnMemoryReporter)(
			const char* type,
			const char* file,
			int line,
			const char* func,
			size_t size
		)
	);

	void CoreHeap_Shutdown();
	HANDLE CoreHeap_GetHandle();

	void* CoreHeap_SysAlloc(size_t size, const char* file, int line, const char* func);
	void* CoreHeap_SysRealloc(void* ptr, size_t newSize, const char* file, int line, const char* func);
	void CoreHeap_SysFree(void* ptr);

	void* CoreHeap_SysAllocAlign(size_t size, size_t alignment, const char* file, int line, const char* func);
	void CoreHeap_SysFreeAlign(void* ptr);

	void* CoreHeap_PoolAlloc(size_t size, const char* file, int line, const char* func);
	void* CoreHeap_PoolRealloc(void* ptr, size_t oldSize, size_t newSize, const char* file, int line, const char* func);
	void CoreHeap_PoolFree(void* ptr);

	void* CoreHeap_TempAlloc(size_t size);
	void CoreHeap_TempReset();
}

#define CORE_SYS_ALLOC(size) CoreHeap_SysAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#define CORE_SYS_REALLOC(ptr, size) CoreHeap_SysRealloc(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#define CORE_SYS_FREE(ptr) CoreHeap_SysFree(ptr)

#define CORE_SYS_ALLOC_ALGIN(size, alignment) CoreHeap_SysAllocAlign(size, alignment, __FILE__, __LINE__, __FUNCTION__)
#define CORE_SYS_FREE_ALIGN(ptr) CoreHeap_SysFreeAlign(ptr)

#define CORE_POOL_ALLOC(size) CoreHeap_PoolAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#define CORE_POOL_FREE(ptr) CoreHeap_PoolFree(ptr)

#define CORE_TEMP_ALLOC(size) CoreHeap_TempAlloc(size)
#define CORE_TEMP_RESET() CoreHeap_TempReset()

#define CORE_NEW(type) new (CORE_SYS_ALLOC(sizeof(type))) type
#define CORE_NEW_ARRAY(type, count) new (CORE_SYS_ALLOC(sizeof(type) * (count))) type[count]
#define CORE_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); CORE_SYS_FREE(p); p = nullptr; } }

#define CORE_POOL_NEW(type) new (CORE_POOL_ALLOC(sizeof(type))) type
#define CORE_POOL_NEW_ARRAY(type, count) new (CORE_POOL_ALLOC(sizeof(type) * (count))) type[count]
#define CORE_POOL_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); CORE_POOL_FREE(p); p = nullptr; } }


template<ALLOC_TYPE _AllocType>
struct TCoreAllocator
{
	static inline void* alloc(size_t size, const char* file, int line, const char* func)
	{
		if constexpr (ALLOC_TYPE::SYSCALL == _AllocType)
		{
			return CoreHeap_SysAlloc(size, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::POOL == _AllocType)
		{
			return CoreHeap_PoolAlloc(size, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::TEMP == _AllocType)
		{
			return CoreHeap_TempAlloc(size);
		}
		else
		{
			static_assert(false, "Unsupported Alloc Type");
		}
	}

	static inline void* realloc(void* ptr, size_t oldSize, size_t newSize, const char* file, int line, const char* func)
	{
		if constexpr (ALLOC_TYPE::SYSCALL == _AllocType)
		{
			return CoreHeap_SysRealloc(ptr, newSize, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::POOL == _AllocType)
		{
			return CoreHeap_PoolRealloc(ptr, oldSize, newSize, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::TEMP == _AllocType)
		{
			void* pNewPtr = CoreHeap_TempAlloc(newSize);
			if (!pNewPtr)
				return nullptr;
			memcpy(pNewPtr, ptr, newSize);

			return pNewPtr;
		}
		else
		{
			static_assert(false, "Unsupported Alloc Type");
		}
	}

	static inline void free(void* ptr)
	{
		if constexpr (ALLOC_TYPE::SYSCALL == _AllocType)
		{
			CoreHeap_SysFree(ptr);
		}
		else if constexpr (ALLOC_TYPE::POOL == _AllocType)
		{
			CoreHeap_PoolFree(ptr);
		}
		else if constexpr (ALLOC_TYPE::TEMP == _AllocType)
		{
			// Temp 할당은 해제하지 않음
		}
		else
		{
			static_assert(false, "Unsupported Alloc Type");
		}
	}
};




	


#endif // __CORE_HEAP_H__
