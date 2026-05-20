#include "pch.h"
#include "TestBenchCommon.h"

#include <intrusive_hash_set.h>

#include <string>
#include <vector>

namespace
{
	struct SetNode : public mtl::intrusive_hash_node_key<int>
	{
		int payload = 0;

		SetNode(int key, int value)
			: mtl::intrusive_hash_node_key<int>(key), payload(value)
		{
		}
	};

	struct active_tag {};
	struct name_set_tag {};

	struct TaggedSetNode
		: public mtl::intrusive_hash_node_key<int, active_tag>
		, public mtl::intrusive_hash_node_key<std::string, name_set_tag>
	{
		int payload = 0;

		TaggedSetNode(int id, std::string name, int value)
			: mtl::intrusive_hash_node_key<int, active_tag>(id)
			, mtl::intrusive_hash_node_key<std::string, name_set_tag>(std::move(name))
			, payload(value)
		{
		}
	};
}

void run_test_intrusive_hash_set()
{
	mtl_tb::section("intrusive_hash_set");

	using set_t = mtl::intrusive_hash_set<SetNode, 8>;

	{
		set_t set;
		MTL_CHECK(set.empty());
		MTL_CHECK(set.size() == 0);
		MTL_CHECK(set.bucket_count() == 8);
		MTL_CHECK(set.begin() == set.end());
	}

	{
		set_t set;
		SetNode a(1, 10), b(2, 20), c(3, 30), dup(2, 200);

		auto [it1, ok1] = set.insert(a);
		MTL_CHECK(ok1 && it1->payload == 10);
		MTL_CHECK(set_t::is_linked(a));

		set.insert(b);
		set.insert(c);
		MTL_CHECK(set.size() == 3);

		auto [dup_it, dup_ok] = set.insert(dup);
		MTL_CHECK(!dup_ok);
		MTL_CHECK(dup_it->payload == 20);
		MTL_CHECK(!set_t::is_linked(dup));

		MTL_CHECK(set.contains(1));
		MTL_CHECK(!set.contains(99));
		MTL_CHECK(set.find(2)->payload == 20);

		auto it = set.iterator_to(c);
		MTL_CHECK(it->payload == 30);
		const auto& cset = set;
		MTL_CHECK(cset.iterator_to(c)->payload == 30);
	}

	{
		set_t set;
		SetNode a(1, 10), b(2, 20), c(3, 30), d(4, 40);
		set.insert(a);
		set.insert(b);
		set.insert(c);
		set.insert(d);

		int key_sum = 0;
		int payload_sum = 0;
		for (auto& e : set)
		{
			key_sum += e.m_key;
			payload_sum += e.payload;
		}
		MTL_CHECK(key_sum == 10);
		MTL_CHECK(payload_sum == 100);

		MTL_CHECK(set.erase(2) == 1);
		MTL_CHECK(!set.contains(2));
		MTL_CHECK(!set_t::is_linked(b));
		MTL_CHECK(set.erase(99) == 0);

		auto next = set.erase(set.find(3));
		MTL_CHECK(!set_t::is_linked(c));
		MTL_CHECK(set.size() == 2);
		MTL_CHECK(next == set.find(4) || next == set.end());

		set.unlink(a);
		MTL_CHECK(!set_t::is_linked(a));
		MTL_CHECK(set.size() == 1);

		set.clear();
		MTL_CHECK(set.empty());
		MTL_CHECK(!set_t::is_linked(d));
	}

	{
		set_t a;
		set_t b;
		SetNode x(1, 10), y(2, 20), z(9, 90);
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
		using active_set_t = mtl::intrusive_hash_set<TaggedSetNode, 8, active_tag, int>;
		using name_set_t = mtl::intrusive_hash_set<TaggedSetNode, 8, name_set_tag, std::string>;

		active_set_t active;
		name_set_t by_name;
		TaggedSetNode a(1, "one", 10), b(2, "two", 20);

		MTL_CHECK(active.insert(a).second);
		MTL_CHECK(by_name.insert(a).second);
		MTL_CHECK(active.insert(b).second);
		MTL_CHECK(by_name.insert(b).second);

		MTL_CHECK(active.find(1)->payload == 10);
		MTL_CHECK(by_name.find("two")->payload == 20);
		MTL_CHECK(active_set_t::is_linked(a));
		MTL_CHECK(name_set_t::is_linked(a));

		active.erase(1);
		MTL_CHECK(!active_set_t::is_linked(a));
		MTL_CHECK(name_set_t::is_linked(a));
		MTL_CHECK(by_name.find("one")->payload == 10);

		by_name.clear();
		MTL_CHECK(!name_set_t::is_linked(a));
		MTL_CHECK(!name_set_t::is_linked(b));
		MTL_CHECK(active_set_t::is_linked(b));
		active.clear();
	}
}
