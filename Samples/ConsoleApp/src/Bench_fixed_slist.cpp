#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_slist.h>

#include <forward_list>
#include <cstdint>

void run_bench_fixed_slist()
{
	mtl_tb::section("BENCH fixed_slist vs std::forward_list");

	constexpr std::size_t kN = 64;
	constexpr std::size_t kIter = 50000;

	auto std_push = mtl_tb::bench_ns("std::forward_list push_front x N", kIter, [] {
		std::forward_list<int> l;
		for (int i = 0; i < (int)kN; ++i) l.push_front(i);
		mtl_tb::do_not_optimize(l);
	});
	auto fixed_push = mtl_tb::bench_ns("mtl::fixed_slist push_front x N", kIter, [] {
		mtl::fixed_slist<int, kN> l;
		for (int i = 0; i < (int)kN; ++i) l.push_front(i);
		mtl_tb::do_not_optimize(l);
	});
	mtl_tb::bench_compare("mtl::fixed_slist push_front", fixed_push, "std::forward_list push_front", std_push);

	auto std_iter = mtl_tb::bench_ns("std::forward_list iterate sum", kIter, [] {
		std::forward_list<int> l;
		for (int i = 0; i < (int)kN; ++i) l.push_front(i);
		std::int64_t s = 0;
		for (int v : l) s += v;
		mtl_tb::do_not_optimize(s);
	});
	auto fixed_iter = mtl_tb::bench_ns("mtl::fixed_slist iterate sum", kIter, [] {
		mtl::fixed_slist<int, kN> l;
		for (int i = 0; i < (int)kN; ++i) l.push_front(i);
		std::int64_t s = 0;
		for (int v : l) s += v;
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_compare("mtl::fixed_slist iterate", fixed_iter, "std::forward_list iterate", std_iter);

	auto std_pop = mtl_tb::bench_ns("std::forward_list erase_after x N", kIter, [] {
		std::forward_list<int> l;
		for (int i = 0; i < (int)kN; ++i) l.push_front(i);
		while (!l.empty()) l.pop_front();
		mtl_tb::do_not_optimize(l);
	});
	auto fixed_pop = mtl_tb::bench_ns("mtl::fixed_slist pop_front x N", kIter, [] {
		mtl::fixed_slist<int, kN> l;
		for (int i = 0; i < (int)kN; ++i) l.push_front(i);
		while (!l.empty()) l.pop_front();
		mtl_tb::do_not_optimize(l);
	});
	mtl_tb::bench_compare("mtl::fixed_slist pop_front", fixed_pop, "std::forward_list pop_front", std_pop);
}
