#include "pch.h"
#include "TAllocator.h"
#include "Heap.h"
#include "TList.h"
#include "TQueue.h"
#include "TDeque.h"
#include "TStack.h"
#include "TArray.h"
#include "THashMap.h"

namespace mark
{
	void* talloc_syscall(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	) 
	{
#if defined(USE_PROFILE_MEMORY)
		return HeapAlloc2(size, file, line, func);
#else
		return HeapAlloc_(size);
#endif 
	}

	void* talloc_pool(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	)
	{
#if defined(USE_PROFILE_MEMORY)
		return PoolAlloc(size, file, line, func);
#else
		return PoolAlloc(size);
#endif // USE_PROFILE_MEMORY
	}

	void* talloc_temp(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	)
	{
		return TempAlloc(size);
	}

	void* trealloc_syscall(
		void* ptr,
		MEM_SIZE old_size,
		MEM_SIZE new_size,
		const char* file,
		int line,
		const char* func
	)
	{
#if defined(USE_PROFILE_MEMORY)
		return HeapRealloc(ptr, new_size, file, line, func);
#else
		return HeapRealloc(ptr, new_size);
#endif // USE_PROFILE_MEMORY
	}

	void* trealloc_pool(
		void* ptr,
		MEM_SIZE old_size,
		MEM_SIZE new_size,
		const char* file,
		int line,
		const char* func
	)
	{
#if defined(USE_PROFILE_MEMORY)
		void* new_ptr = PoolAlloc(new_size, file, line, func);
#else
		void* new_ptr = PoolAlloc(new_size);
#endif // USE_PROFILE_MEMORY
		
		if (!new_ptr)
			return nullptr;

		if (ptr)
		{
			memcpy(new_ptr, ptr, old_size);
			PoolFree(ptr);
		}
		return new_ptr;
	}

	void* trealloc_temp(
		void* ptr,
		MEM_SIZE new_size
	)
	{
#if defined(USE_PROFILE_MEMORY)
		void* new_ptr = TempAlloc(new_size);
#endif 
		void* new_ptr = TempAlloc(new_size);

		if (!new_ptr)
			return nullptr;
		if (ptr)
		{
			// Temp 방식은 기존 포인터를 해제하지 않음
		}

		return new_ptr;
	}

	void tfree_syscall(
		void* ptr
	)
	{
		HeapFree_(ptr);
	}

	void tfree_pool(
		void* ptr
	)
	{
		PoolFree(ptr);
	}

	void tfree_temp(
		void* ptr
	)
	{
		// Temp 방식은 개별 해제를 지원하지 않음
	}
}