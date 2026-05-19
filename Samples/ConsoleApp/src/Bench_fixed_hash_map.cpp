#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_hash_map.h>

#include <unordered_map>
#include <cstdint>

void run_bench_fixed_hash_map()
{
	mtl_tb::section("BENCH fixed_hash_map vs std::unordered_map");

	constexpr std::size_t kN = 64;
	constexpr std::size_t kIter = 100000;

	auto std_insert = mtl_tb::bench_ns("std::unordered_map insert N", kIter, [] {
		std::unordered_map<int, int> m;
		m.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) m.emplace(i, i * 2);
		mtl_tb::do_not_optimize(m);
	});
	auto fixed_insert = mtl_tb::bench_ns("mtl::fixed_hash_map insert N", kIter, [] {
		mtl::fixed_hash_map<int, int, kN> m;
		for (int i = 0; i < (int)kN; ++i) m.try_emplace(i, i * 2);
		mtl_tb::do_not_optimize(m);
	});
	mtl_tb::bench_compare("mtl::fixed_hash_map insert", fixed_insert, "std::unordered_map insert", std_insert);

	{
		std::unordered_map<int, int> sm;
		sm.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) sm.emplace(i, i * 2);

		mtl::fixed_hash_map<int, int, kN> fm;
		for (int i = 0; i < (int)kN; ++i) fm.try_emplace(i, i * 2);

		auto std_find = mtl_tb::bench_ns("std::unordered_map find hit", kIter, [&] {
			std::int64_t s = 0;
			for (int i = 0; i < (int)kN; ++i) s += sm.find(i)->second;
			mtl_tb::do_not_optimize(s);
		});
		auto fixed_find = mtl_tb::bench_ns("mtl::fixed_hash_map find hit", kIter, [&] {
			std::int64_t s = 0;
			for (int i = 0; i < (int)kN; ++i) s += fm.find(i)->second;
			mtl_tb::do_not_optimize(s);
		});
		mtl_tb::bench_compare("mtl::fixed_hash_map find", fixed_find, "std::unordered_map find", std_find);
	}

	auto std_erase = mtl_tb::bench_ns("std::unordered_map erase N", kIter, [] {
		std::unordered_map<int, int> m;
		m.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) m.emplace(i, i * 2);
		for (int i = 0; i < (int)kN; ++i) m.erase(i);
		mtl_tb::do_not_optimize(m);
	});
	auto fixed_erase = mtl_tb::bench_ns("mtl::fixed_hash_map erase N", kIter, [] {
		mtl::fixed_hash_map<int, int, kN> m;
		for (int i = 0; i < (int)kN; ++i) m.try_emplace(i, i * 2);
		for (int i = 0; i < (int)kN; ++i) m.erase(i);
		mtl_tb::do_not_optimize(m);
	});
	mtl_tb::bench_compare("mtl::fixed_hash_map erase", fixed_erase, "std::unordered_map erase", std_erase);
}
