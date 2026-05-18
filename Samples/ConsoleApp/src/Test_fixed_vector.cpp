#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_vector.h>

#include <numeric>
#include <vector>
#include <string>

namespace
{
	struct NonTrivial
	{
		int x = 0;
		std::string s;

		NonTrivial() = default;
		NonTrivial(int v) : x(v), s(std::to_string(v)) {}
		NonTrivial(int v, const char* str) : x(v), s(str) {}
		bool operator==(const NonTrivial& other) const { return x == other.x && s == other.s; }
		bool operator<(const NonTrivial& other) const { return x < other.x; }
	};
}

void run_test_fixed_vector()
{
	mtl_tb::section("fixed_vector");

	using mtl::fixed_vector;

	// ---- 생성/기본 상태 ------------------------------------------------
	{
		fixed_vector<int, 8> v;
		MTL_CHECK(v.empty());
		MTL_CHECK(!v.full());
		MTL_CHECK(v.size() == 0);
		MTL_CHECK(v.capacity() == 8);
		MTL_CHECK(v.max_size() == 8);
		MTL_CHECK(v.begin() == v.end());
	}

	// count, value 생성자 + assign
	{
		fixed_vector<int, 8> v(5, 7);
		MTL_CHECK(v.size() == 5);
		for (auto x : v) MTL_CHECK(x == 7);

		v.assign(3, 1);
		MTL_CHECK(v.size() == 3);
		MTL_CHECK(v[0] == 1 && v[2] == 1);
	}

	// initializer_list / iterator 생성자
	{
		fixed_vector<int, 8> v{1,2,3,4};
		MTL_CHECK(v.size() == 4);
		MTL_CHECK(v.front() == 1 && v.back() == 4);

		std::vector<int> src{10,20,30};
		fixed_vector<int, 8> v2(src.begin(), src.end());
		MTL_CHECK(v2.size() == 3);
		MTL_CHECK(v2[0] == 10 && v2[2] == 30);

		v2.assign(src.begin(), src.end());
		MTL_CHECK(v2.size() == 3);
		v2.assign({4,5,6,7});
		MTL_CHECK(v2.size() == 4 && v2[3] == 7);
	}

	// 복사/이동 생성자/대입
	{
		fixed_vector<NonTrivial, 4> a;
		a.emplace_back(1);
		a.emplace_back(2);
		fixed_vector<NonTrivial, 4> b(a);
		MTL_CHECK(b.size() == 2 && b[1] == NonTrivial(2));

		fixed_vector<NonTrivial, 4> c(std::move(a));
		MTL_CHECK(c.size() == 2);
		MTL_CHECK(a.empty()); // moved-from cleared

		fixed_vector<NonTrivial, 4> d;
		d = c;
		MTL_CHECK(d == c);
		fixed_vector<NonTrivial, 4> e;
		e = std::move(d);
		MTL_CHECK(e.size() == 2);
		MTL_CHECK(d.empty());

		e = {NonTrivial(9), NonTrivial(8)};
		MTL_CHECK(e.size() == 2 && e[0].x == 9);
	}

	// 요소 접근
	{
		fixed_vector<int, 4> v{1,2,3};
		MTL_CHECK(v.at(0) == 1 && v.at(2) == 3);
		MTL_CHECK(v[1] == 2);
		MTL_CHECK(v.front() == 1 && v.back() == 3);
		MTL_CHECK(v.data() == &v[0]);
		const auto& cv = v;
		MTL_CHECK(cv.at(0) == 1 && cv.front() == 1 && cv.back() == 3 && cv.data()[1] == 2);
	}

	// 이터레이터(정/역방향, const)
	{
		fixed_vector<int, 8> v{1,2,3,4,5};
		int sum = 0;
		for (auto it = v.begin(); it != v.end(); ++it) sum += *it;
		MTL_CHECK(sum == 15);

		sum = 0;
		for (auto it = v.cbegin(); it != v.cend(); ++it) sum += *it;
		MTL_CHECK(sum == 15);

		std::vector<int> rev;
		for (auto it = v.rbegin(); it != v.rend(); ++it) rev.push_back(*it);
		MTL_CHECK((rev == std::vector<int>{5,4,3,2,1}));

		rev.clear();
		for (auto it = v.crbegin(); it != v.crend(); ++it) rev.push_back(*it);
		MTL_CHECK((rev == std::vector<int>{5,4,3,2,1}));
	}

	// push_back / emplace_back / pop_back / clear
	{
		fixed_vector<NonTrivial, 4> v;
		v.push_back(NonTrivial(1));
		NonTrivial tmp(2);
		v.push_back(tmp);
		auto& ref = v.emplace_back(3, "three");
		MTL_CHECK(ref.x == 3 && ref.s == "three");
		MTL_CHECK(v.size() == 3);
		MTL_CHECK(v.full() == false);
		v.emplace_back(4);
		MTL_CHECK(v.full());

		v.pop_back();
		MTL_CHECK(v.size() == 3);

		v.clear();
		MTL_CHECK(v.empty());
	}

	// insert (single/range/count/il) + emplace
	{
		fixed_vector<int, 16> v{1,2,5,6};
		v.insert(v.begin() + 2, 3);                 // 1 2 3 5 6
		MTL_CHECK(v[2] == 3);
		v.insert(v.begin() + 3, 1, 4);              // 1 2 3 4 5 6
		MTL_CHECK(v[3] == 4 && v.size() == 6);

		int more[] = {7, 8};
		v.insert(v.end(), more, more + 2);          // 1..8
		MTL_CHECK(v.back() == 8 && v.size() == 8);

		v.insert(v.begin(), {-1, 0});               // -1 0 1..8
		MTL_CHECK(v.front() == -1 && v[1] == 0 && v.size() == 10);

		v.emplace(v.begin() + 1, 100);
		MTL_CHECK(v[1] == 100);
	}

	// erase / erase 범위 / erase_unsorted
	{
		fixed_vector<int, 16> v{1,2,3,4,5};
		auto it = v.erase(v.begin() + 1);
		MTL_CHECK(*it == 3 && v.size() == 4);

		v.erase(v.begin(), v.begin() + 2);
		MTL_CHECK(v.size() == 2 && v[0] == 4 && v[1] == 5);

		fixed_vector<int, 8> u{10,20,30,40};
		u.erase_unsorted(u.begin() + 1); // -> {10, 40, 30}
		MTL_CHECK(u.size() == 3);
		MTL_CHECK(u[0] == 10 && u[1] == 40 && u[2] == 30);

		u.erase_unsorted(u.end() - 1); // 마지막
		MTL_CHECK(u.size() == 2);
	}

	// swap (멤버 + ADL)
	{
		fixed_vector<int, 8> a{1,2,3};
		fixed_vector<int, 8> b{9,8,7,6};
		a.swap(b);
		MTL_CHECK(a.size() == 4 && a[0] == 9);
		MTL_CHECK(b.size() == 3 && b[0] == 1);
		using std::swap; swap(a, b);
		MTL_CHECK(a.size() == 3 && a[0] == 1);
	}

	// 비교 연산자
	{
		fixed_vector<int, 4> a{1,2,3};
		fixed_vector<int, 8> b{1,2,3};
		fixed_vector<int, 8> c{1,2,4};
		MTL_CHECK(a == b);
		MTL_CHECK(a != c);
		MTL_CHECK(a < c);
		MTL_CHECK(c > a);
		MTL_CHECK(a <= b);
		MTL_CHECK(a >= b);
	}
}
