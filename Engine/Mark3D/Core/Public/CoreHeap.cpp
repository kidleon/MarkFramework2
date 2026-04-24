#include "pch.h"
#include "CoreHeap.h"


namespace mark
{
	class sync_pool_memory_resource2 final : public std::pmr::synchronized_pool_resource
	{
	public:
		explicit sync_pool_memory_resource2(const std::pmr::pool_options& options = std::pmr::pool_options())
			: std::pmr::synchronized_pool_resource(options, std::pmr::get_default_resource())
		{
		}
	};

	class usync_pool_memory_resource2 final : public std::pmr::unsynchronized_pool_resource
	{
	public:
		explicit usync_pool_memory_resource2(const std::pmr::pool_options& options = std::pmr::pool_options())
			: std::pmr::unsynchronized_pool_resource(options, std::pmr::get_default_resource())
		{
		}
	};

	class temp_pool_memory_resource2 final : public std::pmr::monotonic_buffer_resource
	{
	public:
		explicit temp_pool_memory_resource2(size_t buffer_size)
			: std::pmr::monotonic_buffer_resource(buffer_size, std::pmr::get_default_resource())
			, m_buffer_size(buffer_size)
		{
		}

		explicit temp_pool_memory_resource2(void* buffer, size_t buffer_size)
			: std::pmr::monotonic_buffer_resource(buffer, buffer_size, std::pmr::get_default_resource())
			, m_buffer_size(buffer_size)
		{
		}

	private:
		inline void* do_allocate(size_t bytes, size_t alignment) final
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (m_used + bytes > m_buffer_size)
			{
				assert(false && "Temp pool buffer overflow detected");
				return nullptr;
			}
			m_used += bytes;
			
#endif // 디버그 모드에서는 할당된 총 바이트 수를 추적하여 버퍼 초과 여부를 확인할 수 있도록 함 (실제 할당은 monotonic_buffer_resource가 처리)
			return std::pmr::monotonic_buffer_resource::do_allocate(bytes, alignment);
		}

		inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
#if defined(DEBUG) || defined(_DEBUG)
			m_used -= bytes;
#endif 
			std::pmr::monotonic_buffer_resource::do_deallocate(ptr, bytes, alignment);
		}

	private:
		size_t m_used = 0;
		size_t m_buffer_size = 0;

	};

	static sync_pool_memory_resource2* s_default_sync_pool_memory_resource2 = nullptr;
	static usync_pool_memory_resource2* s_default_usync_pool_memory_resource2 = nullptr;
	static temp_pool_memory_resource2* s_default_temp_memory_resource2 = nullptr;

	constexpr size_t DEFAULT_ALIGNMENT = alignof(std::max_align_t);

	bool initialize_core_memory2(
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	)
	{
		try
		{
			s_default_sync_pool_memory_resource2 = new sync_pool_memory_resource2({ sync_pool_count_per_chunk, sync_pool_max_size_per_block });
			s_default_usync_pool_memory_resource2 = new usync_pool_memory_resource2({ unsync_pool_count_per_chunk, unsync_pool_max_size_per_block });
			s_default_temp_memory_resource2 = new temp_pool_memory_resource2(temp_buffer_size);
		}
		catch (const std::exception& e)
		{
			assert(false && e.what());
			shutdown_core_memory2();
			return false;
		}

		return true;
	}

	void shutdown_core_memory2()
	{
		if (s_default_temp_memory_resource2)
		{
			delete s_default_temp_memory_resource2;
			s_default_temp_memory_resource2 = nullptr;
		}

		if (s_default_usync_pool_memory_resource2)
		{
			delete s_default_usync_pool_memory_resource2;
			s_default_usync_pool_memory_resource2 = nullptr;
		}

		if (s_default_sync_pool_memory_resource2)
		{
			delete s_default_sync_pool_memory_resource2;
			s_default_sync_pool_memory_resource2 = nullptr;
		}
	}

	void* sys_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		return std::pmr::get_default_resource()->allocate(bytes, alignment); // 시스템 메모리 리소스에서 할당 (기본 리소스는 system_memory_resource)
	}

	void* spool_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		return s_default_sync_pool_memory_resource2->allocate(bytes, alignment);
	}
	void* upool_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		return s_default_usync_pool_memory_resource2->allocate(bytes, alignment);
	}

	void* sys_alloc(size_t bytes, size_t alignment)
	{
		return std::pmr::get_default_resource()->allocate(bytes, alignment); // 시스템 메모리 리소스에서 할당 (기본 리소스는 system_memory_resource)
	}

	void* spool_alloc(size_t bytes, size_t alignment)
	{
		return s_default_sync_pool_memory_resource2->allocate(bytes, alignment);
	}

	void* upool_alloc(size_t bytes, size_t alignment)
	{
		return s_default_usync_pool_memory_resource2->allocate(bytes, alignment);
	}

	void* temp_alloc(size_t bytes, size_t alignment)
	{
		return s_default_temp_memory_resource2->allocate(bytes, alignment);
	}

	void temp_reset()
	{
		s_default_temp_memory_resource2->release();
	}

	void sys_free(void* ptr, size_t bytes, size_t alignment)
	{
		std::pmr::get_default_resource()->deallocate(ptr, bytes, alignment); // 시스템 메모리 리소스에서 해제 (기본 리소스는 system_memory_resource)
	}

	void spool_free(void* ptr, size_t bytes, size_t alignment)
	{
		s_default_sync_pool_memory_resource2->deallocate(ptr, bytes, alignment);
	}

	void upool_free(void* ptr, size_t bytes, size_t alignment)
	{
		s_default_usync_pool_memory_resource2->deallocate(ptr, bytes, alignment);
	}

	std::pmr::memory_resource* get_default_system_memory_resource_ptr() noexcept
	{
		return std::pmr::get_default_resource(); // 시스템 메모리 리소스 반환 (기본 리소스는 system_memory_resource)
	}
	std::pmr::memory_resource* get_default_spool_memory_resource_ptr() noexcept
	{
		return s_default_sync_pool_memory_resource2;
	}
	std::pmr::memory_resource* get_default_usync_pool_memory_resource_ptr() noexcept
	{
		return s_default_usync_pool_memory_resource2;
	}
	std::pmr::memory_resource* get_default_temp_memory_resource_ptr() noexcept
	{
		return s_default_temp_memory_resource2;
	}
}
