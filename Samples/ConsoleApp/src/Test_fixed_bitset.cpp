#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_bitset.h>

#include <vector>

void run_test_fixed_bitset()
{
	mtl_tb::section("fixed_bitset");

	using bits_t = mtl::fixed_bitset<130>;

	{
		bits_t b;
		MTL_CHECK(b.none());
		MTL_CHECK(!b.any());
		MTL_CHECK(b.count() == 0);
		MTL_CHECK(b.size() == 130);
		MTL_CHECK(b.find_first() == bits_t::npos);
	}

	{
		bits_t b;
		b.set(0).set(64).set(129);
		MTL_CHECK(b.test(0));
		MTL_CHECK(b[64]);
		b[64] = false;
		MTL_CHECK(!b.test(64));
		b[64].flip();
		MTL_CHECK(b.test(64));
		b.reset(64);
		MTL_CHECK(!b.test(64));
		b.flip(64);
		MTL_CHECK(b.test(64));
	}

	{
		bits_t b;
		b.set();
		MTL_CHECK(b.all());
		MTL_CHECK(b.count() == 130);
		b.reset();
		MTL_CHECK(b.none());
		b.flip();
		MTL_CHECK(b.all());
	}

	{
		bits_t b;
		b.set(2).set(5).set(70).set(129);
		MTL_CHECK(b.find_first() == 2);
		MTL_CHECK(b.find_next(2) == 5);
		MTL_CHECK(b.find_next(5) == 70);
		MTL_CHECK(b.find_next(70) == 129);
		MTL_CHECK(b.find_next(129) == bits_t::npos);
		MTL_CHECK(b.find_first_unset() == 0);
		MTL_CHECK(b.find_next_unset(0) == 1);
	}

	{
		bits_t b;
		b.set(1).set(4).set(7);
		std::vector<size_t> idx;
		b.for_each_set([&](size_t i) { idx.push_back(i); });
		MTL_CHECK((idx == std::vector<size_t>{1, 4, 7}));

		idx.clear();
		for (auto i : b.set_indices()) idx.push_back(i);
		MTL_CHECK((idx == std::vector<size_t>{1, 4, 7}));
	}

	{
		bits_t a;
		a.set(1).set(3).set(5);
		bits_t b;
		b.set(3).set(4);

		auto c = a & b;
		auto d = a | b;
		auto e = a ^ b;
		auto n = ~a;
		MTL_CHECK(c.count() == 1 && c.test(3));
		MTL_CHECK(d.count() == 4);
		MTL_CHECK(e.count() == 3);
		MTL_CHECK(n.count() == (130 - a.count()));
		MTL_CHECK(d.contains(a));
		MTL_CHECK(a.intersects(b));
		MTL_CHECK(!(c != (a & b)));
	}
}
