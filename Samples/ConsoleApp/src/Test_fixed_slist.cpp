#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_slist.h>

#include <vector>

void run_test_fixed_slist()
{
	mtl_tb::section("fixed_slist");

	using mtl::fixed_slist;

	{
		fixed_slist<int, 8> l;
		MTL_CHECK(l.empty());
		MTL_CHECK(l.size() == 0);
		MTL_CHECK(l.capacity() == 8);
		MTL_CHECK(l.max_size() > 0);
	}

	{
		fixed_slist<int, 8> l{1, 2, 3};
		l.push_front(0);
		auto& f = l.emplace_front(-1);
		MTL_CHECK(f == -1);
		MTL_CHECK(l.front() == -1);
		MTL_CHECK(l.size() == 5);
	}

	{
		fixed_slist<int, 16> l{1, 2, 3};
		auto it = l.before_begin();
		it = l.insert_after(it, 0);
		it = l.insert_after(it, 9);
		l.insert_after(l.begin(), 2, 7);
		int more[] = {8, 10};
		l.insert_after(l.begin(), more, more + 2);
		l.insert_after(l.before_begin(), { -1 });
		MTL_CHECK(l.size() == 10);

		auto e1 = l.erase_after(l.before_begin());
		MTL_CHECK(*e1 == 0);
		auto bb = l.before_begin();
		auto next = l.begin();
		++next;
		l.erase_after(bb, next);
		MTL_CHECK(l.size() == 8);
	}

	{
		fixed_slist<int, 16> l{1, 2, 2, 3, 4, 5};
		auto r1 = l.remove(2);
		MTL_CHECK(r1 == 2);
		auto r2 = l.remove_if([](int v) { return (v % 2) == 1; });
		MTL_CHECK(r2 == 3);
		MTL_CHECK(l.size() == 1 && l.front() == 4);
	}

	{
		fixed_slist<int, 8> l{1, 2, 3, 4};
		l.reverse();
		std::vector<int> out;
		for (int v : l) out.push_back(v);
		MTL_CHECK((out == std::vector<int>{4, 3, 2, 1}));
	}

	{
		fixed_slist<int, 8> a{1, 2, 3};
		fixed_slist<int, 8> b{9, 8};
		a.swap(b);
		MTL_CHECK(a.size() == 2 && a.front() == 9);
		using std::swap;
		swap(a, b);
		MTL_CHECK(a.size() == 3 && a.front() == 1);

		fixed_slist<int, 4> c{1, 2};
		fixed_slist<int, 8> d{1, 2};
		fixed_slist<int, 8> e{1, 3};
		MTL_CHECK(c == d);
		MTL_CHECK(c != e);
		MTL_CHECK(c < e);
		MTL_CHECK(e > c);
	}
}
