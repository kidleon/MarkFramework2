#pragma once

#include <chrono>
#include <cstdio>
#include <cstddef>
#include <string>

namespace mtl_tb
{
	// -----------------------------------------------------------------------
	// 미니 테스트 프레임워크
	// -----------------------------------------------------------------------
	struct test_stats
	{
		int total = 0;
		int passed = 0;
		int failed = 0;
	};

	inline test_stats& g_stats()
	{
		static test_stats s;
		return s;
	}

	inline void report_check(bool ok, const char* expr, const char* file, int line)
	{
		auto& s = g_stats();
		++s.total;
		if (ok) { ++s.passed; }
		else
		{
			++s.failed;
			std::printf("    [FAIL] %s:%d  %s\n", file, line, expr);
		}
	}

	inline void section(const char* name)
	{
		std::printf("\n=== %s ===\n", name);
	}

	inline void summary()
	{
		const auto& s = g_stats();
		std::printf("\n--------------------------------------------------\n");
		std::printf("Total: %d, Passed: %d, Failed: %d\n", s.total, s.passed, s.failed);
		std::printf("--------------------------------------------------\n");
	}

	// -----------------------------------------------------------------------
	// 미니 벤치 헬퍼
	// -----------------------------------------------------------------------
	template <typename F>
	inline double bench_ns(const char* label, std::size_t iterations, F&& f)
	{
		// warm-up
		for (std::size_t i = 0; i < 4; ++i) f();

		using clock = std::chrono::high_resolution_clock;
		const auto t0 = clock::now();
		for (std::size_t i = 0; i < iterations; ++i) f();
		const auto t1 = clock::now();
		const double total_ns =
			static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
		const double per_iter = total_ns / static_cast<double>(iterations);
		std::printf("    %-60s %10.2f ns/iter  (x%zu)\n", label, per_iter, iterations);
		return per_iter;
	}

	inline void bench_compare(const char* lhs_label, double lhs_ns, const char* rhs_label, double rhs_ns)
	{
		if (lhs_ns <= 0.0 || rhs_ns <= 0.0) return;

		if (lhs_ns < rhs_ns)
		{
			std::printf("      -> %s is %.2fx faster than %s\n", lhs_label, rhs_ns / lhs_ns, rhs_label);
		}
		else if (rhs_ns < lhs_ns)
		{
			std::printf("      -> %s is %.2fx faster than %s\n", rhs_label, lhs_ns / rhs_ns, lhs_label);
		}
		else
		{
			std::printf("      -> %s and %s are equal speed\n", lhs_label, rhs_label);
		}
	}

	// 컴파일러가 결과를 버리지 않도록 강제 (간단 버전)
	template <typename T>
	inline void do_not_optimize(T const& v)
	{
		// volatile 변수에 대입해 사이드 이펙트 발생.
		volatile auto sink = reinterpret_cast<std::uintptr_t>(&v);
		(void)sink;
	}
} // namespace mtl_tb

#define MTL_CHECK(expr) ::mtl_tb::report_check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

// -----------------------------------------------------------------------
// 각 컨테이너 테스트/벤치 진입점
// -----------------------------------------------------------------------
void run_test_fixed_vector();
void run_test_fixed_string();
void run_test_fixed_wstring();
void run_test_fixed_list();
void run_test_fixed_slist();
void run_test_fixed_hash_map();
void run_test_fixed_hash_set();
void run_test_fixed_bitset();
void run_test_fixed_ring_buffer();
void run_test_fixed_pool();

void run_bench_fixed_vector();
void run_bench_fixed_string();
void run_bench_fixed_wstring();
void run_bench_fixed_list();
void run_bench_fixed_slist();
void run_bench_fixed_hash_map();
void run_bench_fixed_hash_set();
void run_bench_fixed_bitset();
void run_bench_fixed_ring_buffer();
void run_bench_fixed_pool();
