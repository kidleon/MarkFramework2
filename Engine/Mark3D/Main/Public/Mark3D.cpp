#include "pch.h"
#include "Mark3D.h"
#include "CoreHeap.h"
#include "MemoryTracker.h"



namespace mark
{
	static memory_tracker* s_memory_tracker = nullptr; // 메모리 트래커 인스턴스 생성

	bool Mark3D::Initialize()
	{
#if defined(__LOG_ENABLED__)
		log::init(
			static_cast<uint32_t>(log::level::info),
			static_cast<uint32_t>(log::target::console) | static_cast<uint32_t>(log::target::file),
			false
		);
#endif // __LOG_ENABLED__

#if defined(__MEMORY_TRACKER_ENABLED__)
		s_memory_tracker = new memory_tracker;
#endif // __MEMORY_TRACKER_ENABLED__

		initialize_core_memory(
			1024 * 1024 * 32 // 32MB 임시 버퍼
		);

		LOG("Mark3D core memory initialized.");

		return true;
	}

	void Mark3D::Shutdown()
	{
		shutdown_core_memory();

#if defined(__MEMORY_TRACKER_ENABLED__)
		if (s_memory_tracker)
		{
			delete s_memory_tracker;
			s_memory_tracker = nullptr;
		}
#endif // __MEMORY_TRACKER_ENABLED__

#if defined(__LOG_ENABLED__)
		log::shutdown();
#endif // __LOG_ENABLED__


	}

}
