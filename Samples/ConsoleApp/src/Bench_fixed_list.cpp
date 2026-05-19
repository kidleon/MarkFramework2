#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_list.h>

#include <list>
#include <cstdint>

void run_bench_fixed_list()
{
	mtl_tb::section("BENCH fixed_list vs std::list");

	constexpr std::size_t kN = 64;
	constexpr std::size_t kIter = 50000;

	auto std_push = mtl_tb::bench_ns("std::list push_back x N", kIter, [] {
		std::list<int> l;
		for (int i = 0; i < (int)kN; ++i) l.push_back(i);
		mtl_tb::do_not_optimize(l);
	});
	auto fixed_push = mtl_tb::bench_ns("mtl::fixed_list push_back x N", kIter, [] {
		mtl::fixed_list<int, kN> l;
		for (int i = 0; i < (int)kN; ++i) l.push_back(i);
		mtl_tb::do_not_optimize(l);
	});
	mtl_tb::bench_compare("mtl::fixed_list push_back", fixed_push, "std::list push_back", std_push);

	auto std_iter = mtl_tb::bench_ns("std::list iterate sum", kIter, [] {
		std::list<int> l;
		for (int i = 0; i < (int)kN; ++i) l.push_back(i);
		std::int64_t s = 0;
		for (int v : l) s += v;
		mtl_tb::do_not_optimize(s);
	});
	auto fixed_iter = mtl_tb::bench_ns("mtl::fixed_list iterate sum", kIter, [] {
		mtl::fixed_list<int, kN> l;
		for (int i = 0; i < (int)kN; ++i) l.push_back(i);
		std::int64_t s = 0;
		for (int v : l) s += v;
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_compare("mtl::fixed_list iterate", fixed_iter, "std::list iterate", std_iter);

	auto std_erase = mtl_tb::bench_ns("std::list erase middle until empty", kIter, [] {
		std::list<int> l;
		for (int i = 0; i < (int)kN; ++i) l.push_back(i);
		while (!l.empty())
		{
			auto it = l.begin();
			std::advance(it, (int)(l.size() / 2));
			l.erase(it);
		}
		mtl_tb::do_not_optimize(l);
	});
	auto fixed_erase = mtl_tb::bench_ns("mtl::fixed_list erase middle until empty", kIter, [] {
		mtl::fixed_list<int, kN> l;
		for (int i = 0; i < (int)kN; ++i) l.push_back(i);
		while (!l.empty())
		{
			auto it = l.begin();
			std::advance(it, (int)(l.size() / 2));
			l.erase(it);
		}
		mtl_tb::do_not_optimize(l);
	});
	mtl_tb::bench_compare("mtl::fixed_list erase", fixed_erase, "std::list erase", std_erase);
}
