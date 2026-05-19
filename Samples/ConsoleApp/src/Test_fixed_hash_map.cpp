#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_hash_map.h>

#include <string>

void run_test_fixed_hash_map()
{
	mtl_tb::section("fixed_hash_map");

	using map_t = mtl::fixed_hash_map<int, std::string, 16>;

	{
		map_t m;
		MTL_CHECK(m.empty());
		MTL_CHECK(m.size() == 0);
		MTL_CHECK(m.capacity() == 16);
		MTL_CHECK(m.bucket_count() >= 32);
	}

	{
		map_t m;
		auto [it1, ok1] = m.insert({1, "one"});
		MTL_CHECK(ok1 && it1->first == 1 && it1->second == "one");
		auto [it2, ok2] = m.insert({1, "one2"});
		MTL_CHECK(!ok2 && it2->second == "one");

		auto [it3, ok3] = m.try_emplace(2, "two");
		MTL_CHECK(ok3 && it3->second == "two");
		auto [it4, ok4] = m.try_emplace(2, "two-new");
		MTL_CHECK(!ok4 && it4->second == "two");

		m[3] = "three";
		MTL_CHECK(m.at(3) == "three");
		MTL_CHECK(m.contains(2));
		MTL_CHECK(m.count(2) == 1);
		MTL_CHECK(m.find(99) == m.end());

		auto [it5, inserted] = m.insert_or_assign(2, "TWO");
		MTL_CHECK(!inserted && it5->second == "TWO");
	}

	{
		map_t m{{1, "a"}, {2, "b"}, {3, "c"}};
		MTL_CHECK(m.erase(2) == 1);
		MTL_CHECK(!m.contains(2));

		auto it = m.find(1);
		MTL_CHECK(it != m.end());
		it = m.erase(it);
		MTL_CHECK(!m.contains(1));
		MTL_CHECK(m.size() == 1);
	}

	{
		map_t m{{1, "a"}, {2, "b"}, {3, "c"}};
		int key_sum = 0;
		size_t value_len = 0;
		for (const auto& [k, v] : m)
		{
			key_sum += k;
			value_len += v.size();
		}
		MTL_CHECK(key_sum == 6);
		MTL_CHECK(value_len == 3);
		MTL_CHECK(m.load_factor() > 0.0f);
	}

	{
		map_t a{{1, "x"}, {2, "y"}};
		map_t b(a);
		MTL_CHECK(a == b);

		map_t c(std::move(b));
		MTL_CHECK(c.size() == 2);

		map_t d{{9, "z"}};
		d = c;
		MTL_CHECK(d == c);

		map_t e{{7, "w"}};
		e = std::move(d);
		MTL_CHECK(e.size() == 2);

		map_t f{{100, "f"}};
		e.swap(f);
		MTL_CHECK(e.size() == 1 && e.contains(100));
		using std::swap;
		swap(e, f);
		MTL_CHECK(f.size() == 1 && f.contains(100));
	}
}
