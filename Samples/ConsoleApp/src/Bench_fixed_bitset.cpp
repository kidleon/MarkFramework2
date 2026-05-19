#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_bitset.h>

#include <bitset>
#include <cstdint>

void run_bench_fixed_bitset()
{
	mtl_tb::section("BENCH fixed_bitset vs std::bitset");

	constexpr std::size_t kBits = 256;
	constexpr std::size_t kIter = 200000;

	auto std_mixed = mtl_tb::bench_ns("std::bitset set/reset/test mixed", kIter, [] {
		std::bitset<kBits> b;
		for (std::size_t i = 0; i < kBits; i += 3) b.set(i);
		for (std::size_t i = 1; i < kBits; i += 5) b.reset(i);
		std::uint64_t c = 0;
		for (std::size_t i = 0; i < kBits; ++i) c += b.test(i) ? 1u : 0u;
		mtl_tb::do_not_optimize(c);
	});
	auto fixed_mixed = mtl_tb::bench_ns("mtl::fixed_bitset set/reset/test mixed", kIter, [] {
		mtl::fixed_bitset<kBits> b;
		for (std::size_t i = 0; i < kBits; i += 3) b.set(i);
		for (std::size_t i = 1; i < kBits; i += 5) b.reset(i);
		std::uint64_t c = 0;
		for (std::size_t i = 0; i < kBits; ++i) c += b.test(i) ? 1u : 0u;
		mtl_tb::do_not_optimize(c);
	});
	mtl_tb::bench_compare("mtl::fixed_bitset mixed", fixed_mixed, "std::bitset mixed", std_mixed);

	auto std_count = mtl_tb::bench_ns("std::bitset count", kIter, [] {
		std::bitset<kBits> b;
		for (std::size_t i = 0; i < kBits; i += 2) b.set(i);
		auto c = b.count();
		mtl_tb::do_not_optimize(c);
	});
	auto fixed_count = mtl_tb::bench_ns("mtl::fixed_bitset count", kIter, [] {
		mtl::fixed_bitset<kBits> b;
		for (std::size_t i = 0; i < kBits; i += 2) b.set(i);
		auto c = b.count();
		mtl_tb::do_not_optimize(c);
	});
	mtl_tb::bench_compare("mtl::fixed_bitset count", fixed_count, "std::bitset count", std_count);

	mtl_tb::bench_ns("mtl::fixed_bitset iterate set bits", kIter, [] {
		mtl::fixed_bitset<kBits> b;
		for (std::size_t i = 0; i < kBits; i += 3) b.set(i);
		std::uint64_t s = 0;
		for (auto idx : b.set_indices()) s += idx;
		mtl_tb::do_not_optimize(s);
	});
}
