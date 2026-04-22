#include "pch.h"
#include "Mark3D.h"
#include "PrivateMemory.h"
#include "MemoryTracker.h"


namespace mark
{
	static memory_tracker* s_memory_tracker = nullptr; // 메모리 트래커 인스턴스 생성

	bool Mark3D::Initialize()
	{
#if defined(__MEMORY_TRACKING_ENABLED__)
		std::pmr::pool_options opts = { 
			.max_blocks_per_chunk = 1024, // 각 청크당 최대 블록 수
			.largest_required_pool_block = 64 // 최대 블록 크기 (바이트 단위)
		};
		s_memory_tracker = new memory_tracker(opts);
#endif // __MEMORY_TRACKING_ENABLED__

		initialize_core_memory(
			1024 * 1024 * 256, // initialize_core_memory (256MB)
			64,  // sync_pool_count_per_chunk = 64
			64,  // unsync_pool_count_per_chunk = 64
			1024 * 1024,  // sync_pool_max_size_per_block = 1MB
			1024 * 1024,  // unsync_pool_max_size_per_block = 1MB
			1024 * 1024 * 32 // 32MB 임시 버퍼
		);

		return true;
	}

	void Mark3D::Shutdown()
	{
		shutdown_core_memory();

#if defined(__MEMORY_TRACKING_ENABLED__)
		if (s_memory_tracker)
		{
			delete s_memory_tracker;
			s_memory_tracker = nullptr;
		}
#endif // __MEMORY_TRACKING_ENABLED__
	}

}
