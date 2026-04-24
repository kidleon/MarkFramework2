/*
#pragma once
#include "CoreHeap.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>

// ============================================================
//  Benchmark_CoreHeap.h
//  mark_sys / mark_spool / mark_upool / mark_temp vs malloc
// ============================================================

namespace mark
{
    namespace benchmark_coreheap_detail
    {
        // --------------------------------------------------------
        // Test data structures of various sizes
        // --------------------------------------------------------
        struct SmallData       // 32 bytes
        {
            uint32_t a, b, c, d;
            float    x, y, z, w;
        };

        struct MediumData      // 256 bytes
        {
            uint8_t payload[256];
        };

        struct LargeData       // 2048 bytes
        {
            uint8_t payload[2048];
        };

        struct HugeData        // 16384 bytes
        {
            uint8_t payload[16384];
        };

        // --------------------------------------------------------
        // High-resolution timer helper
        // --------------------------------------------------------
        using Clock = std::chrono::high_resolution_clock;

        inline double elapsed_ms(Clock::time_point start, Clock::time_point end)
        {
            return std::chrono::duration<double, std::milli>(end - start).count();
        }

        // --------------------------------------------------------
        // Benchmark parameters
        // --------------------------------------------------------
        static constexpr int    ITERATIONS        = 100000;
        static constexpr int    THREAD_COUNT      = 4;
        static constexpr int    THREAD_ITERS      = 50000;
        static constexpr size_t TEMP_POOL_SIZE    = 1024 * 1024 * 32; // Mark3D 초기화 시 설정한 temp 버퍼 크기 (32MB)

        // temp 풀 크기 한도 내에서 실제로 실행 가능한 최대 반복 횟수 계산
        // heap_header(8bytes) + alignment padding이 붙으므로 헤더 크기까지 감안
        template<typename T>
        constexpr int calc_temp_iters(int default_iters) noexcept
        {
            constexpr size_t header_size  = 8; // sizeof(heap_header)
            constexpr size_t alloc_size   = header_size + sizeof(T);
            constexpr int    max_iters    = static_cast<int>(TEMP_POOL_SIZE / alloc_size);
            return max_iters < default_iters ? max_iters : default_iters;
        }

        // --------------------------------------------------------
        // Single-threaded benchmark helpers
        // --------------------------------------------------------

        // malloc baseline
        template<typename T>
        inline double bench_malloc(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = malloc(sizeof(T));
                // prevent dead-store elimination
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                free(p);
            }
            return elapsed_ms(start, Clock::now());
        }

        // mark_sys_alloc / mark_sys_free
        template<typename T>
        inline double bench_sys(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = mark_sys_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                mark_sys_free(p, alignof(T));
            }
            return elapsed_ms(start, Clock::now());
        }

        // mark_spool_alloc / mark_spool_free
        template<typename T>
        inline double bench_spool(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = mark_spool_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                mark_spool_free(p, alignof(T));
            }
            return elapsed_ms(start, Clock::now());
        }

        // mark_upool_alloc / mark_upool_free  (single-thread only)
        template<typename T>
        inline double bench_upool(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = mark_upool_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                mark_upool_free(p, alignof(T));
            }
            return elapsed_ms(start, Clock::now());
        }

        // mark_temp_alloc / mark_temp_reset
        // temp pool is linear: allocate batch then reset once
        template<typename T>
        inline double bench_temp(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = mark_temp_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
            }
            mark_temp_reset();
            return elapsed_ms(start, Clock::now());
        }

        // --------------------------------------------------------
        // Print one benchmark row
        // --------------------------------------------------------
        inline void print_row(const char* label, const char* type_name,
                               double malloc_ms, double target_ms, int iters)
        {
            double ratio = (malloc_ms > 0.0) ? (target_ms / malloc_ms) : 0.0;
            printf("  %-14s | %-10s | malloc=%8.3f ms | target=%8.3f ms | ratio=%.3fx\n",
                   label, type_name, malloc_ms, target_ms, ratio);
        }

        // --------------------------------------------------------
        // Single-threaded suite for one data type T
        // temp_alloc은 32MB 한도 내에서 가능한 최대 횟수로 자동 조정
        // --------------------------------------------------------
        template<typename T>
        inline void run_single_suite(const char* type_name, int iters)
        {
            constexpr int temp_iters = calc_temp_iters<T>(ITERATIONS);

            double t_malloc = bench_malloc<T>(iters);
            double t_sys    = bench_sys<T>(iters);
            double t_spool  = bench_spool<T>(iters);
            double t_upool  = bench_upool<T>(iters);
            double t_temp   = bench_temp<T>(temp_iters);

            print_row("sys_alloc",   type_name, t_malloc, t_sys,   iters);
            print_row("spool_alloc", type_name, t_malloc, t_spool, iters);
            print_row("upool_alloc", type_name, t_malloc, t_upool, iters);

            // temp는 iters가 달라질 수 있으므로 기준 malloc도 동일 횟수로 재측정
            double t_malloc_temp = (temp_iters == iters) ? t_malloc : bench_malloc<T>(temp_iters);
            printf("  %-14s | %-10s | malloc=%8.3f ms | target=%8.3f ms | ratio=%.3fx  (iters=%d)\n",
                   "temp_alloc", type_name, t_malloc_temp, t_temp,
                   (t_malloc_temp > 0.0 ? t_temp / t_malloc_temp : 0.0), temp_iters);
            printf("\n");
        }

        // --------------------------------------------------------
        // Multi-threaded worker functions
        // --------------------------------------------------------
        inline void thread_worker_malloc(int iters, std::atomic<int>& barrier)
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) { }

            for (int i = 0; i < iters; ++i)
            {
                void* p = malloc(64);
                volatile char* vp = reinterpret_cast<volatile char*>(p);
                (void)vp;
                free(p);
            }
        }

        inline void thread_worker_sys(int iters, std::atomic<int>& barrier)
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) { /* spin wait for all threads */ }

            for (int i = 0; i < iters; ++i)
            {
                void* p = mark_sys_alloc(64, 8);
                volatile char* vp = reinterpret_cast<volatile char*>(p);
                (void)vp;
                mark_sys_free(p, 8);
            }
        }

        inline void thread_worker_spool(int iters, std::atomic<int>& barrier)
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) { /* spin wait for all threads */ }

            for (int i = 0; i < iters; ++i)
            {
                void* p = mark_spool_alloc(64, 8);
                volatile char* vp = reinterpret_cast<volatile char*>(p);
                (void)vp;
                mark_spool_free(p, 8);
            }
        }

        // --------------------------------------------------------
        // Multi-threaded benchmark runner
        // --------------------------------------------------------
        template<typename WorkerFn>
        inline double bench_mt(WorkerFn worker_fn, int num_threads, int iters_per_thread)
        {
            std::atomic<int> barrier(0);
            std::vector<std::thread> threads;
            threads.reserve(num_threads);

            for (int t = 0; t < num_threads; ++t)
                threads.emplace_back(worker_fn, iters_per_thread, std::ref(barrier));

            auto start = Clock::now();
            barrier.fetch_add(1); // release all threads simultaneously
            for (auto& th : threads) th.join();
            return elapsed_ms(start, Clock::now());
        }

    } // namespace benchmark_coreheap_detail


    // ============================================================
    //  Public entry point
    // ============================================================
    inline void Benchamrk_CoreHeap()
    {
        using namespace benchmark_coreheap_detail;

        printf("============================================================\n");
        printf("  CoreHeap Benchmark  (iterations=%d)\n", ITERATIONS);
        printf("  Format: ratio = target_ms / malloc_ms  (lower is better)\n");
        printf("============================================================\n\n");

        // --------------------------------------------------------
        // [1] Single-threaded: SmallData (32 bytes)
        // --------------------------------------------------------
        printf("---- [Single-Thread] SmallData (32 bytes) ----\n");
        run_single_suite<SmallData>("Small(32)", ITERATIONS);

        // --------------------------------------------------------
        // [2] Single-threaded: MediumData (256 bytes)
        // --------------------------------------------------------
        printf("---- [Single-Thread] MediumData (256 bytes) ----\n");
        run_single_suite<MediumData>("Med(256)", ITERATIONS);

        // --------------------------------------------------------
        // [3] Single-threaded: LargeData (2048 bytes)
        // --------------------------------------------------------
        printf("---- [Single-Thread] LargeData (2048 bytes) ----\n");
        run_single_suite<LargeData>("Large(2K)", ITERATIONS);

        // --------------------------------------------------------
        // [4] Single-threaded: HugeData (16384 bytes)
        // --------------------------------------------------------
        printf("---- [Single-Thread] HugeData (16384 bytes) ----\n");
        run_single_suite<HugeData>("Huge(16K)", ITERATIONS);

        // --------------------------------------------------------
        // [5] Multi-threaded: mark_sys_alloc vs malloc (64 bytes)
        // --------------------------------------------------------
        printf("============================================================\n");
        printf("  Multi-Thread Benchmark  (threads=%d, iters/thread=%d)\n",
               THREAD_COUNT, THREAD_ITERS);
        printf("============================================================\n\n");

        {
            double t_malloc = bench_mt(thread_worker_malloc, THREAD_COUNT, THREAD_ITERS);
            double t_sys    = bench_mt(thread_worker_sys,    THREAD_COUNT, THREAD_ITERS);
            double ratio    = (t_malloc > 0.0) ? (t_sys / t_malloc) : 0.0;

            printf("---- [Multi-Thread] sys_alloc vs malloc (64 bytes) ----\n");
            printf("  malloc    : %.3f ms\n", t_malloc);
            printf("  sys_alloc : %.3f ms  ratio=%.3fx\n\n", t_sys, ratio);
        }

        // --------------------------------------------------------
        // [6] Multi-threaded: mark_spool_alloc vs malloc (64 bytes)
        // --------------------------------------------------------
        {
            double t_malloc = bench_mt(thread_worker_malloc, THREAD_COUNT, THREAD_ITERS);
            double t_spool  = bench_mt(thread_worker_spool,  THREAD_COUNT, THREAD_ITERS);
            double ratio    = (t_malloc > 0.0) ? (t_spool / t_malloc) : 0.0;

            printf("---- [Multi-Thread] spool_alloc vs malloc (64 bytes) ----\n");
            printf("  malloc      : %.3f ms\n", t_malloc);
            printf("  spool_alloc : %.3f ms  ratio=%.3fx\n\n", t_spool, ratio);
        }

        printf("============================================================\n");
        printf("  Benchmark Complete\n");
        printf("============================================================\n");
    }

} // namespace mark
