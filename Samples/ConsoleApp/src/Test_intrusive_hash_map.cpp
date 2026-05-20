#include "pch.h"
#include "TestBenchCommon.h"

#include <intrusive_hash_map.h>

#include <string>
#include <vector>

namespace
{
	struct MapEntry : public mtl::intrusive_hash_node_key<int>
	{
		int value = 0;

		MapEntry(int key, int v)
			: mtl::intrusive_hash_node_key<int>(key), value(v)
		{
		}
	};

	struct id_tag {};
	struct name_tag {};

	struct TaggedMapEntry
		: public mtl::intrusive_hash_node_key<int, id_tag>
		, public mtl::intrusive_hash_node_key<std::string, name_tag>
	{
		int payload = 0;

		TaggedMapEntry(int id, std::string name, int value)
			: mtl::intrusive_hash_node_key<int, id_tag>(id)
			, mtl::intrusive_hash_node_key<std::string, name_tag>(std::move(name))
			, payload(value)
		{
		}
	};
}

void run_test_intrusive_hash_map()
{
	mtl_tb::section("intrusive_hash_map");

	using map_t = mtl::intrusive_hash_map<MapEntry, 8>;

	{
		map_t map;
		MTL_CHECK(map.empty());
		MTL_CHECK(map.size() == 0);
		MTL_CHECK(map.bucket_count() == 8);
		MTL_CHECK(map.begin() == map.end());
	}

	{
		map_t map;
		MapEntry a(1, 10), b(2, 20), c(3, 30), dup(2, 200);

		auto [it1, ok1] = map.insert(a);
		MTL_CHECK(ok1 && it1->value == 10);
		MTL_CHECK(map_t::is_linked(a));

		auto [it2, ok2] = map.insert(b);
		MTL_CHECK(ok2 && it2->value == 20);
		map.insert(c);
		MTL_CHECK(map.size() == 3);

		auto [dup_it, dup_ok] = map.insert(dup);
		MTL_CHECK(!dup_ok);
		MTL_CHECK(dup_it->value == 20);
		MTL_CHECK(!map_t::is_linked(dup));
		MTL_CHECK(map.size() == 3);

		MTL_CHECK(map.contains(1));
		MTL_CHECK(!map.contains(99));
		MTL_CHECK(map.find(2)->value == 20);

		map.find(2)->value = 22;
		MTL_CHECK(map.find(2)->value == 22);

		auto it = map.iterator_to(c);
		MTL_CHECK(it->value == 30);
		const auto& cmap = map;
		MTL_CHECK(cmap.iterator_to(c)->value == 30);
	}

	{
		map_t map;
		MapEntry a(1, 10), b(2, 20), c(3, 30), d(4, 40);
		map.insert(a);
		map.insert(b);
		map.insert(c);
		map.insert(d);

		int key_sum = 0;
		int value_sum = 0;
		for (auto& e : map)
		{
			key_sum += e.m_key;
			value_sum += e.value;
		}
		MTL_CHECK(key_sum == 10);
		MTL_CHECK(value_sum == 100);

		MTL_CHECK(map.erase(2) == 1);
		MTL_CHECK(!map.contains(2));
		MTL_CHECK(!map_t::is_linked(b));
		MTL_CHECK(map.erase(99) == 0);

		auto next = map.erase(map.find(3));
		MTL_CHECK(!map_t::is_linked(c));
		MTL_CHECK(map.size() == 2);
		MTL_CHECK(next == map.find(4) || next == map.end());

		map.unlink(a);
		MTL_CHECK(!map_t::is_linked(a));
		MTL_CHECK(map.size() == 1);

		map.clear();
		MTL_CHECK(map.empty());
		MTL_CHECK(!map_t::is_linked(d));
	}

	{
		map_t a;
		map_t b;
		MapEntry x(1, 10), y(2, 20), z(9, 90);
		a.insert(x);
		a.insert(y);
		b.insert(z);

		a.swap(b);
		MTL_CHECK(a.size() == 1 && a.contains(9));
		MTL_CHECK(b.size() == 2 && b.contains(1) && b.contains(2));

		using std::swap;
		swap(a, b);
		MTL_CHECK(a.size() == 2 && a.contains(1) && a.contains(2));
		MTL_CHECK(b.size() == 1 && b.contains(9));
	}

	{
		using by_id_t = mtl::intrusive_hash_map<TaggedMapEntry, 8, id_tag, int>;
		using by_name_t = mtl::intrusive_hash_map<TaggedMapEntry, 8, name_tag, std::string>;

		by_id_t by_id;
		by_name_t by_name;
		TaggedMapEntry a(1, "one", 10), b(2, "two", 20);

		MTL_CHECK(by_id.insert(a).second);
		MTL_CHECK(by_name.insert(a).second);
		MTL_CHECK(by_id.insert(b).second);
		MTL_CHECK(by_name.insert(b).second);

		MTL_CHECK(by_id.find(1)->payload == 10);
		MTL_CHECK(by_name.find("two")->payload == 20);
		MTL_CHECK(by_id_t::is_linked(a));
		MTL_CHECK(by_name_t::is_linked(a));

		by_id.erase(1);
		MTL_CHECK(!by_id_t::is_linked(a));
		MTL_CHECK(by_name_t::is_linked(a));
		MTL_CHECK(by_name.find("one")->payload == 10);

		by_name.clear();
		MTL_CHECK(!by_name_t::is_linked(a));
		MTL_CHECK(!by_name_t::is_linked(b));
		MTL_CHECK(by_id_t::is_linked(b));
		by_id.clear();
	}
}
