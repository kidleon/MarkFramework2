#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_wstring.h>

#include <string>
#include <string_view>
#include <format>

using namespace std::literals;

void run_test_fixed_wstring()
{
	mtl_tb::section("fixed_wstring");

	using mtl::fixed_wstring;

	{
		fixed_wstring<16> s;
		MTL_CHECK(s.empty() && s.c_str()[0] == L'\0');

		fixed_wstring<32> a(L"hello");
		MTL_CHECK(a.size() == 5);
		MTL_CHECK(a == std::wstring_view(L"hello"));

		fixed_wstring<32> b(L"hello world", 5);
		MTL_CHECK(b == std::wstring_view(L"hello"));

		fixed_wstring<32> c(4, L'x');
		MTL_CHECK(c == std::wstring_view(L"xxxx"));

		fixed_wstring<32> d(std::wstring_view(L"view"));
		MTL_CHECK(d == std::wstring_view(L"view"));
	}

	// assign/append/format
	{
		fixed_wstring<32> s;
		s.assign(L"abc");
		MTL_CHECK(s == std::wstring_view(L"abc"));
		s.assign(L"XYZ", 2);
		MTL_CHECK(s == L"XY"sv);
		s.assign(3, L'?');
		MTL_CHECK(s == L"???"sv);
		s.assign(std::wstring_view(L"svv"));
		MTL_CHECK(s == L"svv"sv);

		s = L"world";
		MTL_CHECK(s == L"world"sv);
		s = std::wstring_view(L"v");
		MTL_CHECK(s == L"v"sv);
		s = { L'q',L'w',L'e' };
		MTL_CHECK(s == L"qwe"sv);
		s = L'!';
		MTL_CHECK(s == L"!"sv);

		fixed_wstring<64> big = L"long wide string";
		s = big;
		MTL_CHECK(s == big);

		s.clear();
		s.push_back(L'a');
		s.push_back(L'b');
		MTL_CHECK(s == L"ab"sv);
		s.pop_back();
		MTL_CHECK(s == L"a"sv);
		s.assign(L"abc");

		s.append(L"DEF");
		MTL_CHECK(s == std::wstring_view(L"abcDEF"));
		s += L'!';
		MTL_CHECK(s.back() == L'!');
		s.append(2, L'!');
		MTL_CHECK(s.ends_with(L"!!!"sv));
		s.append(std::wstring_view(L" end"));
		MTL_CHECK(s.contains(L" end"sv));
		fixed_wstring<8> tail = L" t";
		s += tail;
		MTL_CHECK(s.ends_with(L" t"sv));

		fixed_wstring<16> short_s = L"x";
		short_s += L"yz";
		short_s += L'!';
		short_s += std::wstring_view(L"!!");
		fixed_wstring<8> b = L"??";
		short_s += b;
		MTL_CHECK(short_s == L"xyz!!!??"sv);

		fixed_wstring<8> conv = L"abc";
		std::wstring_view v = conv.to_string_view();
		MTL_CHECK(v == L"abc"sv);
		std::wstring_view v2 = conv;
		MTL_CHECK(v2 == L"abc"sv);

		s.format_append(L" {}+{}", 1, 2);
		MTL_CHECK(s.contains(std::wstring_view(L"1+2")));

		auto r = fixed_wstring<32>::from_format(L"v={}", 42);
		MTL_CHECK(r == std::wstring_view(L"v=42"));
	}

	// insert/erase/resize/swap
	{
		fixed_wstring<32> s = L"helo";
		s.insert(2, L"l", 1);
		MTL_CHECK(s == std::wstring_view(L"hello"));
		s.insert(5, L" world");
		MTL_CHECK(s == L"hello world"sv);
		s.insert(0, std::wstring_view(L"!! "));
		MTL_CHECK(s == L"!! hello world"sv);
		s.insert(0, 2, L'@');
		MTL_CHECK(s == L"@@!! hello world"sv);

		s.erase(0, 5);
		MTL_CHECK(s == L"hello world"sv);
		s.erase(s.begin());
		MTL_CHECK(s == L"ello world"sv);
		s.erase(s.begin(), s.begin() + 4);
		MTL_CHECK(s == L" world"sv);

		s.resize(3);
		MTL_CHECK(s == L" wo"sv);
		s.resize(6, L'!');
		MTL_CHECK(s == L" wo!!!"sv);

		fixed_wstring<32> other = L"xx";
		using std::swap;
		swap(s, other);
		MTL_CHECK(s == std::wstring_view(L"xx") && other == std::wstring_view(L" wo!!!"));
		s.erase(0, 1);
		MTL_CHECK(s == std::wstring_view(L"x"));
	}

	// 검색
	{
		fixed_wstring<32> s = L"hello world hello";
		MTL_CHECK(s.find(L'o') == 4);
		MTL_CHECK(s.find(L"world") == 6);
		MTL_CHECK(s.find(std::wstring_view(L"hello"), 1) == 12);
		MTL_CHECK(s.find(L'z') == fixed_wstring<32>::npos);

		MTL_CHECK(s.rfind(L'o') == 16);
		MTL_CHECK(s.rfind(std::wstring_view(L"hello")) == 12);

		MTL_CHECK(s.starts_with(L"hello"));
		MTL_CHECK(s.starts_with(L'h'));
		MTL_CHECK(s.ends_with(L"hello"));
		MTL_CHECK(s.ends_with(L'o'));

		MTL_CHECK(!s.ends_with(L"world"));
		MTL_CHECK(s.contains(L"lo wo"));
		auto sub = s.substr(6, 5);
		MTL_CHECK(sub == std::wstring_view(L"world"));
		MTL_CHECK(s.compare(std::wstring_view(L"hello world hello")) == 0);
	}

	// 비교 연산자
	{
		fixed_wstring<8> a = L"abc";
		fixed_wstring<16> b = L"abc";
		fixed_wstring<16> c = L"abd";
		MTL_CHECK(a == b);
		MTL_CHECK(a != c);
		MTL_CHECK(a < c);
		MTL_CHECK(c > a);
		MTL_CHECK(a <= b);
		MTL_CHECK(b >= a);
		MTL_CHECK(a == L"abc");
		MTL_CHECK(L"abc" == a);
		MTL_CHECK(a == std::wstring_view(L"abc"));
		MTL_CHECK(std::wstring_view(L"abc") == a);
	}

	// std::format
	{
		fixed_wstring<16> name = L"world";
		auto out = std::format(L"hello, {}", name);
		MTL_CHECK(out == L"hello, world");
	}
}
