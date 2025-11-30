#include "pch.h"
#include "TAllocator.h"
#include "MemoryManager.h"
#include "TList.h"
#include "TQueue.h"
#include "TDeque.h"
#include "TStack.h"
#include "TArray.h"


namespace mark
{
	void* talloc_syscall(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	) 
	{
		return MemoryManager::Alloc_SysCall(size, file, line, func);
	}

	void* talloc_pool(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	)
	{
		return MemoryManager::AllocPool(size, file, line, func);
	}

	void* talloc_temp(
		MEM_SIZE size,
		const char* file,
		int line,
		const char* func
	)
	{
		return MemoryManager::AllocTemp(size);
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
		return MemoryManager::Realloc_Syscall(ptr, new_size, file, line, func);
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
		void* new_ptr = MemoryManager::AllocPool(new_size, file, line, func);
		if (!new_ptr)
			return nullptr;
		if (ptr)
		{
			memcpy(new_ptr, ptr, old_size);
			MemoryManager::FreePool(ptr);
		}
		return new_ptr;
	}

	void* trealloc_temp(
		void* ptr,
		MEM_SIZE new_size,
		const char* file,
		int line,
		const char* func
	)
	{
		void* new_ptr = MemoryManager::AllocTemp(new_size);
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
		MemoryManager::Free_SysCall(ptr);
	}

	void tfree_pool(
		void* ptr
	)
	{
		MemoryManager::FreePool(ptr);
	}

	void tfree_temp(
		void* ptr
	)
	{
		// Temp 방식은 개별 해제를 지원하지 않음
	}
}