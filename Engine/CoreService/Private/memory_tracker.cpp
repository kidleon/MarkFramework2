#include "pch.h"
#include "memory_tracker.h"
#include "mathlib.h"
#include "core_heap.h"


namespace mark
{
	memory_tracker::memory_tracker()
		: m_total_allocated(0)
		, m_total_deallocated(0)
		, m_current_usage(0)
		, m_peak_usage(0)
		, m_alloc_count(0)
	{
		m_allocations.reserve(2048); // 초기 용량을 2048로 설정하여 성능 최적화 (필요에 따라 조정 가능)
	}

	memory_tracker::~memory_tracker()
	{
		reset(); // 소멸자에서 리소스 정리 및 통계 초기화
	}

	void memory_tracker::on_allocate(void* ptr, size_t bytes, size_t alignment, std::source_location location)
	{
		if (!ptr) [[unlikely]]
			return;

		allocation_info info;
		info.bytes = static_cast<uint32_t>(bytes);
		info.alignment = static_cast<uint32_t>(alignment);
		safe_strcpy(info.file, MAX_FILE_NAME, location.file_name());
		safe_strcpy(info.function, MAX_FUNCTION_NAME, location.function_name());
		info.line = static_cast<uint32_t>(location.line());

		m_allocations[reinterpret_cast<uintptr_t>(ptr)] = info;

		m_total_allocated += bytes;
		m_current_usage += bytes;
		m_alloc_count++;
		size_t current = m_current_usage.load();
		size_t peak = m_peak_usage.load();
		while (current > peak && !m_peak_usage.compare_exchange_weak(peak, current)) {}

		//printf("[MemoryTracker] Allocated: %d bytes at %p (Alignment: %d) - %s:%d in function %s\n",
			//info.bytes, ptr, info.alignment, info.location.file_name(), info.location.line(), info.location.function_name());
	}

	void memory_tracker::on_deallocate(void* ptr)
	{
		if (!ptr) [[unlikely]]
			return;

		auto it = m_allocations.find(reinterpret_cast<uintptr_t>(ptr));
		if (it != m_allocations.end())
		{
			const allocation_info& info = it->second;

			//printf("[MemoryTracker] Deallocated: %d bytes at %p (Alignment: %d) - %s:%d in function %s\n",
				//info.bytes, ptr, info.alignment, info.location.file_name(), info.location.line(), info.location.function_name());

			m_total_deallocated += info.bytes;
			m_current_usage -= info.bytes;
			m_alloc_count--;
			m_allocations.erase(it);
		}
	}

	void memory_tracker::reset()
	{
		m_total_allocated = 0;
		m_total_deallocated = 0;
		m_current_usage = 0;
		m_peak_usage = 0;
		m_alloc_count = 0;
		m_allocations.clear();

		//printf("[MemoryTracker] Reset all statistics and records.\n");
	}

	void memory_tracker::print_report(std::function<void(const char*)> func) const
	{
		if (func) [[likely]]
		{
			char buffer[512] = {};

			sprintf(buffer, "=================================================================\n");
			func(buffer);

			snprintf(buffer, sizeof(buffer),
				"[MemoryTracker] Total Allocated: %zu bytes\n"
				"[MemoryTracker] Total Deallocated: %zu bytes\n"
				"[MemoryTracker] Current Usage: %zu bytes\n"
				"[MemoryTracker] Peak Usage: %zu bytes\n"
				"[MemoryTracker] Allocation Count: %zu\n",
				get_total_allocated(),
				get_total_deallocated(),
				get_current_usage(),
				get_peak_usage(),
				get_alloc_count());
			func(buffer);

			sprintf(buffer, "=================================================================\n");
			func(buffer);

			for(auto& [key, info] : m_allocations)
			{
				snprintf(buffer, sizeof(buffer),
					"[MemoryTracker] Allocated: %d bytes at %p (Alignment: %d) - %s:%d in function %s\n",
					info.bytes, reinterpret_cast<void*>(key), info.alignment, info.file, info.line, info.function);
				func(buffer);
			}	
		}
	}
}

