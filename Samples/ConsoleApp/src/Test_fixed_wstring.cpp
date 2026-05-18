#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_wstring.h>

#include <string>
#include <string_view>
#include <format>

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
		s.append(L"DEF");
		MTL_CHECK(s == std::wstring_view(L"abcDEF"));
		s += L'!';
		MTL_CHECK(s.back() == L'!');
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
		s.erase(0, 1);
		MTL_CHECK(s == std::wstring_view(L"ello"));
		s.resize(2);
		MTL_CHECK(s == std::wstring_view(L"el"));

		fixed_wstring<16> a = L"xx";
		s.swap(a);
		MTL_CHECK(s == std::wstring_view(L"xx") && a == std::wstring_view(L"el"));
	}

	// 검색
	{
		fixed_wstring<32> s = L"hello world";
		MTL_CHECK(s.find(L'o') == 4);
		MTL_CHECK(s.find(L"world") == 6);
		MTL_CHECK(s.starts_with(L"hello"));
		MTL_CHECK(s.ends_with(L"world"));
		MTL_CHECK(s.contains(L"lo wo"));
		MTL_CHECK(s.substr(6, 5) == std::wstring_view(L"world"));
	}

	// std::format
	{
		fixed_wstring<16> name = L"world";
		auto out = std::format(L"hello, {}", name);
		MTL_CHECK(out == L"hello, world");
	}
}
