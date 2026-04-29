#include "pch.h"
#include "core.h"
#include "log.h"
#include "memory_tracker.h"


namespace mark
{
	static memory_tracker* s_memory_tracker = nullptr; // 메모리 트래커 인스턴스 생성

	void initialaize_core_service(size_t temp_buffer_size)
	{
		log::init(
			static_cast<uint32_t>(log::level::info),
			static_cast<uint32_t>(log::target::console) | static_cast<uint32_t>(log::target::file),
			false
		);

#if defined(__MEMORY_TRACKER_ENABLED__)
		s_memory_tracker = new memory_tracker;
#endif // __MEMORY_TRACKER_ENABLED__

		initialize_core_memory(
			1024 * 1024 * 32 // 32MB 임시 버퍼
		);
	}

	void shutdown_core_service()
	{
		shutdown_core_memory();

#if defined(__MEMORY_TRACKER_ENABLED__)
		if (s_memory_tracker)
		{
			delete s_memory_tracker;
			s_memory_tracker = nullptr;
		}
#endif // __MEMORY_TRACKER_ENABLED__

		log::shutdown();
	}
}
