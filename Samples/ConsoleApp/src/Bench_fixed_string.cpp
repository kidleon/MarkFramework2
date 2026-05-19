#include "pch.h"
#include "TestBenchCommon.h"

#include <fixed_string.h>
#include <string>
#include <format>

void run_bench_fixed_string()
{
	mtl_tb::section("BENCH fixed_string vs std::string");

	constexpr std::size_t kIter = 200000;

	auto std_short = mtl_tb::bench_ns("std::string short construct+append", kIter, []{
		std::string s = "hello";
		s += " world";
		mtl_tb::do_not_optimize(s);
	});
	auto fixed_short = mtl_tb::bench_ns("mtl::fixed_string<32> short construct+append", kIter, []{
		mtl::fixed_string<32> s = "hello";
		s += " world";
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_compare("mtl::fixed_string short append", fixed_short, "std::string short append", std_short);

	auto std_fmt = mtl_tb::bench_ns("std::string format", kIter, []{
		auto s = std::format("frame={} dt={:.3f}", 12345, 0.01666);
		mtl_tb::do_not_optimize(s);
	});
	auto fixed_fmt = mtl_tb::bench_ns("mtl::fixed_string<64> format_append", kIter, []{
		auto s = mtl::fixed_string<64>::from_format("frame={} dt={:.3f}", 12345, 0.01666);
		mtl_tb::do_not_optimize(s);
	});
	mtl_tb::bench_compare("mtl::fixed_string format", fixed_fmt, "std::string format", std_fmt);

	auto std_find = mtl_tb::bench_ns("std::string find substr", kIter, []{
		std::string s = "hello world hello world hello";
		auto p = s.find("world", 6);
		mtl_tb::do_not_optimize(p);
	});
	auto fixed_find = mtl_tb::bench_ns("mtl::fixed_string<64> find substr", kIter, []{
		mtl::fixed_string<64> s = "hello world hello world hello";
		auto p = s.find("world", 6);
		mtl_tb::do_not_optimize(p);
	});
	mtl_tb::bench_compare("mtl::fixed_string find", fixed_find, "std::string find", std_find);
}
