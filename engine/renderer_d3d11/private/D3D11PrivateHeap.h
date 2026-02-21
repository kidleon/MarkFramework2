#ifndef __D3D11_PRIVATE_HEAP_H__
#define __D3D11_PRIVATE_HEAP_H__


// Core Heap API
extern "C"
{
	BOOL D3D11Heap_Init(
		size_t TempSize,
		void (*pfnMemoryReporter)(
			const char* type,
			const char* file,
			int line,
			const char* func,
			size_t size
			)
	);

	void D3D11Heap_Shutdown();
	HANDLE D3D11Heap_GetHandle();
	void D3D11Heap_ReportLeaks();

	void* D3D11Heap_SysAlloc(size_t size, const char* file, int line, const char* func);
	void* D3D11Heap_SysRealloc(void* ptr, size_t newSize, const char* file, int line, const char* func);
	void D3D11Heap_SysFree(void* ptr);

	void* D3D11Heap_SysAllocAlign(size_t size, size_t alignment, const char* file, int line, const char* func);
	void D3D11Heap_SysFreeAlign(void* ptr);

	void* D3D11Heap_PoolAlloc(size_t size, const char* file, int line, const char* func);
	void* D3D11Heap_PoolRealloc(void* ptr, size_t oldSize, size_t newSize, const char* file, int line, const char* func);
	void D3D11Heap_PoolFree(void* ptr);

	//void* D3D11Heap_TempAlloc(size_t size);
	//void D3D11Heap_TempReset();
}

#define D3D11_SYS_ALLOC(size) D3D11Heap_SysAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#define D3D11_SYS_REALLOC(ptr, size) D3D11Heap_SysRealloc(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#define D3D11_SYS_FREE(ptr) D3D11Heap_SysFree(ptr)

#define D3D11_SYS_ALLOC_ALGIN(size, alignment) D3D11Heap_SysAllocAlign(size, alignment, __FILE__, __LINE__, __FUNCTION__)
#define D3D11_SYS_FREE_ALIGN(ptr) D3D11Heap_SysFreeAlign(ptr)

#define D3D11_POOL_ALLOC(size) D3D11Heap_PoolAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#define D3D11_POOL_FREE(ptr) D3D11Heap_PoolFree(ptr)


#define D3D11_NEW(type) new (D3D11_SYS_ALLOC(sizeof(type))) type
#define D3D11_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); D3D11_SYS_FREE(p); p = nullptr; } }
#define D3D11_POOL_NEW(type) new (D3D11_POOL_ALLOC(sizeof(type))) type
#define D3D11_POOL_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); D3D11_POOL_FREE(p); p = nullptr; } }


template<ALLOC_TYPE _AllocType>
struct TD3D11Allocator
{
	static inline void* alloc(size_t size, const char* file, int line, const char* func)
	{
		if constexpr (ALLOC_TYPE::SYSCALL == _AllocType)
		{
			return D3D11Heap_SysAlloc(size, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::POOL == _AllocType)
		{
			return D3D11Heap_PoolAlloc(size, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::TEMP == _AllocType)
		{
			return D3D11Heap_TempAlloc(size);
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
			return D3D11Heap_SysRealloc(ptr, newSize, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::POOL == _AllocType)
		{
			return D3D11Heap_PoolRealloc(ptr, oldSize, newSize, file, line, func);
		}
		else if constexpr (ALLOC_TYPE::TEMP == _AllocType)
		{
			void* pNewPtr = D3D11Heap_TempAlloc(newSize);
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
			D3D11Heap_SysFree(ptr);
		}
		else if constexpr (ALLOC_TYPE::POOL == _AllocType)
		{
			D3D11Heap_PoolFree(ptr);
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

using TA_POOL = TD3D11Allocator<ALLOC_TYPE::POOL>;
using TA_SYSCALL = TD3D11Allocator<ALLOC_TYPE::SYSCALL>;



#endif // __D3D11_PRIVATE_HEAP_H__
