// ============================================================================
// Test_intrusive_list.cpp
//
// mtl::intrusive_list<T>의 모든 공개 API에 대한 테스트.
// ============================================================================

#include "pch.h"
#include "TestBenchCommon.h"

#include <intrusive_list.h>

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace
{
	// 테스트용 노드 타입 — 값 비교/정렬을 위해 operator<, operator==를 정의.
	struct Item : public mtl::intrusive_list_node
	{
		int value;

		Item() : value(0) {}
		explicit Item(int v) : value(v) {}

		// 복사/이동은 base 의미를 따라야 함 (link 상태 영향 없음).
		Item(const Item&) = default;
		Item(Item&&) noexcept = default;
		Item& operator=(const Item&) = default;
		Item& operator=(Item&&) noexcept = default;

		friend bool operator==(const Item& a, const Item& b) noexcept { return a.value == b.value; }
		friend bool operator<(const Item& a, const Item& b) noexcept { return a.value < b.value; }
	};

	template <typename List>
	std::vector<int> snapshot(const List& list)
	{
		std::vector<int> out;
		for (const auto& x : list) out.push_back(x.value);
		return out;
	}
}

void run_test_intrusive_list()
{
	using namespace mtl;

	mtl_tb::section("intrusive_list — construction / empty / size");
	{
		intrusive_list<Item> list;
		MTL_CHECK(list.empty());
		MTL_CHECK(list.size() == 0);
		MTL_CHECK(list.begin() == list.end());
		MTL_CHECK(list.cbegin() == list.cend());
	}

	mtl_tb::section("intrusive_list — push_back / push_front / front / back");
	{
		intrusive_list<Item> list;
		Item a(1), b(2), c(3);

		list.push_back(a);
		list.push_back(b);
		list.push_front(c);

		MTL_CHECK(!list.empty());
		MTL_CHECK(list.size() == 3);
		MTL_CHECK(list.front().value == 3);
		MTL_CHECK(list.back().value == 2);
		MTL_CHECK((snapshot(list) == std::vector<int>{3, 1, 2}));

		// is_linked 확인
		MTL_CHECK(intrusive_list<Item>::is_linked(a));
		MTL_CHECK(intrusive_list<Item>::is_linked(b));
		MTL_CHECK(intrusive_list<Item>::is_linked(c));

		list.clear();
		MTL_CHECK(list.empty());
		MTL_CHECK(!intrusive_list<Item>::is_linked(a));
		MTL_CHECK(!intrusive_list<Item>::is_linked(b));
		MTL_CHECK(!intrusive_list<Item>::is_linked(c));
	}

	mtl_tb::section("intrusive_list — pop_front / pop_back");
	{
		intrusive_list<Item> list;
		Item a(1), b(2), c(3);
		list.push_back(a);
		list.push_back(b);
		list.push_back(c);

		list.pop_front();
		MTL_CHECK(list.size() == 2);
		MTL_CHECK(list.front().value == 2);
		MTL_CHECK(!intrusive_list<Item>::is_linked(a));

		list.pop_back();
		MTL_CHECK(list.size() == 1);
		MTL_CHECK(list.back().value == 2);
		MTL_CHECK(!intrusive_list<Item>::is_linked(c));

		list.clear();
	}

	mtl_tb::section("intrusive_list — iterators (forward / reverse / const)");
	{
		intrusive_list<Item> list;
		Item a(10), b(20), c(30);
		list.push_back(a);
		list.push_back(b);
		list.push_back(c);

		// forward
		int expected[] = { 10, 20, 30 };
		int idx = 0;
		for (auto it = list.begin(); it != list.end(); ++it, ++idx)
			MTL_CHECK(it->value == expected[idx]);
		MTL_CHECK(idx == 3);

		// post-increment / pre/post decrement
		auto it = list.begin();
		auto saved = it++;
		MTL_CHECK(saved->value == 10);
		MTL_CHECK(it->value == 20);
		--it;
		MTL_CHECK(it->value == 10);
		auto it2 = it++;
		MTL_CHECK(it2->value == 10);
		MTL_CHECK(it->value == 20);

		// reverse
		int rexpected[] = { 30, 20, 10 };
		idx = 0;
		for (auto rit = list.rbegin(); rit != list.rend(); ++rit, ++idx)
			MTL_CHECK(rit->value == rexpected[idx]);
		MTL_CHECK(idx == 3);

		// const begin/end/cbegin/cend/crbegin/crend
		const auto& clist = list;
		idx = 0;
		for (auto cit = clist.begin(); cit != clist.end(); ++cit, ++idx)
			MTL_CHECK(cit->value == expected[idx]);
		idx = 0;
		for (auto cit = clist.cbegin(); cit != clist.cend(); ++cit, ++idx)
			MTL_CHECK(cit->value == expected[idx]);
		idx = 0;
		for (auto crit = clist.crbegin(); crit != clist.crend(); ++crit, ++idx)
			MTL_CHECK(crit->value == rexpected[idx]);

		// non-const → const 이터레이터 변환
		intrusive_list<Item>::const_iterator cit = list.begin();
		MTL_CHECK(cit->value == 10);

		list.clear();
	}

	mtl_tb::section("intrusive_list — insert / erase (single / range)");
	{
		intrusive_list<Item> list;
		Item a(1), b(3), c(2);

		list.push_back(a);
		list.push_back(b);

		// 중간에 insert
		auto it = list.begin();
		++it; // points to b
		auto ret = list.insert(it, c);
		MTL_CHECK(ret->value == 2);
		MTL_CHECK((snapshot(list) == std::vector<int>{1, 2, 3}));
		MTL_CHECK(list.size() == 3);

		// erase 단일
		auto next = list.erase(list.begin());
		MTL_CHECK(next->value == 2);
		MTL_CHECK((snapshot(list) == std::vector<int>{2, 3}));
		MTL_CHECK(!intrusive_list<Item>::is_linked(a));

		// 다시 채워서 range erase
		Item d(4), e(5);
		list.push_back(d);
		list.push_back(e);
		// 현재: 2, 3, 4, 5  → [3..5) 삭제 = 3,4 삭제
		auto first = std::next(list.begin());      // 3
		auto last = std::next(first, 2);           // 5
		auto after = list.erase(first, last);
		MTL_CHECK(after->value == 5);
		MTL_CHECK((snapshot(list) == std::vector<int>{2, 5}));
		MTL_CHECK(list.size() == 2);
		MTL_CHECK(!intrusive_list<Item>::is_linked(b));
		MTL_CHECK(!intrusive_list<Item>::is_linked(d));

		list.clear();
	}

	mtl_tb::section("intrusive_list — iterator_to / contains / unlink");
	{
		intrusive_list<Item> list;
		Item a(1), b(2), c(3);
		list.push_back(a);
		list.push_back(b);
		list.push_back(c);

		auto it = list.iterator_to(b);
		MTL_CHECK(it->value == 2);

		const auto& clist = list;
		auto cit = clist.iterator_to(static_cast<const Item&>(c));
		MTL_CHECK(cit->value == 3);

		MTL_CHECK(list.contains(a));
		MTL_CHECK(list.contains(b));
		MTL_CHECK(list.contains(c));

		Item outsider(99);
		MTL_CHECK(!list.contains(outsider));
		MTL_CHECK(!intrusive_list<Item>::is_linked(outsider));

		// O(1) unlink
		list.unlink(b);
		MTL_CHECK(list.size() == 2);
		MTL_CHECK(!intrusive_list<Item>::is_linked(b));
		MTL_CHECK((snapshot(list) == std::vector<int>{1, 3}));

		list.clear();
	}

	mtl_tb::section("intrusive_list — move construction / move assignment");
	{
		intrusive_list<Item> src;
		Item a(1), b(2), c(3);
		src.push_back(a);
		src.push_back(b);
		src.push_back(c);

		intrusive_list<Item> moved(std::move(src));
		MTL_CHECK(src.empty());
		MTL_CHECK(src.size() == 0);
		MTL_CHECK(moved.size() == 3);
		MTL_CHECK((snapshot(moved) == std::vector<int>{1, 2, 3}));

		// 이동 후에도 노드의 prev/next는 새 컨테이너의 sentinel을 가리켜야
		// 정상 순회 가능.
		int sum = 0;
		for (auto& x : moved) sum += x.value;
		MTL_CHECK(sum == 6);

		// 이동 대입
		intrusive_list<Item> dst;
		Item z(99);
		dst.push_back(z);
		dst = std::move(moved);
		MTL_CHECK(moved.empty());
		MTL_CHECK(dst.size() == 3);
		MTL_CHECK((snapshot(dst) == std::vector<int>{1, 2, 3}));
		MTL_CHECK(!intrusive_list<Item>::is_linked(z)); // 이전 내용은 clear됨

		dst.clear();
	}

	mtl_tb::section("intrusive_list — swap (멤버 / 비멤버 / 빈-비빈 / 자기자신)");
	{
		intrusive_list<Item> a_list, b_list;
		Item a1(1), a2(2), b1(10), b2(20), b3(30);
		a_list.push_back(a1);
		a_list.push_back(a2);
		b_list.push_back(b1);
		b_list.push_back(b2);
		b_list.push_back(b3);

		a_list.swap(b_list);
		MTL_CHECK(a_list.size() == 3);
		MTL_CHECK(b_list.size() == 2);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{10, 20, 30}));
		MTL_CHECK((snapshot(b_list) == std::vector<int>{1, 2}));

		// 비멤버 swap
		using std::swap;
		swap(a_list, b_list);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{1, 2}));
		MTL_CHECK((snapshot(b_list) == std::vector<int>{10, 20, 30}));

		// 빈-비빈 swap
		intrusive_list<Item> empty_list;
		empty_list.swap(a_list);
		MTL_CHECK(a_list.empty());
		MTL_CHECK((snapshot(empty_list) == std::vector<int>{1, 2}));
		// 다시 순회 안정성 확인
		int sum = 0;
		for (auto& x : empty_list) sum += x.value;
		MTL_CHECK(sum == 3);

		// 자기자신 swap (no-op)
		empty_list.swap(empty_list);
		MTL_CHECK((snapshot(empty_list) == std::vector<int>{1, 2}));

		empty_list.clear();
		b_list.clear();
	}

	mtl_tb::section("intrusive_list — splice (전체 / 단일 / 구간)");
	{
		intrusive_list<Item> a_list, b_list;
		Item a1(1), a2(2), a3(3);
		Item b1(10), b2(20), b3(30), b4(40);
		a_list.push_back(a1); a_list.push_back(a2); a_list.push_back(a3);
		b_list.push_back(b1); b_list.push_back(b2); b_list.push_back(b3); b_list.push_back(b4);

		// 전체 splice: a_list 끝에 b_list 전체를 붙임
		a_list.splice(a_list.end(), b_list);
		MTL_CHECK(b_list.empty());
		MTL_CHECK(a_list.size() == 7);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{1, 2, 3, 10, 20, 30, 40}));

		// 단일 splice: a_list 의 '20' 노드(b2)를 begin 앞으로 이동
		auto it_b2 = a_list.iterator_to(b2);
		a_list.splice(a_list.begin(), a_list, it_b2);
		MTL_CHECK(a_list.size() == 7);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{20, 1, 2, 3, 10, 30, 40}));

		// 같은 자리 splice (no-op) 검증
		auto it_first = a_list.begin();
		a_list.splice(a_list.begin(), a_list, it_first);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{20, 1, 2, 3, 10, 30, 40}));

		// 다른 리스트로 단일 splice
		intrusive_list<Item> c_list;
		auto it_three = std::next(a_list.begin(), 3); // value=3
		c_list.splice(c_list.end(), a_list, it_three);
		MTL_CHECK(a_list.size() == 6);
		MTL_CHECK(c_list.size() == 1);
		MTL_CHECK(c_list.front().value == 3);

		// 구간 splice (다른 리스트로) — a_list의 [pos1, pos2) 구간을 c_list 끝으로
		// a_list 현재: 20, 1, 2, 10, 30, 40
		auto rfirst = std::next(a_list.begin(), 2); // value=2
		auto rlast = std::next(a_list.begin(), 5);  // value=40
		// 옮길 값: 2, 10, 30
		c_list.splice(c_list.end(), a_list, rfirst, rlast);
		MTL_CHECK(a_list.size() == 3);
		MTL_CHECK(c_list.size() == 4);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{20, 1, 40}));
		MTL_CHECK((snapshot(c_list) == std::vector<int>{3, 2, 10, 30}));

		// 동일 리스트 내 구간 splice — c_list 의 [first..last) 를 끝으로
		// c_list: 3, 2, 10, 30 → 첫 2개를 끝으로 옮김 → 10, 30, 3, 2
		auto cf = c_list.begin();
		auto cl = std::next(c_list.begin(), 2);
		c_list.splice(c_list.end(), c_list, cf, cl);
		MTL_CHECK((snapshot(c_list) == std::vector<int>{10, 30, 3, 2}));

		a_list.clear();
		c_list.clear();
	}

	mtl_tb::section("intrusive_list — remove / remove_if");
	{
		intrusive_list<Item> list;
		Item a(1), b(2), c(3), d(2), e(4);
		list.push_back(a);
		list.push_back(b);
		list.push_back(c);
		list.push_back(d);
		list.push_back(e);

		// remove(값)
		auto removed = list.remove(Item(2));
		MTL_CHECK(removed == 2);
		MTL_CHECK((snapshot(list) == std::vector<int>{1, 3, 4}));
		MTL_CHECK(!intrusive_list<Item>::is_linked(b));
		MTL_CHECK(!intrusive_list<Item>::is_linked(d));

		// remove_if
		auto removed2 = list.remove_if([](const Item& x) { return x.value >= 3; });
		MTL_CHECK(removed2 == 2);
		MTL_CHECK((snapshot(list) == std::vector<int>{1}));

		list.clear();
	}

	mtl_tb::section("intrusive_list — reverse");
	{
		intrusive_list<Item> list;
		Item a(1), b(2), c(3), d(4);
		list.push_back(a); list.push_back(b); list.push_back(c); list.push_back(d);

		list.reverse();
		MTL_CHECK((snapshot(list) == std::vector<int>{4, 3, 2, 1}));
		// 역방향 순회도 일관성
		int rev[] = { 1, 2, 3, 4 };
		int i = 0;
		for (auto rit = list.rbegin(); rit != list.rend(); ++rit, ++i)
			MTL_CHECK(rit->value == rev[i]);

		// 빈 리스트 reverse는 no-op
		intrusive_list<Item> empty_list;
		empty_list.reverse();
		MTL_CHECK(empty_list.empty());

		list.clear();
	}

	mtl_tb::section("intrusive_list — merge (default less / custom comp)");
	{
		intrusive_list<Item> a_list, b_list;
		Item a1(1), a2(3), a3(5);
		Item b1(2), b2(3), b3(4), b4(6);
		a_list.push_back(a1); a_list.push_back(a2); a_list.push_back(a3);
		b_list.push_back(b1); b_list.push_back(b2); b_list.push_back(b3); b_list.push_back(b4);

		a_list.merge(b_list);
		MTL_CHECK(b_list.empty());
		MTL_CHECK(a_list.size() == 7);
		MTL_CHECK((snapshot(a_list) == std::vector<int>{1, 2, 3, 3, 4, 5, 6}));

		// 안정성: 동일 키일 때 *this의 원소(value=3, a2)가 other의 원소(b2)보다 앞.
		// 주소를 비교해 검증.
		int seen = 0;
		const Item* threes[2] = { nullptr, nullptr };
		for (auto& x : a_list)
		{
			if (x.value == 3) threes[seen++] = &x;
		}
		MTL_CHECK(seen == 2);
		MTL_CHECK(threes[0] == &a2);
		MTL_CHECK(threes[1] == &b2);

		// custom comparator (내림차순으로 다시 병합)
		intrusive_list<Item> p, q;
		Item p1(9), p2(5), p3(1);
		Item q1(8), q2(4), q3(0);
		p.push_back(p1); p.push_back(p2); p.push_back(p3);
		q.push_back(q1); q.push_back(q2); q.push_back(q3);

		auto greater = [](const Item& x, const Item& y) { return x.value > y.value; };
		p.merge(q, greater);
		MTL_CHECK(q.empty());
		MTL_CHECK((snapshot(p) == std::vector<int>{9, 8, 5, 4, 1, 0}));

		a_list.clear();
		p.clear();
	}

	mtl_tb::section("intrusive_list — sort (default / custom)");
	{
		const int data[] = { 5, 2, 8, 1, 9, 3, 7, 4, 6, 0, 5, 2 };
		constexpr int N = sizeof(data) / sizeof(data[0]);

		Item items[N];
		for (int i = 0; i < N; ++i) items[i].value = data[i];

		intrusive_list<Item> list;
		for (auto& it : items) list.push_back(it);
		MTL_CHECK(list.size() == static_cast<std::size_t>(N));

		list.sort();
		std::vector<int> expected(data, data + N);
		std::sort(expected.begin(), expected.end());
		MTL_CHECK(snapshot(list) == expected);

		// 노드 주소 보존 — sort는 재배선만 한다.
		for (int i = 0; i < N; ++i)
			MTL_CHECK(intrusive_list<Item>::is_linked(items[i]));

		// custom comparator (내림차순)
		list.sort([](const Item& x, const Item& y) { return x.value > y.value; });
		std::sort(expected.begin(), expected.end(), std::greater<int>());
		MTL_CHECK(snapshot(list) == expected);

		// 1개 / 0개 sort no-op
		list.clear();
		list.sort();
		MTL_CHECK(list.empty());

		Item solo(42);
		list.push_back(solo);
		list.sort();
		MTL_CHECK(list.size() == 1);
		MTL_CHECK(list.front().value == 42);

		list.clear();
	}

	mtl_tb::section("intrusive_list — destructor unlink invariant");
	{
		Item a(1), b(2);
		{
			intrusive_list<Item> tmp;
			tmp.push_back(a);
			tmp.push_back(b);
			MTL_CHECK(intrusive_list<Item>::is_linked(a));
			MTL_CHECK(intrusive_list<Item>::is_linked(b));
		}
		// 컨테이너 소멸 후, 노드들은 자동으로 분리되어 재사용 가능해야 한다.
		MTL_CHECK(!intrusive_list<Item>::is_linked(a));
		MTL_CHECK(!intrusive_list<Item>::is_linked(b));

		intrusive_list<Item> next_owner;
		next_owner.push_back(a);
		next_owner.push_back(b);
		MTL_CHECK(next_owner.size() == 2);
		next_owner.clear();
	}

	mtl_tb::summary();
}
