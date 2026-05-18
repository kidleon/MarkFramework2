#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_wstring.h>
#include <string>
#include <format>

void run_bench_fixed_wstring()
{
	mtl_tb::section("BENCH fixed_wstring vs std::wstring");

	constexpr std::size_t kIter = 200000;

	mtl_tb::bench_ns("std::wstring construct+append", kIter, []{
		std::wstring s = L"hello";
		s += L" world";
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_ns("mtl::fixed_wstring<32> construct+append", kIter, []{
		mtl::fixed_wstring<32> s = L"hello";
		s += L" world";
		mtl_tb::do_not_optimize(s);
	});

	mtl_tb::bench_ns("std::wstring format", kIter, []{
		auto s = std::format(L"path/{}/{}", 12345, L"asset");
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_ns("mtl::fixed_wstring<64> format", kIter, []{
		auto s = mtl::fixed_wstring<64>::from_format(L"path/{}/{}", 12345, L"asset");
		mtl_tb::do_not_optimize(s);
	});
}
