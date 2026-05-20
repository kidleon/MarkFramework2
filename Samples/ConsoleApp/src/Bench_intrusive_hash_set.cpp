#include "pch.h"
#include "TestBenchCommon.h"

#include <intrusive_hash_set.h>

#include <unordered_set>
#include <vector>
#include <cstdint>

namespace
{
	struct IntrusiveSetNode : public mtl::intrusive_hash_node_key<int>
	{
		int value = 0;
		IntrusiveSetNode() = default;
		IntrusiveSetNode(int key, int v) : mtl::intrusive_hash_node_key<int>(key), value(v) {}
	};
}

void run_bench_intrusive_hash_set()
{
	mtl_tb::section("BENCH intrusive_hash_set vs std::unordered_set");

	constexpr std::size_t kN = 1024;
	constexpr std::size_t kBucketCount = 2048;
	constexpr std::size_t kIter = 2000;

	std::vector<IntrusiveSetNode> nodes;
	nodes.reserve(kN);
	for (std::size_t i = 0; i < kN; ++i)
		nodes.emplace_back(static_cast<int>(i), static_cast<int>(i * 2));

	auto std_insert = mtl_tb::bench_ns("std::unordered_set insert N", kIter, [] {
		std::unordered_set<int> set;
		set.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) set.emplace(i);
		mtl_tb::do_not_optimize(set);
	});

	auto intrusive_insert = mtl_tb::bench_ns("mtl::intrusive_hash_set insert N", kIter, [&] {
		mtl::intrusive_hash_set<IntrusiveSetNode, kBucketCount> set;
		for (auto& node : nodes) set.insert(node);
		mtl_tb::do_not_optimize(set);
		set.clear();
	});
	mtl_tb::bench_compare("mtl::intrusive_hash_set insert", intrusive_insert, "std::unordered_set insert", std_insert);

	{
		std::unordered_set<int> sset;
		sset.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) sset.emplace(i);

		mtl::intrusive_hash_set<IntrusiveSetNode, kBucketCount> iset;
		for (auto& node : nodes) iset.insert(node);

		auto std_find = mtl_tb::bench_ns("std::unordered_set contains hit", kIter, [&] {
			std::int64_t sum = 0;
			for (int i = 0; i < (int)kN; ++i) sum += sset.contains(i) ? 1 : 0;
			mtl_tb::do_not_optimize(sum);
		});

		auto intrusive_find = mtl_tb::bench_ns("mtl::intrusive_hash_set contains hit", kIter, [&] {
			std::int64_t sum = 0;
			for (int i = 0; i < (int)kN; ++i) sum += iset.contains(i) ? 1 : 0;
			mtl_tb::do_not_optimize(sum);
		});
		mtl_tb::bench_compare("mtl::intrusive_hash_set contains", intrusive_find, "std::unordered_set contains", std_find);

		iset.clear();
	}

	auto std_erase = mtl_tb::bench_ns("std::unordered_set erase N", kIter, [] {
		std::unordered_set<int> set;
		set.reserve(kN);
		for (int i = 0; i < (int)kN; ++i) set.emplace(i);
		for (int i = 0; i < (int)kN; ++i) set.erase(i);
		mtl_tb::do_not_optimize(set);
	});

	auto intrusive_erase = mtl_tb::bench_ns("mtl::intrusive_hash_set erase N", kIter, [&] {
		mtl::intrusive_hash_set<IntrusiveSetNode, kBucketCount> set;
		for (auto& node : nodes) set.insert(node);
		for (int i = 0; i < (int)kN; ++i) set.erase(i);
		mtl_tb::do_not_optimize(set);
	});
	mtl_tb::bench_compare("mtl::intrusive_hash_set erase", intrusive_erase, "std::unordered_set erase", std_erase);
}
