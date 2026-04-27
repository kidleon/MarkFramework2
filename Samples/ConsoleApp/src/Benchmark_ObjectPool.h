#pragma once
#include "ObjectPool.h"
#include "CoreGeneric.h"
#include "CoreHeap.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <new>

namespace mark
{
    // =========================================================================
    //  Allocator aliases (Test_ObjectPool.h와 별도로 재선언)
    // =========================================================================
    template<typename _T>
    using bm_sys_object_pool = mark::object_pool<_T, ALLOC::SYS<_T>>;

    template<typename _T>
    using bm_spool_object_pool = mark::object_pool<_T, ALLOC::SPOOL<_T>>;

    template<typename _T>
    using bm_upool_object_pool = mark::object_pool<_T, ALLOC::UPOOL<_T>>;

    template<typename _T>
    using bm_temp_object_pool = mark::object_pool<_T, ALLOC::TEMP<_T>>;

    // =========================================================================
    //  Benchmark infrastructure
    // =========================================================================
    namespace benchmark_objectpool_detail
    {
        using Clock = std::chrono::high_resolution_clock;

        inline double elapsed_ms(Clock::time_point s, Clock::time_point e)
        {
            return std::chrono::duration<double, std::milli>(e - s).count();
        }

        // -----------------------------------------------------------------------
        //  Benchmark parameters
        // -----------------------------------------------------------------------
        static constexpr int ITER          = 100'000;
        static constexpr int THREAD_COUNT  = 4;
        static constexpr int THREAD_ITERS  = 50'000;
        static constexpr int PREFILL       = 512;    // 모든 풀 공통 prefill
        static constexpr int MAX_POOL_SIZE = 1024;   // 풀 최대 슬롯 수

        // -----------------------------------------------------------------------
        //  Test object types
        // -----------------------------------------------------------------------
        struct SmallObj  { int32_t a, b, c, d; };
        struct MediumObj { uint8_t payload[256]; };
        struct LargeObj  { uint8_t payload[2048]; };

        // -----------------------------------------------------------------------
        //  출력 헬퍼
        // -----------------------------------------------------------------------
        inline void print_section(const char* title)
        {
            printf("------------------------------------------------------------\n");
            printf("  %s\n", title);
            printf("------------------------------------------------------------\n");
        }

        inline void print_row(const char* label, const char* type_name,
                               double base_ms, double target_ms, int iters)
        {
            double ratio = (base_ms > 0.0) ? (target_ms / base_ms) : 0.0;
            printf("  %-16s | %-10s | base=%8.3f ms | target=%8.3f ms | ratio=%.3fx  (iters=%d)\n",
                   label, type_name, base_ms, target_ms, ratio, iters);
        }

        // -----------------------------------------------------------------------
        //  Single-thread: new/delete 기준선
        // -----------------------------------------------------------------------
        template<typename T>
        inline double bench_newdelete(int iters)
        {
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                T* p = new T{};
                volatile uint8_t* vp = reinterpret_cast<volatile uint8_t*>(p);
                (void)vp;
                delete p;
            }
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        //  Single-thread: object_pool acquire/release
        // -----------------------------------------------------------------------
        template<typename Pool>
        inline double bench_pool_single(Pool& pool, int iters)
        {
            using T = typename Pool::value_type;
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                T* p = pool.acquire();
                volatile uint8_t* vp = reinterpret_cast<volatile uint8_t*>(p);
                (void)vp;
                pool.release(p);
            }
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        //  Single-thread: make_unique acquire/release
        // -----------------------------------------------------------------------
        template<typename Pool>
        inline double bench_pool_unique(Pool& pool, int iters)
        {
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                auto h = pool.make_unique();
                volatile auto* vp = h.get();
                (void)vp;
                // 스코프 종료 시 자동 release
            }
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        //  Multi-thread: new/delete 기준선
        // -----------------------------------------------------------------------
        template<typename T>
        inline void mt_worker_newdelete(int iters, std::atomic<int>& barrier)
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) {}

            for (int i = 0; i < iters; ++i)
            {
                T* p = new T{};
                volatile uint8_t* vp = reinterpret_cast<volatile uint8_t*>(p);
                (void)vp;
                delete p;
            }
        }

        // -----------------------------------------------------------------------
        //  Multi-thread: object_pool acquire/release
        // -----------------------------------------------------------------------
        template<typename Pool>
        inline void mt_worker_pool(int iters, std::atomic<int>& barrier, Pool* pool)
        {
            using T = typename Pool::value_type;
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) {}

            for (int i = 0; i < iters; ++i)
            {
                T* p = pool->acquire();
                volatile uint8_t* vp = reinterpret_cast<volatile uint8_t*>(p);
                (void)vp;
                pool->release(p);
            }
        }

        // -----------------------------------------------------------------------
        //  Multi-thread 실행기
        // -----------------------------------------------------------------------
        template<typename WorkerFn, typename... Args>
        inline double bench_mt(WorkerFn fn, int num_threads, int iters_per_thread, Args&&... args)
        {
            std::atomic<int> barrier{ 0 };
            std::vector<std::thread> threads;
            threads.reserve(num_threads);
            for (int t = 0; t < num_threads; ++t)
                threads.emplace_back(fn, iters_per_thread, std::ref(barrier), std::forward<Args>(args)...);

            auto start = Clock::now();
            barrier.fetch_add(1); // 모든 스레드 동시 출발
            for (auto& th : threads) th.join();
            return elapsed_ms(start, Clock::now());
        }

        // -----------------------------------------------------------------------
        //  Single-thread suite — 타입 T 전체 비교
        // -----------------------------------------------------------------------
        template<typename T>
        inline void run_single_suite(const char* type_name)
        {
            double t_base = bench_newdelete<T>(ITER);

            // SYS
            {
                bm_sys_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_pool_single(pool, ITER);
                print_row("sys   acquire", type_name, t_base, t, ITER);
            }
            // SPOOL
            {
                bm_spool_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_pool_single(pool, ITER);
                print_row("spool acquire", type_name, t_base, t, ITER);
            }
            // UPOOL (단일 스레드 전용)
            {
                bm_upool_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_pool_single(pool, ITER);
                print_row("upool acquire", type_name, t_base, t, ITER);
            }
            // TEMP (단일 스레드 전용, 마지막에 reset)
            {
                bm_temp_object_pool<T> pool(MAX_POOL_SIZE, 0);
                double t = bench_pool_single(pool, ITER);
                print_row("temp  acquire", type_name, t_base, t, ITER);
            }
            mark::temp_reset();

            printf("\n");
        }

        // -----------------------------------------------------------------------
        //  Single-thread suite — make_unique
        // -----------------------------------------------------------------------
        template<typename T>
        inline void run_unique_suite(const char* type_name)
        {
            double t_base = bench_newdelete<T>(ITER);

            {
                bm_sys_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_pool_unique(pool, ITER);
                print_row("sys   unique", type_name, t_base, t, ITER);
            }
            {
                bm_spool_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_pool_unique(pool, ITER);
                print_row("spool unique", type_name, t_base, t, ITER);
            }
            {
                bm_upool_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_pool_unique(pool, ITER);
                print_row("upool unique", type_name, t_base, t, ITER);
            }

            printf("\n");
        }

        // -----------------------------------------------------------------------
        //  Multi-thread suite — sys / spool vs new/delete
        // -----------------------------------------------------------------------
        template<typename T>
        inline void run_mt_suite(const char* type_name)
        {
            // new/delete 기준
            double t_base = bench_mt(
                mt_worker_newdelete<T>, THREAD_COUNT, THREAD_ITERS);

            // sys (lock-free, thread-safe)
            {
                bm_sys_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_mt(
                    mt_worker_pool<bm_sys_object_pool<T>>,
                    THREAD_COUNT, THREAD_ITERS, &pool);
                print_row("sys   MT", type_name, t_base, t, THREAD_COUNT * THREAD_ITERS);
            }
            // spool (lock-free, thread-safe)
            {
                bm_spool_object_pool<T> pool(MAX_POOL_SIZE, PREFILL);
                double t = bench_mt(
                    mt_worker_pool<bm_spool_object_pool<T>>,
                    THREAD_COUNT, THREAD_ITERS, &pool);
                print_row("spool MT", type_name, t_base, t, THREAD_COUNT * THREAD_ITERS);
            }

            printf("\n");
        }

    } // namespace benchmark_objectpool_detail


    // =========================================================================
    //  Entry point
    // =========================================================================
    inline void Benchmark_ObjectPool()
    {
        using namespace benchmark_objectpool_detail;

		const bool initialized = initialize_core_memory(
			256,
			256,
			2048,
			2048,
			1024 * 1024 * 64);

		if (!initialized)
		{
			printf("[PrivateMemory2 Benchmark] initialize_core_memory2 failed.\n");
			return;
		}

        printf("============================================================\n");
        printf("  Benchmark_ObjectPool  (base = new/delete)\n");
        printf("  Format: ratio = target_ms / base_ms  (lower is better)\n");
        printf("============================================================\n\n");

        // ----------------------------------------------------------------
        //  [1] Single-thread acquire/release — SmallObj (16 bytes)
        // ----------------------------------------------------------------
        print_section("[Single-Thread] acquire/release — SmallObj (16 bytes)");
        run_single_suite<SmallObj>("Small(16)");

        // ----------------------------------------------------------------
        //  [2] Single-thread acquire/release — MediumObj (256 bytes)
        // ----------------------------------------------------------------
        print_section("[Single-Thread] acquire/release — MediumObj (256 bytes)");
        run_single_suite<MediumObj>("Med(256)");

        // ----------------------------------------------------------------
        //  [3] Single-thread acquire/release — LargeObj (2048 bytes)
        // ----------------------------------------------------------------
        print_section("[Single-Thread] acquire/release — LargeObj (2048 bytes)");
        run_single_suite<LargeObj>("Large(2K)");

        // ----------------------------------------------------------------
        //  [4] Single-thread make_unique — SmallObj
        // ----------------------------------------------------------------
        print_section("[Single-Thread] make_unique — SmallObj (16 bytes)");
        run_unique_suite<SmallObj>("Small(16)");

        // ----------------------------------------------------------------
        //  [5] Single-thread make_unique — MediumObj
        // ----------------------------------------------------------------
        print_section("[Single-Thread] make_unique — MediumObj (256 bytes)");
        run_unique_suite<MediumObj>("Med(256)");

        // ----------------------------------------------------------------
        //  [6] Multi-thread acquire/release — SmallObj
        //      (sys / spool만 thread-safe, upool/temp 제외)
        // ----------------------------------------------------------------
        printf("============================================================\n");
        printf("  Multi-Thread Benchmark  (threads=%d, iters/thread=%d)\n",
               THREAD_COUNT, THREAD_ITERS);
        printf("  base = new/delete  (4 threads)\n");
        printf("============================================================\n\n");

        print_section("[Multi-Thread] acquire/release — SmallObj (16 bytes)");
        run_mt_suite<SmallObj>("Small(16)");

        print_section("[Multi-Thread] acquire/release — MediumObj (256 bytes)");
        run_mt_suite<MediumObj>("Med(256)");

        print_section("[Multi-Thread] acquire/release — LargeObj (2048 bytes)");
        run_mt_suite<LargeObj>("Large(2K)");

        printf("============================================================\n");
        printf("  Benchmark_ObjectPool Complete\n");
        printf("============================================================\n\n");

		shutdown_core_memory();
    }

} // namespace mark
