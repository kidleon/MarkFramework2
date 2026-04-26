#include "pch.h"
#include "CoreHeap.h"
#include "SpinLock.h"


#if defined(__MEMORY_TRACKING_ENABLED__)
#include "MemoryTracker.h"
#define MEM_ON_ALLOCATE(ptr, bytes, alignment, loc) mark::memory_tracker::get().on_allocate(ptr, bytes, alignment, loc)
#define MEM_ON_DEALLOCATE(ptr) mark::memory_tracker::get().on_deallocate(ptr)
#else
#define MEM_ON_ALLOCATE(ptr, bytes, alignment, loc) ((void)0)
#define MEM_ON_DEALLOCATE(ptr) ((void)0)
#endif // __MEMORY_TRACKING_ENABLED__

namespace mark
{
	constexpr size_t DEFAULT_ALIGNMENT = alignof(std::max_align_t);

	class sync_pool_memory_resource final : public std::pmr::memory_resource
	{
		spin_lock_t m_spin_lock;
		std::pmr::unsynchronized_pool_resource m_pool;

	public:
		explicit sync_pool_memory_resource(const std::pmr::pool_options& options = std::pmr::pool_options())
			: m_pool(options, std::pmr::get_default_resource())
		{
			init_spin_lock(&m_spin_lock);
		}

	private:
		inline void* do_allocate(size_t bytes, size_t alignment) final
		{
			AUTO_SPIN_LOCK auto_lock(&m_spin_lock);
			return m_pool.allocate(bytes, alignment);
		}

		inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
			AUTO_SPIN_LOCK auto_lock(&m_spin_lock);
			m_pool.deallocate(ptr, bytes, alignment);
		}

		inline bool do_is_equal(const memory_resource& other) const noexcept final
		{
			return this == &other;
		}
	};

	class usync_pool_memory_resource final : public std::pmr::unsynchronized_pool_resource
	{
	public:
		using std::pmr::unsynchronized_pool_resource::unsynchronized_pool_resource; // 부모 클래스의 생성자 상속
	};

	class temp_pool_memory_resource final : public std::pmr::memory_resource
	{
	public:
		explicit temp_pool_memory_resource(size_t buffer_size)
			: m_buffer((char*)::malloc(buffer_size))
			, m_buffer_size(buffer_size)
			, m_used(0)
			, m_owns_buffer(true)
		{
			if (!m_buffer) [[unlikely]]
				throw std::bad_alloc();
			MEM_ON_ALLOCATE(m_buffer, buffer_size, DEFAULT_ALIGNMENT, std::source_location::current());
		}

		explicit temp_pool_memory_resource(void* buffer, size_t buffer_size)
			: m_buffer((char*)buffer)
			, m_buffer_size(buffer_size)
			, m_used(0)
			, m_owns_buffer(false)
		{
		}

		virtual ~temp_pool_memory_resource()
		{
			if (m_owns_buffer && m_buffer)
			{
				MEM_ON_DEALLOCATE(m_buffer);
				::free(m_buffer);
				m_buffer = nullptr;
			}
		}

		inline void release()
		{
			m_used = 0;
		}

	private:
		inline void* do_allocate(size_t bytes, size_t alignment) final
		{
			if (!m_buffer) [[unlikely]]
			{
				assert(false && "Temp pool buffer is not initialized");
				return nullptr;
			}

			// 현재 포인터를 alignment 경계에 맞게 올림
			auto current = reinterpret_cast<std::uintptr_t>(m_buffer + m_used);
			auto aligned = (current + alignment - 1) & ~(alignment - 1);
			auto offset = aligned - reinterpret_cast<std::uintptr_t>(m_buffer);

			if (offset + bytes > m_buffer_size)
			{
				assert(false && "Temp pool buffer overflow");
				return nullptr;
			}

			m_used = offset + bytes;

			return reinterpret_cast<void*>(aligned);
		}
			
		inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
			// bump-style arena allocator이므로 개별 deallocate는 의도적으로 무시.
			// 메모리는 release() 호출 또는 객체 파괴 시 한꺼번에 회수된다.
			// PMR 인터페이스 규약상 빈 구현이 허용된다.
			(void)ptr; (void)bytes; (void)alignment;
		}

		inline bool do_is_equal(const memory_resource& other) const noexcept final
		{
			return this == &other;
		}

	private:
		char* m_buffer = nullptr;
		size_t m_buffer_size = 0;
		size_t m_used = 0;
		bool m_owns_buffer = true;

	};

	static sync_pool_memory_resource* s_default_sync_pool_memory_resource = nullptr;
	static usync_pool_memory_resource* s_default_usync_pool_memory_resource = nullptr;
	static size_t s_temp_buffer_size = 1024 * 1024 * 32; // 기본값

	struct thread_local_temp_memory_resource
	{
		temp_pool_memory_resource* resource = nullptr;
		~thread_local_temp_memory_resource()
		{
			if (resource)
			{
				delete resource;
				resource = nullptr;
			}
		}

		temp_pool_memory_resource* get()
		{
			if (!resource)
				resource = new temp_pool_memory_resource(s_temp_buffer_size);
			return resource;
		}
	};
	
	thread_local thread_local_temp_memory_resource s_default_temp_memory_resource; // 32MB 임시 버퍼
	std::atomic<bool> is_core_memory_initialized = false;

	bool initialize_core_memory(
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	)
	{
		if (is_core_memory_initialized.load()) return false;

		try
		{
			s_default_sync_pool_memory_resource = new sync_pool_memory_resource({ sync_pool_count_per_chunk, sync_pool_max_size_per_block });
			s_default_usync_pool_memory_resource = new usync_pool_memory_resource({ unsync_pool_count_per_chunk, unsync_pool_max_size_per_block });
			s_temp_buffer_size = temp_buffer_size;
			is_core_memory_initialized = true;
		}
		catch (const std::exception& e)
		{
			assert(false && e.what());
			shutdown_core_memory();
			return false;
		}

		return true;
	}

	void shutdown_core_memory()
	{
		if (s_default_usync_pool_memory_resource)
		{
			delete s_default_usync_pool_memory_resource;
			s_default_usync_pool_memory_resource = nullptr;
		}

		if (s_default_sync_pool_memory_resource)
		{
			delete s_default_sync_pool_memory_resource;
			s_default_sync_pool_memory_resource = nullptr;
		}
		is_core_memory_initialized = false;
	}

	void* sys_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* p = std::pmr::get_default_resource()->allocate(bytes, alignment); // 시스템 메모리 리소스에서 할당 (기본 리소스는 system_memory_resource)
		MEM_ON_ALLOCATE(p, bytes, alignment, loc);

		return p;
	}

	void* spool_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* p = s_default_sync_pool_memory_resource->allocate(bytes, alignment);
		MEM_ON_ALLOCATE(p, bytes, alignment, loc);

		return p;
	}
	void* upool_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* p = s_default_usync_pool_memory_resource->allocate(bytes, alignment);
		MEM_ON_ALLOCATE(p, bytes, alignment, loc);

		return p;
	}

	void* sys_alloc(size_t bytes, size_t alignment)
	{
		return std::pmr::get_default_resource()->allocate(bytes, alignment); // 시스템 메모리 리소스에서 할당 (기본 리소스는 system_memory_resource)
	}

	void* spool_alloc(size_t bytes, size_t alignment)
	{
		return s_default_sync_pool_memory_resource->allocate(bytes, alignment);
	}

	void* upool_alloc(size_t bytes, size_t alignment)
	{
		return s_default_usync_pool_memory_resource->allocate(bytes, alignment);
	}

	void* temp_alloc(size_t bytes, size_t alignment)
	{
		return s_default_temp_memory_resource.get()->allocate(bytes, alignment);
	}

	void temp_reset()
	{
		s_default_temp_memory_resource.get()->release();
	}

	void sys_free(void* ptr, size_t bytes, size_t alignment)
	{
		MEM_ON_DEALLOCATE(ptr);

		std::pmr::get_default_resource()->deallocate(ptr, bytes, alignment); // 시스템 메모리 리소스에서 해제 (기본 리소스는 system_memory_resource)
	}

	void spool_free(void* ptr, size_t bytes, size_t alignment)
	{
		MEM_ON_DEALLOCATE(ptr);

		s_default_sync_pool_memory_resource->deallocate(ptr, bytes, alignment);
	}

	void upool_free(void* ptr, size_t bytes, size_t alignment)
	{
		MEM_ON_DEALLOCATE(ptr);

		s_default_usync_pool_memory_resource->deallocate(ptr, bytes, alignment);
	}

	std::pmr::memory_resource* get_default_system_memory_resource_ptr() noexcept
	{
		return std::pmr::get_default_resource(); // 시스템 메모리 리소스 반환 (기본 리소스는 system_memory_resource)
	}

	std::pmr::memory_resource* get_default_spool_memory_resource_ptr() noexcept
	{
		return s_default_sync_pool_memory_resource;
	}

	std::pmr::memory_resource* get_default_usync_pool_memory_resource_ptr() noexcept
	{
		return s_default_usync_pool_memory_resource;
	}

	std::pmr::memory_resource* get_default_temp_memory_resource_ptr() noexcept
	{
		return s_default_temp_memory_resource.get();
	}
}
