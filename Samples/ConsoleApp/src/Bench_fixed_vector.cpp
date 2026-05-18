#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_vector.h>

#include <vector>
#include <cstdint>

void run_bench_fixed_vector()
{
	mtl_tb::section("BENCH fixed_vector vs std::vector");

	constexpr std::size_t kN = 64;
	constexpr std::size_t kIter = 100000;

	// push_back N개
	mtl_tb::bench_ns("std::vector push_back x N + clear", kIter, []{
		std::vector<int> v;
		v.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) v.push_back(i);
		mtl_tb::do_not_optimize(v);
	});
	mtl_tb::bench_ns("mtl::fixed_vector push_back x N", kIter, []{
		mtl::fixed_vector<int, kN> v;
		for (int i = 0; i < (int)kN; ++i) v.push_back(i);
		mtl_tb::do_not_optimize(v);
	});

	// emplace_back N개
	mtl_tb::bench_ns("std::vector emplace_back x N + clear", kIter, []{
		std::vector<int> v;
		v.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) v.emplace_back(i);
		mtl_tb::do_not_optimize(v);
	});
	mtl_tb::bench_ns("mtl::fixed_vector emplace_back x N", kIter, []{
		mtl::fixed_vector<int, kN> v;
		for (int i = 0; i < (int)kN; ++i) v.emplace_back(i);
		mtl_tb::do_not_optimize(v);
	});

	// 순회 합산
	{
		std::vector<int> sv(kN);
		for (int i = 0; i < (int)kN; ++i) sv[i] = i;
		mtl::fixed_vector<int, kN> fv;
		for (int i = 0; i < (int)kN; ++i) fv.push_back(i);

		mtl_tb::bench_ns("std::vector iterate sum", kIter, [&]{
			std::int64_t s = 0;
			for (int x : sv) s += x;
			mtl_tb::do_not_optimize(s);
		});
		mtl_tb::bench_ns("mtl::fixed_vector iterate sum", kIter, [&]{
			std::int64_t s = 0;
			for (int x : fv) s += x;
			mtl_tb::do_not_optimize(s);
		});
	}

	// erase_unsorted vs erase 중간
	mtl_tb::bench_ns("mtl::fixed_vector erase(middle) x N", kIter, []{
		mtl::fixed_vector<int, kN> v;
		for (int i = 0; i < (int)kN; ++i) v.push_back(i);
		while (!v.empty()) v.erase(v.begin() + v.size()/2);
		mtl_tb::do_not_optimize(v);
	});
	mtl_tb::bench_ns("mtl::fixed_vector erase_unsorted x N", kIter, []{
		mtl::fixed_vector<int, kN> v;
		for (int i = 0; i < (int)kN; ++i) v.push_back(i);
		while (!v.empty()) v.erase_unsorted(v.begin() + v.size()/2);
		mtl_tb::do_not_optimize(v);
	});
}
