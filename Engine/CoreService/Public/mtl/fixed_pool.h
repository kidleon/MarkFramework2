#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>
#include <cassert>
#include "allocator_malloc.h"


namespace mtl
{
	template <std::size_t Size, std::size_t Alignment>
	struct aligned_buffer
	{
		alignas(Alignment) std::byte data[Size];

		void* ptr() noexcept { return data; }
		const void* ptr() const noexcept { return data; }

		static constexpr std::size_t size() noexcept { return Size; }
		static constexpr std::size_t alignment() noexcept { return Alignment; }

	};

	class default_overflow_allocator
	{
	public:
		void* allocate(
			std::size_t n,
			std::size_t alignment = alignof(std::max_align_t)
		)
		{
			return sys_malloc(n, alignment);
		}

		void deallocate(
			void* p,
			std::size_t n,
			std::size_t alignment = alignof(std::max_align_t)
		) noexcept
		{
			sys_free(p);
		}
	};

	class fixed_pool_base
	{
	public:
		struct Link { Link* next; };

		fixed_pool_base() noexcept = default;

		void init(
			void* memory,
			std::size_t memory_size,
			std::size_t node_size,
			std::size_t alignment,
			std::size_t alignment_offset = 0
		) noexcept
		{
			assert(memory != nullptr);
			assert(node_size >= sizeof(Link));
			assert(alignment > 0 && (alignment & (alignment - 1)) == 0);

			const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(memory);
			const std::uintptr_t aligned = (base + alignment_offset + alignment - 1) & ~(alignment - 1);
			const std::uintptr_t first_node = aligned - alignment_offset;
			const std::uintptr_t end = base + memory_size;

			const std::size_t available = (end > first_node) ? static_cast<std::size_t>(end - first_node) : 0;
			const std::size_t count = available / node_size;

			head_ = nullptr;
			next_ = reinterpret_cast<Link*>(first_node);
			capacity_ = reinterpret_cast<Link*>(first_node + count * node_size);
			node_size_ = node_size;
			current_size_ = 0;
			peak_size_ = 0;
		}

		bool can_allocate() const noexcept
		{
			return head_ != nullptr || next_ != capacity_;
		}

		std::size_t node_size() const noexcept { return node_size_; }
		std::size_t current_size() const noexcept { return current_size_; }
		std::size_t peak_size() const noexcept { return peak_size_; }
		std::size_t memory_in_use() const noexcept { return current_size_ * node_size_; }
		std::size_t peak_memory_in_use() const noexcept { return peak_size_ * node_size_; }

	protected:
		Link* head_ = nullptr;
		Link* next_ = nullptr;
		Link* capacity_ = nullptr;
		std::size_t node_size_ = 0;
		std::size_t current_size_ = 0;
		std::size_t peak_size_ = 0;

	};


	class fixed_pool : public fixed_pool_base
	{
	public:
		fixed_pool() noexcept = default;

		fixed_pool(
			void* memory,
			std::size_t memory_size,
			std::size_t node_size,
			std::size_t alignment,
			std::size_t alignment_offset = 0) noexcept
		{
			init(memory, memory_size, node_size, alignment, alignment_offset);
		}

		void* allocate() noexcept
		{
			Link* link;
			if (head_)
			{
				link = head_;
				head_ = link->next;
			}
			else if (next_ != capacity_)
			{
				link = next_;
				next_ = reinterpret_cast<Link*>(reinterpret_cast<std::byte*>(next_) + node_size_);
			}
			else
			{
				return nullptr;
			}

			if (++current_size_ > peak_size_)
				peak_size_ = current_size_;

			return link;
		}

		void* allocate(std::size_t) noexcept
		{
			return allocate();
		}

		void deallocate(void* p) noexcept
		{
			assert(p != nullptr);
			assert(current_size_ > 0);

			--current_size_;
			Link* link = static_cast<Link*>(p);
			link->next = head_;
			head_ = link;
		}
	};

}
