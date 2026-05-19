#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_hash_set.h>

void run_test_fixed_hash_set()
{
	mtl_tb::section("fixed_hash_set");

	using set_t = mtl::fixed_hash_set<int, 16>;

	{
		set_t s;
		MTL_CHECK(s.empty());
		MTL_CHECK(s.size() == 0);
		MTL_CHECK(s.capacity() == 16);
		MTL_CHECK(s.bucket_count() >= 32);
	}

	{
		set_t s;
		auto [it1, ok1] = s.insert(1);
		MTL_CHECK(ok1 && *it1 == 1);
		auto [it2, ok2] = s.insert(1);
		MTL_CHECK(!ok2 && *it2 == 1);

		auto [it3, ok3] = s.emplace(2);
		MTL_CHECK(ok3 && *it3 == 2);

		MTL_CHECK(s.contains(2));
		MTL_CHECK(s.count(2) == 1);
		MTL_CHECK(s.find(99) == s.end());
	}

	{
		set_t s{1, 2, 3, 4};
		MTL_CHECK(s.erase(2) == 1);
		MTL_CHECK(!s.contains(2));

		auto it = s.find(1);
		MTL_CHECK(it != s.end());
		it = s.erase(it);
		MTL_CHECK(!s.contains(1));
		MTL_CHECK(s.size() == 2);
	}

	{
		set_t s{1, 2, 3};
		int sum = 0;
		for (int v : s) sum += v;
		MTL_CHECK(sum == 6);
		MTL_CHECK(s.load_factor() > 0.0f);
	}

	{
		set_t a{1, 2};
		set_t b(a);
		MTL_CHECK(a == b);

		set_t c(std::move(b));
		MTL_CHECK(c.size() == 2);

		set_t d{9};
		d = c;
		MTL_CHECK(d == c);

		set_t e{7};
		e = std::move(d);
		MTL_CHECK(e.size() == 2);

		set_t f{100};
		e.swap(f);
		MTL_CHECK(e.size() == 1 && e.contains(100));
		using std::swap;
		swap(e, f);
		MTL_CHECK(f.size() == 1 && f.contains(100));
	}
}
