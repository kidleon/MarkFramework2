#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_string.h>

#include <string>
#include <string_view>
#include <format>

void run_test_fixed_string()
{
	mtl_tb::section("fixed_string");

	using mtl::fixed_string;

	// ---- 생성 ----------------------------------------------------------
	{
		fixed_string<32> s;
		MTL_CHECK(s.empty() && s.size() == 0 && s.c_str()[0] == '\0');

		fixed_string<32> a("hello");
		MTL_CHECK(a.size() == 5 && a == "hello");

		fixed_string<32> b("hello world", 5);
		MTL_CHECK(b == "hello");

		fixed_string<32> c(4, 'x');
		MTL_CHECK(c == "xxxx");

		fixed_string<32> d(std::string_view("view"));
		MTL_CHECK(d == "view");

		fixed_string<32> e{ 'a','b','c' };
		MTL_CHECK(e == "abc");

		std::string src = "iters";
		fixed_string<32> f(src.begin(), src.end());
		MTL_CHECK(f == "iters");

		fixed_string<32> g(a);
		MTL_CHECK(g == a);

		fixed_string<64> h(a); // 다른 N
		MTL_CHECK(h == a);
	}

	// assign / operator=
	{
		fixed_string<16> s;
		s.assign("ABCDEFG");
		MTL_CHECK(s == "ABCDEFG");
		s.assign("XYZ", 2);
		MTL_CHECK(s == "XY");
		s.assign(3, '?');
		MTL_CHECK(s == "???");
		s.assign(std::string_view("svv"));
		MTL_CHECK(s == "svv");

		s = "world";
		MTL_CHECK(s == "world");
		s = std::string_view("v");
		MTL_CHECK(s == "v");
		s = { 'q','w','e' };
		MTL_CHECK(s == "qwe");
		s = '!';
		MTL_CHECK(s == "!");

		fixed_string<32> big = "long string here";
		s = big;
		MTL_CHECK(s == big);
	}

	// 요소 접근
	{
		fixed_string<16> s = "hello";
		MTL_CHECK(s[0] == 'h' && s[4] == 'o');
		MTL_CHECK(s.at(1) == 'e');
		MTL_CHECK(s.front() == 'h' && s.back() == 'o');
		MTL_CHECK(std::string(s.c_str()) == "hello");
		MTL_CHECK(s.data()[2] == 'l');
		const auto& cs = s;
		MTL_CHECK(cs[1] == 'e' && cs.at(1) == 'e' && cs.front() == 'h' && cs.back() == 'o');
	}

	// 이터레이터
	{
		fixed_string<16> s = "abc";
		std::string out;
		for (auto it = s.begin(); it != s.end(); ++it) out += *it;
		MTL_CHECK(out == "abc");
		out.clear();
		for (auto it = s.cbegin(); it != s.cend(); ++it) out += *it;
		MTL_CHECK(out == "abc");
		out.clear();
		for (auto it = s.rbegin(); it != s.rend(); ++it) out += *it;
		MTL_CHECK(out == "cba");
		out.clear();
		for (auto it = s.crbegin(); it != s.crend(); ++it) out += *it;
		MTL_CHECK(out == "cba");
	}

	// capacity
	{
		fixed_string<8> s = "abc";
		MTL_CHECK(s.size() == 3 && s.length() == 3);
		MTL_CHECK(!s.empty() && !s.full());
		MTL_CHECK(s.capacity() == 8 && s.max_size() == 8);
		MTL_CHECK(s.available() == 5);
		s.assign("ABCDEFGH");
		MTL_CHECK(s.full());
	}

	// 변환
	{
		fixed_string<8> s = "abc";
		std::string_view v = s.to_string_view();
		MTL_CHECK(v == "abc");
		std::string_view v2 = s;
		MTL_CHECK(v2 == "abc");
	}

	// clear / push_back / pop_back
	{
		fixed_string<8> s;
		s.push_back('a'); s.push_back('b');
		MTL_CHECK(s == "ab");
		s.pop_back();
		MTL_CHECK(s == "a");
		s.clear();
		MTL_CHECK(s.empty() && s.c_str()[0] == '\0');
	}

	// append / operator+=
	{
		fixed_string<32> s = "he";
		s.append("llo");
		MTL_CHECK(s == "hello");
		s.append(" world!!!", 6);
		MTL_CHECK(s == "hello world");
		s.append(2, '!');
		MTL_CHECK(s == "hello world!!");
		s.append(std::string_view(" end"));
		MTL_CHECK(s == "hello world!! end");
		fixed_string<8> tail = " tail";
		s.append(tail);
		MTL_CHECK(s.ends_with(" tail"));

		fixed_string<16> a = "x";
		a += "yz";
		a += '!';
		a += std::string_view("!!");
		fixed_string<8> b = "??";
		a += b;
		MTL_CHECK(a == "xyz!!!??");
	}

	// format_append / from_format
	{
		fixed_string<32> s;
		s.format_append("{}+{}={}", 1, 2, 3);
		MTL_CHECK(s == "1+2=3");
		s.format_append(" [{:.2f}]", 3.14159);
		MTL_CHECK(s.contains("3.14"));

		auto r = fixed_string<32>::from_format("hello {}", 7);
		MTL_CHECK(r == "hello 7");
	}

	// insert / erase / resize
	{
		fixed_string<32> s = "helo";
		s.insert(2, "l", 1);
		MTL_CHECK(s == "hello");
		s.insert(5, " world");
		MTL_CHECK(s == "hello world");
		s.insert(0, std::string_view("!! "));
		MTL_CHECK(s == "!! hello world");
		s.insert(0, 2, '@');
		MTL_CHECK(s == "@@!! hello world");

		s.erase(0, 5);
		MTL_CHECK(s == "hello world");
		s.erase(s.begin());
		MTL_CHECK(s == "ello world");
		s.erase(s.begin(), s.begin() + 4);
		MTL_CHECK(s == " world");

		s.resize(3);
		MTL_CHECK(s == " wo");
		s.resize(6, '!');
		MTL_CHECK(s == " wo!!!");
	}

	// swap
	{
		fixed_string<16> a = "abc";
		fixed_string<16> b = "12345";
		a.swap(b);
		MTL_CHECK(a == "12345" && b == "abc");
		using std::swap; swap(a, b);
		MTL_CHECK(a == "abc" && b == "12345");
	}

	// 검색
	{
		fixed_string<32> s = "hello world hello";
		MTL_CHECK(s.find('o') == 4);
		MTL_CHECK(s.find("world") == 6);
		MTL_CHECK(s.find(std::string_view("hello"), 1) == 12);
		MTL_CHECK(s.find('z') == fixed_string<32>::npos);

		MTL_CHECK(s.rfind('o') == 7);
		MTL_CHECK(s.rfind(std::string_view("hello")) == 12);

		MTL_CHECK(s.contains('w'));
		MTL_CHECK(s.contains("world"));
		MTL_CHECK(s.contains(std::string_view("hello")));

		MTL_CHECK(s.starts_with("hello"));
		MTL_CHECK(s.starts_with('h'));
		MTL_CHECK(s.ends_with("hello"));
		MTL_CHECK(s.ends_with('o'));

		auto sub = s.substr(6, 5);
		MTL_CHECK(sub == "world");
		MTL_CHECK(s.compare(std::string_view("hello world hello")) == 0);
	}

	// 비교 연산자
	{
		fixed_string<8> a = "abc";
		fixed_string<16> b = "abc";
		fixed_string<16> c = "abd";
		MTL_CHECK(a == b);
		MTL_CHECK(a != c);
		MTL_CHECK(a < c);
		MTL_CHECK(c > a);
		MTL_CHECK(a <= b);
		MTL_CHECK(b >= a);
		MTL_CHECK(a == "abc");
		MTL_CHECK("abc" == a);
		MTL_CHECK(a == std::string_view("abc"));
		MTL_CHECK(std::string_view("abc") == a);
	}

	// std::format 통합
	{
		fixed_string<16> name = "world";
		auto formatted = std::format("hello, {}", name);
		MTL_CHECK(formatted == "hello, world");
	}
}
