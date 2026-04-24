#pragma once
#include <source_location>


namespace mark
{
	MARKENGINE_API bool initialize_core_memory2(
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	);

	MARKENGINE_API void shutdown_core_memory2();

	// source_location 버전 (디버그용)
	[[nodiscard]] MARKENGINE_API void* sys_alloc(size_t bytes, size_t alignment, std::source_location loc);
	[[nodiscard]] MARKENGINE_API void* spool_alloc(size_t bytes, size_t alignment, std::source_location loc);
	[[nodiscard]] MARKENGINE_API void* upool_alloc(size_t bytes, size_t alignment, std::source_location loc);

	[[nodiscard]] MARKENGINE_API void* sys_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] MARKENGINE_API void* spool_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] MARKENGINE_API void* upool_alloc(size_t bytes, size_t alignment);

	MARKENGINE_API void sys_free(void* ptr, size_t bytes, size_t alignment);
	MARKENGINE_API void spool_free(void* ptr, size_t bytes, size_t alignment);
	MARKENGINE_API void upool_free(void* ptr, size_t bytes, size_t alignment);

	[[nodiscard]] MARKENGINE_API void* temp_alloc(size_t bytes, size_t alignment);
	MARKENGINE_API void temp_reset();

	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_default_system_memory_resource_ptr() noexcept;
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_default_spool_memory_resource_ptr() noexcept;
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_default_usync_pool_memory_resource_ptr() noexcept;
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_default_temp_memory_resource_ptr() noexcept;

	namespace implements
	{
		// sys_new/sys_delete 스타일의 실제 실행 함수들
		template<typename T, size_t Align>
		[[nodiscard]] inline T* sys_new_impl(size_t count, std::source_location loc)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(sys_alloc(sizeof(T) * count, Align, loc));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		[[nodiscard]] inline T* sys_new_impl(size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(sys_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* sys_new_impl(size_t count, std::source_location loc, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(sys_alloc(sizeof(T) * count, Align, loc));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* sys_new_impl(size_t count, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(sys_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		inline void sys_delete_impl(T* ptr, size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			if (ptr)
			{
				for (size_t i = 0; i < count; ++i)
					ptr[i].~T();
				sys_free(ptr, sizeof(T) * count, Align);
			}
		}

		// spool_new/spool_delete 스타일의 실제 실행 함수들
		template<typename T, size_t Align>
		[[nodiscard]] inline T* spool_new_impl(size_t count, std::source_location loc)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(spool_alloc(sizeof(T) * count, Align, loc));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		[[nodiscard]] inline T* spool_new_impl(size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(spool_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* spool_new_impl(size_t count, std::source_location loc, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(spool_alloc(sizeof(T) * count, Align, loc));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* spool_new_impl(size_t count, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(spool_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		inline void spool_delete_impl(T* ptr, size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			if (ptr)
			{
				for (size_t i = 0; i < count; ++i)
					ptr[i].~T();
				spool_free(ptr, sizeof(T) * count, Align);
			}
		}

		// upool_new/spool_delete 스타일의 실제 실행 함수들
		template<typename T, size_t Align>
		[[nodiscard]] inline T* upool_new_impl(size_t count, std::source_location loc)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(upool_alloc(sizeof(T) * count, Align, loc));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		[[nodiscard]] inline T* upool_new_impl(size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(upool_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* upool_new_impl(size_t count, std::source_location loc, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(upool_alloc(sizeof(T) * count, Align, loc));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* upool_new_impl(size_t count, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(upool_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		inline void upool_delete_impl(T* ptr, size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			if (ptr)
			{
				for (size_t i = 0; i < count; ++i)
					ptr[i].~T();
				upool_free(ptr, sizeof(T) * count, Align);
			}
		}

		// temp_new/temp_delete 스타일의 실제 실행 함수들
		template<typename T, size_t Align>
		[[nodiscard]] inline T* temp_new_impl(size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(temp_alloc(sizeof(T) * count, Align));
			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T();

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, typename... Args>
		[[nodiscard]] inline T* temp_new_impl(size_t count, Args&&... args)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			void* p = static_cast<T*>(temp_alloc(sizeof(T) * count, Align));
			if (!p)
			{
				assert(false && "temp_alloc failed in temp_new_impl");
				return nullptr;
			}

			for (size_t i = 0; i < count; ++i)
				new (static_cast<T*>(p) + i) T(std::forward<Args>(args)...);

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align>
		inline void temp_delete_impl(T* ptr, size_t count)
		{
			static_assert(Align % 2 == 0, "Alignment must be a power of 2");

			// temp 메모리는 reset 시점에 한 번에 해제되므로, 개별 객체 해제는 소멸자 호출만 수행한다.
			if (ptr)
			{
				for (size_t i = 0; i < count; ++i)
					ptr[i].~T();
			}
		}

	}

	//--------------------------------------------------------------------------------------
	// Alloc/Free APIs

	//--------------------------------------------------------------------------------
	// sys_new/sys_delete 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new()
	{
		return implements::sys_new_impl<T, Align>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new()
	{
		return sys_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new(Args&&... args)
	{
		return implements::sys_new_impl<T, Align, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new(Args&&... args)
	{
		return sys_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new(std::source_location loc)
	{
		return implements::sys_new_impl<T, Align>(1, loc);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new(std::source_location loc)
	{
		return sys_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new(std::source_location loc, Args&&... args)
	{
		return implements::sys_new_impl<T, Align, Args...>(1, loc, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new(std::source_location loc, Args&&... args)
	{
		return sys_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	inline void sys_delete(T* ptr)
	{
		implements::sys_delete_impl<T, Align>(ptr, 1);
	}

	template<typename T>
	inline void sys_delete(T* ptr)
	{
		sys_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// sys_new_array/sys_delete_array 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new_array(size_t count)
	{
		return implements::sys_new_impl<T, Align>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new_array(size_t count)
	{
		return sys_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new_array(size_t count, Args&&... args)
	{
		return implements::sys_new_impl<T, Align, Args...>(count);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new_array(size_t count, Args&&... args)
	{
		return sys_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new_array(size_t count, std::source_location loc)
	{
		return implements::sys_new_impl<T, Align>(count, loc);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new_array(size_t count, std::source_location loc)
	{
		return sys_new_array<T, alignof(T)>(count, loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new_array(size_t count, std::source_location loc, Args&&... args)
	{
		return implements::sys_new_impl<T, Align, Args...>(count, loc, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new_array(size_t count, std::source_location loc, Args&&... args)
	{
		return sys_new_array<T, alignof(T), Args...>(count, loc, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	void sys_delete_array(T* ptr, size_t count)
	{
		implements::sys_delete_impl<T, Align>(ptr, count);
	}

	template<typename T>
	void sys_delete_array(T* ptr, size_t count)
	{
		sys_delete_array<T, alignof(T)>(ptr, count);
	}

	//--------------------------------------------------------------------------------
	// spool_new/spool_delete 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new()
	{
		return implements::spool_new_impl<T, Align>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new()
	{
		return spool_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new(Args&&... args)
	{
		return implements::spool_new_impl<T, Align, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new(Args&&... args)
	{
		return spool_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new(std::source_location loc)
	{
		return implements::spool_new_impl<T, Align>(1, loc);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new(std::source_location loc)
	{
		return spool_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new(std::source_location loc, Args&&... args)
	{
		return implements::spool_new_impl<T, Align, Args...>(1, loc, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new(std::source_location loc, Args&&... args)
	{
		return spool_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	inline void spool_delete(T* ptr)
	{
		implements::spool_delete_impl<T, Align>(ptr, 1);
	}

	template<typename T>
	inline void spool_delete(T* ptr)
	{
		spool_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// spool_new_array/spool_delete_array 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new_array(size_t count)
	{
		return implements::spool_new_impl<T, Align>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new_array(size_t count)
	{
		return spool_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new_array(size_t count, Args&&... args)
	{
		return implements::spool_new_impl<T, Align, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new_array(size_t count, Args&&... args)
	{
		return spool_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new_array(size_t count, std::source_location loc)
	{
		return implements::spool_new_impl<T, Align>(count, loc);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new_array(size_t count, std::source_location loc)
	{
		return spool_new_array<T, alignof(T)>(count, loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new_array(size_t count, std::source_location loc, Args&&... args)
	{
		return implements::spool_new_impl<T, Align, Args...>(count, loc, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new_array(size_t count, std::source_location loc, Args&&... args)
	{
		return spool_new_array<T, alignof(T), Args...>(count, loc, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	void spool_delete_array(T* ptr, size_t count)
	{
		implements::spool_delete_impl<T, Align>(ptr, count);
	}

	template<typename T>
	void spool_delete_array(T* ptr, size_t count)
	{
		spool_delete_array<T, alignof(T)>(ptr, count);
	}

	//--------------------------------------------------------------------------------
	// upool_new/spool_delete 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new()
	{
		return implements::upool_new_impl<T, Align>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new()
	{
		return upool_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new(Args&&... args)
	{
		return implements::upool_new_impl<T, Align, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new(Args&&... args)
	{
		return upool_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new(std::source_location loc)
	{
		return implements::upool_new_impl<T, Align>(1, loc);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new(std::source_location loc)
	{
		return upool_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new(std::source_location loc, Args&&... args)
	{
		return implements::upool_new_impl<T, Align, Args...>(1, loc, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new(std::source_location loc, Args&&... args)
	{
		return upool_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	inline void upool_delete(T* ptr)
	{
		implements::upool_delete_impl<T, Align>(ptr, 1);
	}

	template<typename T>
	inline void upool_delete(T* ptr)
	{
		upool_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// upool_new_array/upool_delete_array 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new_array(size_t count)
	{
		return implements::upool_new_impl<T, Align>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new_array(size_t count)
	{
		return upool_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new_array(size_t count, Args&&... args)
	{
		return implements::upool_new_impl<T, Align, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new_array(size_t count, Args&&... args)
	{
		return upool_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new_array(size_t count, std::source_location loc)
	{
		return implements::upool_new_impl<T, Align>(count, loc);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new_array(size_t count, std::source_location loc)
	{
		return upool_new_array<T, alignof(T)>(count, loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new_array(size_t count, std::source_location loc, Args&&... args)
	{
		return implements::upool_new_impl<T, Align, Args...>(count, loc, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new_array(size_t count, std::source_location loc, Args&&... args)
	{
		return upool_new_array<T, alignof(T), Args...>(count, loc, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	void upool_delete_array(T* ptr, size_t count)
	{
		implements::upool_delete_impl<T, Align>(ptr, count);
	}

	template<typename T>
	void upool_delete_array(T* ptr, size_t count)
	{
		upool_delete_array<T, alignof(T)>(ptr, count);
	}

	//--------------------------------------------------------------------------------
	// temp_new/temp_delete 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* temp_new()
	{
		return implements::temp_new_impl<T, Align>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* temp_new()
	{
		return temp_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* temp_new(Args&&... args)
	{
		return implements::temp_new_impl<T, Align, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* temp_new(Args&&... args)
	{
		return temp_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	inline void temp_delete(T* ptr)
	{
		implements::temp_delete_impl<T, Align>(ptr, 1);
	}

	template<typename T>
	inline void temp_delete(T* ptr)
	{
		temp_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// temp_new_array/temp_delete_array 스타일의 간단한 인터페이스
	template<typename T, size_t Align>
	[[nodiscard]] inline T* temp_new_array(size_t count)
	{
		return implements::temp_new_impl<T, Align>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* temp_new_array(size_t count)
	{
		return temp_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* temp_new_array(size_t count, Args&&... args)
	{
		return implements::temp_new_impl<T, Align, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* temp_new_array(size_t count, Args&&... args)
	{
		return temp_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, size_t Align>
	void temp_delete_array(T* ptr, size_t count)
	{
		implements::temp_delete_impl<T, Align>(ptr, count);
	}

	template<typename T>
	void temp_delete_array(T* ptr, size_t count)
	{
		temp_delete_array<T, alignof(T)>(ptr, count);
	}
}

#if defined(__MEMORY_TRACKING_ENABLED__)
#define SYS_NEW_A(T, A)								mark::sys_new<T, A>(std::source_location::current())
#define SYS_NEW_ARG_A(T, A, ...)					mark::sys_new<T, A>(std::source_location::current(), __VA_ARGS__)
#define SYS_NEW(T)									SYS_NEW_A(T, alignof(T))
#define SYS_NEW_ARG(T, ...)							SYS_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define SYS_DELETE_A(T, A, ptr)						mark::sys_delete<T, A>(ptr)
#define SYS_DELETE(T, ptr)							SYS_DELETE_A(T, alignof(T), ptr)
#define SYS_NEW_ARRAY_A(T, A, Count)				mark::sys_new_array<T, A>(Count, std::source_location::current())
#define SYS_NEW_ARRAY(T, Count)						SYS_NEW_ARRAY_A(T, alignof(T), Count)
#define SYS_DELETE_ARRAY_A(T, A, ptr, Count)		mark::sys_delete_array<T, A>(ptr, Count)
#define SYS_DELETE_ARRAY(T, ptr, Count)				SYS_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#define SPOOL_NEW_A(T, A)							mark::spool_new<T, A>(std::source_location::current())
#define SPOOL_NEW_ARG_A(T, A, ...)					mark::spool_new<T, A>(std::source_location::current(), __VA_ARGS__)
#define SPOOL_NEW(T)								SPOOL_NEW_A(T, alignof(T))
#define SPOOL_NEW_ARG(T, ...)						SPOOL_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define SPOOL_DELETE_A(T, A, ptr)					mark::spool_delete<T, A>(ptr)
#define SPOOL_DELETE(T, ptr)						SPOOL_DELETE_A(T, alignof(T), ptr)
#define SPOOL_NEW_ARRAY_A(T, A, Count)				mark::spool_new_array<T, A>(Count, std::source_location::current())
#define SPOOL_NEW_ARRAY(T, Count)					SPOOL_NEW_ARRAY_A(T, alignof(T), Count)
#define SPOOL_DELETE_ARRAY_A(T, A, ptr, Count)		mark::spool_delete_array<T, A>(ptr, Count)
#define SPOOL_DELETE_ARRAY(T, ptr, Count)			SPOOL_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#define UPOOL_NEW_A(T, A)							mark::upool_new<T, A>(std::source_location::current())
#define UPOOL_NEW_ARG_A(T, A, ...)					mark::upool_new<T, A>(std::source_location::current(), __VA_ARGS__)
#define UPOOL_NEW(T)								UPOOL_NEW_A(T, alignof(T))
#define UPOOL_NEW_ARG(T, ...)						UPOOL_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define UPOOL_DELETE_A(T, A, ptr)					mark::upool_delete<T, A>(ptr)
#define UPOOL_DELETE(T, ptr)						UPOOL_DELETE_A(T, alignof(T), ptr)
#define UPOOL_NEW_ARRAY_A(T, A, Count)				mark::upool_new_array<T, A>(Count, std::source_location::current())
#define UPOOL_NEW_ARRAY(T, Count)					UPOOL_NEW_ARRAY_A(T, alignof(T), Count)
#define UPOOL_DELETE_ARRAY_A(T, A, ptr, Count)		mark::upool_delete_array<T, A>(ptr, Count)
#define UPOOL_DELETE_ARRAY(T, ptr, Count)			UPOOL_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#define TEMP_NEW_A(T, A)							mark::temp_new<T, A>(std::source_location::current())
#define TEMP_NEW_ARG_A(T, A, ...)					mark::temp_new<T, A>(std::source_location::current(), __VA_ARGS__)
#define TEMP_NEW(T)									TEMP_NEW_A(T, alignof(T))
#define TEMP_NEW_ARG(T, ...)						TEMP_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define TEMP_DELETE_A(T, A, ptr)					mark::temp_delete<T, A>(ptr)
#define TEMP_DELETE(T, ptr)							TEMP_DELETE_A(T, alignof(T), ptr)
#define TEMP_NEW_ARRAY_A(T, A, Count)				mark::temp_new_array<T, A>(Count, std::source_location::current())
#define TEMP_NEW_ARRAY(T, Count)					TEMP_NEW_ARRAY_A(T, alignof(T), Count)
#define TEMP_DELETE_ARRAY_A(T, A, ptr, Count)		mark::temp_delete_array<T, A>(ptr, Count)
#define TEMP_DELETE_ARRAY(T, ptr, Count)			TEMP_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#else

#define SYS_NEW_A(T, A)								mark::sys_new<T, A>()
#define SYS_NEW_ARG_A(T, A, ...)					mark::sys_new<T, A>(__VA_ARGS__)
#define SYS_NEW(T)									SYS_NEW_A(T, alignof(T))
#define SYS_NEW_ARG(T, ...)							SYS_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define SYS_DELETE_A(T, A, ptr)						mark::sys_delete<T, A>(ptr)
#define SYS_DELETE(T, ptr)							SYS_DELETE_A(T, alignof(T), ptr)
#define SYS_NEW_ARRAY_A(T, A, Count)				mark::sys_new_array<T, A>(Count)
#define SYS_NEW_ARRAY(T, Count)						SYS_NEW_ARRAY_A(T, alignof(T), Count)
#define SYS_DELETE_ARRAY_A(T, A, ptr, Count)		mark::sys_delete_array<T, A>(ptr, Count)
#define SYS_DELETE_ARRAY(T, ptr, Count)				SYS_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#define SPOOL_NEW_A(T, A)							mark::spool_new<T, A>()
#define SPOOL_NEW_ARG_A(T, A, ...)					mark::spool_new<T, A>(__VA_ARGS__)
#define SPOOL_NEW(T)								SPOOL_NEW_A(T, alignof(T))
#define SPOOL_NEW_ARG(T, ...)						SPOOL_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define SPOOL_DELETE_A(T, A, ptr)					mark::spool_delete<T, A>(ptr)
#define SPOOL_DELETE(T, ptr)						SPOOL_DELETE_A(T, alignof(T), ptr)
#define SPOOL_NEW_ARRAY_A(T, A, Count)				mark::spool_new_array<T, A>(Count)
#define SPOOL_NEW_ARRAY(T, Count)					SPOOL_NEW_ARRAY_A(T, alignof(T), Count)
#define SPOOL_DELETE_ARRAY_A(T, A, ptr, Count)		mark::spool_delete_array<T, A>(ptr, Count)
#define SPOOL_DELETE_ARRAY(T, ptr, Count)			SPOOL_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#define UPOOL_NEW_A(T, A)							mark::upool_new<T, A>()
#define UPOOL_NEW_ARG_A(T, A, ...)					mark::upool_new<T, A>(__VA_ARGS__)
#define UPOOL_NEW(T)								UPOOL_NEW_A(T, alignof(T))
#define UPOOL_NEW_ARG(T, ...)						UPOOL_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define UPOOL_DELETE_A(T, A, ptr)					mark::upool_delete<T, A>(ptr)
#define UPOOL_DELETE(T, ptr)						UPOOL_DELETE_A(T, alignof(T), ptr)
#define UPOOL_NEW_ARRAY_A(T, A, Count)				mark::upool_new_array<T, A>(Count)
#define UPOOL_NEW_ARRAY(T, Count)					UPOOL_NEW_ARRAY_A(T, alignof(T), Count)
#define UPOOL_DELETE_ARRAY_A(T, A, ptr, Count)		mark::upool_delete_array<T, A>(ptr, Count)
#define UPOOL_DELETE_ARRAY(T, ptr, Count)			UPOOL_DELETE_ARRAY_A(T, alignof(T), ptr, Count)

#define TEMP_NEW_A(T, A)							mark::temp_new<T, A>()
#define TEMP_NEW_ARG_A(T, A, ...)					mark::temp_new<T, A>(__VA_ARGS__)
#define TEMP_NEW(T)									TEMP_NEW_A(T, alignof(T))
#define TEMP_NEW_ARG(T, ...)						TEMP_NEW_ARG_A(T, alignof(T), __VA_ARGS__)
#define TEMP_DELETE_A(T, A, ptr)					mark::temp_delete<T, A>(ptr)
#define TEMP_DELETE(T, ptr)							TEMP_DELETE_A(T, alignof(T), ptr)
#define TEMP_NEW_ARRAY_A(T, A, Count)				mark::temp_new_array<T, A>(Count)
#define TEMP_NEW_ARRAY(T, Count)					TEMP_NEW_ARRAY_A(T, alignof(T), Count)
#define TEMP_DELETE_ARRAY_A(T, A, ptr, Count)		mark::temp_delete_array<T, A>(ptr, Count)
#define TEMP_DELETE_ARRAY(T, ptr, Count)			TEMP_DELETE_ARRAY_A(T, alignof(T), ptr, Count)


#endif // __MEMORY_TRACKING_ENABLED__






