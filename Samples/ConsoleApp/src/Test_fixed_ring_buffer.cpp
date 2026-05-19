#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_ring_buffer.h>

#include <vector>

void run_test_fixed_ring_buffer()
{
	mtl_tb::section("fixed_ring_buffer");

	using rb_t = mtl::fixed_ring_buffer<int, 4>;

	{
		rb_t q;
		MTL_CHECK(q.empty());
		MTL_CHECK(q.size() == 0);
		MTL_CHECK(q.capacity() == 4);
		MTL_CHECK(q.available() == 4);
	}

	{
		rb_t q;
		MTL_CHECK(q.push(1));
		MTL_CHECK(q.emplace(2));
		MTL_CHECK(q.push(3));
		MTL_CHECK(q.front() == 1 && q.back() == 3);
		MTL_CHECK(q[1] == 2);

		int out = 0;
		MTL_CHECK(q.pop(out));
		MTL_CHECK(out == 1);
		MTL_CHECK(q.front() == 2);
		MTL_CHECK(q.pop());
		MTL_CHECK(q.pop());
		MTL_CHECK(!q.pop());
	}

	{
		rb_t q;
		q.push(1);
		q.push(2);
		q.push(3);
		q.push(4);
		MTL_CHECK(q.full());
		MTL_CHECK(!q.push(5));
		q.push_overwrite(5);
		MTL_CHECK(q.size() == 4);
		std::vector<int> v;
		for (int x : q) v.push_back(x);
		MTL_CHECK((v == std::vector<int>{2, 3, 4, 5}));

		q.emplace_overwrite(6);
		v.clear();
		for (int x : q) v.push_back(x);
		MTL_CHECK((v == std::vector<int>{3, 4, 5, 6}));
	}

	{
		rb_t a{1, 2, 3};
		rb_t b(a);
		MTL_CHECK(a == b);

		rb_t c(std::move(b));
		MTL_CHECK(c.size() == 3);

		rb_t d{9};
		d = c;
		MTL_CHECK(d == c);

		rb_t e{7};
		e = std::move(d);
		MTL_CHECK(e.size() == 3);

		rb_t f{100};
		e.swap(f);
		MTL_CHECK(e.size() == 1 && e.front() == 100);
		using std::swap;
		swap(e, f);
		MTL_CHECK(f.size() == 1 && f.front() == 100);
	}

	{
		rb_t q{1, 2, 3, 4};
		int sum = 0;
		for (int x : q) sum += x;
		MTL_CHECK(sum == 10);
		q.clear();
		MTL_CHECK(q.empty());
	}
}
