#include "pch.h"
#include "core.h"
#include "log.h"
#include "memory_tracker.h"
#include <mimalloc/mimalloc.h>


namespace mark
{
	static void MimallocStatsOutput(const char* msg, void* arg)
	{
#if defined(__TARGET_OS_WINDOWS)
		::OutputDebugStringA(msg);
		//::OutputDebugStringA("\n");
#endif // Windows 디버그 출력
	}

	static void MemoryTrackerPrint(const char* msg)
	{
#if defined(__TARGET_OS_WINDOWS)
		::OutputDebugStringA(msg);
		::OutputDebugStringA("\n");
#endif // Windows 디버그 출력
	}

	static void ReportMimallocLeak()
	{
		mi_collect(true);   // 미사용 세그먼트 반환

#if defined(__MEMORY_TRACKER_ENABLED__)
		memory_tracker::get().print_report(MemoryTrackerPrint);
#endif // __MEMORY_TRACKER_ENABLED__

		OutputDebugStringA("=== mimalloc leak check ===\n");
		mi_stats_print(MimallocStatsOutput);
		OutputDebugStringA("===========================\n");
	}


	void initialize_core_service(size_t temp_buffer_size)
	{
		log::init(
			static_cast<uint32_t>(log::level::max_lv),
			static_cast<uint32_t>(log::target::console) | static_cast<uint32_t>(log::target::file),
			false
		);

		initialize_core_memory(
			1024 * 1024 * 16 // 16MB 임시 버퍼
		);
	}

	void shutdown_core_service()
	{
		//|mi_stats_print(MimallocStatsOutput);
		shutdown_core_memory();

		ReportMimallocLeak();

		log::shutdown();

	}

}
