#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_pool.h>

void run_test_fixed_pool()
{
	mtl_tb::section("fixed_pool");

	{
		alignas(std::max_align_t) std::byte mem[sizeof(std::uint64_t) * 4]{};
		mtl::fixed_pool pool(mem, sizeof(mem), sizeof(std::uint64_t), alignof(std::uint64_t));

		void* a = pool.allocate();
		void* b = pool.allocate();
		void* c = pool.allocate();
		void* d = pool.allocate();
		void* e = pool.allocate();
		MTL_CHECK(a && b && c && d);
		MTL_CHECK(e == nullptr);
		MTL_CHECK(pool.current_size() == 4);
		MTL_CHECK(pool.peak_size() == 4);
		MTL_CHECK(pool.memory_in_use() == sizeof(std::uint64_t) * 4);

		pool.deallocate(b);
		pool.deallocate(a);
		MTL_CHECK(pool.current_size() == 2);

		void* x = pool.allocate();
		void* y = pool.allocate();
		MTL_CHECK(x && y);
		MTL_CHECK(pool.current_size() == 4);
		pool.deallocate(c);
		pool.deallocate(d);
		pool.deallocate(x);
		pool.deallocate(y);
		MTL_CHECK(pool.current_size() == 0);
	}

	{
		alignas(std::max_align_t) std::byte mem[sizeof(std::uint64_t) * 2]{};
		mtl::fixed_pool_with_overflow<> pool(mem, sizeof(mem), sizeof(std::uint64_t), alignof(std::uint64_t));

		void* a = pool.allocate();
		void* b = pool.allocate();
		void* c = pool.allocate();
		MTL_CHECK(a && b && c);
		MTL_CHECK(pool.current_size() == 3);
		MTL_CHECK(pool.has_overflowed());
		MTL_CHECK(pool.overflow_peak() >= 1);

		pool.deallocate(a);
		pool.deallocate(b);
		pool.deallocate(c);
		MTL_CHECK(pool.current_size() == 0);
	}

	{
		using node_t = std::uintptr_t;
		alignas(std::max_align_t) std::byte mem[mtl::fixed_node_allocator<sizeof(node_t), 4>::kBufferSize]{};
		mtl::fixed_node_allocator<sizeof(node_t), 4, alignof(node_t), 0, false> alloc(mem);

		void* p0 = alloc.allocate(sizeof(node_t));
		void* p1 = alloc.allocate(sizeof(node_t));
		MTL_CHECK(p0 && p1);
		MTL_CHECK(alloc.current_size() == 2);
		MTL_CHECK(alloc.can_allocate());

		alloc.deallocate(p1, sizeof(node_t));
		alloc.deallocate(p0, sizeof(node_t));
		MTL_CHECK(alloc.current_size() == 0);
	}
}
