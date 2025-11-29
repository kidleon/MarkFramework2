#include "pch.h"
#include "MemoryRecorder.h"
#include "paged_object_pool.h"
#include "hash_table.h"
#include "fnv.h"


namespace mark
{
	static const size_t DEFAULT_MEMORY_RECORD_POOL_COUNT = 4096; // 페이지 당 4096개의 메모리 풀 블록
	static const size_t DEFAULT_MEMORY_RECORD_BUCKET_SIZE = 1024; // 1024개의 버킷

	struct RECRODR_MEMORY_BLOCK
	{
		void* ptr;
		size_t size;
		char* file;
		char* func;
		int line;
		int reserve;

		HASH_NODE hash_node;
	};

	MemoryRecorder::MemoryRecorder()
		: m_hPool(nullptr)
		, m_pMemTable_SysAlloc(nullptr)
		, m_pMemTable_PoolAlloc(nullptr)
		, m_pfnReportMemoryLeak(nullptr)
		, m_AllocCount_Syscall(0)
		, m_AllocCount_Pool(0)
		, m_AllocSize_Syscall(0)
		, m_AllocSize_Pool(0)
	{
		init_spin_lock(&m_SysLock);
		init_spin_lock(&m_PoolLock);
	}

	MemoryRecorder::~MemoryRecorder()
	{
		Cleanup();
	}

	BOOL MemoryRecorder::Initialize(pfnReportMemoryLeak leak_reporter)
	{
		if (m_hPool)
			return TRUE;

		m_hPool = create_paged_object_pool(
			0,
			sizeof(RECRODR_MEMORY_BLOCK),
			DEFAULT_MEMORY_RECORD_POOL_COUNT,
			TRUE
		);

		if (!m_hPool)
			return FALSE;

		m_pfnReportMemoryLeak = leak_reporter;

		m_pMemTable_SysAlloc = create_hash_table(DEFAULT_MEMORY_RECORD_BUCKET_SIZE);
		m_pMemTable_PoolAlloc = create_hash_table(DEFAULT_MEMORY_RECORD_BUCKET_SIZE);

		return TRUE;
	}

	void MemoryRecorder::Cleanup()
	{
		if (m_pMemTable_PoolAlloc)
		{
			delete_hash_table(m_pMemTable_PoolAlloc);
			m_pMemTable_PoolAlloc = nullptr;
		}

		if (m_pMemTable_SysAlloc)
		{
			delete_hash_table(m_pMemTable_SysAlloc);
			m_pMemTable_SysAlloc = nullptr;
		}

		if (m_hPool)
		{
			destroy_paged_object_pool(m_hPool);
			m_hPool = nullptr;
		}

		m_pfnReportMemoryLeak = nullptr;
	}

	void MemoryRecorder::OnAlloc_Syscall(
		void* ptr,
		size_t size,
		const char* file,
		int line,
		const char* func
	)
	{
		if (!m_pMemTable_SysAlloc)
			return;

		RECRODR_MEMORY_BLOCK* node = (RECRODR_MEMORY_BLOCK*)paged_object_pool_alloc(m_hPool);
		if (!node)
			return;

		node->ptr = ptr;
		node->size = size;
		node->file = (char*)file;
		node->func = (char*)func;
		node->line = line;
		node->hash_node.data = (void*)node;

		uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

		acquire_spin_lock(&m_SysLock);

		insert_hash_node(
			m_pMemTable_SysAlloc,
			hash_key,
			&node->hash_node
		);

		m_AllocCount_Syscall++;
		m_AllocSize_Syscall += size;

		release_spin_lock(&m_SysLock);
	}

	void MemoryRecorder::OnFree_Syscall(void* ptr)
	{
		if (!m_pMemTable_SysAlloc || !ptr) return;

		uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

		acquire_spin_lock(&m_SysLock);

		RECRODR_MEMORY_BLOCK* node = (RECRODR_MEMORY_BLOCK*)query_hash_node(m_pMemTable_SysAlloc, hash_key);
		if (node)
		{
			delete_hash_node(m_pMemTable_SysAlloc, hash_key);
			m_AllocCount_Syscall--;
			m_AllocSize_Syscall -= node->size;
		}

		release_spin_lock(&m_SysLock);

		paged_object_pool_free(m_hPool, node);
	}

	void MemoryRecorder::OnAlloc_Pool(
		void* ptr,
		size_t size,
		const char* file,
		int line,
		const char* func
	)
	{
		if (!m_pMemTable_PoolAlloc)
			return;

		RECRODR_MEMORY_BLOCK* node = (RECRODR_MEMORY_BLOCK*)paged_object_pool_alloc(m_hPool);
		if (!node)
			return;

		node->ptr = ptr;
		node->size = size;
		node->file = (char*)file;
		node->func = (char*)func;
		node->line = line;
		node->hash_node.data = (void*)node;

		uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

		acquire_spin_lock(&m_PoolLock);

		insert_hash_node(
			m_pMemTable_PoolAlloc,
			hash_key,
			&node->hash_node
		);

		m_AllocCount_Pool++;
		m_AllocSize_Pool += size;

		release_spin_lock(&m_PoolLock);
	}

	void MemoryRecorder::OnFree_Pool(void* ptr)
	{
		if (!m_pMemTable_PoolAlloc || !ptr) return;

		uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

		acquire_spin_lock(&m_PoolLock);

		RECRODR_MEMORY_BLOCK* node = (RECRODR_MEMORY_BLOCK*)query_hash_node(m_pMemTable_PoolAlloc, hash_key);
		if (node)
		{
			delete_hash_node(m_pMemTable_PoolAlloc, hash_key);
			m_AllocCount_Pool--;
			m_AllocSize_Pool -= node->size;
		}
			

		release_spin_lock(&m_PoolLock);

		paged_object_pool_free(m_hPool, ptr);
	}

	void MemoryRecorder::ReportMemoryLeaks() noexcept
	{
		if (!m_pfnReportMemoryLeak)
			return;

		// Syscall 할당 누수 보고
		acquire_spin_lock(&m_SysLock);
		for (size_t i = 0; i < m_pMemTable_SysAlloc->bucket_size; ++i)
		{
			HASH_NODE* current = m_pMemTable_SysAlloc->buckets[i];
			while (current)
			{
				RECRODR_MEMORY_BLOCK* block = (RECRODR_MEMORY_BLOCK*)current->data;
				m_pfnReportMemoryLeak(
					"Syscall",
					block->file,
					block->line,
					block->func,
					block->size
				);
				current = current->next;
			}
		}
		release_spin_lock(&m_SysLock);

		// Pool 할당 누수 보고
		acquire_spin_lock(&m_PoolLock);
		for (size_t i = 0; i < m_pMemTable_PoolAlloc->bucket_size; ++i)
		{
			HASH_NODE* current = m_pMemTable_PoolAlloc->buckets[i];
			while (current)
			{
				RECRODR_MEMORY_BLOCK* block = (RECRODR_MEMORY_BLOCK*)current->data;
				m_pfnReportMemoryLeak(
					"Pool",
					block->file,
					block->line,
					block->func,
					block->size
				);
				current = current->next;
			}
		}
		release_spin_lock(&m_PoolLock);
	}

	MEM_SIZE MemoryRecorder::GetAllocCount_Syscall() noexcept
	{
		MEM_SIZE result = 0;

		acquire_spin_lock(&m_SysLock);

		result = m_AllocCount_Syscall;

		release_spin_lock(&m_SysLock);

		return result;
	}

	MEM_SIZE MemoryRecorder::GetAllocCount_Pool() noexcept
	{
		MEM_SIZE result = 0;

		acquire_spin_lock(&m_PoolLock);

		result = m_AllocCount_Pool;

		release_spin_lock(&m_PoolLock);

		return result;
	}

	MEM_SIZE MemoryRecorder::GetAllocSize_Syscall() noexcept
	{
		MEM_SIZE result = 0;

		acquire_spin_lock(&m_SysLock);

		result = m_AllocSize_Syscall;

		release_spin_lock(&m_SysLock);

		return result;
	}

	MEM_SIZE MemoryRecorder::GetAllocSize_Pool() noexcept
	{
		MEM_SIZE result = 0;

		acquire_spin_lock(&m_PoolLock);

		result = m_AllocSize_Pool;

		release_spin_lock(&m_PoolLock);

		return result;
	}

	// 할당 통계 조회 (LOCK 한번에 조회한다)
	void MemoryRecorder::GetAllocStats(
		MEM_SIZE* pSyscallCount,
		MEM_SIZE* pPoolCount,
		MEM_SIZE* pSyscallSize,
		MEM_SIZE* pPoolSize
	) noexcept
	{
		acquire_spin_lock(&m_SysLock);

		*pSyscallCount = m_AllocCount_Syscall;
		*pSyscallSize = m_AllocSize_Syscall;

		release_spin_lock(&m_SysLock);

		acquire_spin_lock(&m_PoolLock);

		*pPoolCount = m_AllocCount_Pool;
		*pPoolSize = m_AllocSize_Pool;

		release_spin_lock(&m_PoolLock);
	}
	
}

