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

		uint64 hash_key = fnv64(ptr, sizeof(void*)); // 해시값 계산

		acquire_spin_lock(&m_SysLock);

		insert_hash_node(
			m_pMemTable_SysAlloc,
			hash_key,
			&node->hash_node
		);

		release_spin_lock(&m_SysLock);
	}

	void MemoryRecorder::OnFree_Syscall(void* ptr)
	{
		if (!m_pMemTable_SysAlloc || !ptr) return;

		uint64 hash_key = fnv64(ptr, sizeof(void*)); // 해시값 계산

		acquire_spin_lock(&m_SysLock);

		void* node = query_hash_node(m_pMemTable_SysAlloc, hash_key);
		if (node)
			delete_hash_node(m_pMemTable_SysAlloc, hash_key);

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

		uint64 hash_key = fnv64(ptr, sizeof(void*)); // 해시값 계산

		acquire_spin_lock(&m_PoolLock);

		insert_hash_node(
			m_pMemTable_PoolAlloc,
			hash_key,
			&node->hash_node
		);

		release_spin_lock(&m_PoolLock);
	}

	void MemoryRecorder::OnFree_Pool(void* ptr)
	{
		if (!m_pMemTable_PoolAlloc || !ptr) return;

		uint64 hash_key = fnv64(ptr, sizeof(void*)); // 해시값 계산

		acquire_spin_lock(&m_PoolLock);

		void* node = query_hash_node(m_pMemTable_PoolAlloc, hash_key);
		if (node)
			delete_hash_node(m_pMemTable_PoolAlloc, hash_key);

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

	// 시스템 콜 할당 카운트 조회
	size_t MemoryRecorder::GetSysAllocCount() const noexcept
	{
		return (nullptr != m_pMemTable_SysAlloc) ? m_pMemTable_SysAlloc->node_count : 0;
	}

	// 풀 할당 카운트 조회
	size_t MemoryRecorder::GetPoolAllocCount() const noexcept
	{
		return (nullptr != m_pMemTable_PoolAlloc) ? m_pMemTable_PoolAlloc->node_count : 0;
	}

	size_t MemoryRecorder::GetMemoryBlockPoolAllocSize() const noexcept
	{
		if (!m_hPool)
			return 0;

		return paged_object_pool_get_used_size(m_hPool);
	}
}

