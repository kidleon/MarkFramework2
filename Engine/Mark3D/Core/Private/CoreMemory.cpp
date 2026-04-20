#include "pch.h"
#include "CoreMemory.h"


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
	static std::pmr::memory_resource* s_new_delete_memory = nullptr;
	static std::pmr::memory_resource* s_default_system_memory = nullptr;
	static limited_memory_resource* s_default_limited_memory = nullptr;
	static std::pmr::synchronized_pool_resource* s_default_sync_pool_memory = nullptr;
	static std::pmr::unsynchronized_pool_resource* s_default_unsync_pool_memory = nullptr;
	static std::pmr::monotonic_buffer_resource* s_default_temp_memory = nullptr;

	bool initialize_core_memory(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	)
	{
		try
		{
			if (!s_new_delete_memory)
			{
				s_new_delete_memory = std::pmr::new_delete_resource();
			}

			if (!s_default_limited_memory)
			{
#if defined(__MEMORY_LIMIT_ENABLED__)
				s_default_limited_memory = new limited_memory_resource(
					limited_memory_size,
					static_cast<std::pmr::memory_resource*>(s_new_delete_memory)
				);

				std::pmr::set_default_resource(s_default_limited_memory);
#else
				std::pmr::set_default_resource(s_new_delete_memory);

#endif // __MEMORY_LIMIT_ENABLED__
			}

			if (!s_default_system_memory)
			{
				s_default_system_memory = std::pmr::get_default_resource();
			}

			if (!s_default_sync_pool_memory)
			{
				std::pmr::pool_options sync_pool_options;
				sync_pool_options.max_blocks_per_chunk = sync_pool_count_per_chunk;
				sync_pool_options.largest_required_pool_block = sync_pool_max_size_per_block;
				s_default_sync_pool_memory = new std::pmr::synchronized_pool_resource(
					sync_pool_options,
					s_default_system_memory
				);
			}

			if (!s_default_unsync_pool_memory)
			{
				std::pmr::pool_options unsync_pool_options;
				unsync_pool_options.max_blocks_per_chunk = unsync_pool_count_per_chunk;
				unsync_pool_options.largest_required_pool_block = unsync_pool_max_size_per_block;
				s_default_unsync_pool_memory = new std::pmr::unsynchronized_pool_resource(
					unsync_pool_options,
					s_default_system_memory
				);
			}

			if (!s_default_temp_memory)
			{
				s_default_temp_memory = new std::pmr::monotonic_buffer_resource(
					temp_buffer_size,
					s_default_system_memory
				);
			}

			return true;
		}
		catch (const std::exception& e)
		{
			assert(false && e.what());

			shutdown_core_memory();

			return false;
		}
	}

	void shutdown_core_memory()
	{
		CHECK_DELETE(s_default_temp_memory);
		CHECK_DELETE(s_default_unsync_pool_memory);
		CHECK_DELETE(s_default_sync_pool_memory);
		CHECK_DELETE(s_default_limited_memory);
		s_default_system_memory = nullptr;
		s_new_delete_memory = nullptr;
	}


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
		inline void* alloc_impl_with_header(
			std::pmr::memory_resource* res,
			size_t bytes,
			size_t count,
			size_t alignment
#if defined(__MEMORY_TRACKING_ENABLED__)
			, std::source_location loc
#endif // __MEMORY_TRACKING_ENABLED__
		) noexcept
		{
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
		inline void free_impl_with_header(
			std::pmr::memory_resource* res,
			void* ptr,
			size_t alignment
		) noexcept
		{
			//size_t alignment = *((char*)ptr - 1); // 헤더 바로 뒤에 데이터가 있으므로, 데이터 주소에서 1바이트 뒤로 가면 정렬 정보가 있다.

			const size_t header_size = get_heap_header_size(alignment);
			void* raw_ptr = (char*)ptr - header_size;

			MEM_TRACK_FREE(raw_ptr);

			heap_header* header = static_cast<heap_header*>(raw_ptr);
			res->deallocate(raw_ptr, header->total_bytes, header->alignment);
		}

		// 헤더 없이 단순히 bytes, alignment 만으로 할당하는 구현
		inline void* alloc_impl(
			std::pmr::memory_resource* res,
			size_t bytes,
			size_t alignment
#if defined(__MEMORY_TRACKING_ENABLED__)
			, std::source_location loc
#endif // __MEMORY_TRACKING_ENABLED__
		) noexcept
		{
			void* ptr = res->allocate(bytes, alignment);
			if (!ptr) [[unlikely]] return nullptr;

#if defined(__MEMORY_TRACKING_ENABLED__)
			MEM_TRACK_ALLOC(ptr, bytes, alignment, loc);
#endif // __MEMORY_TRACKING_ENABLED__

			return ptr;
		}

		// 헤더 없이 단순히 ptr, bytes, alignment 만으로 해제하는 구현
		inline void free_impl(
			std::pmr::memory_resource* res,
			void* ptr,
			size_t bytes,
			size_t alignment
		) noexcept
		{
			MEM_TRACK_FREE(ptr);
			res->deallocate(ptr, bytes, alignment);
		}

		// NEW_ARRAY (기본 생성자), DELETE_ARRAY IMPLEMENTs
		template<typename _T, size_t Alignment = alignof(_T)>
		inline _T* T_CORE_NEW_ARRAY_IMPL(
			std::pmr::memory_resource* res,
			size_t Count
#if defined(__MEMORY_TRACKING_ENABLED__)
			, std::source_location loc = std::source_location::current()
#endif // __MEMORY_TRACKING_ENABLED__
		) noexcept(false)
		{
			if (!Count) [[unlikely]] return nullptr;

			_T* ptr = (_T*)alloc_impl_with_header(
				res,
				sizeof(_T) * Count,
				Count,
				Alignment
#if defined(__MEMORY_TRACKING_ENABLED__)
				, loc
#endif // defined(__MEMORY_TRACKING_ENABLED__)
			);

			if (!ptr) [[unlikely]] return nullptr;

			for (size_t i = 0; i < Count; ++i)
				::new (ptr + i) _T();

			return static_cast<_T*>(ptr);
		}

		// NEW_ARRAY, DELETE_ARRAY 
		template<typename _T, size_t Alignment = alignof(_T), typename... _Args>
		[[nodiscard]] inline _T* T_CORE_NEW_ARRAY_IMPL(
			std::pmr::memory_resource* res,
			size_t Count,
			_Args&&... args
#if defined(__MEMORY_TRACKING_ENABLED__)
			, std::source_location loc = std::source_location::current()
#endif // __MEMORY_TRACKING_ENABLED__
		) noexcept(false)
		{
			if (!Count) [[unlikely]] return nullptr;

			_T* ptr = (_T*)alloc_impl_with_header(
				res,
				sizeof(_T) * Count,
				Count,
				Alignment
#if defined(__MEMORY_TRACKING_ENABLED__)
				, loc
#endif // defined(__MEMORY_TRACKING_ENABLED__)
			);

			if (!ptr) [[unlikely]] return nullptr;

			for (size_t i = 0; i < Count; ++i)
				::new (ptr + i) _T(std::forward<_Args>(args)...);

			return static_cast<_T*>(ptr);
		}

		template<typename _T, size_t Alignment = alignof(_T)>
		[[nodiscard]] inline void T_CORE_DELETE_ARRAY_IMPL(
			std::pmr::memory_resource* res,
			_T* Ptr
		) noexcept(false)
		{
			if (!Ptr) [[unlikely]] return;

			heap_header* header = (heap_header*)((char*)Ptr - get_heap_header_size(Alignment));

			size_t count = header->count;
			for (size_t i = count; i > 0; --i)
				Ptr[i - 1].~_T();

			free_impl_with_header(
				res,
				Ptr,
				Alignment
			);
		}
	} // namespace private_core_detail

#ifndef __MEMORY_TRACKING_ENABLED__
	void* core_sys_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_system_memory,
			bytes,
			1,
			alignment
		);
	}

	void* core_spool_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_sync_pool_memory,
			bytes,
			1,
			alignment
		);
	}

	void* core_upool_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_unsync_pool_memory,
			bytes,
			1,
			alignment
		);
	}

	void* core_temp_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_temp_memory,
			bytes,
			1,
			alignment
		);
	}
#endif // __MEMORY_TRACKING_ENABLED__


	void core_sys_free(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return;

		private_core_detail::free_impl_with_header(
			s_default_system_memory,
			ptr,
			alignment
		);
	}

	void core_spool_free(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return;

		private_core_detail::free_impl_with_header(
			s_default_sync_pool_memory,
			ptr,
			alignment
		);
	}

	void core_upool_free(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return;

		private_core_detail::free_impl_with_header(
			s_default_unsync_pool_memory,
			ptr,
			alignment
		);
	}

	

	void core_temp_reset()
	{
		if (s_default_temp_memory)
		{
			static_cast<std::pmr::monotonic_buffer_resource*>(s_default_temp_memory)->release();
		}
	}

	size_t get_alloc_count(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return 0;
		private_core_detail::heap_header* header =
			(private_core_detail::heap_header*)((char*)ptr - private_core_detail::get_heap_header_size(alignment));
		return header->count;
	}

#if defined(__MEMORY_TRACKING_ENABLED__)
	void* core_sys_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_system_memory,
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_spool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_sync_pool_memory,
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_upool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_unsync_pool_memory,
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_temp_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			s_default_temp_memory,
			bytes,
			1,
			alignment,
			loc
		);
	}
#endif // __MEMORY_TRACKING_ENABLED__
}
