#pragma once
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>
#include <new>
#include <memory>
#include "CoreGeneric.h"


namespace mark
{
#if defined(__cpp_lib_hardware_interference_size)
	inline constexpr std::size_t POOL_CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
#else
	inline constexpr std::size_t POOL_CACHE_LINE_SIZE = 64;
#endif

	/**
	* @brief object_pool<T, Allocator>는 객체 풀을 구현한 클래스입니다. Lock-free로 구현되어 있음.
	* @tparam T 풀에서 관리할 객체 타입입니다. T는 default 생성 가능해야 합니다.
	* @tparam Allocator 슬롯 메모리 할당에 사용할 표준 Allocator. 기본값은 system_allocator<T>.
	*/
	template <typename T, typename Allocator = system_allocator<T>>
	class object_pool
	{
	private:
		struct slot
		{
			std::atomic<slot*> next_in_pool{ nullptr };

			alignas(T) unsigned char buffer[sizeof(T)];

			[[nodiscard]] inline T* as_object() noexcept
			{
				return reinterpret_cast<T*>(buffer);
			}

			[[nodiscard]] inline static slot* from_object(T* obj) noexcept
			{
				auto* p = reinterpret_cast<unsigned char*>(obj);
				return reinterpret_cast<slot*>(p - offsetof(slot, buffer));
			}
		};

		using slot_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<slot>;
		using slot_alloc_traits = std::allocator_traits<slot_allocator_type>;

		/**
		* @brief tagged_ptr는 ABA 문제를 방지하기 위해 포인터와 태그를 함께 저장하는 구조체.
		* @details tagged_ptr는 64비트 정수로 포인터와 태그를 함께 저장함. 하위 48비트는 포인터, 상위 16비트는 태그로 사용됩니다.
		*/
		struct tagged_ptr
		{
			std::uintptr_t raw{ 0 };

			static constexpr std::uintptr_t PTR_MASK = 0x0000'FFFF'FFFF'FFFFULL;
			static constexpr std::uintptr_t TAG_SHIFT = 48;

			[[nodiscard]] inline slot* ptr() const noexcept
			{
				return reinterpret_cast<slot*>(raw & PTR_MASK);
			}

			[[nodiscard]] inline std::uint16_t tag() const noexcept
			{
				return static_cast<std::uint16_t>(raw >> TAG_SHIFT);
			}

			[[nodiscard]] static inline tagged_ptr make(slot* p, std::uint16_t t) noexcept
			{
				tagged_ptr r;
				r.raw = (reinterpret_cast<std::uintptr_t>(p) & PTR_MASK) | (static_cast<std::uintptr_t>(t) << TAG_SHIFT);
				return r;
			}
		};

		static_assert(sizeof(tagged_ptr) == sizeof(std::uintptr_t), "tagged_ptr must fit in a single word");

		alignas(POOL_CACHE_LINE_SIZE) std::atomic<tagged_ptr> _top;
		alignas(POOL_CACHE_LINE_SIZE) std::atomic<std::size_t> _count{ 0 };
		const std::size_t _max_size;

		[[no_unique_address]] slot_allocator_type _slot_alloc;

	public:
		using value_type = T;
		using allocator_type = Allocator;

		/**
		* @brief object_pool 생성자입니다.
		* @param max_size 풀에 보관할 최대 객체 개수. 이 값을 초과하는 release는 즉시 delete되어 메모리 무한 증가를 방지.
		* @param prefill 생성 시 미리 채워둘 슬롯 수. 0이면 lazy방식으로 초기화 (첫 호출때 초기화)
		* @param alloc 슬롯 메모리 할당에 사용할 allocator 인스턴스. stateful allocator의 경우 이 값을 통해 상태를 전달.
		*/
		explicit object_pool(
			std::size_t max_size = 1024,
			std::size_t prefill = 0,
			const Allocator& alloc = Allocator{}
		)
			: _max_size(max_size)
			, _slot_alloc(alloc)
		{
			_top.store(tagged_ptr{}, std::memory_order_relaxed);

			std::size_t to_fill = (prefill > max_size) ? max_size : prefill;

			for (std::size_t i = 0; i < to_fill; ++i)
			{
				slot* s = slot_alloc_traits::allocate(_slot_alloc, 1);
				::new (&s->next_in_pool) std::atomic<slot*>(nullptr);

				tagged_ptr t = _top.load(std::memory_order_relaxed);
				s->next_in_pool.store(t.ptr(), std::memory_order_relaxed);
				_top.store(tagged_ptr::make(s, t.tag() + 1), std::memory_order_relaxed);
			}

			_count.store(to_fill, std::memory_order_relaxed);
		}

		/**
		* @brief object_pool 소멸자입니다. 풀에 남아있는 모든 슬롯의 메모리를 해제함.
		* 이미 acquire되어 외부로 나간 객체는 사용자가 미리 release하거나 delete해야함 (단 make_unique의 RAII로 생성시 자동처리 됨)
		*/
		~object_pool()
		{
			slot* s = _top.load(std::memory_order_relaxed).ptr();
			while (s)
			{
				slot* nxt = s->next_in_pool.load(std::memory_order_relaxed);
				slot_alloc_traits::deallocate(_slot_alloc, s, 1);
				s = nxt;
			}
		}

		object_pool(const object_pool&) = delete;
		object_pool& operator=(const object_pool&) = delete;
		object_pool(object_pool&&) = delete;
		object_pool& operator=(object_pool&&) = delete;

		/**
		* @brief 현재 사용 중인 allocator의 사본을 반환합니다.
		*/
		[[nodiscard]] allocator_type get_allocator() const noexcept
		{
			return allocator_type(_slot_alloc);
		}

		/**
		* @brief acquire 메서드는 풀에서 객체를 하나 꺼내거나, 풀이 비었을 때 새로 할당하여 반환함.
		* @tparam Args T 객체 생성에 필요한 인자들의 타입입니다.
		* @param args T 객체 생성에 필요한 인자들입니다. perfect forwarding으로 전달됩니다.
		* @return T 객체의 포인터를 반환합니다. 풀에서 꺼낸 객체이거나 새로 할당된 객체입니다.
		*/
		template <typename... Args>
		[[nodiscard]] inline T* acquire(Args&&... args)
		{
			slot* s = pop_slot();
			if (!s)
			{
				s = slot_alloc_traits::allocate(_slot_alloc, 1);
				::new (&s->next_in_pool) std::atomic<slot*>(nullptr);
			}

			T* obj = ::new (s->buffer) T(std::forward<Args>(args)...);
			return obj;
		}

		/**
		* @brief acquire_raw 메서드는 풀에서 객체를 생성하지 않고 raw 슬롯을 하나 꺼내거나, 풀이 비었을 때 새로 할당하여 반환함.
		* @return raw 슬롯의 버퍼 포인터를 반환합니다. 생성자 호출을 안하므로 사용자가 placement new로 직접 객체를 생성해야 함.
		* @example
		*	void* mem = pool.acquire_raw();
		*	T* obj = new (mem) T(...);
		*	...
		*	obj->~T();
		*	pool.release_raw(mem);
		*/
		[[nodiscard]] inline void* acquire_raw()
		{
			slot* s = pop_slot();
			if (!s)
			{
				s = slot_alloc_traits::allocate(_slot_alloc, 1);
				::new (&s->next_in_pool) std::atomic<slot*>(nullptr);
			}
			return s->buffer;
		}

		/**
		* @brief release 메서드는 풀에 객체를 반환합니다. 객체의 소유권이 풀로 넘어가며, 이후 호출자는 해당 객체를 사용해서는 안 됩니다.
		* @param obj 반환할 객체의 포인터입니다. nullptr이면 아무 작업도 수행하지 않습니다.
		* @details release 메서드는 먼저 객체의 소멸자를 호출하여 객체를 파괴한 후, 풀에 슬롯을 반환합니다.
		* 만약 풀에 이미 최대 개수의 슬롯이 있다면, 해당 슬롯은 즉시 해제됩니다.
		* release 메서드를 호출한 후에는 obj 포인터를 사용해서는 안 됩니다.
		* @return 없음
		*/
		void release(T* obj) noexcept
		{
			if (!obj) return;

			obj->~T();
			slot* s = slot::from_object(obj);

			std::size_t cur = _count.load(std::memory_order_relaxed);
			if (cur >= _max_size)
			{
				slot_alloc_traits::deallocate(_slot_alloc, s, 1);
				return;
			}

			push_slot(s);
		}

		/**
		* @brief release_raw 메서드는 acquire_raw로 받은 raw 슬롯을 풀에 반환합니다.
		* 호출자가 이미 해당 메모리에 placement new로 객체를 생성했고, 소멸자도 호출했다고 가정합니다.
		* @param mem 반환할 raw 슬롯의 버퍼 포인터입니다. nullptr이면 아무 작업도 수행하지 않습니다.
		* @return 없음
		*/
		void release_raw(void* mem) noexcept
		{
			if (!mem) return;
			slot* s = slot::from_object(static_cast<T*>(mem));

			std::size_t cur = _count.load(std::memory_order_relaxed);
			if (cur >= _max_size)
			{
				slot_alloc_traits::deallocate(_slot_alloc, s, 1);
				return;
			}
			push_slot(s);
		}

		/**
		* @brief size 메서드는 현재 풀에 보관된 객체 수를 반환함.
		* 이 값은 정확한 실시간 개수가 아니라, 풀에 release된 슬롯의 대략적인 개수.
		*/
		[[nodiscard]] inline std::size_t size() const noexcept
		{
			return _count.load(std::memory_order_relaxed);
		}

		/**
		* @brief max_size 메서드는 풀에 보관할 수 있는 최대 객체 수를 반환함.
		* 이 값을 초과하는 release는 즉시 delete되어 메모리 무한 증가를 방지.
		*/
		[[nodiscard]] inline std::size_t max_size() const noexcept
		{
			return _max_size;
		}


		/**
		* @brief unique_handle은 object_pool에서 획득한 객체의 소유권을 관리하는 RAII 래퍼 클래스.
		* 소멸시 자동으로 풀에 release, unique_ptr과 유사하게 move-only semantics를 가짐
		*/
		class unique_handle
		{
			T* _ptr{ nullptr };
			object_pool* _pool{ nullptr };

		public:
			unique_handle() noexcept = default;

			unique_handle(T* p, object_pool* pool) noexcept
				: _ptr(p)
				, _pool(pool)
			{
			}

			~unique_handle() noexcept
			{
				if (_ptr && _pool)
					_pool->release(_ptr);
			}

			unique_handle(const unique_handle&) = delete;
			unique_handle& operator=(const unique_handle&) = delete;

			unique_handle(unique_handle&& other) noexcept
				: _ptr(other._ptr)
				, _pool(other._pool)
			{
				other._ptr = nullptr;
				other._pool = nullptr;
			}

			unique_handle& operator=(unique_handle&& other) noexcept
			{
				if (this != &other)
				{
					if (_ptr && _pool)
						_pool->release(_ptr);
					_ptr = other._ptr;
					_pool = other._pool;
					other._ptr = nullptr;
					other._pool = nullptr;
				}
				return *this;
			}

			[[nodiscard]] inline T* get() noexcept { return _ptr; }
			[[nodiscard]] inline const T* get()  const noexcept { return _ptr; }

			[[nodiscard]] inline T& operator*() noexcept { return *_ptr; }
			[[nodiscard]] inline const T& operator*()  const noexcept { return *_ptr; }

			[[nodiscard]] inline T* operator->() noexcept { return _ptr; }
			[[nodiscard]] inline const T* operator->() const noexcept { return _ptr; }

			explicit operator bool() const noexcept { return _ptr != nullptr; }

			void reset() noexcept
			{
				if (_ptr && _pool)
					_pool->release(_ptr);
				_ptr = nullptr;
			}

			T* release_ownership() noexcept
			{
				T* p = _ptr;
				_ptr = nullptr;
				return p;
			}
		};

		/**
		* @brief make_unique 메서드는 풀에서 객체를 획득하여 unique_handle로 감싸 반환하는 편의 메서드. perfect forwarding 지원
		*/
		template <typename... Args>
		unique_handle make_unique(Args&&... args)
		{
			T* p = acquire(std::forward<Args>(args)...);
			return unique_handle(p, this);
		}

	private:
		/**
		* @brief pop_slot 메서드는 풀에서 슬롯을 하나 꺼내는 lock-free 연산입니다. 풀이 비었으면 nullptr 반환.
		* @return 풀에서 꺼낸 슬롯의 포인터를 반환합니다. 풀이 비었으면 nullptr을 반환합니다.
		*/
		slot* pop_slot() noexcept
		{
			tagged_ptr old_top = _top.load(std::memory_order_acquire);

			for (;;)
			{
				slot* s = old_top.ptr();
				if (!s)
				{
					return nullptr;
				}

				slot* nxt = s->next_in_pool.load(std::memory_order_acquire);

				tagged_ptr new_top = tagged_ptr::make(nxt, static_cast<std::uint16_t>(old_top.tag() + 1));

				if (_top.compare_exchange_weak(
					old_top,
					new_top,
					std::memory_order_acq_rel,
					std::memory_order_acquire
				))
				{
					_count.fetch_sub(1, std::memory_order_relaxed);
					return s;
				}
			}
		}

		/**
		* @brief push_slot 메서드는 슬롯을 풀에 반환하는 lock-free 연산. 풀에 이미 최대 개수의 슬롯이 있다면, 해당 슬롯은 즉시 해제됩니다.
		* @param s 풀에 반환할 슬롯의 포인터. nullptr이면 아무 작업도 수행하지 않음
		*/
		void push_slot(slot* s) noexcept
		{
			tagged_ptr old_top = _top.load(std::memory_order_relaxed);

			for (;;)
			{
				s->next_in_pool.store(old_top.ptr(), std::memory_order_relaxed);

				tagged_ptr new_top = tagged_ptr::make(s,
					static_cast<std::uint16_t>(old_top.tag() + 1));

				if (_top.compare_exchange_weak(
					old_top,
					new_top,
					std::memory_order_release,
					std::memory_order_relaxed))
				{
					_count.fetch_add(1, std::memory_order_relaxed);
					return;
				}
			}
		}
	};

} // namespace mark
