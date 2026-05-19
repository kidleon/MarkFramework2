#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_ring_buffer.h>

#include <deque>
#include <cstdint>

void run_bench_fixed_ring_buffer()
{
	mtl_tb::section("BENCH fixed_ring_buffer vs std::deque");

	constexpr std::size_t kN = 128;
	constexpr std::size_t kIter = 100000;

	auto std_fifo = mtl_tb::bench_ns("std::deque push_back+pop_front x N", kIter, [] {
		std::deque<int> q;
		for (int i = 0; i < (int)kN; ++i) q.push_back(i);
		for (int i = 0; i < (int)kN; ++i) q.pop_front();
		mtl_tb::do_not_optimize(q);
	});
	auto fixed_fifo = mtl_tb::bench_ns("mtl::fixed_ring_buffer push+pop x N", kIter, [] {
		mtl::fixed_ring_buffer<int, kN> q;
		for (int i = 0; i < (int)kN; ++i) q.push(i);
		for (int i = 0; i < (int)kN; ++i) q.pop();
		mtl_tb::do_not_optimize(q);
	});
	mtl_tb::bench_compare("mtl::fixed_ring_buffer push+pop", fixed_fifo, "std::deque push+pop", std_fifo);

	auto overwrite = mtl_tb::bench_ns("mtl::fixed_ring_buffer push_overwrite x N", kIter, [] {
		mtl::fixed_ring_buffer<int, kN> q;
		for (int i = 0; i < (int)(kN * 2); ++i) q.push_overwrite(i);
		mtl_tb::do_not_optimize(q);
	});
	mtl_tb::bench_compare("push+pop", fixed_fifo, "push_overwrite", overwrite);

	{
		mtl::fixed_ring_buffer<int, kN> q;
		for (int i = 0; i < (int)kN; ++i) q.push(i);

		mtl_tb::bench_ns("mtl::fixed_ring_buffer iterate sum", kIter, [&] {
			std::int64_t s = 0;
			for (int v : q) s += v;
			mtl_tb::do_not_optimize(s);
		});
	}
}
