#pragma once
#include <source_location>


namespace mark
{
	MARKENGINE_API bool initialize_core_memory(
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	);

	MARKENGINE_API void shutdown_core_memory();

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

	enum class POOL
	{
		SYS,
		SPOOL,
		UPOOL,
		TEMP
	};

	namespace implements
	{
		// sys_new/sys_delete 스타일의 실제 실행 함수들
#if defined(__MEMORY_TRACKING_ENABLED__)
		template<typename T, size_t Align, POOL Pool>
		[[nodiscard]] inline T* alloc_impl(std::source_location loc, size_t count)
		{
			static_assert(Align != 0 && (Align & (Align - 1)) == 0, "Alignment must be a non-zero power of 2");

			void* p;
			if constexpr (Pool == POOL::SYS)
				p = static_cast<T*>(sys_alloc(sizeof(T) * count, Align, loc));
			else if constexpr (Pool == POOL::SPOOL)
				p = static_cast<T*>(spool_alloc(sizeof(T) * count, Align, loc));
			else if constexpr (Pool == POOL::UPOOL)
				p = static_cast<T*>(upool_alloc(sizeof(T) * count, Align, loc));
			else if constexpr (Pool == POOL::TEMP)
				p = static_cast<T*>(temp_alloc(sizeof(T) * count, Align));

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, POOL Pool>
		[[nodiscard]] inline T* new_impl(std::source_location loc, size_t count)
		{
			T* p = alloc_impl<T, Align, Pool>(loc, count);
			if (!p) [[unlikely]]
				return nullptr;

			size_t constructed = 0;
			try
			{
				if (1 == count)
					new (p) T();
				else
				{
					for (size_t i = 0; i < count; ++i)
					{
						new (p + i) T();
						++constructed;
					}
				}

				return p;
			}
			catch (...)
			{
				for (size_t i = constructed; i > 0; --i)
					(p + i - 1)->~T();

				if constexpr (Pool == POOL::SYS) sys_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::SPOOL) spool_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::UPOOL) upool_free(p, sizeof(T) * count, Align);

				return nullptr;
			}
		}

		template<typename T, size_t Align, POOL Pool, typename... Args>
		[[nodiscard]] inline T* new_impl(std::source_location loc, size_t count, Args&&... args)
		{
			T* p = alloc_impl<T, Align, Pool>(loc, count);

			if (!p) [[unlikely]]
				return nullptr;

			size_t constructed = 0;
			try
			{
				if (1 == count)
					new (p) T(std::forward<Args>(args)...);
				else
				{
					for (size_t i = 0; i < count; ++i)
					{
						new (p + i) T(args...);
						++constructed;
					}
				}

				return p;
			}
			catch (...)
			{
				for (size_t i = constructed; i > 0; --i)
					(p + i - 1)->~T();

				if constexpr (Pool == POOL::SYS) sys_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::SPOOL) spool_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::UPOOL) upool_free(p, sizeof(T) * count, Align);

				return nullptr;
			}
		}

#else
		template<typename T, size_t Align, POOL Pool>
		[[nodiscard]] inline T* alloc_impl(size_t count)
		{
			static_assert(Align != 0 && (Align & (Align - 1)) == 0, "Alignment must be a non-zero power of 2");

			void* p;
			if constexpr (Pool == POOL::SYS)
				p = static_cast<T*>(sys_alloc(sizeof(T) * count, Align));
			else if constexpr (Pool == POOL::SPOOL)
				p = static_cast<T*>(spool_alloc(sizeof(T) * count, Align));
			else if constexpr (Pool == POOL::UPOOL)
				p = static_cast<T*>(upool_alloc(sizeof(T) * count, Align));
			else if constexpr (Pool == POOL::TEMP)
				p = static_cast<T*>(temp_alloc(sizeof(T) * count, Align));

			return static_cast<T*>(p);
		}

		template<typename T, size_t Align, POOL Pool>
		[[nodiscard]] inline T* new_impl(size_t count)
		{
			T* p = alloc_impl<T, Align, Pool>(count);
			if (!p) [[unlikely]]
				return nullptr;

			size_t constructed = 0;

			try
			{
				if (1 == count)
					new (p) T();
				else
				{
					for (size_t i = 0; i < count; ++i)
					{
						new (p + i) T();
						++constructed;
					}
						
				}

				return p;
			}
			catch(...)
			{
				for (size_t i = constructed; i > 0; --i)
					(p + i - 1)->~T();

				if constexpr (Pool == POOL::SYS) sys_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::SPOOL) spool_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::UPOOL) upool_free(p, sizeof(T) * count, Align);
				return nullptr;
			}
		}

		template<typename T, size_t Align, POOL Pool, typename... Args>
		[[nodiscard]] inline T* new_impl(size_t count, Args&&... args)
		{
			T* p = alloc_impl<T, Align, Pool>(count);

			if (!p) [[unlikely]]
				return nullptr;

			size_t constructed = 0;

			try
			{
				if (1 == count)
					new (p) T(std::forward<Args>(args)...);
				else
				{
					for (size_t i = 0; i < count; ++i)
					{
						new (p + i) T(args...);
						++constructed;
					}
				}

				return static_cast<T*>(p);
			}
			catch (...)
			{
				for (size_t i = constructed; i > 0; --i)
					(p + i - 1)->~T();

				if constexpr (Pool == POOL::SYS) sys_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::SPOOL) spool_free(p, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::UPOOL) upool_free(p, sizeof(T) * count, Align);

				return nullptr;
			}
		}
#endif // __MEMORY_TRACKING_ENABLED__

		template<typename T, size_t Align, POOL Pool>
		inline void delete_impl(T* ptr, size_t count)
		{
			static_assert(Align != 0 && (Align & (Align - 1)) == 0, "Alignment must be a non-zero power of 2");

			if (ptr)
			{
				for (size_t i = 0; i < count; ++i)
					ptr[i].~T();

				if constexpr (Pool == POOL::SYS)
					sys_free(ptr, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::SPOOL)
					spool_free(ptr, sizeof(T) * count, Align);
				else if constexpr (Pool == POOL::UPOOL)
					upool_free(ptr, sizeof(T) * count, Align);
			}
		}
	}

	//--------------------------------------------------------------------------------------
	// Alloc/Free APIs

	//--------------------------------------------------------------------------------
	// sys_new/sys_delete 스타일의 간단한 인터페이스
#if defined(__MEMORY_TRACKING_ENABLED__)

	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new(std::source_location loc = std::source_location::current())
	{
		return implements::new_impl<T, Align, POOL::SYS>(loc, 1);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new(std::source_location loc = std::source_location::current())
	{
		return sys_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new(std::source_location loc, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SYS, Args...>(loc, 1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new(std::source_location loc, Args&&... args)
	{
		return sys_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new()
	{
		return implements::new_impl<T, Align, POOL::SYS>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new()
	{
		return sys_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new(Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SYS, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new(Args&&... args)
	{
		return sys_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Align>
	inline void sys_delete(T* ptr)
	{
		implements::delete_impl<T, Align, POOL::SYS>(ptr, 1);
	}

	template<typename T>
	inline void sys_delete(T* ptr)
	{
		sys_delete<T, alignof(T)>(ptr);
	}


	//--------------------------------------------------------------------------------
	// sys_new_array/sys_delete_array 스타일의 간단한 인터페이스
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new_array(std::source_location loc, size_t count)
	{
		return implements::new_impl<T, Align, POOL::SYS>(loc, count);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new_array(std::source_location loc, size_t count)
	{
		return sys_new_array<T, alignof(T)>(loc, count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SYS, Args...>(loc, count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return sys_new_array<T, alignof(T), Args...>(loc, count, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* sys_new_array(size_t count)
	{
		return implements::new_impl<T, Align, POOL::SYS>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* sys_new_array(size_t count)
	{
		return sys_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* sys_new_array(size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SYS, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* sys_new_array(size_t count, Args&&... args)
	{
		return sys_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}
		
#endif // __MEMORY_TRACKING_ENABLED__
	template<typename T, size_t Align>
	void sys_delete_array(T* ptr, size_t count)
	{
		implements::delete_impl<T, Align, POOL::SYS>(ptr, count);
	}

	template<typename T>
	void sys_delete_array(T* ptr, size_t count)
	{
		sys_delete_array<T, alignof(T)>(ptr, count);
	}

	//--------------------------------------------------------------------------------
	// spool_new/spool_delete 스타일의 간단한 인터페이스
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new(std::source_location loc = std::source_location::current())
	{
		return implements::new_impl<T, Align, POOL::SPOOL>(loc, 1);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new(std::source_location loc = std::source_location::current())
	{
		return spool_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new(std::source_location loc, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SPOOL, Args...>(loc, 1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new(std::source_location loc, Args&&... args)
	{
		return spool_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new()
	{
		return implements::new_impl<T, Align, POOL::SPOOL>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new()
	{
		return spool_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new(Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SPOOL, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new(Args&&... args)
	{
		return spool_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Align>
	inline void spool_delete(T* ptr)
	{
		implements::delete_impl<T, Align, POOL::SPOOL>(ptr, 1);
	}

	template<typename T>
	inline void spool_delete(T* ptr)
	{
		spool_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// spool_new_array/spool_delete_array 스타일의 간단한 인터페이스
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new_array(std::source_location loc, size_t count)
	{
		return implements::new_impl<T, Align, POOL::SPOOL>(loc, count);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new_array(std::source_location loc, size_t count)
	{
		return spool_new_array<T, alignof(T)>(loc, count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SPOOL, Args...>(loc, count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return spool_new_array<T, alignof(T), Args...>(loc, count, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* spool_new_array(size_t count)
	{
		return implements::new_impl<T, Align, POOL::SPOOL>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* spool_new_array(size_t count)
	{
		return spool_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* spool_new_array(size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::SPOOL, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* spool_new_array(size_t count, Args&&... args)
	{
		return spool_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Align>
	void spool_delete_array(T* ptr, size_t count)
	{
		implements::delete_impl<T, Align, POOL::SPOOL>(ptr, count);
	}

	template<typename T>
	void spool_delete_array(T* ptr, size_t count)
	{
		spool_delete_array<T, alignof(T)>(ptr, count);
	}

	//--------------------------------------------------------------------------------
	// upool_new/spool_delete 스타일의 간단한 인터페이스
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new(std::source_location loc = std::source_location::current())
	{
		return implements::new_impl<T, Align, POOL::UPOOL>(loc, 1);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new(std::source_location loc = std::source_location::current())
	{
		return upool_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new(std::source_location loc, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::UPOOL, Args...>(loc, 1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new(std::source_location loc, Args&&... args)
	{
		return upool_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new()
	{
		return implements::new_impl<T, Align, POOL::UPOOL>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new()
	{
		return upool_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new(Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::UPOOL, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new(Args&&... args)
	{
		return upool_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Align>
	inline void upool_delete(T* ptr)
	{
		implements::delete_impl<T, Align, POOL::UPOOL>(ptr, 1);
	}

	template<typename T>
	inline void upool_delete(T* ptr)
	{
		upool_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// upool_new_array/upool_delete_array 스타일의 간단한 인터페이스
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new_array(std::source_location loc, size_t count)
	{
		return implements::new_impl<T, Align, POOL::UPOOL>(loc, count);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new_array(std::source_location loc, size_t count)
	{
		return upool_new_array<T, alignof(T)>(loc, count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::UPOOL, Args...>(loc, count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return upool_new_array<T, alignof(T), Args...>(loc, count, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* upool_new_array(size_t count)
	{
		return implements::new_impl<T, Align, POOL::UPOOL>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* upool_new_array(size_t count)
	{
		return upool_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* upool_new_array(size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::UPOOL, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* upool_new_array(size_t count, Args&&... args)
	{
		return upool_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__

	template<typename T, size_t Align>
	void upool_delete_array(T* ptr, size_t count)
	{
		implements::delete_impl<T, Align, POOL::UPOOL>(ptr, count);
	}

	template<typename T>
	void upool_delete_array(T* ptr, size_t count)
	{
		upool_delete_array<T, alignof(T)>(ptr, count);
	}

	//--------------------------------------------------------------------------------
	// temp_new/temp_delete 스타일의 간단한 인터페이스.
	// 단일 쓰레드에서만 사용해야 한다. 멀티 쓰레드 접근시 lock/unlock이 필요하다.
	// temp객체도 반드시 temp_delete로 해제해야 소멸자가 호출된다.
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* temp_new(std::source_location loc = std::source_location::current())
	{
		return implements::new_impl<T, Align, POOL::TEMP>(loc, 1);
	}

	template<typename T>
	[[nodiscard]] inline T* temp_new(std::source_location loc = std::source_location::current())
	{
		return temp_new<T, alignof(T)>(loc);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* temp_new(std::source_location loc, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::TEMP, Args...>(loc, 1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* temp_new(std::source_location loc, Args&&... args)
	{
		return temp_new<T, alignof(T), Args...>(loc, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* temp_new()
	{
		return implements::new_impl<T, Align, POOL::TEMP>(1);
	}

	template<typename T>
	[[nodiscard]] inline T* temp_new()
	{
		return temp_new<T, alignof(T)>();
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* temp_new(Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::TEMP, Args...>(1, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* temp_new(Args&&... args)
	{
		return temp_new<T, alignof(T), Args...>(std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__
	

	template<typename T, size_t Align>
	inline void temp_delete(T* ptr)
	{
		implements::delete_impl<T, Align, POOL::TEMP>(ptr, 1);
	}

	template<typename T>
	inline void temp_delete(T* ptr)
	{
		temp_delete<T, alignof(T)>(ptr);
	}

	//--------------------------------------------------------------------------------
	// temp_new_array/temp_delete_array 스타일의 간단한 인터페이스
	// 단일 쓰레드에서만 사용해야 한다. 멀티 쓰레드 접근시 lock/unlock이 필요하다.
	// temp객체도 반드시 temp_delete로 해제해야 소멸자가 호출된다.
#if defined(__MEMORY_TRACKING_ENABLED__)
	template<typename T, size_t Align>
	[[nodiscard]] inline T* temp_new_array(std::source_location loc, size_t count)
	{
		return implements::new_impl<T, Align, POOL::TEMP>(loc, count);
	}

	template<typename T>
	[[nodiscard]] inline T* temp_new_array(std::source_location loc, size_t count)
	{
		return temp_new_array<T, alignof(T)>(loc, count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* temp_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::TEMP, Args...>(loc, count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* temp_new_array(std::source_location loc, size_t count, Args&&... args)
	{
		return temp_new_array<T, alignof(T), Args...>(loc, count, std::forward<Args>(args)...);
	}
#else
	template<typename T, size_t Align>
	[[nodiscard]] inline T* temp_new_array(size_t count)
	{
		return implements::new_impl<T, Align, POOL::TEMP>(count);
	}

	template<typename T>
	[[nodiscard]] inline T* temp_new_array(size_t count)
	{
		return temp_new_array<T, alignof(T)>(count);
	}

	template<typename T, size_t Align, typename... Args>
	[[nodiscard]] inline T* temp_new_array(size_t count, Args&&... args)
	{
		return implements::new_impl<T, Align, POOL::TEMP, Args...>(count, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	[[nodiscard]] inline T* temp_new_array(size_t count, Args&&... args)
	{
		return temp_new_array<T, alignof(T), Args...>(count, std::forward<Args>(args)...);
	}
#endif // __MEMORY_TRACKING_ENABLED__
	template<typename T, size_t Align>
	void temp_delete_array(T* ptr, size_t count)
	{
		implements::delete_impl<T, Align, POOL::TEMP>(ptr, count);
	}

	template<typename T>
	void temp_delete_array(T* ptr, size_t count)
	{
		temp_delete_array<T, alignof(T)>(ptr, count);
	}
}

#if defined(__MEMORY_TRACKING_ENABLED__)
#define SYS_NEW_A(T, A, ...)					::mark::sys_new<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#define SPOOL_NEW_A(T, A, ...)					::mark::spool_new<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#define UPOOL_NEW_A(T, A, ...)					::mark::upool_new<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#define TEMP_NEW_A(T, A, ...)					::mark::temp_new<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)

#define SYS_NEW_ARRAY_A(T, A, ...)				::mark::sys_new_array<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#define SPOOL_NEW_ARRAY_A(T, A, ...)			::mark::spool_new_array<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#define UPOOL_NEW_ARRAY_A(T, A, ...)			::mark::upool_new_array<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#define TEMP_NEW_ARRAY_A(T, A, ...)				::mark::temp_new_array<T, A>(std::source_location::current()__VA_OPT__(,) __VA_ARGS__)
#else
#define SYS_NEW_A(T, A, ...)					::mark::sys_new<T, A>(__VA_ARGS__)
#define SPOOL_NEW_A(T, A, ...)					::mark::spool_new<T, A>(__VA_ARGS__)
#define UPOOL_NEW_A(T, A, ...)					::mark::upool_new<T, A>(__VA_ARGS__)
#define TEMP_NEW_A(T, A, ...)					::mark::temp_new<T, A>(__VA_ARGS__)

#define SYS_NEW_ARRAY_A(T, A, ...)				::mark::sys_new_array<T, A>(__VA_ARGS__)
#define SPOOL_NEW_ARRAY_A(T, A, ...)			::mark::spool_new_array<T, A>(__VA_ARGS__)
#define UPOOL_NEW_ARRAY_A(T, A, ...)			::mark::upool_new_array<T, A>(__VA_ARGS__)
#define TEMP_NEW_ARRAY_A(T, A, ...)				::mark::temp_new_array<T, A>(__VA_ARGS__)
#endif // __MEMORY_TRACKING_ENABLED__

#define SYS_NEW(T, ...)							SYS_NEW_A(T, alignof(T), __VA_ARGS__)
#define SYS_DELETE_A(T, A, ptr)					::mark::sys_delete<T, A>(ptr)
#define SYS_DELETE(T, ptr)						SYS_DELETE_A(T, alignof(T), ptr)

#define SPOOL_NEW(T, ...)						SPOOL_NEW_A(T, alignof(T), __VA_ARGS__)
#define SPOOL_DELETE_A(T, A, ptr)				::mark::spool_delete<T, A>(ptr)
#define SPOOL_DELETE(T, ptr)					SPOOL_DELETE_A(T, alignof(T), ptr)

#define UPOOL_NEW(T, ...)						UPOOL_NEW_A(T, alignof(T), __VA_ARGS__)
#define UPOOL_DELETE_A(T, A, ptr)				::mark::upool_delete<T, A>(ptr)
#define UPOOL_DELETE(T, ptr)					UPOOL_DELETE_A(T, alignof(T), ptr)	

#define TEMP_NEW(T, ...)						TEMP_NEW_A(T, alignof(T), __VA_ARGS__)
#define TEMP_DELETE_A(T, A, ptr)				::mark::temp_delete<T, A>(ptr)
#define TEMP_DELETE(T, ptr)						TEMP_DELETE_A(T, alignof(T), ptr)

#define SYS_NEW_ARRAY(T, ...)					SYS_NEW_ARRAY_A(T, alignof(T), __VA_ARGS__)
#define SYS_DELETE_ARRAY_A(T, A, ptr, count)	::mark::sys_delete_array<T, A>(ptr, count)
#define SYS_DELETE_ARRAY(T, ptr, count)			SYS_DELETE_ARRAY_A(T, alignof(T), ptr, count)

#define SPOOL_NEW_ARRAY(T, ...)					SPOOL_NEW_ARRAY_A(T, alignof(T), __VA_ARGS__)
#define SPOOL_DELETE_ARRAY_A(T, A, ptr, count)	::mark::spool_delete_array<T, A>(ptr, count)
#define SPOOL_DELETE_ARRAY(T, ptr, count)		SPOOL_DELETE_ARRAY_A(T, alignof(T), ptr, count)

#define UPOOL_NEW_ARRAY(T, ...)					UPOOL_NEW_ARRAY_A(T, alignof(T), __VA_ARGS__)
#define UPOOL_DELETE_ARRAY_A(T, A, ptr, count)	::mark::upool_delete_array<T, A>(ptr, count)
#define UPOOL_DELETE_ARRAY(T, ptr, count)		UPOOL_DELETE_ARRAY_A(T, alignof(T), ptr, count)

#define TEMP_NEW_ARRAY(T, ...)					TEMP_NEW_ARRAY_A(T, alignof(T), __VA_ARGS__)
#define TEMP_DELETE_ARRAY_A(T, A, ptr, count)	::mark::temp_delete_array<T, A>(ptr, count)
#define TEMP_DELETE_ARRAY(T, ptr, count)		TEMP_DELETE_ARRAY_A(T, alignof(T), ptr, count)
