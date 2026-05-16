#include "allocator.h"
#include <mimalloc/mimalloc.h>

#include <cstring>
#include <atomic>

namespace mtl
{
	namespace
	{
		allocator g_default_allocator{ DEFAULT_ALLOCATOR_NAME };
		std::atomic<allocator*> g_default_allocator_ptr{ &g_default_allocator };
	}

	allocator::allocator(const char* name)
		: m_name(name ? name : DEFAULT_ALLOCATOR_NAME)
	{
	}

	allocator::allocator(const allocator& other)
		: m_name(other.m_name)
	{
	}

	allocator::allocator(const allocator& other, const char* name)
		: m_name(name ? name : other.m_name)
	{
	}

	allocator& allocator::operator=(const allocator& other)
	{
		if (this != &other)
		{
			m_name = other.m_name;
		}
		return *this;
	}

	void* allocator::allocate(size_t n)
	{
		return mi_malloc(n);
	}

	void* allocator::allocate(size_t n, size_t alignment, size_t offset)
	{
		if (alignment <= alignof(std::max_align_t) && offset == 0)
			return mi_malloc(n);

		if (offset == 0)
			return mi_malloc_aligned(n, alignment);
		
		return mi_malloc_aligned_at(n, alignment, offset);
	}

	void allocator::deallocate(void* p, size_t /*n*/)
	{
		mi_free(p);
	}

	const char* allocator::get_name() const
	{
		return m_name;
	}

	void allocator::set_name(const char* name)
	{
		m_name = name ? name : DEFAULT_ALLOCATOR_NAME;
	}

	bool operator==(const allocator& a, const allocator& b)
	{
		if (a.get_name() == b.get_name()) return true;
		const char* na = a.get_name();
		const char* nb = b.get_name();
		if (!na || !nb) return false;
		return std::strcmp(na, nb) == 0;
	}

	bool operator!=(const allocator& a, const allocator& b)
	{
		return !(a == b);
	}

	allocator* get_default_mtlallocator(const allocator*)
	{
		return g_default_allocator_ptr.load(std::memory_order_acquire);
	}

	void set_default_allocator(allocator* alloc)
	{
		g_default_allocator_ptr.store(
			alloc ? alloc : &g_default_allocator,
			std::memory_order_release);
	}
}
