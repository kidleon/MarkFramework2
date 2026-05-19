#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_hash_set.h>

#include <unordered_set>
#include <cstdint>

void run_bench_fixed_hash_set()
{
	mtl_tb::section("BENCH fixed_hash_set vs std::unordered_set");

	constexpr std::size_t kN = 64;
	constexpr std::size_t kIter = 100000;

	auto std_insert = mtl_tb::bench_ns("std::unordered_set insert N", kIter, [] {
		std::unordered_set<int> s;
		s.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) s.emplace(i);
		mtl_tb::do_not_optimize(s);
	});
	auto fixed_insert = mtl_tb::bench_ns("mtl::fixed_hash_set insert N", kIter, [] {
		mtl::fixed_hash_set<int, kN> s;
		for (int i = 0; i < (int)kN; ++i) s.emplace(i);
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_compare("mtl::fixed_hash_set insert", fixed_insert, "std::unordered_set insert", std_insert);

	{
		std::unordered_set<int> ss;
		ss.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) ss.emplace(i);

		mtl::fixed_hash_set<int, kN> fs;
		for (int i = 0; i < (int)kN; ++i) fs.emplace(i);

		auto std_contains = mtl_tb::bench_ns("std::unordered_set contains hit", kIter, [&] {
			std::int64_t c = 0;
			for (int i = 0; i < (int)kN; ++i) c += ss.contains(i) ? 1 : 0;
			mtl_tb::do_not_optimize(c);
		});
		auto fixed_contains = mtl_tb::bench_ns("mtl::fixed_hash_set contains hit", kIter, [&] {
			std::int64_t c = 0;
			for (int i = 0; i < (int)kN; ++i) c += fs.contains(i) ? 1 : 0;
			mtl_tb::do_not_optimize(c);
		});
		mtl_tb::bench_compare("mtl::fixed_hash_set contains", fixed_contains, "std::unordered_set contains", std_contains);
	}

	auto std_erase = mtl_tb::bench_ns("std::unordered_set erase N", kIter, [] {
		std::unordered_set<int> s;
		s.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) s.emplace(i);
		for (int i = 0; i < (int)kN; ++i) s.erase(i);
		mtl_tb::do_not_optimize(s);
	});
	auto fixed_erase = mtl_tb::bench_ns("mtl::fixed_hash_set erase N", kIter, [] {
		mtl::fixed_hash_set<int, kN> s;
		for (int i = 0; i < (int)kN; ++i) s.emplace(i);
		for (int i = 0; i < (int)kN; ++i) s.erase(i);
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_compare("mtl::fixed_hash_set erase", fixed_erase, "std::unordered_set erase", std_erase);
}
