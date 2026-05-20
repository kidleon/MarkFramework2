#include "pch.h"
#include "TestBenchCommon.h"

#include <intrusive_hash_map.h>

#include <unordered_map>
#include <vector>
#include <cstdint>

namespace
{
	struct IntrusiveMapEntry : public mtl::intrusive_hash_node_key<int>
	{
		int value = 0;
		IntrusiveMapEntry() = default;
		IntrusiveMapEntry(int key, int v) : mtl::intrusive_hash_node_key<int>(key), value(v) {}
	};
}

void run_bench_intrusive_hash_map()
{
	mtl_tb::section("BENCH intrusive_hash_map vs std::unordered_map");

	constexpr std::size_t kN = 1024;
	constexpr std::size_t kBucketCount = 2048;
	constexpr std::size_t kIter = 2000;

	std::vector<IntrusiveMapEntry> nodes;
	nodes.reserve(kN);
	for (std::size_t i = 0; i < kN; ++i)
		nodes.emplace_back(static_cast<int>(i), static_cast<int>(i * 2));

	auto std_insert = mtl_tb::bench_ns("std::unordered_map insert N", kIter, [] {
		std::unordered_map<int, int> map;
		map.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) map.emplace(i, i * 2);
		mtl_tb::do_not_optimize(map);
	});

	auto intrusive_insert = mtl_tb::bench_ns("mtl::intrusive_hash_map insert N", kIter, [&] {
		mtl::intrusive_hash_map<IntrusiveMapEntry, kBucketCount> map;
		for (auto& node : nodes) map.insert(node);
		mtl_tb::do_not_optimize(map);
		map.clear();
	});
	mtl_tb::bench_compare("mtl::intrusive_hash_map insert", intrusive_insert, "std::unordered_map insert", std_insert);

	{
		std::unordered_map<int, int> smap;
		smap.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) smap.emplace(i, i * 2);

		mtl::intrusive_hash_map<IntrusiveMapEntry, kBucketCount> imap;
		for (auto& node : nodes) imap.insert(node);

		auto std_find = mtl_tb::bench_ns("std::unordered_map find hit", kIter, [&] {
			std::int64_t sum = 0;
			for (int i = 0; i < (int)kN; ++i) sum += smap.find(i)->second;
			mtl_tb::do_not_optimize(sum);
		});

		auto intrusive_find = mtl_tb::bench_ns("mtl::intrusive_hash_map find hit", kIter, [&] {
			std::int64_t sum = 0;
			for (int i = 0; i < (int)kN; ++i) sum += imap.find(i)->value;
			mtl_tb::do_not_optimize(sum);
		});
		mtl_tb::bench_compare("mtl::intrusive_hash_map find", intrusive_find, "std::unordered_map find", std_find);

		imap.clear();
	}

	auto std_erase = mtl_tb::bench_ns("std::unordered_map erase N", kIter, [] {
		std::unordered_map<int, int> map;
		map.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) map.emplace(i, i * 2);
		for (int i = 0; i < (int)kN; ++i) map.erase(i);
		mtl_tb::do_not_optimize(map);
	});

	auto intrusive_erase = mtl_tb::bench_ns("mtl::intrusive_hash_map erase N", kIter, [&] {
		mtl::intrusive_hash_map<IntrusiveMapEntry, kBucketCount> map;
		for (auto& node : nodes) map.insert(node);
		for (int i = 0; i < (int)kN; ++i) map.erase(i);
		mtl_tb::do_not_optimize(map);
	});
	mtl_tb::bench_compare("mtl::intrusive_hash_map erase", intrusive_erase, "std::unordered_map erase", std_erase);
}
