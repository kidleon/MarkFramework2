#pragma once
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "CoreHeap.h"
#include "Timer.h"


namespace mark
{
	namespace test_heap_detail
	{
		static int g_total = 0;
		static int g_passed = 0;
		static int g_failed = 0;

		inline void check(bool cond, const char* tag)
		{
			++g_total;
			if (cond)
			{
				++g_passed;
			}
			else
			{
				++g_failed;
				printf("  [FAIL] %s\n", tag);
			}
		}

		inline void reset_counters()
		{
			g_total = 0;
			g_passed = 0;
			g_failed = 0;
		}

		inline void print_summary()
		{
			printf("========================================\n");
			printf("  Total : %d   Passed : %d   Failed : %d\n", g_total, g_passed, g_failed);
			printf("========================================\n");
		}

#define HEAP_CHECK(expr, tag) mark::test_heap_detail::check((expr), (tag))


		// ========================================================================
		// sys_alloc / sys_free
		// ========================================================================
		inline void TestHeap_SysAlloc()
		{
			printf("[TestHeap_SysAlloc]\n");

			// 기본 할당
			void* p = CORE_SYS_ALLOC(128);
			HEAP_CHECK(p != nullptr, "sys_alloc: non-null");

			// 메모리 쓰기/읽기 확인
			memset(p, 0xAB, 128);
			uint8_t* bytes = static_cast<uint8_t*>(p);
			bool write_ok = true;
			for (int i = 0; i < 128; ++i)
			{
				if (bytes[i] != 0xAB) { write_ok = false; break; }
			}
			HEAP_CHECK(write_ok, "sys_alloc: write/read integrity");

			CORE_SYS_FREE(p);
			HEAP_CHECK(true, "sys_free: no crash");

			// nullptr free
			void* np = nullptr;
			CORE_SYS_FREE(np);
			HEAP_CHECK(true, "sys_free nullptr: no crash");
		}

		// ========================================================================
		// sys_alloc (aligned)
		// ========================================================================
		inline void TestHeap_SysAllocAligned()
		{
			printf("[TestHeap_SysAllocAligned]\n");

			const size_t alignments[] = { 16, 32, 64, 128, 256 };
			for (size_t align : alignments)
			{
				void* p = CORE_SYS_ALLOC_ALIGNED(256, align);
				HEAP_CHECK(p != nullptr, "sys_alloc_aligned: non-null");
				bool aligned = (reinterpret_cast<uintptr_t>(p) % align) == 0;
				HEAP_CHECK(aligned, "sys_alloc_aligned: alignment check");
				CORE_SYS_FREE(p);
			}
		}

		// ========================================================================
		// sys_calloc
		// ========================================================================
		inline void TestHeap_SysCalloc()
		{
			printf("[TestHeap_SysCalloc]\n");

			void* p = CORE_SYS_CALLOC(256);
			HEAP_CHECK(p != nullptr, "sys_calloc: non-null");

			uint8_t* bytes = static_cast<uint8_t*>(p);
			bool zeroed = true;
			for (int i = 0; i < 256; ++i)
			{
				if (bytes[i] != 0) { zeroed = false; break; }
			}
			HEAP_CHECK(zeroed, "sys_calloc: zero-initialized");

			CORE_SYS_FREE(p);
		}

		// ========================================================================
		// sys_calloc (aligned)
		// ========================================================================
		inline void TestHeap_SysCallocAligned()
		{
			printf("[TestHeap_SysCallocAligned]\n");

			void* p = CORE_SYS_CALLOC_ALIGNED(256, 64);
			HEAP_CHECK(p != nullptr, "sys_calloc_aligned: non-null");
			HEAP_CHECK((reinterpret_cast<uintptr_t>(p) % 64) == 0, "sys_calloc_aligned: alignment check");

			uint8_t* bytes = static_cast<uint8_t*>(p);
			bool zeroed = true;
			for (int i = 0; i < 256; ++i)
			{
				if (bytes[i] != 0) { zeroed = false; break; }
			}
			HEAP_CHECK(zeroed, "sys_calloc_aligned: zero-initialized");

			CORE_SYS_FREE(p);
		}

		// ========================================================================
		// sys_realloc
		// ========================================================================
		inline void TestHeap_SysRealloc()
		{
			printf("[TestHeap_SysRealloc]\n");

			void* p = CORE_SYS_ALLOC(64);
			HEAP_CHECK(p != nullptr, "sys_realloc: initial alloc non-null");

			memset(p, 0x55, 64);

			// 크기 확장
			p = CORE_SYS_REALLOC(p, 256);
			HEAP_CHECK(p != nullptr, "sys_realloc: expand non-null");

			// 기존 데이터 보존 확인
			uint8_t* bytes = static_cast<uint8_t*>(p);
			bool preserved = true;
			for (int i = 0; i < 64; ++i)
			{
				if (bytes[i] != 0x55) { preserved = false; break; }
			}
			HEAP_CHECK(preserved, "sys_realloc: data preserved after expand");

			// 크기 축소
			p = CORE_SYS_REALLOC(p, 32);
			HEAP_CHECK(p != nullptr, "sys_realloc: shrink non-null");

			CORE_SYS_FREE(p);
		}

		// ========================================================================
		// temp_alloc / temp_reset
		// ========================================================================
		inline void TestHeap_TempAlloc()
		{
			printf("[TestHeap_TempAlloc]\n");

			void* p1 = CORE_TEMP_ALLOC(128);
			HEAP_CHECK(p1 != nullptr, "temp_alloc: first alloc non-null");

			memset(p1, 0x77, 128);

			void* p2 = CORE_TEMP_ALLOC(64);
			HEAP_CHECK(p2 != nullptr, "temp_alloc: second alloc non-null");
			HEAP_CHECK(p1 != p2, "temp_alloc: distinct pointers");

			CORE_TEMP_RESET();
			HEAP_CHECK(true, "temp_reset: no crash");

			// 리셋 후 재사용 가능 확인
			void* p3 = CORE_TEMP_ALLOC(128);
			HEAP_CHECK(p3 != nullptr, "temp_alloc: alloc after reset non-null");

			CORE_TEMP_RESET();
		}

		// ========================================================================
		// temp_alloc (aligned)
		// ========================================================================
		inline void TestHeap_TempAllocAligned()
		{
			printf("[TestHeap_TempAllocAligned]\n");

			const size_t alignments[] = { 16, 32, 64 };
			for (size_t align : alignments)
			{
				void* p = CORE_TEMP_ALLOC_ALIGNED(128, align);
				HEAP_CHECK(p != nullptr, "temp_alloc_aligned: non-null");
				HEAP_CHECK((reinterpret_cast<uintptr_t>(p) % align) == 0, "temp_alloc_aligned: alignment check");
			}

			CORE_TEMP_RESET();
		}

		// ========================================================================
		// temp_calloc
		// ========================================================================
		inline void TestHeap_TempCalloc()
		{
			printf("[TestHeap_TempCalloc]\n");

			void* p = CORE_TEMP_CALLOC(256);
			HEAP_CHECK(p != nullptr, "temp_calloc: non-null");

			uint8_t* bytes = static_cast<uint8_t*>(p);
			bool zeroed = true;
			for (int i = 0; i < 256; ++i)
			{
				if (bytes[i] != 0) { zeroed = false; break; }
			}
			HEAP_CHECK(zeroed, "temp_calloc: zero-initialized");

			CORE_TEMP_RESET();
		}

		// ========================================================================
		// temppool_create / destroy
		// ========================================================================
		inline void TestHeap_TempPool()
		{
			printf("[TestHeap_TempPool]\n");

			HANDLE pool = temppool_create(4 * 1024);
			HEAP_CHECK(pool != nullptr, "temppool_create: non-null");

			void* p1 = temppool_alloc(pool, 128);
			HEAP_CHECK(p1 != nullptr, "temppool_alloc: non-null");

			void* p2 = temppool_alloc(pool, 64);
			HEAP_CHECK(p2 != nullptr, "temppool_alloc: second non-null");
			HEAP_CHECK(p1 != p2, "temppool_alloc: distinct pointers");

			// calloc zero check
			void* p3 = temppool_calloc(pool, 64);
			HEAP_CHECK(p3 != nullptr, "temppool_calloc: non-null");
			uint8_t* bytes = static_cast<uint8_t*>(p3);
			bool zeroed = true;
			for (int i = 0; i < 64; ++i)
			{
				if (bytes[i] != 0) { zeroed = false; break; }
			}
			HEAP_CHECK(zeroed, "temppool_calloc: zero-initialized");

			// aligned alloc
			void* p4 = temppool_alloc(pool, 128, 64);
			HEAP_CHECK(p4 != nullptr, "temppool_alloc_aligned: non-null");
			HEAP_CHECK((reinterpret_cast<uintptr_t>(p4) % 64) == 0, "temppool_alloc_aligned: alignment check");

			// reset & reuse
			temppool_reset(pool);
			HEAP_CHECK(true, "temppool_reset: no crash");

			void* p5 = temppool_alloc(pool, 128);
			HEAP_CHECK(p5 != nullptr, "temppool_alloc after reset: non-null");

			temppool_destroy(pool);
			HEAP_CHECK(true, "temppool_destroy: no crash");

			// nullptr destroy
			temppool_destroy(nullptr);
			HEAP_CHECK(true, "temppool_destroy nullptr: no crash");
		}

		// ========================================================================
		// CORE_NEW / CORE_DELETE
		// ========================================================================
		struct HeapTestObj
		{
			int value;
			float fval;
			explicit HeapTestObj(int v = 0, float f = 0.f) : value(v), fval(f) {}
			~HeapTestObj() {}
		};

		inline void TestHeap_CoreNew()
		{
			printf("[TestHeap_CoreNew]\n");

			HeapTestObj* obj = CORE_NEW(HeapTestObj)(42, 3.14f);
			HEAP_CHECK(obj != nullptr, "CORE_NEW: non-null");
			HEAP_CHECK(obj->value == 42, "CORE_NEW: value correct");
			HEAP_CHECK(obj->fval == 3.14f, "CORE_NEW: fval correct");

			CORE_DELETE(HeapTestObj, obj);
			HEAP_CHECK(obj == nullptr, "CORE_DELETE: ptr set to nullptr");
		}

		// ========================================================================
		// BENCHMARK helpers
		// ========================================================================
		static constexpr int BENCH_ITERATIONS = 100000;

		inline double bench_sys_alloc_free(int iterations)
		{
			HANDLE timer = timer::highrestimer_create();
			timer::highrestimer_start(timer);

			for (int i = 0; i < iterations; ++i)
			{
				void* p = CORE_SYS_ALLOC(64);
				CORE_SYS_FREE(p);
			}

			double elapsed = timer::highrestimer_gettime(timer);
			timer::highrestimer_destroy(timer);
			return elapsed;
		}

		inline double bench_sys_alloc_free_varsize(int iterations)
		{
			HANDLE timer = timer::highrestimer_create();
			timer::highrestimer_start(timer);

			for (int i = 0; i < iterations; ++i)
			{
				size_t sz = 16 + (i % 8) * 64;
				void* p = CORE_SYS_ALLOC(sz);
				CORE_SYS_FREE(p);
			}

			double elapsed = timer::highrestimer_gettime(timer);
			timer::highrestimer_destroy(timer);
			return elapsed;
		}

		inline double bench_sys_calloc_free(int iterations)
		{
			HANDLE timer = timer::highrestimer_create();
			timer::highrestimer_start(timer);

			for (int i = 0; i < iterations; ++i)
			{
				void* p = CORE_SYS_CALLOC(64);
				CORE_SYS_FREE(p);
			}

			double elapsed = timer::highrestimer_gettime(timer);
			timer::highrestimer_destroy(timer);
			return elapsed;
		}

		inline double bench_temp_alloc_reset(int iterations)
		{
			HANDLE timer = timer::highrestimer_create();
			timer::highrestimer_start(timer);

			for (int i = 0; i < iterations; ++i)
			{
				CORE_TEMP_ALLOC(64);
				if ((i & 0xFF) == 0xFF)
					CORE_TEMP_RESET();
			}
			CORE_TEMP_RESET();

			double elapsed = timer::highrestimer_gettime(timer);
			timer::highrestimer_destroy(timer);
			return elapsed;
		}

		inline double bench_temppool_alloc_reset(int iterations)
		{
			HANDLE pool = temppool_create(static_cast<size_t>(iterations) * 64 + 4096);

			HANDLE timer = timer::highrestimer_create();
			timer::highrestimer_start(timer);

			for (int i = 0; i < iterations; ++i)
			{
				temppool_alloc(pool, 64);
				if ((i & 0xFF) == 0xFF)
					temppool_reset(pool);
			}
			temppool_reset(pool);

			double elapsed = timer::highrestimer_gettime(timer);
			timer::highrestimer_destroy(timer);
			temppool_destroy(pool);
			return elapsed;
		}

		inline double bench_malloc_free(int iterations)
		{
			HANDLE timer = timer::highrestimer_create();
			timer::highrestimer_start(timer);

			for (int i = 0; i < iterations; ++i)
			{
				void* p = malloc(64);
				free(p);
			}

			double elapsed = timer::highrestimer_gettime(timer);
			timer::highrestimer_destroy(timer);
			return elapsed;
		}
	} // namespace test_heap_detail

	// ========================================================================
   // Test entry point
   // ========================================================================
	inline void Test_CoreHeap()
	{
		printf("\n======== Test_CoreHeap ========\n");
		test_heap_detail::reset_counters();

		test_heap_detail::TestHeap_SysAlloc();
		test_heap_detail::TestHeap_SysAllocAligned();
		test_heap_detail::TestHeap_SysCalloc();
		test_heap_detail::TestHeap_SysCallocAligned();
		test_heap_detail::TestHeap_SysRealloc();
		test_heap_detail::TestHeap_TempAlloc();
		test_heap_detail::TestHeap_TempAllocAligned();
		test_heap_detail::TestHeap_TempCalloc();
		test_heap_detail::TestHeap_TempPool();
		test_heap_detail::TestHeap_CoreNew();

		test_heap_detail::print_summary();
	}

	// ========================================================================
	// Benchmark entry point
	// ========================================================================
	inline void Bench_CoreHeap()
	{
		printf("\n======== Bench_CoreHeap (%d iterations) ========\n", test_heap_detail::BENCH_ITERATIONS);

		double t;

		t = test_heap_detail::bench_malloc_free(test_heap_detail::BENCH_ITERATIONS);
		printf("  malloc/free (64B)             : %.4f ms\n", t * 1000.0);

		t = test_heap_detail::bench_sys_alloc_free(test_heap_detail::BENCH_ITERATIONS);
		printf("  sys_alloc/sys_free (64B)      : %.4f ms\n", t * 1000.0);

		t = test_heap_detail::bench_sys_alloc_free_varsize(test_heap_detail::BENCH_ITERATIONS);
		printf("  sys_alloc/sys_free (varsize)  : %.4f ms\n", t * 1000.0);

		t = test_heap_detail::bench_sys_calloc_free(test_heap_detail::BENCH_ITERATIONS);
		printf("  sys_calloc/sys_free (64B)     : %.4f ms\n", t * 1000.0);

		t = test_heap_detail::bench_temp_alloc_reset(test_heap_detail::BENCH_ITERATIONS);
		printf("  temp_alloc+reset (64B)        : %.4f ms\n", t * 1000.0);

		t = test_heap_detail::bench_temppool_alloc_reset(test_heap_detail::BENCH_ITERATIONS);
		printf("  temppool_alloc+reset (64B)    : %.4f ms\n", t * 1000.0);

		printf("================================================\n");
	}

} // namespace mark
   
