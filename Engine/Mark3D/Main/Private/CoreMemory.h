#pragma once
#include <memory_resource>

#if defined(__MEMORY_TRACKING_ENABLED__)
#include <source_location>
#endif // __MEMORY_TRACKING_ENABLED__


namespace mark
{
	static bool initialize_core_memory(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	);

	static void shutdown_core_memory();

	[[nodiscard]] static std::pmr::memory_resource* get_default_system_memory_resource_ptr() noexcept;

	[[nodiscard]] static std::pmr::memory_resource* get_default_limited_memory_resource_ptr() noexcept;

	[[nodiscard]] static std::pmr::memory_resource* get_default_sync_pool_memory_resource_ptr() noexcept;

	[[nodiscard]] static std::pmr::memory_resource* get_default_unsync_pool_memory_resource_ptr() noexcept;

	[[nodiscard]] static std::pmr::memory_resource* get_default_temp_memory_resource_ptr() noexcept;


	/**
   * @brief 할당된 메모리 양에 제한을 두는 메모리 리소스
   */
	class limited_memory_resource final : public std::pmr::memory_resource
	{
	public:
		explicit limited_memory_resource(
			size_t max_size,
			std::pmr::memory_resource* upstream
		)
			: m_limit(max_size)
			, m_used(0)
			, m_upstream(upstream)
		{
			m_limited = (max_size > 0); // max_size가 0이면 제한이 없는 것으로 간주
		}

		[[nodiscard]] inline size_t get_limit() const noexcept { return m_limit.load(); }
		[[nodiscard]] inline size_t get_used() const noexcept { return m_used.load(); }
		[[nodiscard]] inline bool is_full() const noexcept { return m_limited && m_used.load() >= m_limit.load(); }

	private:
		virtual void* do_allocate(size_t bytes, size_t alignment) final
		{
			if (bytes == 0) [[unlikely]]
				return nullptr;

			size_t aligned_bytes = ALIGN_UP(bytes, alignment);

			// 제한이 없는 경우
			if (m_limited)
			{
				if (m_used + aligned_bytes > m_limit)
				{
					assert(false && "Memory limit exceeded in limited_memory_resource");
				}
			}

			void* ptr = m_upstream->allocate(aligned_bytes, alignment);

			if (!ptr) [[unlikely]]
				return nullptr;

			m_used += aligned_bytes;

			return ptr;
		}

		virtual void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
			size_t aligned_bytes = ALIGN_UP(bytes, alignment);
			m_upstream->deallocate(ptr, aligned_bytes, alignment);
			m_used -= aligned_bytes;
		}

		virtual bool do_is_equal(const std::pmr::memory_resource& Other) const noexcept final
		{
			return this == &Other;
		}

	private:
		std::atomic<size_t> m_limit;
		std::atomic<size_t> m_used;
		std::pmr::memory_resource* m_upstream;
		bool m_limited; // true if m_limit is a valid limit, false if m_limit is ignored (unlimited)
	};

#if defined(__MEMORY_TRACKING_ENABLED__)
	[[nodiscard]] extern void* core_sys_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] extern void* core_spool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] extern void* core_upool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] extern void* core_temp_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);
#else
	[[nodiscard]] extern void* core_sys_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] extern void* core_spool_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] extern void* core_upool_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] extern void* core_temp_alloc(size_t bytes, size_t alignment);
#endif // __MEMORY_TRACKING_ENABLED__
	
	extern void core_sys_free(void* ptr, size_t alignment);
	extern void core_spool_free(void* ptr, size_t alignment);
	extern void core_upool_free(void* ptr, size_t alignment);
	extern void core_temp_reset();
	[[nodiscard]] extern size_t get_alloc_count(void* ptr, size_t alignment);


	// sys new/delete 
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment>
	[[nodiscard]] inline T* CORE_SYS_NEW(std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T), Alignment), loc);
		if (!ptr) [[unlikely]]
			return nullptr;

		new (ptr) T();

		return ptr;
	}

	template<typename T, size_t Alignment = alignof(T), typename... Args>
	[[nodiscard]] inline T* CORE_SYS_NEW(Args&&... args, std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T), Alignment), loc);
		if (!ptr) [[unlikely]]
			return nullptr;
		new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#else
	template<typename T, size_t Alignment>
	[[nodiscard]] inline T* CORE_SYS_NEW()
	{
		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}

	template<typename T, size_t Alignment = alignof(T), typename... Args>
	[[nodiscard]] inline T* CORE_SYS_NEW(Args&&... args)
	{
		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Alignment>
	inline void CORE_SYS_DELETE(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;

		static_cast<T*>(ptr)->~T();

		core_sys_free(ptr, Alignment);
	}


	// spool new / delete
	

#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment>
	[[nodiscard]] inline T* CORE_SPOOL_NEW(std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T), Alignment, loc));
		if (ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}

	template<typename T, size_t Alignment, typename... Args>
	[[nodiscard]] inline T* CORE_SPOOL_NEW(std::source_location loc, Args&&... args)
	{
		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T), Alignment, loc));
		if (ptr) [[likely]]
			new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#else
	template<typename T, size_t Alignment>
	[[nodiscard]] inline T* CORE_SPOOL_NEW()
	{
		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}

	template<typename T, size_t Alignment, typename... Args>
	[[nodiscard]] inline T* CORE_SPOOL_NEW(Args&&... args)
	{
		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Alignment>
	inline void CORE_SPOOL_DELETE(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;

		static_cast<T*>(ptr)->~T();

		core_spool_free(ptr, Alignment);
	}

	// upool new / delete
	

#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment>
	[[nodiscard]] inline T* CORE_UPOOL_NEW(std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T), Alignment, loc));
		if (ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}

	template<typename T, size_t Alignment, typename... Args>
	[[nodiscard]] inline T* CORE_UPOOL_NEW(Args&&... args, std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T), Alignment, loc));
		if (ptr) [[likely]]
		new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#else
	template<typename T, size_t Alignment>
	[[nodiscard]] inline T* CORE_UPOOL_NEW()
	{
		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T), Alignment));
		if (!ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}

	template<typename T, size_t Alignment, typename... Args>
	[[nodiscard]] inline T* CORE_UPOOL_NEW(Args&&... args)
	{
		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Alignment = alignof(T), typename... Args>
	[[nodiscard]] inline T* CORE_TEMP_NEW(Args&&... args)
	{
		T* ptr = static_cast<T*>(core_temp_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}

#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment = alignof(T)>
	[[nodiscard]] inline T* CORE_TEMP_NEW(std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_temp_alloc(sizeof(T), Alignment, loc));
		if (ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}

	template<typename T, size_t Alignment = alignof(T), typename... Args>
	[[nodiscard]] inline T* CORE_TEMP_NEW(Args&&... args, std::source_location loc)
	{
		T* ptr = static_cast<T*>(core_temp_alloc(sizeof(T), Alignment, loc));
		if (ptr) [[likely]]
			new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}
#else
	template<typename T, size_t Alignment>
	inline void CORE_UPOOL_DELETE(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;
		static_cast<T*>(ptr)->~T();
		core_upool_free(ptr, Alignment);
	}

	// temp new (delete는 없고 reset으로 전체 해제)
	template<typename T, size_t Alignment = alignof(T)>
	[[nodiscard]] inline T* CORE_TEMP_NEW()
	{
		T* ptr = static_cast<T*>(core_temp_alloc(sizeof(T), Alignment));
		if (ptr) [[likely]]
			new (ptr) T();
		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template <typename T, size_t Alignment>
	inline void CORE_TEMP_DELETE(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;
		static_cast<T*>(ptr)->~T();
		// TEMP는 개별 해제가 없으므로 core_temp_free 같은 함수도 없음. 전체 해제는 core_temp_reset()으로 해야 함.
	}

	// sys new / delete array
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment, size_t Count>
	[[nodiscard]] inline T* CORE_SYS_NEW_ARRAY(std::source_location loc)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T) * Count, Alignment, loc));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T();
		}

		return ptr;
	}

	template<typename T, size_t Alignment, size_t Count, typename... Args>
	[[nodiscard]] inline T* CORE_SYS_NEW_ARRAY(std::source_location loc, Args&&... args)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T) * Count, Alignment, loc));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);
		}

		return ptr;
	}
#else
	template<typename T, size_t Alignment, size_t Count>
	[[nodiscard]] inline T* CORE_SYS_NEW_ARRAY()
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T) * Count, Alignment));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T();
		}

		return ptr;
	}

	template<typename T, size_t Alignment, size_t Count, typename... Args>
	[[nodiscard]] inline T* CORE_SYS_NEW_ARRAY(Args&&... args)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_sys_alloc(sizeof(T) * Count, Alignment));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);
		}

		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Alignment>
	inline void CORE_SYS_DELETE_ARRAY(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;

		size_t count = get_alloc_count(ptr, Alignment);

		T* typed_ptr = static_cast<T*>(ptr);

		for (size_t i = count; i > 0; --i)
			typed_ptr[i - 1].~T();

		core_sys_free(ptr, Alignment);
	}

	// spool new delete array
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment, size_t Count>
	[[nodiscard]] inline T* CORE_SPOOL_NEW_ARRAY(std::source_location loc)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T) * Count, Alignment, loc));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T();
		}

		return ptr;
	}

	template<typename T, size_t Alignment, size_t Count, typename... Args>
	[[nodiscard]] inline T* CORE_SPOOL_NEW_ARRAY(std::source_location loc, Args&&... args)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T) * Count, Alignment, loc));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);
		}

		return ptr;
	}
#else
	template<typename T, size_t Alignment, size_t Count>
	[[nodiscard]] inline T* CORE_SPOOL_NEW_ARRAY()
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T) * Count, Alignment));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T();
		}

		return ptr;
	}

	template<typename T, size_t Alignment, size_t Count, typename... Args>
	[[nodiscard]] inline T* CORE_SPOOL_NEW_ARRAY(Args&&... args)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_spool_alloc(sizeof(T) * Count, Alignment));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);
		}

		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Alignment>
	inline void CORE_SPOOL_DELETE_ARRAY(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;

		size_t count = get_alloc_count(ptr, Alignment);

		T* typed_ptr = static_cast<T*>(ptr);

		for (size_t i = count; i > 0; --i)
			typed_ptr[i - 1].~T();

		core_spool_free(ptr, Alignment);
	}


	// upool new delete array
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Alignment, size_t Count>
	[[nodiscard]] inline T* CORE_UPOOL_NEW_ARRAY(std::source_location loc)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T) * Count, Alignment, loc));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T();
		}

		return ptr;
	}

	template<typename T, size_t Alignment, size_t Count, typename... Args>
	[[nodiscard]] inline T* CORE_UPOOL_NEW_ARRAY(std::source_location loc, Args&&... args)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T) * Count, Alignment, loc));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);
		}

		return ptr;
	}
#else
	template<typename T, size_t Alignment, size_t Count>
	[[nodiscard]] inline T* CORE_UPOOL_NEW_ARRAY()
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T) * Count, Alignment));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T();
		}

		return ptr;
	}

	template<typename T, size_t Alignment, size_t Count, typename... Args>
	[[nodiscard]] inline T* CORE_UPOOL_NEW_ARRAY(Args&&... args)
	{
		if (Count == 0) [[unlikely]]
			return nullptr;

		T* ptr = static_cast<T*>(core_upool_alloc(sizeof(T) * Count, Alignment));

		if (ptr) [[likely]]
		{
			for (size_t i = 0; i < Count; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);
		}

		return ptr;
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Alignment>
	inline void CORE_UPOOL_DELETE_ARRAY(void* ptr)
	{
		if (!ptr)
			[[unlikely]] return;

		size_t count = get_alloc_count(ptr, Alignment);

		T* typed_ptr = static_cast<T*>(ptr);

		for (size_t i = count; i > 0; --i)
			typed_ptr[i - 1].~T();

		core_spool_free(ptr, Alignment);
	}
}

// malloc/free 스타일의 간단한 인터페이스 매크로 정의 (정렬은 uintptr_t 크기로 기본값 설정)
#if defined(__MEMORY_TRACKING_ENABLED__)
#define CORE_SYS_ALLOC_A(Bytes, Align)          ::mark::core_sys_alloc(Bytes, Align, std::source_location::current())
#define CORE_SYS_FREE_A(ptr, Align)             ::mark::core_sys_free(ptr, Align)
#define CORE_SYS_ALLOC(Bytes)		            CORE_SYS_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SYS_FREE(ptr)				        CORE_SYS_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_SPOOL_ALLOC_A(Bytes, Align)        ::mark::core_spool_alloc(Bytes, Align, std::source_location::current())
#define CORE_SPOOL_FREE_A(ptr, Align)           ::mark::core_spool_free(ptr, Align)
#define CORE_SPOOL_ALLOC(Bytes)                 CORE_SPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SPOOL_FREE(ptr)                    CORE_SPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_UPOOL_ALLOC_A(Bytes, Align)        ::mark::core_upool_alloc(Bytes, Align, std::source_location::current())
#define CORE_UPOOL_FREE_A(ptr, Align)           ::mark::core_upool_free(ptr, Align)
#define CORE_UPOOL_ALLOC(Bytes)                 CORE_UPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_UPOOL_FREE(ptr)                    CORE_UPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_TEMP_ALLOC_A(Bytes, Align)         ::mark::core_temp_alloc(Bytes, Align, std::source_location::current())
#define CORE_TEMP_ALLOC(Bytes)                  CORE_TEMP_ALLOC_A(Bytes, sizeof(uintptr_t))

// sys new / delete predefine
#define CORE_SYS_NEW(T)							::mark::CORE_SYS_NEW<T, sizeof(uintptr_t)>(std::source_location::current())
#define CORE_SYS_NEW_ARGS(T, ...)				::mark::CORE_SYS_NEW<T, sizeof(uintptr_t)>(std::source_location::current(), __VA_ARGS__)
#define CORE_SYS_NEW_A(T, A)					::mark::CORE_SYS_NEW<T, A>(std::source_location::current())
#define CORE_SYS_NEW_ARGS_A(T, A, ...)			::mark::CORE_SYS_NEW<T, A>(std::source_location::current(), __VA_ARGS__)
#define CORE_SYS_DELETE(T, ptr)					::mark::CORE_SYS_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_SYS_DELETE_A(T, ptr, A)			::mark::CORE_SYS_DELETE<T, A>(ptr)

// spool new / delete predefine
#define CORE_SPOOL_NEW(T)						::mark::CORE_SPOOL_NEW<T, sizeof(uintptr_t)>(std::source_location::current())
#define CORE_SPOOL_NEW_ARGS(T, ...)				::mark::CORE_SPOOL_NEW<T, sizeof(uintptr_t)>(__VA_ARGS__, std::source_location::current())
#define CORE_SPOOL_NEW_A(T, A)					::mark::CORE_SPOOL_NEW<T, A>(std::source_location::current())
#define CORE_SPOOL_NEW_ARGS_A(T, A, ...)		::mark::CORE_SPOOL_NEW<T, A>(std::source_location::current(), __VA_ARGS__)
#define CORE_SPOOL_DELETE(T, ptr)				::mark::CORE_SPOOL_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_SPOOL_DELETE_A(T, ptr, A)			::mark::CORE_SPOOL_DELETE<T, A>(ptr)

// upool new / delete predefine
#define CORE_UPOOL_NEW(T)						::mark::CORE_UPOOL_NEW<T, sizeof(uintptr_t)>(std::source_location::current())
#define CORE_UPOOL_NEW_ARGS(T, ...)				::mark::CORE_UPOOL_NEW<T, sizeof(uintptr_t)>(std::source_location::current(), __VA_ARGS__)
#define CORE_UPOOL_NEW_A(T, A)					::mark::CORE_UPOOL_NEW<T, A>(std::source_location::current())
#define CORE_UPOOL_NEW_ARGS_A(T, A, ...)		::mark::CORE_UPOOL_NEW<T, A>(std::source_location::current(), __VA_ARGS__)
#define CORE_UPOOL_DELETE(T, ptr)				::mark::CORE_UPOOL_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_UPOOL_DELETE_A(T, ptr, A)			::mark::CORE_UPOOL_DELETE<T, A>(ptr)

// temp new / delete predefine
#define CORE_TEMP_NEW(T)						::mark::CORE_TEMP_NEW<T, sizeof(uintptr_t)>(std::source_location::current())
#define CORE_TEMP_NEW_ARGS(T, ...)				::mark::CORE_TEMP_NEW<T, sizeof(uintptr_t)>(std::source_location::current(), __VA_ARGS__)
#define CORE_TEMP_NEW_A(T, A)					::mark::CORE_TEMP_NEW<T, A>(std::source_location::current())
#define CORE_TEMP_NEW_ARGS_A(T, A, ...)			::mark::CORE_TEMP_NEW<T, A>(std::source_location::current(), __VA_ARGS__)
#define CORE_TEMP_DELETE(T, ptr)				::mark::CORE_TEMP_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_TEMP_DELETE_A(T, ptr, A)			::mark::CORE_TEMP_DELETE<T, A>(ptr)

// sys new / delete array predefine
#define CORE_SYS_NEW_ARRAY(T, Count)			::mark::CORE_SYS_NEW_ARRAY<T, sizeof(uintptr_t), Count>(std::source_location::current())
#define CORE_SYS_NEW_ARRAY_ARGS(T, Count, ...)	::mark::CORE_SYS_NEW_ARRAY<T, sizeof(uintptr_t), Count>(std::source_location::current(), __VA_ARGS__)
#define CORE_SYS_NEW_ARRAY_A(T, A, Count)		::mark::CORE_SYS_NEW_ARRAY<T, A, Count>(std::source_location::current())
#define CORE_SYS_NEW_ARRAY_ARGS_A(T, A, Count, ...)	::mark::CORE_SYS_NEW_ARRAY<T, A, Count>(std::source_location::current(), __VA_ARGS__)
#define CORE_SYS_DELETE_ARRAY(T, ptr)			::mark::CORE_SYS_DELETE_ARRAY<T, sizeof(uintptr_t)>(ptr)
#define CORE_SYS_DELETE_ARRAY_A(T, ptr, A)		::mark::CORE_SYS_DELETE_ARRAY<T, A>(ptr)

#else

#define CORE_SYS_ALLOC_A(Bytes, Align)          ::mark::core_sys_alloc(Bytes, Align)
#define CORE_SYS_FREE_A(ptr, Align)             ::mark::core_sys_free(ptr, Align)
#define CORE_SYS_ALLOC(Bytes)		            CORE_SYS_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SYS_FREE(ptr)				        CORE_SYS_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_SPOOL_ALLOC_A(Bytes, Align)        ::mark::core_spool_alloc(Bytes, Align)
#define CORE_SPOOL_FREE_A(ptr, Align)           ::mark::core_spool_free(ptr, Align)
#define CORE_SPOOL_ALLOC(Bytes)                 CORE_SPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SPOOL_FREE(ptr)                    CORE_SPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_UPOOL_ALLOC_A(Bytes, Align)        ::mark::core_upool_alloc(Bytes, Align)
#define CORE_UPOOL_FREE_A(ptr, Align)           ::mark::core_upool_free(ptr, Align)
#define CORE_UPOOL_ALLOC(Bytes)                 CORE_UPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_UPOOL_FREE(ptr)                    CORE_UPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_TEMP_ALLOC_A(Bytes, Align)         ::mark::core_temp_alloc(Bytes, Align)
#define CORE_TEMP_ALLOC(Bytes)                  CORE_TEMP_ALLOC_A(Bytes, sizeof(uintptr_t))

// sys new / delete predefine
#define CORE_SYS_NEW(T)							::mark::CORE_SYS_NEW<T, sizeof(uintptr_t)>()
#define CORE_SYS_NEW_ARGS(T, ...)				::mark::CORE_SYS_NEW<T, sizeof(uintptr_t)>(__VA_ARGS__)
#define CORE_SYS_NEW_A(T, A)					::mark::CORE_SYS_NEW<T, A>()
#define CORE_SYS_NEW_ARGS_A(T, A, ...)			::mark::CORE_SYS_NEW<T, A>(__VA_ARGS__)
#define CORE_SYS_DELETE(T, ptr)					::mark::CORE_SYS_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_SYS_DELETE_A(T, ptr, A)			::mark::CORE_SYS_DELETE<T, A>(ptr)

// spool new / delete predefine
#define CORE_SPOOL_NEW(T)						::mark::CORE_SPOOL_NEW<T, sizeof(uintptr_t)>()
#define CORE_SPOOL_NEW_ARGS(T, ...)				::mark::CORE_SPOOL_NEW<T, sizeof(uintptr_t)>(__VA_ARGS__)
#define CORE_SPOOL_NEW_A(T, A)					::mark::CORE_SPOOL_NEW<T, A>()
#define CORE_SPOOL_NEW_ARGS_A(T, A, ...)		::mark::CORE_SPOOL_NEW<T, A>(__VA_ARGS__)
#define CORE_SPOOL_DELETE(T, ptr)				::mark::CORE_SPOOL_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_SPOOL_DELETE_A(T, ptr, A)			::mark::CORE_SPOOL_DELETE<T, A>(ptr)

// upool new / delete predefine
#define CORE_UPOOL_NEW(T)						::mark::CORE_UPOOL_NEW<T, sizeof(uintptr_t)>()
#define CORE_UPOOL_NEW_ARGS(T, ...)				::mark::CORE_UPOOL_NEW<T, sizeof(uintptr_t)>(__VA_ARGS__)
#define CORE_UPOOL_NEW_A(T, A)					::mark::CORE_UPOOL_NEW<T, A>()
#define CORE_UPOOL_NEW_ARGS_A(T, A, ...)		::mark::CORE_UPOOL_NEW<T, A>(__VA_ARGS__)
#define CORE_UPOOL_DELETE(T, ptr)				::mark::CORE_UPOOL_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_UPOOL_DELETE_A(T, ptr, A)			::mark::CORE_UPOOL_DELETE<T, A>(ptr)

// temp new / delete predefine
#define CORE_TEMP_NEW(T)						::mark::CORE_TEMP_NEW<T, sizeof(uintptr_t)>()
#define CORE_TEMP_NEW_ARGS(T, ...)				::mark::CORE_TEMP_NEW<T, sizeof(uintptr_t)>(__VA_ARGS__)
#define CORE_TEMP_NEW_A(T, A)					::mark::CORE_TEMP_NEW<T, A>()
#define CORE_TEMP_NEW_ARGS_A(T, A, ...)			::mark::CORE_TEMP_NEW<T, A>(__VA_ARGS__)
#define CORE_TEMP_DELETE(T, ptr)				::mark::CORE_TEMP_DELETE<T, sizeof(uintptr_t)>(ptr)
#define CORE_TEMP_DELETE_A(T, ptr, A)			::mark::CORE_TEMP_DELETE<T, A>(ptr)

// sys new / delete array predefine
#define CORE_SYS_NEW_ARRAY(T, Count)			::mark::CORE_SYS_NEW_ARRAY<T, sizeof(uintptr_t), Count>()
#define CORE_SYS_NEW_ARRAY_ARGS(T, Count, ...)	::mark::CORE_SYS_NEW_ARRAY<T, sizeof(uintptr_t), Count>(__VA_ARGS__)
#define CORE_SYS_NEW_ARRAY_A(T, A, Count)		::mark::CORE_SYS_NEW_ARRAY<T, A, Count>()
#define CORE_SYS_NEW_ARRAY_ARGS_A(T, A, Count, ...)	::mark::CORE_SYS_NEW_ARRAY<T, A, Count>(__VA_ARGS__)
#define CORE_SYS_DELETE_ARRAY(T, ptr)			::mark::CORE_SYS_DELETE_ARRAY<T, sizeof(uintptr_t)>(ptr)
#define CORE_SYS_DELETE_ARRAY_A(T, ptr, A)		::mark::CORE_SYS_DELETE_ARRAY<T, A>(ptr)

#endif // __MEMORY_TRACKING_ENABLED__
