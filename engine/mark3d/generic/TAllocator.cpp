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
	void* talloc(
		MEM_SIZE size,
		ALLOC_TYPE alloc_type,
		const char* file,
		int line,
		const char* func
	) 
	{
		switch (alloc_type)
		{
			case ALLOC_TYPE::SYSCALL:
				return MemoryManager::Alloc_SysCall(size, file, line, func);

			case ALLOC_TYPE::POOL:
				return MemoryManager::AllocPool(size, file, line, func);

			case ALLOC_TYPE::TEMP:
				return MemoryManager::AllocTemp(size);
		}

		return nullptr;
	}

	void* trealloc(
		void* ptr,
		MEM_SIZE old_size,
		MEM_SIZE new_size,
		ALLOC_TYPE alloc_type,
		const char* file,
		int line,
		const char* func
	)
	{
		switch (alloc_type)
		{
			case ALLOC_TYPE::SYSCALL:
			{
				return MemoryManager::Realloc_Syscall(ptr, new_size, file, line, func);
			}

			case ALLOC_TYPE::POOL:
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

			case ALLOC_TYPE::TEMP:
			{
				void* new_ptr = MemoryManager::AllocTemp(new_size);
				if (!new_ptr)
					return nullptr;

				if (ptr)
				{
					memcpy(new_ptr, ptr, old_size);
					// Temp 방식은 기존 포인터를 해제하지 않음
				}

				return new_ptr;
			}
		}

		return nullptr;
	}

	void tfree(
		void* ptr,
		ALLOC_TYPE alloc_type
	)
	{
		switch (alloc_type)
		{
			case ALLOC_TYPE::SYSCALL:
				MemoryManager::Free_SysCall(ptr);
				break;

			case ALLOC_TYPE::POOL:
				MemoryManager::FreePool(ptr);
				break;

			case ALLOC_TYPE::TEMP:
				// Temp 방식은 개별 해제를 지원하지 않음
				break;
		}
	}
}