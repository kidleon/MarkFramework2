#pragma once
#include <memory_resource>


#if defined(__MEMORY_TRACKING_ENABLED__)
#   include "MemoryTracker.h"
#   define MEM_TRACK_ALLOC(ptr, bytes, align, loc) mark::memory_tracker::get().on_allocate((ptr), (bytes), (align), (loc))
#   define MEM_TRACK_FREE(ptr) mark::memory_tracker::get().on_deallocate((ptr))
#else
#   define MEM_TRACK_ALLOC(ptr, bytes, align, loc) ((void)0)
#   define MEM_TRACK_FREE(ptr) ((void)0)
#endif

namespace mark
{
	bool initialize_core_memory(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	);

	void shutdown_core_memory();


	class system_memory_resource final : public std::pmr::memory_resource
	{
	private:
		void* do_allocate(size_t bytes, size_t alignment) final
		{
#if defined(__TARGET_OS_WINDOWS)
			return ::_aligned_malloc(bytes, alignment);
#else
			return std::aligned_alloc(alignment, bytes);
#endif // __TARGET_OS_WINDOWS
		}

		void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
#if defined(__TARGET_OS_WINDOWS)
			::_aligned_free(ptr);
#else
			std::free(ptr);
#endif // __TARGET_OS_WINDOWS
		}

		bool do_is_equal(const std::pmr::memory_resource& Other) const noexcept final
		{
			return this == &Other;
		}
	};

	/**
	* @brief 할당된 메모리 양에 제한을 두는 메모리 리소스
	*/
	class limited_memory_resource final : public std::pmr::memory_resource
	{
	public:
		explicit limited_memory_resource(size_t max_size)
			: m_limit(max_size)
			, m_used(0)
		{
			m_limited = (max_size > 0); // max_size가 0이면 제한이 없는 것으로 간주
		}

		[[nodiscard]] inline size_t get_limit() const noexcept { return m_limit.load(); }
		[[nodiscard]] inline size_t get_used() const noexcept { return m_used.load(); }
		[[nodiscard]] inline bool is_full() const noexcept { return m_limited && m_used.load() >= m_limit.load(); }

	private:
		inline void* do_allocate(size_t bytes, size_t alignment) final
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

#if defined(__TARGET_OS_WINDOWS)
			void* ptr = ::_aligned_malloc(bytes, alignment);
#else
			void* ptr = std::aligned_alloc(alignment, bytes);
#endif // __TARGET_OS_WINDOWS

			if (!ptr) [[unlikely]]
				return nullptr;

			m_used += aligned_bytes;

			return ptr;
		}

		inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
			size_t aligned_bytes = ALIGN_UP(bytes, alignment);
			m_used -= aligned_bytes;

#if defined(__TARGET_OS_WINDOWS)
			::_aligned_free(ptr);
#else
			std::free(ptr);
#endif // __TARGET_OS_WINDOWS
		}

		inline bool do_is_equal(const std::pmr::memory_resource& Other) const noexcept final
		{
			return this == &Other;
		}

	private:
		std::atomic<size_t> m_limit;
		std::atomic<size_t> m_used;
		bool m_limited; // true if m_limit is a valid limit, false if m_limit is ignored (unlimited)
	};

#if defined(__MEMORY_LIMIT_ENABLED__)
	using select_system_memory_resource = limited_memory_resource;
#else
	using select_system_memory_resource = system_memory_resource;
#endif // __MEMORY_LIMIT_ENABLED__

	class sync_pool_memory_resource;
	class usync_pool_memory_resource;
	class temp_pool_memory_resource;

	[[nodiscard]] extern select_system_memory_resource* get_default_system_memory_resource_ptr() noexcept;
	[[nodiscard]] extern sync_pool_memory_resource* get_default_sync_pool_memory_resource_ptr() noexcept;
	[[nodiscard]] extern usync_pool_memory_resource* get_default_usync_pool_memory_resource_ptr() noexcept;
	[[nodiscard]] extern temp_pool_memory_resource* get_default_temp_memory_resource_ptr() noexcept;

	class sync_pool_memory_resource final : public std::pmr::synchronized_pool_resource
	{
	public:
		explicit sync_pool_memory_resource(const std::pmr::pool_options& options = std::pmr::pool_options())
			: std::pmr::synchronized_pool_resource(options, get_default_system_memory_resource_ptr())
		{
		}
	};

	class usync_pool_memory_resource final : public std::pmr::unsynchronized_pool_resource
	{
	public:
		explicit usync_pool_memory_resource(const std::pmr::pool_options& options = std::pmr::pool_options())
			: std::pmr::unsynchronized_pool_resource(options, get_default_system_memory_resource_ptr())
		{
		}
	};

	class temp_pool_memory_resource final : public std::pmr::monotonic_buffer_resource
	{
	public:
		explicit temp_pool_memory_resource(size_t initial_size)
			: std::pmr::monotonic_buffer_resource(initial_size, get_default_system_memory_resource_ptr())
		{
		}

		explicit temp_pool_memory_resource(size_t initial_size, std::pmr::memory_resource* upstream)
			: std::pmr::monotonic_buffer_resource(initial_size, upstream)
		{
		}

		explicit temp_pool_memory_resource(void* buffer, size_t buffer_size, std::pmr::memory_resource* upstream)
			: std::pmr::monotonic_buffer_resource(buffer, buffer_size, upstream)
		{
		}
	};


	namespace private_core_detail
	{
		// 힙 할당 시 헤더 구조체 정의
		struct heap_header
		{
			uint32_t total_bytes; // 전체 할당 크기 (헤더 + 데이터)
			uint32_t count : 24; // 원소 수 (24비트로 최대 16M 원소 지원)
			uint32_t alignment : 8; // 향후 확장용 (예: 정렬)
		};

		// 헤더 크기는 heap_header 구조체 크기와 alignment 중 큰 값으로 결정
		inline size_t get_heap_header_size(size_t alignment) noexcept
		{
			return std::max(sizeof(heap_header), alignment);
		}

		// 헤더 포함 할당 구현
		template<typename T>
		inline void* alloc_impl_with_header(
			T* res,
			size_t bytes,
			size_t count,
			size_t alignment
#if defined(__MEMORY_TRACKING_ENABLED__)
			, std::source_location loc
#endif // __MEMORY_TRACKING_ENABLED__
		) noexcept
		{
			static_assert(std::is_base_of_v<std::pmr::memory_resource, T>, "res must be a memory_resource");

			const size_t header_size = get_heap_header_size(alignment);
			size_t total_bytes = header_size + bytes;

			void* raw_ptr = res->allocate(total_bytes, alignment);
			if (!raw_ptr) [[unlikely]] return nullptr;

			heap_header* header = static_cast<heap_header*>(raw_ptr);
			header->total_bytes = static_cast<uint32_t>(total_bytes);
			header->count = count;
			header->alignment = static_cast<uint8_t>(alignment); // 향후 확장용 (예: 정렬 정보 저장)

			// 실제 데이터는 헤더 바로 뒤에 위치
			void* data_ptr = (char*)raw_ptr + header_size;

#if defined(__MEMORY_TRACKING_ENABLED__)
			MEM_TRACK_ALLOC(raw_ptr, total_bytes, alignment, loc);
#endif // __MEMORY_TRACKING_ENABLED__

			return data_ptr;
		}

		// 헤더 포함 해제 구현
		template<typename T>
		inline void free_impl_with_header(
			T* res,
			void* ptr,
			size_t alignment
		) noexcept
		{
			static_assert(std::is_base_of_v<std::pmr::memory_resource, T>, "res must be a memory_resource");
			//size_t alignment = *((char*)ptr - 1); // 헤더 바로 뒤에 데이터가 있으므로, 데이터 주소에서 1바이트 뒤로 가면 정렬 정보가 있다.

			const size_t header_size = get_heap_header_size(alignment);
			void* raw_ptr = (char*)ptr - header_size;

			MEM_TRACK_FREE(raw_ptr);

			heap_header* header = static_cast<heap_header*>(raw_ptr);
			res->deallocate(raw_ptr, header->total_bytes, header->alignment);
		}
	} // namespace private_core_detail

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

	[[nodiscard]] extern void* core_sys_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] extern void* core_spool_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] extern void* core_upool_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	[[nodiscard]] extern void* core_temp_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);
#else
	[[nodiscard]] extern void* core_sys_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] extern void* core_spool_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] extern void* core_upool_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] extern void* core_temp_alloc(size_t bytes, size_t alignment);

	[[nodiscard]] extern void* core_sys_alloc(size_t bytes, size_t count, size_t alignment);
	[[nodiscard]] extern void* core_spool_alloc(size_t bytes, size_t count, size_t alignment);
	[[nodiscard]] extern void* core_upool_alloc(size_t bytes, size_t count, size_t alignment);
	[[nodiscard]] extern void* core_temp_alloc(size_t bytes, size_t count, size_t alignment);

#endif // __MEMORY_TRACKING_ENABLED__

	extern void core_sys_free(void* ptr, size_t alignment);
	extern void core_spool_free(void* ptr, size_t alignment);
	extern void core_upool_free(void* ptr, size_t alignment);
	extern void core_temp_reset();
	[[nodiscard]] extern size_t get_alloc_count(void* ptr, size_t alignment);

	// temppool 관련 함수
	[[nodiscard]] extern HANDLE temppool_create(size_t initial_size);
	extern void temppool_destroy(HANDLE pool_handle);
	[[nodiscard]] extern void* temppool_alloc(HANDLE pool_handle, size_t bytes, size_t alignment);
	extern void temppool_reset(HANDLE pool_handle);
	
}

// malloc/free 스타일의 간단한 인터페이스 매크로 정의 (정렬은 uintptr_t 크기로 기본값 설정)
#if defined(__MEMORY_TRACKING_ENABLED__)
#define CORE_SYS_ALLOC_A(Bytes, Align)          ::mark::core_sys_alloc(Bytes, Align, std::source_location::current())
#define CORE_SYS_FREE_A(ptr, Align)             ::mark::core_sys_free(ptr, Align)
#define CORE_SYS_ALLOC(Bytes)		            CORE_SYS_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SYS_FREE(ptr)				        CORE_SYS_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_SYS_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_sys_alloc(Bytes, Count, Align, std::source_location::current())
#define CORE_SYS_ALLOC_ARRAY(Bytes, Count)		CORE_SYS_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#define CORE_SYS_FREE_ARRAY(ptr, Align)			::mark::core_sys_free(ptr, Align)

#define CORE_SPOOL_ALLOC_A(Bytes, Align)        ::mark::core_spool_alloc(Bytes, Align, std::source_location::current())
#define CORE_SPOOL_FREE_A(ptr, Align)           ::mark::core_spool_free(ptr, Align)
#define CORE_SPOOL_ALLOC(Bytes)                 CORE_SPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SPOOL_FREE(ptr)                    CORE_SPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_SPOOL_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_spool_alloc(Bytes, Count, Align, std::source_location::current())
#define CORE_SPOOL_ALLOC_ARRAY(Bytes, Count)    CORE_SPOOL_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#define CORE_SPOOL_FREE_ARRAY(ptr, Align)       ::mark::core_spool_free(ptr, Align)

#define CORE_UPOOL_ALLOC_A(Bytes, Align)        ::mark::core_upool_alloc(Bytes, Align, std::source_location::current())
#define CORE_UPOOL_FREE_A(ptr, Align)           ::mark::core_upool_free(ptr, Align)
#define CORE_UPOOL_ALLOC(Bytes)                 CORE_UPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_UPOOL_FREE(ptr)                    CORE_UPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_UPOOL_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_upool_alloc(Bytes, Count, Align, std::source_location::current())
#define CORE_UPOOL_ALLOC_ARRAY(Bytes, Count)    CORE_UPOOL_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#define CORE_UPOOL_FREE_ARRAY(ptr, Align)       ::mark::core_upool_free(ptr, Align)

#define CORE_TEMP_ALLOC_A(Bytes, Align)         ::mark::core_temp_alloc(Bytes, Align, std::source_location::current())
#define CORE_TEMP_ALLOC(Bytes)                  CORE_TEMP_ALLOC_A(Bytes, sizeof(uintptr_t))

#define CORE_TEMP_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_temp_alloc(Bytes, Count, Align, std::source_location::current())
#define CORE_TEMP_ALLOC_ARRAY(Bytes, Count)     CORE_TEMP_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))

#else
#define CORE_SYS_ALLOC_A(Bytes, Align)          ::mark::core_sys_alloc(Bytes, Align)
#define CORE_SYS_FREE_A(ptr, Align)             ::mark::core_sys_free(ptr, Align)
#define CORE_SYS_ALLOC(Bytes)		            CORE_SYS_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SYS_FREE(ptr)				        CORE_SYS_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_SYS_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_sys_alloc(Bytes, Count, Align)
#define CORE_SYS_ALLOC_ARRAY(Bytes, Count)		CORE_SYS_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#define CORE_SYS_FREE_ARRAY(ptr, Align)			::mark::core_sys_free(ptr, Align)

#define CORE_SPOOL_ALLOC_A(Bytes, Align)        ::mark::core_spool_alloc(Bytes, Align)
#define CORE_SPOOL_FREE_A(ptr, Align)           ::mark::core_spool_free(ptr, Align)
#define CORE_SPOOL_ALLOC(Bytes)                 CORE_SPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_SPOOL_FREE(ptr)                    CORE_SPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_SPOOL_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_spool_alloc(Bytes, Count, Align)
#define CORE_SPOOL_ALLOC_ARRAY(Bytes, Count)    CORE_SPOOL_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#define CORE_SPOOL_FREE_ARRAY(ptr, Align)       ::mark::core_spool_free(ptr, Align)

#define CORE_UPOOL_ALLOC_A(Bytes, Align)        ::mark::core_upool_alloc(Bytes, Align)
#define CORE_UPOOL_FREE_A(ptr, Align)           ::mark::core_upool_free(ptr, Align)
#define CORE_UPOOL_ALLOC(Bytes)                 CORE_UPOOL_ALLOC_A(Bytes, sizeof(uintptr_t))
#define CORE_UPOOL_FREE(ptr)                    CORE_UPOOL_FREE_A(ptr, sizeof(uintptr_t))

#define CORE_UPOOL_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_upool_alloc(Bytes, Count, Align)
#define CORE_UPOOL_ALLOC_ARRAY(Bytes, Count)    CORE_UPOOL_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#define CORE_UPOOL_FREE_ARRAY(ptr, Align)       ::mark::core_upool_free(ptr, Align)

#define CORE_TEMP_ALLOC_A(Bytes, Align)         ::mark::core_temp_alloc(Bytes, Align)
#define CORE_TEMP_ALLOC(Bytes)                  CORE_TEMP_ALLOC_A(Bytes, sizeof(uintptr_t))

#define CORE_TEMP_ALLOC_ARRAY_A(Bytes, Count, Align)  ::mark::core_temp_alloc(Bytes, Count, Align)
#define CORE_TEMP_ALLOC_ARRAY(Bytes, Count)     CORE_TEMP_ALLOC_ARRAY_A(Bytes, Count, sizeof(uintptr_t))
#endif // __MEMORY_TRACKING_ENABLED__

// sys new / delete predefine
#define CORE_SYS_NEW(T)							new (CORE_SYS_ALLOC(sizeof(T))) T
#define CORE_SYS_NEW_A(T, A)					new (CORE_SYS_ALLOC_A(sizeof(T), A)) T
#define CORE_SYS_DELETE(T, ptr)					{T* p = ptr; if(p) { (p)->~T(); CORE_SYS_FREE(p); ptr = nullptr;} }
#define CORE_SYS_DELETE_A(T, ptr, A)			{T* p = ptr; if(p) { (p)->~T(); CORE_SYS_FREE_A(p, A); ptr = nullptr;} }

// spool new / delete predefine
#define CORE_SPOOL_NEW(T)						new (CORE_SPOOL_ALLOC(sizeof(T))) T
#define CORE_SPOOL_NEW_A(T, A)					new (CORE_SPOOL_ALLOC_A(sizeof(T), A)) T
#define CORE_SPOOL_DELETE(T, ptr)				{T* p = ptr; if(p) { (p)->~T(); CORE_SPOOL_FREE(p); ptr = nullptr;} }
#define CORE_SPOOL_DELETE_A(T, ptr, A)			{T* p = ptr; if(p) { (p)->~T(); CORE_SPOOL_FREE_A(p, A); ptr = nullptr;} }

// upool new / delete predefine
#define CORE_UPOOL_NEW(T)						new (CORE_UPOOL_ALLOC(sizeof(T))) T
#define CORE_UPOOL_NEW_A(T, A)					new (CORE_UPOOL_ALLOC_A(sizeof(T), A)) T
#define CORE_UPOOL_DELETE(T, ptr)				{T* p = ptr; if(p) { (p)->~T(); CORE_UPOOL_FREE(p); ptr = nullptr;} }
#define CORE_UPOOL_DELETE_A(T, ptr, A)			{T* p = ptr; if(p) { (p)->~T(); CORE_UPOOL_FREE_A(p, A); ptr = nullptr;} }

// temp new / delete predefine
#define CORE_TEMP_NEW(T)						new (CORE_TEMP_ALLOC(sizeof(T))) T
#define CORE_TEMP_NEW_A(T, A)					new (CORE_TEMP_ALLOC_A(sizeof(T), A)) T
#define CORE_TEMP_DELETE(T, ptr)				{T* p = ptr; if(p) { (p)->~T(); ptr = nullptr;} }
#define CORE_TEMP_DELETE_A(T, ptr, A)			{T* p = ptr; if(p) { (p)->~T(); ptr = nullptr;} }

// sys new / delete array predefine
#define CORE_SYS_NEW_ARRAY(T, Count)			new (CORE_SYS_ALLOC_ARRAY_A(sizeof(T), Count, sizeof(uintptr_t))) T[Count]
#define CORE_SYS_NEW_ARRAY_A(T, A, Count)		new (CORE_SYS_ALLOC_ARRAY_A(sizeof(T), Count, A)) T[Count]
#define CORE_SYS_DELETE_ARRAY(T, ptr)			{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } CORE_SYS_FREE_ARRAY(p, sizeof(uintptr_t)); ptr = nullptr;} }
#define CORE_SYS_DELETE_ARRAY_A(T, ptr, A)		{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } CORE_SYS_FREE_ARRAY_A(p, A); ptr = nullptr;} }

// spool new delete array predefine
#define CORE_SPOOL_NEW_ARRAY(T, Count)			new (CORE_SPOOL_ALLOC_ARRAY_A(sizeof(T), Count, sizeof(uintptr_t))) T[Count]
#define CORE_SPOOL_NEW_ARRAY_A(T, A, Count)		new (CORE_SPOOL_ALLOC_ARRAY_A(sizeof(T), Count, A)) T[Count]
#define CORE_SPOOL_DELETE_ARRAY(T, ptr)			{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } CORE_SPOOL_FREE_ARRAY(p, sizeof(uintptr_t)); ptr = nullptr;} }
#define CORE_SPOOL_DELETE_ARRAY_A(T, ptr, A)	{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } CORE_SPOOL_FREE_ARRAY_A(p, A); ptr = nullptr;} }

// upool new delete array predefine
#define CORE_UPOOL_NEW_ARRAY(T, Count)			new (CORE_UPOOL_ALLOC_ARRAY_A(sizeof(T), Count, sizeof(uintptr_t))) T[Count]
#define CORE_UPOOL_NEW_ARRAY_A(T, A, Count)		new (CORE_UPOOL_ALLOC_ARRAY_A(sizeof(T), Count, A)) T[Count]
#define CORE_UPOOL_DELETE_ARRAY(T, ptr)			{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } CORE_UPOOL_FREE_ARRAY(p, sizeof(uintptr_t)); ptr = nullptr;} }
#define CORE_UPOOL_DELETE_ARRAY_A(T, ptr, A)	{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } CORE_UPOOL_FREE_ARRAY_A(p, A); ptr = nullptr;} }

// temp new delete array predefine
#define CORE_TEMP_NEW_ARRAY(T, Count)			new (CORE_TEMP_ALLOC_ARRAY_A(sizeof(T), Count, sizeof(uintptr_t))) T[Count]
#define CORE_TEMP_NEW_ARRAY_A(T, A, Count)		new (CORE_TEMP_ALLOC_ARRAY_A(sizeof(T), Count, A)) T[Count]
#define CORE_TEMP_DELETE_ARRAY(T, ptr)			{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); }  ptr = nullptr;} }
#define CORE_TEMP_DELETE_ARRAY_A(T, ptr, A)	{T* p = ptr; if(p) { size_t count = get_alloc_count(ptr, Alignment); for(size_t i = 0; i < count; ++i) { (p[i]).~T(); } ptr = nullptr;} }




