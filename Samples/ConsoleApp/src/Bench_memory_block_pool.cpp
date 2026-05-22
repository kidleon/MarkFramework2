#include "pch.h"
#include "TestBenchCommon.h"

#include <core.h>
#include <memory_block_pool.h>

#include <cstdint>
#include <cstdlib>
#include <vector>

void run_bench_memory_block_pool()
{
	mtl_tb::section("BENCH memory_block_pool");

	constexpr std::size_t kN    = 256;
	constexpr std::size_t kIter = 10000;
	using node_t = std::uint64_t;

	// memory_block_pool: acquire + release N개 한 사이클.
	auto bench_block_pool = mtl_tb::bench_ns("memory_block_pool acquire+release N", kIter, [] {
		HANDLE pool = mark::create_memory_block_pool(sizeof(node_t), kN);
		mark::memory_block_id ids[kN];
		for (std::size_t i = 0; i < kN; ++i)
			ids[i] = mark::acquire_memory_block(pool);
		for (std::size_t i = 0; i < kN; ++i)
			mark::release_memory_block(pool, ids[i]);
		mtl_tb::do_not_optimize(pool);
		mark::destroy_memory_block_pool(pool);
	});

	// 풀 재사용 시(생성/파괴 비용 제외) 순수 acquire/release 비용.
	{
		HANDLE pool = mark::create_memory_block_pool(sizeof(node_t), kN);
		auto bench_reuse = mtl_tb::bench_ns("memory_block_pool acquire+release N (reuse pool)", kIter, [pool] {
			mark::memory_block_id ids[kN];
			for (std::size_t i = 0; i < kN; ++i)
				ids[i] = mark::acquire_memory_block(pool);
			for (std::size_t i = 0; i < kN; ++i)
				mark::release_memory_block(pool, ids[i]);
			mtl_tb::do_not_optimize(pool);
		});
		mark::destroy_memory_block_pool(pool);

		mtl_tb::bench_compare("memory_block_pool (recreate)", bench_block_pool,
							  "memory_block_pool (reuse)",   bench_reuse);
	}

	// malloc/free 비교군.
	auto bench_malloc = mtl_tb::bench_ns("malloc+free N", kIter, [] {
		void* ptrs[kN]{};
		for (std::size_t i = 0; i < kN; ++i) ptrs[i] = std::malloc(sizeof(node_t));
		for (std::size_t i = 0; i < kN; ++i) std::free(ptrs[i]);
		mtl_tb::do_not_optimize(ptrs);
	});

	mtl_tb::bench_compare("memory_block_pool (recreate)", bench_block_pool,
						  "malloc/free",                  bench_malloc);

	// is_memory_block_alive lock-free 경로의 cost.
	{
		HANDLE pool = mark::create_memory_block_pool(sizeof(node_t), 64);
		std::vector<mark::memory_block_id> ids;
		ids.reserve(64);
		for (std::size_t i = 0; i < 64; ++i)
			ids.push_back(mark::acquire_memory_block(pool));

		mtl_tb::bench_ns("memory_block_pool is_alive x64", kIter, [&pool, &ids] {
			std::size_t alive = 0;
			for (mark::memory_block_id id : ids)
				alive += mark::is_memory_block_alive(pool, id) ? 1 : 0;
			mtl_tb::do_not_optimize(alive);
		});

		for (mark::memory_block_id id : ids)
			mark::release_memory_block(pool, id);
		mark::destroy_memory_block_pool(pool);
	}
}
