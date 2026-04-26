#pragma once

#include "coreheap.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>

namespace mark
{
    namespace benchmark_privatememory_detail
    {
        struct SmallData
        {
            uint32_t a, b, c, d;
            float x, y, z, w;
        };

        struct MediumData
        {
            uint8_t payload[256];
        };

        struct LargeData
        {
            uint8_t payload[2048];
        };

        struct HugeData
        {
            uint8_t payload[16384];
        };

        using Clock = std::chrono::high_resolution_clock;

        inline double elapsed_ms(Clock::time_point start, Clock::time_point end)
        {
            return std::chrono::duration<double, std::milli>(end - start).count();
        }

        static constexpr int ITERATIONS = 100000;
        static constexpr int THREAD_COUNT = 4;
        static constexpr int THREAD_ITERS = 50000;
        static constexpr size_t TEMP_POOL_SIZE = 1024 * 1024 * 32;

        template<typename T>
        constexpr int calc_temp_iters(int default_iters) noexcept
        {
            constexpr int max_iters = static_cast<int>((TEMP_POOL_SIZE / 2) / sizeof(T)) - 1;
            return max_iters < default_iters ? max_iters : default_iters;
        }

        template<typename T>
        inline double bench_malloc(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = malloc(sizeof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                free(p);
            }
            return elapsed_ms(start, Clock::now());
        }

        template<typename T>
        inline double bench_sys(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = sys_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                sys_free(p, sizeof(T), alignof(T));
            }
            return elapsed_ms(start, Clock::now());
        }

        template<typename T>
        inline double bench_spool(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = spool_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                spool_free(p, sizeof(T), alignof(T));
            }
            return elapsed_ms(start, Clock::now());
        }

        template<typename T>
        inline double bench_upool(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = upool_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
                upool_free(p, sizeof(T), alignof(T));
            }
            return elapsed_ms(start, Clock::now());
        }

        template<typename T>
        inline double bench_temp(int iters)
        {
            auto start = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                void* p = temp_alloc(sizeof(T), alignof(T));
                volatile T* vp = reinterpret_cast<volatile T*>(p);
                (void)vp;
            }
            temp_reset();
            return elapsed_ms(start, Clock::now());
        }

        inline void print_row(const char* label, const char* type_name, double malloc_ms, double target_ms)
        {
            double ratio = (malloc_ms > 0.0) ? (target_ms / malloc_ms) : 0.0;
            printf("  %-14s | %-10s | malloc=%8.3f ms | target=%8.3f ms | ratio=%.3fx\n",
                label, type_name, malloc_ms, target_ms, ratio);
        }

        template<typename T>
        inline void run_single_suite(const char* type_name, int iters)
        {
            constexpr int temp_iters = calc_temp_iters<T>(ITERATIONS);

            double t_malloc = bench_malloc<T>(iters);
            double t_sys = bench_sys<T>(iters);
            double t_spool = bench_spool<T>(iters);
            double t_upool = bench_upool<T>(iters);
            double t_temp = bench_temp<T>(temp_iters);

            print_row("sys_alloc", type_name, t_malloc, t_sys);
            print_row("spool_alloc", type_name, t_malloc, t_spool);
            print_row("upool_alloc", type_name, t_malloc, t_upool);

            double t_malloc_temp = (temp_iters == iters) ? t_malloc : bench_malloc<T>(temp_iters);
            printf("  %-14s | %-10s | malloc=%8.3f ms | target=%8.3f ms | ratio=%.3fx  (iters=%d)\n\n",
                "temp_alloc", type_name, t_malloc_temp, t_temp,
                (t_malloc_temp > 0.0 ? t_temp / t_malloc_temp : 0.0), temp_iters);
        }

        inline void thread_worker_malloc(int iters, std::atomic<int>& barrier)
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) {}

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
            while (barrier.load() < THREAD_COUNT + 1) {}

            for (int i = 0; i < iters; ++i)
            {
                void* p = sys_alloc(64, 8);
                volatile char* vp = reinterpret_cast<volatile char*>(p);
                (void)vp;
                sys_free(p, 64, 8);
            }
        }

        inline void thread_worker_spool(int iters, std::atomic<int>& barrier)
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT + 1) {}

            for (int i = 0; i < iters; ++i)
            {
                void* p = spool_alloc(64, 8);
                volatile char* vp = reinterpret_cast<volatile char*>(p);
                (void)vp;
                spool_free(p, 64, 8);
            }
        }

        template<typename WorkerFn>
        inline double bench_mt(WorkerFn worker_fn, int num_threads, int iters_per_thread)
        {
            std::atomic<int> barrier(0);
            std::vector<std::thread> threads;
            threads.reserve(num_threads);

            for (int t = 0; t < num_threads; ++t)
                threads.emplace_back(worker_fn, iters_per_thread, std::ref(barrier));

            auto start = Clock::now();
            barrier.fetch_add(1);
            for (auto& th : threads) th.join();
            return elapsed_ms(start, Clock::now());
        }
    }

    inline void Benchmark_PrivateMemory2()
    {
        using namespace benchmark_privatememory_detail;

        const bool initialized = initialize_core_memory(
            256,
            256,
            2048,
            2048,
            TEMP_POOL_SIZE);

        if (!initialized)
        {
            printf("[PrivateMemory2 Benchmark] initialize_core_memory2 failed.\n");
            return;
        }

        printf("============================================================\n");
        printf("  PrivateMemory2 Benchmark  (iterations=%d)\n", ITERATIONS);
        printf("  Format: ratio = target_ms / malloc_ms  (lower is better)\n");
        printf("============================================================\n\n");

        printf("---- [Single-Thread] SmallData (32 bytes) ----\n");
        run_single_suite<SmallData>("Small(32)", ITERATIONS);

        printf("---- [Single-Thread] MediumData (256 bytes) ----\n");
        run_single_suite<MediumData>("Med(256)", ITERATIONS);

        printf("---- [Single-Thread] LargeData (2048 bytes) ----\n");
        run_single_suite<LargeData>("Large(2K)", ITERATIONS);

        printf("---- [Single-Thread] HugeData (16384 bytes) ----\n");
        run_single_suite<HugeData>("Huge(16K)", ITERATIONS);

        printf("============================================================\n");
        printf("  Multi-Thread Benchmark  (threads=%d, iters/thread=%d)\n", THREAD_COUNT, THREAD_ITERS);
        printf("============================================================\n\n");

        {
            double t_malloc = bench_mt(thread_worker_malloc, THREAD_COUNT, THREAD_ITERS);
            double t_sys = bench_mt(thread_worker_sys, THREAD_COUNT, THREAD_ITERS);
            double ratio = (t_malloc > 0.0) ? (t_sys / t_malloc) : 0.0;

            printf("---- [Multi-Thread] sys_alloc vs malloc (64 bytes) ----\n");
            printf("  malloc    : %.3f ms\n", t_malloc);
            printf("  sys_alloc : %.3f ms  ratio=%.3fx\n\n", t_sys, ratio);
        }

        {
            double t_malloc = bench_mt(thread_worker_malloc, THREAD_COUNT, THREAD_ITERS);
            double t_spool = bench_mt(thread_worker_spool, THREAD_COUNT, THREAD_ITERS);
            double ratio = (t_malloc > 0.0) ? (t_spool / t_malloc) : 0.0;

            printf("---- [Multi-Thread] spool_alloc vs malloc (64 bytes) ----\n");
            printf("  malloc      : %.3f ms\n", t_malloc);
            printf("  spool_alloc : %.3f ms  ratio=%.3fx\n\n", t_spool, ratio);
        }

        shutdown_core_memory();

        printf("============================================================\n");
        printf("  Benchmark Complete\n");
        printf("============================================================\n");
    }
}
