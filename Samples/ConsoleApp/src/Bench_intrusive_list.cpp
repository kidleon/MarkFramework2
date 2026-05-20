// ============================================================================
// Bench_intrusive_list.cpp
//
// mtl::intrusive_list 와 std::list 의 성능 비교.
//
// 비교 시 유의:
//   - std::list 는 push_back 시마다 노드 메모리를 할당한다.
//   - intrusive_list 는 노드 본체가 호출자 메모리이므로 할당이 없다.
//     벤치마크에서는 미리 std::vector<Item> 로 노드 풀을 확보해두고
//     포인터(참조)만 등록한다.
// ============================================================================

#include "pch.h"
#include "TestBenchCommon.h"

#include <intrusive_list.h>

#include <algorithm>
#include <iterator>
#include <list>
#include <random>
#include <vector>

namespace
{
	struct Item : public mtl::intrusive_list_node<>
	{
		int value;
		Item() : value(0) {}
		explicit Item(int v) : value(v) {}

		Item(const Item&) = default;
		Item(Item&&) noexcept = default;
		Item& operator=(const Item&) = default;
		Item& operator=(Item&&) noexcept = default;

		friend bool operator<(const Item& a, const Item& b) noexcept { return a.value < b.value; }
		friend bool operator==(const Item& a, const Item& b) noexcept { return a.value == b.value; }
	};
}

void run_bench_intrusive_list()
{
	using namespace mtl;

	constexpr std::size_t N = 4096;
	constexpr std::size_t ITERS = 200;

	// 노드 풀 (intrusive_list 가 사용할 메모리)
	std::vector<Item> pool(N);
	for (std::size_t i = 0; i < N; ++i) pool[i].value = static_cast<int>(i);

	// -----------------------------------------------------------------------
	mtl_tb::section("intrusive_list vs std::list — push_back (N nodes)");
	// -----------------------------------------------------------------------
	{
		const double t_std = mtl_tb::bench_ns("std::list<int>::push_back (alloc per node)", ITERS, [&]
		{
			std::list<int> lst;
			for (std::size_t i = 0; i < N; ++i) lst.push_back(static_cast<int>(i));
			mtl_tb::do_not_optimize(lst);
		});

		const double t_mtl = mtl_tb::bench_ns("mtl::intrusive_list<Item>::push_back (no alloc)", ITERS, [&]
		{
			intrusive_list<Item> lst;
			for (std::size_t i = 0; i < N; ++i) lst.push_back(pool[i]);
			mtl_tb::do_not_optimize(lst);
			lst.clear();
		});

		mtl_tb::bench_compare("mtl::intrusive_list", t_mtl, "std::list", t_std);
	}

	// -----------------------------------------------------------------------
	mtl_tb::section("intrusive_list vs std::list — iteration sum");
	// -----------------------------------------------------------------------
	{
		std::list<int> slist;
		for (std::size_t i = 0; i < N; ++i) slist.push_back(static_cast<int>(i));

		intrusive_list<Item> ilist;
		for (std::size_t i = 0; i < N; ++i) ilist.push_back(pool[i]);

		const double t_std = mtl_tb::bench_ns("std::list iteration sum", ITERS, [&]
		{
			long long sum = 0;
			for (auto& v : slist) sum += v;
			mtl_tb::do_not_optimize(sum);
		});

		const double t_mtl = mtl_tb::bench_ns("mtl::intrusive_list iteration sum", ITERS, [&]
		{
			long long sum = 0;
			for (auto& v : ilist) sum += v.value;
			mtl_tb::do_not_optimize(sum);
		});

		mtl_tb::bench_compare("mtl::intrusive_list", t_mtl, "std::list", t_std);

		ilist.clear();
	}

	// -----------------------------------------------------------------------
	mtl_tb::section("intrusive_list vs std::list — middle insert + erase");
	// -----------------------------------------------------------------------
	{
		// 초기 1/2 개로 채우고, 매 iter 마다 가운데에서 insert/erase 하는 시나리오
		const std::size_t HALF = N / 2;

		const double t_std = mtl_tb::bench_ns("std::list middle insert+erase", ITERS, [&]
		{
			std::list<int> lst;
			for (std::size_t i = 0; i < HALF; ++i) lst.push_back(static_cast<int>(i));

			for (std::size_t i = 0; i < HALF; ++i)
			{
				auto it = lst.begin();
				std::advance(it, lst.size() / 2);
				it = lst.insert(it, 999);
				lst.erase(it);
			}
			mtl_tb::do_not_optimize(lst);
		});

		// intrusive_list 에서는 매번 다른 노드 객체로 insert해야 같은 워크로드를 흉내낼 수 있다.
		// 노드 풀에서 절반은 베이스, 절반은 insert/erase용으로 사용.
		std::vector<Item> base_pool(HALF);
		std::vector<Item> ins_pool(HALF);
		for (std::size_t i = 0; i < HALF; ++i) base_pool[i].value = static_cast<int>(i);
		for (std::size_t i = 0; i < HALF; ++i) ins_pool[i].value = 999;

		const double t_mtl = mtl_tb::bench_ns("mtl::intrusive_list middle insert+erase", ITERS, [&]
		{
			intrusive_list<Item> lst;
			for (std::size_t i = 0; i < HALF; ++i) lst.push_back(base_pool[i]);

			for (std::size_t i = 0; i < HALF; ++i)
			{
				auto it = lst.begin();
				std::advance(it, lst.size() / 2);
				auto ins = lst.insert(it, ins_pool[i]);
				lst.erase(ins);
			}
			mtl_tb::do_not_optimize(lst);
			lst.clear();
		});

		mtl_tb::bench_compare("mtl::intrusive_list", t_mtl, "std::list", t_std);
	}

	// -----------------------------------------------------------------------
	mtl_tb::section("intrusive_list — O(1) unlink (포인터만으로 분리)");
	// -----------------------------------------------------------------------
	{
		// std::list 는 노드 포인터만으로 O(1) erase가 불가능하므로(이터레이터 필요),
		// 가장 가까운 시나리오로 비교: 랜덤 인덱스 노드를 찾아서 erase.
		// intrusive_list 는 unlink(node) 한 줄.

		std::mt19937 rng(12345);

		// std::list: 매 iter 마다 N개를 채운 뒤, 순서대로 advance+erase 반복 → O(N^2)
		// 비용이 너무 크므로 더 작은 N으로 한정.
		constexpr std::size_t N_SMALL = 1024;
		constexpr std::size_t ITERS_SMALL = 50;

		// 0..N_SMALL-1 의 셔플 — intrusive_list 에서 nodes[order_small[i]] 로
		// 곧장 unlink 하기 위함. (인덱스 범위와 등록 여부가 모두 보장돼야 함)
		std::vector<int> order_small(N_SMALL);
		for (std::size_t i = 0; i < N_SMALL; ++i) order_small[i] = static_cast<int>(i);
		std::shuffle(order_small.begin(), order_small.end(), rng);

		const double t_std = mtl_tb::bench_ns("std::list random-position erase (advance+erase)", ITERS_SMALL, [&]
		{
			std::list<int> lst;
			for (std::size_t i = 0; i < N_SMALL; ++i) lst.push_back(static_cast<int>(i));
			for (std::size_t i = 0; i < N_SMALL; ++i)
			{
				auto it = lst.begin();
				std::advance(it, order_small[i] % lst.size());
				lst.erase(it);
			}
			mtl_tb::do_not_optimize(lst);
		});

		std::vector<Item> nodes(N_SMALL);
		for (std::size_t i = 0; i < N_SMALL; ++i) nodes[i].value = static_cast<int>(i);

		const double t_mtl = mtl_tb::bench_ns("mtl::intrusive_list O(1) unlink(node)", ITERS_SMALL, [&]
		{
			intrusive_list<Item> lst;
			for (std::size_t i = 0; i < N_SMALL; ++i) lst.push_back(nodes[i]);
			for (std::size_t i = 0; i < N_SMALL; ++i) lst.unlink(nodes[order_small[i]]);
			mtl_tb::do_not_optimize(lst);
		});

		mtl_tb::bench_compare("mtl::intrusive_list", t_mtl, "std::list", t_std);
	}

	// -----------------------------------------------------------------------
	mtl_tb::section("intrusive_list vs std::list — splice (O(1) 전체 이동)");
	// -----------------------------------------------------------------------
	{
		// 두 리스트를 만들고 한쪽 → 다른쪽으로 splice. 둘 다 O(1).
		std::vector<Item> p1(N), p2(N);
		for (std::size_t i = 0; i < N; ++i) { p1[i].value = (int)i; p2[i].value = (int)i + (int)N; }

		const double t_std = mtl_tb::bench_ns("std::list splice(entire other)", ITERS, [&]
		{
			std::list<int> a, b;
			for (std::size_t i = 0; i < N; ++i) { a.push_back((int)i); b.push_back((int)i + (int)N); }
			a.splice(a.end(), b);
			mtl_tb::do_not_optimize(a);
		});

		const double t_mtl = mtl_tb::bench_ns("mtl::intrusive_list splice(entire other)", ITERS, [&]
		{
			intrusive_list<Item> a, b;
			for (std::size_t i = 0; i < N; ++i) a.push_back(p1[i]);
			for (std::size_t i = 0; i < N; ++i) b.push_back(p2[i]);
			a.splice(a.end(), b);
			mtl_tb::do_not_optimize(a);
			a.clear();
			b.clear();
		});

		mtl_tb::bench_compare("mtl::intrusive_list", t_mtl, "std::list", t_std);
	}

	// -----------------------------------------------------------------------
	mtl_tb::section("intrusive_list vs std::list — sort");
	// -----------------------------------------------------------------------
	{
		// 정렬에 사용할 데이터
		std::mt19937 rng(54321);
		std::vector<int> data(N);
		for (std::size_t i = 0; i < N; ++i) data[i] = (int)i;
		std::shuffle(data.begin(), data.end(), rng);

		std::vector<Item> sort_pool(N);
		for (std::size_t i = 0; i < N; ++i) sort_pool[i].value = data[i];

		const double t_std = mtl_tb::bench_ns("std::list<int>::sort", ITERS, [&]
		{
			std::list<int> lst(data.begin(), data.end());
			lst.sort();
			mtl_tb::do_not_optimize(lst);
		});

		const double t_mtl = mtl_tb::bench_ns("mtl::intrusive_list::sort", ITERS, [&]
		{
			intrusive_list<Item> lst;
			for (std::size_t i = 0; i < N; ++i) lst.push_back(sort_pool[i]);
			lst.sort();
			mtl_tb::do_not_optimize(lst);
			lst.clear();
		});

		mtl_tb::bench_compare("mtl::intrusive_list", t_mtl, "std::list", t_std);
	}
}
