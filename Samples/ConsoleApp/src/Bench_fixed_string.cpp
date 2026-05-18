#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_string.h>
#include <string>
#include <format>

void run_bench_fixed_string()
{
	mtl_tb::section("BENCH fixed_string vs std::string");

	constexpr std::size_t kIter = 200000;

	mtl_tb::bench_ns("std::string short construct+append", kIter, []{
		std::string s = "hello";
		s += " world";
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_ns("mtl::fixed_string<32> short construct+append", kIter, []{
		mtl::fixed_string<32> s = "hello";
		s += " world";
		mtl_tb::do_not_optimize(s);
	});

	mtl_tb::bench_ns("std::string format", kIter, []{
		auto s = std::format("frame={} dt={:.3f}", 12345, 0.01666);
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_ns("mtl::fixed_string<64> format_append", kIter, []{
		auto s = mtl::fixed_string<64>::from_format("frame={} dt={:.3f}", 12345, 0.01666);
		mtl_tb::do_not_optimize(s);
	});

	mtl_tb::bench_ns("std::string find substr", kIter, []{
		std::string s = "hello world hello world hello";
		auto p = s.find("world", 6);
		mtl_tb::do_not_optimize(p);
	});
	mtl_tb::bench_ns("mtl::fixed_string<64> find substr", kIter, []{
		mtl::fixed_string<64> s = "hello world hello world hello";
		auto p = s.find("world", 6);
		mtl_tb::do_not_optimize(p);
	});
}
