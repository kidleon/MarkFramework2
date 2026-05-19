#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_pool.h>

#include <vector>

void run_bench_fixed_pool()
{
	mtl_tb::section("BENCH fixed_pool");

	constexpr std::size_t kN = 256;
	constexpr std::size_t kIter = 100000;
	using node_t = std::uint64_t;

	auto pool_fixed = mtl_tb::bench_ns("fixed_pool allocate+deallocate N", kIter, [] {
		alignas(std::max_align_t) std::byte mem[sizeof(node_t) * kN]{};
		mtl::fixed_pool pool(mem, sizeof(mem), sizeof(node_t), alignof(node_t));
		void* ptrs[kN]{};
		for (std::size_t i = 0; i < kN; ++i) ptrs[i] = pool.allocate();
		for (std::size_t i = 0; i < kN; ++i) pool.deallocate(ptrs[i]);
		mtl_tb::do_not_optimize(pool);
	});

	auto pool_over = mtl_tb::bench_ns("fixed_pool_with_overflow allocate+deallocate N", kIter, [] {
		alignas(std::max_align_t) std::byte mem[sizeof(node_t) * (kN / 2)]{};
		mtl::fixed_pool_with_overflow<> pool(mem, sizeof(mem), sizeof(node_t), alignof(node_t));
		std::vector<void*> ptrs;
		ptrs.reserve(kN);
		for (std::size_t i = 0; i < kN; ++i) ptrs.push_back(pool.allocate());
		for (void* p : ptrs) pool.deallocate(p);
		mtl_tb::do_not_optimize(pool);
	});

	auto pool_node = mtl_tb::bench_ns("fixed_node_allocator allocate+deallocate N", kIter, [] {
		alignas(std::max_align_t) std::byte mem[mtl::fixed_node_allocator<sizeof(node_t), kN>::kBufferSize]{};
		mtl::fixed_node_allocator<sizeof(node_t), kN, alignof(node_t), 0, false> alloc(mem);
		void* ptrs[kN]{};
		for (std::size_t i = 0; i < kN; ++i) ptrs[i] = alloc.allocate(sizeof(node_t));
		for (std::size_t i = 0; i < kN; ++i) alloc.deallocate(ptrs[i], sizeof(node_t));
		mtl_tb::do_not_optimize(alloc);
	});

	mtl_tb::bench_compare("fixed_pool", pool_fixed, "fixed_pool_with_overflow", pool_over);
	mtl_tb::bench_compare("fixed_pool", pool_fixed, "fixed_node_allocator", pool_node);
}
