// ============================================================================
// Test_random.cpp
//
// mark::random 의 모든 공개 API 테스트.
// random.h 의 주석된 의미(범위 포함/제외, 결정론성, 시드 동작 등)를 검증.
// ============================================================================

#include "pch.h"
#include "TestBenchCommon.h"

#include <random.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <set>
#include <vector>

namespace
{
	// 결정론성 검증용: 같은 시드 → 같은 수열인지 비교
	template <typename Gen>
	std::vector<uint64_t> dump_uint64(Gen& g, std::size_t n)
	{
		std::vector<uint64_t> v;
		v.reserve(n);
		for (std::size_t i = 0; i < n; ++i) v.push_back(g.next_uint64());
		return v;
	}

	// [min,max] (or [min,max))에 들어가는지 + min/max 경계가 실제로 등장하는지
	template <typename T, typename Gen, typename Pick>
	void check_inclusive_range(Gen& g, T lo, T hi, std::size_t iters, const char* label, Pick&& pick)
	{
		bool in_range = true;
		bool saw_lo = false, saw_hi = false;
		for (std::size_t i = 0; i < iters; ++i)
		{
			const T v = pick(g, lo, hi);
			if (v < lo || v > hi) { in_range = false; break; }
			if (v == lo) saw_lo = true;
			if (v == hi) saw_hi = true;
		}
		(void)label;
		MTL_CHECK(in_range);
		MTL_CHECK(saw_lo);
		MTL_CHECK(saw_hi);
	}
}

void run_test_random()
{
	using mark::random;

	mtl_tb::section("random - default ctor (DEFAULT_SEED) determinism");
	{
		random a;
		random b;
		// 같은 기본 시드 → 같은 수열
		const auto va = dump_uint64(a, 32);
		const auto vb = dump_uint64(b, 32);
		MTL_CHECK(va == vb);
		// 0이 연속해서 나오는 일은 거의 없어야 함
		bool all_zero = true;
		for (auto x : va) if (x != 0) { all_zero = false; break; }
		MTL_CHECK(!all_zero);
	}

	mtl_tb::section("random - explicit seed equality / different seed -> different sequence");
	{
		random a(12345ull);
		random b(12345ull);
		MTL_CHECK(dump_uint64(a, 16) == dump_uint64(b, 16));

		random c(12345ull);
		random d(67890ull);
		MTL_CHECK(dump_uint64(c, 16) != dump_uint64(d, 16));
	}

	mtl_tb::section("random - zero seed handling (falls back to DEFAULT_SEED)");
	{
		// 생성자에 0 → DEFAULT_SEED 적용 → 기본 생성자와 같은 수열
		random a(0ull);
		random b;
		MTL_CHECK(dump_uint64(a, 16) == dump_uint64(b, 16));

		// seed(s) 는 mix_seed(s)를 거치므로 seed(0) ≠ DEFAULT_SEED 수열일 수
		// 있으나, 어떤 경우에도 내부 state 가 0 으로 남아 다음 호출이 0 만
		// 뱉는 일은 없어야 한다(주석된 안전 보정).
		random c;
		c.seed(0ull);
		bool any_nonzero = false;
		for (int i = 0; i < 32; ++i) if (c.next_uint64() != 0) { any_nonzero = true; break; }
		MTL_CHECK(any_nonzero);
	}

	mtl_tb::section("random - seed() determinism");
	{
		random a;
		random b;
		a.seed(42ull);
		b.seed(42ull);
		MTL_CHECK(dump_uint64(a, 32) == dump_uint64(b, 32));

		// 다른 시드는 다른 수열
		random c;
		random d;
		c.seed(42ull);
		d.seed(43ull);
		MTL_CHECK(dump_uint64(c, 32) != dump_uint64(d, 32));
	}

	mtl_tb::section("random - next_int16 / next_uint16 (no-arg) range and diversity");
	{
		random g(0xC0FFEEull);

		// 16비트 정수는 정의상 모든 16비트 비트패턴 안에 들어와야 한다.
		// signed 측은 [INT16_MIN, INT16_MAX] 범위만 만족하면 충분.
		bool all_in_range = true;
		std::set<int16_t> seen_s;
		std::set<uint16_t> seen_u;
		for (int i = 0; i < 4096; ++i)
		{
			int16_t s = g.next_int16();
			uint16_t u = g.next_uint16();
			if (s < std::numeric_limits<int16_t>::min() || s > std::numeric_limits<int16_t>::max())
				all_in_range = false;
			seen_s.insert(s);
			seen_u.insert(u);
		}
		MTL_CHECK(all_in_range);
		// 다양한 값이 나오는지 (분포 다양성)
		MTL_CHECK(seen_s.size() > 1000);
		MTL_CHECK(seen_u.size() > 1000);
	}

	mtl_tb::section("random - next_int16(min,max): inclusive on both ends");
	{
		random g(101ull);
		check_inclusive_range<int16_t>(g, -5, 5, 5000, "int16",
			[](random& r, int16_t lo, int16_t hi) { return r.next_int16(lo, hi); });

		// min > max → min 반환
		MTL_CHECK(g.next_int16(10, 5) == 10);
		// min == max → min 반환
		MTL_CHECK(g.next_int16(7, 7) == 7);
	}

	mtl_tb::section("random - next_uint16(min,max): inclusive on both ends");
	{
		random g(202ull);
		check_inclusive_range<uint16_t>(g, 1000, 1010, 5000, "uint16",
			[](random& r, uint16_t lo, uint16_t hi) { return r.next_uint16(lo, hi); });

		MTL_CHECK(g.next_uint16(50, 40) == 50);
		MTL_CHECK(g.next_uint16(123, 123) == 123);
	}

	mtl_tb::section("random - next_int32 / next_uint32 (no-arg) diversity");
	{
		random g(303ull);
		std::set<int32_t> seen_s;
		std::set<uint32_t> seen_u;
		for (int i = 0; i < 4096; ++i)
		{
			seen_s.insert(g.next_int32());
			seen_u.insert(g.next_uint32());
		}
		MTL_CHECK(seen_s.size() > 4000);
		MTL_CHECK(seen_u.size() > 4000);
	}

	mtl_tb::section("random - next_int32(min,max): inclusive on both ends");
	{
		random g(404ull);
		check_inclusive_range<int32_t>(g, -100, 100, 20000, "int32",
			[](random& r, int32_t lo, int32_t hi) { return r.next_int32(lo, hi); });

		// 매우 넓은 범위 — 단순 범위 검증
		bool in_full = true;
		for (int i = 0; i < 4096; ++i)
		{
			int32_t v = g.next_int32(std::numeric_limits<int32_t>::min(),
									 std::numeric_limits<int32_t>::max());
			(void)v;
			// 모든 int32 값이 그 안에 들어 있으므로 항상 통과 — 호출이 안전한지 확인 목적.
		}
		MTL_CHECK(in_full);

		MTL_CHECK(g.next_int32(0, -1) == 0);
		MTL_CHECK(g.next_int32(42, 42) == 42);
	}

	mtl_tb::section("random - next_uint32(min,max): inclusive on both ends");
	{
		random g(505ull);
		check_inclusive_range<uint32_t>(g, 100u, 200u, 20000, "uint32",
			[](random& r, uint32_t lo, uint32_t hi) { return r.next_uint32(lo, hi); });

		MTL_CHECK(g.next_uint32(99u, 50u) == 99u);
		MTL_CHECK(g.next_uint32(7u, 7u) == 7u);
	}

	mtl_tb::section("random - next_int64 / next_uint64 (no-arg) diversity");
	{
		random g(606ull);
		std::set<int64_t> seen_s;
		std::set<uint64_t> seen_u;
		for (int i = 0; i < 4096; ++i)
		{
			seen_s.insert(g.next_int64());
			seen_u.insert(g.next_uint64());
		}
		MTL_CHECK(seen_s.size() > 4000);
		MTL_CHECK(seen_u.size() > 4000);
	}

	mtl_tb::section("random - next_int64(min,max): inclusive on both ends");
	{
		random g(707ull);
		check_inclusive_range<int64_t>(g, -100, 100, 20000, "int64",
			[](random& r, int64_t lo, int64_t hi) { return r.next_int64(lo, hi); });

		MTL_CHECK(g.next_int64(100, 50) == 100);
		MTL_CHECK(g.next_int64(0, 0) == 0);

		// 큰 범위 — 호출이 안전하고 범위에 들어오는지
		bool ok = true;
		for (int i = 0; i < 4096; ++i)
		{
			int64_t v = g.next_int64(-1'000'000'000'000LL, 1'000'000'000'000LL);
			if (v < -1'000'000'000'000LL || v > 1'000'000'000'000LL) { ok = false; break; }
		}
		MTL_CHECK(ok);
	}

	mtl_tb::section("random - next_uint64(min,max): inclusive on both ends");
	{
		random g(808ull);
		check_inclusive_range<uint64_t>(g, 1000ull, 1100ull, 20000, "uint64",
			[](random& r, uint64_t lo, uint64_t hi) { return r.next_uint64(lo, hi); });

		MTL_CHECK(g.next_uint64(50ull, 40ull) == 50ull);
		MTL_CHECK(g.next_uint64(9ull, 9ull) == 9ull);

		// span == UINT64_MAX 경계 — [0, UINT64_MAX] 호출이 안전하게 모든
		// 64비트 값을 직접 반환해야 한다(주석된 분기).
		bool ok = true;
		for (int i = 0; i < 4096; ++i)
		{
			uint64_t v = g.next_uint64(0ull, std::numeric_limits<uint64_t>::max());
			(void)v;
		}
		MTL_CHECK(ok);
	}

	mtl_tb::section("random - next_float(): guaranteed [0.0, 1.0)");
	{
		random g(909ull);
		bool in_range = true;
		bool saw_low = false;     // 0.5 미만이 나오는지
		bool saw_high = false;    // 0.5 이상이 나오는지
		bool ever_one = false;    // 1.0 이 절대 나오지 않아야 함
		float min_v = 2.0f, max_v = -1.0f;
		for (int i = 0; i < 200000; ++i)
		{
			float v = g.next_float();
			if (!(v >= 0.0f && v < 1.0f)) { in_range = false; }
			if (v == 1.0f) ever_one = true;
			if (v < 0.5f) saw_low = true;
			if (v >= 0.5f) saw_high = true;
			if (v < min_v) min_v = v;
			if (v > max_v) max_v = v;
		}
		MTL_CHECK(in_range);
		MTL_CHECK(!ever_one);
		MTL_CHECK(saw_low);
		MTL_CHECK(saw_high);
		// 충분히 작은/큰 값까지 도달했는지 (단순 분포 sanity)
		MTL_CHECK(min_v < 0.05f);
		MTL_CHECK(max_v > 0.95f);
	}

	mtl_tb::section("random - next_float(min,max): min inclusive, max exclusive");
	{
		random g(1010ull);
		const float lo = -2.0f;
		const float hi = 5.0f;
		bool in_range = true;
		bool ever_eq_hi = false;
		float min_v = std::numeric_limits<float>::infinity();
		float max_v = -std::numeric_limits<float>::infinity();
		for (int i = 0; i < 200000; ++i)
		{
			float v = g.next_float(lo, hi);
			if (!(v >= lo && v < hi)) { in_range = false; }
			if (v == hi) ever_eq_hi = true;
			if (v < min_v) min_v = v;
			if (v > max_v) max_v = v;
		}
		MTL_CHECK(in_range);
		MTL_CHECK(!ever_eq_hi);
		// 양쪽 끝 근처까지 도달하는지
		MTL_CHECK(min_v < lo + 0.1f);
		MTL_CHECK(max_v > hi - 0.1f);

		// min >= max → min 반환
		MTL_CHECK(g.next_float(3.0f, 1.0f) == 3.0f);
		MTL_CHECK(g.next_float(2.5f, 2.5f) == 2.5f);
	}

	mtl_tb::section("random - determinism: two instances match on same call sequence");
	{
		random a(0xABCDEFull);
		random b(0xABCDEFull);
		for (int i = 0; i < 256; ++i)
		{
			MTL_CHECK(a.next_int16() == b.next_int16());
			MTL_CHECK(a.next_uint16() == b.next_uint16());
			MTL_CHECK(a.next_int32() == b.next_int32());
			MTL_CHECK(a.next_uint32() == b.next_uint32());
			MTL_CHECK(a.next_int64() == b.next_int64());
			MTL_CHECK(a.next_uint64() == b.next_uint64());
			MTL_CHECK(a.next_int16(-10, 10)  == b.next_int16(-10, 10));
			MTL_CHECK(a.next_uint16(0, 100)  == b.next_uint16(0, 100));
			MTL_CHECK(a.next_int32(-1000, 1000) == b.next_int32(-1000, 1000));
			MTL_CHECK(a.next_uint32(0u, 10000u) == b.next_uint32(0u, 10000u));
			MTL_CHECK(a.next_int64(-100000, 100000) == b.next_int64(-100000, 100000));
			MTL_CHECK(a.next_uint64(0ull, 1000000ull) == b.next_uint64(0ull, 1000000ull));
			MTL_CHECK(a.next_float() == b.next_float());
			MTL_CHECK(a.next_float(-1.0f, 1.0f) == b.next_float(-1.0f, 1.0f));
		}
	}

	mtl_tb::section("random - distribution mean sanity (not normality, simple mean)");
	{
		// next_float 의 평균이 대략 0.5 부근에 위치해야 함 (분포 sanity).
		random g(2024ull);
		double sum = 0.0;
		const int N = 200000;
		for (int i = 0; i < N; ++i) sum += g.next_float();
		const double mean = sum / N;
		MTL_CHECK(std::fabs(mean - 0.5) < 0.01);
	}

	mtl_tb::summary();
}
