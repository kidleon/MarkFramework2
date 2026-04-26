#pragma once

#include "CoreHeap.h"
#include <cstdio>
#include <thread>
#include <vector>
#include <atomic>

namespace mark
{
    namespace test_privatememory2_detail
    {
        struct LifecycleProbe
        {
            static int ctor_count;
            static int dtor_count;
            int value;

            LifecycleProbe() : value(7)
            {
                ++ctor_count;
            }

            explicit LifecycleProbe(int v) : value(v)
            {
                ++ctor_count;
            }

            ~LifecycleProbe()
            {
                ++dtor_count;
            }
        };

        inline void reset_lifecycle()
        {
            LifecycleProbe::ctor_count = 0;
            LifecycleProbe::dtor_count = 0;
        }

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

#define PM2_CHECK(expr, tag) mark::test_privatememory2_detail::check((expr), (tag))
    }

    inline int test_privatememory2_detail::LifecycleProbe::ctor_count = 0;
    inline int test_privatememory2_detail::LifecycleProbe::dtor_count = 0;

    inline void TestPM2_InitShutdown()
    {
        printf("[TestPM2_InitShutdown]\n");

        shutdown_core_memory();

        bool ok = initialize_core_memory(256, 256, 1024, 1024, 1024 * 1024 * 4);
        PM2_CHECK(ok, "initialize_core_memory2 success");

        shutdown_core_memory();
        PM2_CHECK(true, "shutdown_core_memory2 no crash");

        ok = initialize_core_memory(256, 256, 1024, 1024, 1024 * 1024 * 4);
        PM2_CHECK(ok, "re-initialize_core_memory2 success");
    }

    inline void TestPM2_SysAllocFree()
    {
        printf("[TestPM2_SysAllocFree]\n");

        void* p1 = sys_alloc(64, 8);
        PM2_CHECK(p1 != nullptr, "sys_alloc 64 bytes");
        PM2_CHECK((reinterpret_cast<uintptr_t>(p1) % 8) == 0, "sys_alloc align 8");
        sys_free(p1, 64, 8);

        void* p2 = sys_alloc(256, 16);
        PM2_CHECK(p2 != nullptr, "sys_alloc 256 bytes");
        PM2_CHECK((reinterpret_cast<uintptr_t>(p2) % 16) == 0, "sys_alloc align 16");
        sys_free(p2, 256, 16);
    }

    inline void TestPM2_SpoolAllocFree()
    {
        printf("[TestPM2_SpoolAllocFree]\n");

        void* p1 = spool_alloc(64, 8);
        PM2_CHECK(p1 != nullptr, "spool_alloc 64 bytes");
        PM2_CHECK((reinterpret_cast<uintptr_t>(p1) % 8) == 0, "spool_alloc align 8");
        spool_free(p1, 64, 8);

        void* p2 = spool_alloc(512, 16);
        PM2_CHECK(p2 != nullptr, "spool_alloc 512 bytes");
        PM2_CHECK((reinterpret_cast<uintptr_t>(p2) % 16) == 0, "spool_alloc align 16");
        spool_free(p2, 512, 16);
    }

    inline void TestPM2_UpoolAllocFree()
    {
        printf("[TestPM2_UpoolAllocFree]\n");

        void* p1 = upool_alloc(64, 8);
        PM2_CHECK(p1 != nullptr, "upool_alloc 64 bytes");
        PM2_CHECK((reinterpret_cast<uintptr_t>(p1) % 8) == 0, "upool_alloc align 8");
        upool_free(p1, 64, 8);

        void* p2 = upool_alloc(512, 16);
        PM2_CHECK(p2 != nullptr, "upool_alloc 512 bytes");
        PM2_CHECK((reinterpret_cast<uintptr_t>(p2) % 16) == 0, "upool_alloc align 16");
        upool_free(p2, 512, 16);
    }

    inline void TestPM2_TempAllocReset()
    {
        printf("[TestPM2_TempAllocReset]\n");

        void* p1 = temp_alloc(128, 8);
        void* p2 = temp_alloc(128, 8);
        PM2_CHECK(p1 != nullptr, "temp_alloc first");
        PM2_CHECK(p2 != nullptr, "temp_alloc second");

        temp_reset();
        PM2_CHECK(true, "temp_reset no crash");

        void* p3 = temp_alloc(128, 8);
        PM2_CHECK(p3 != nullptr, "temp_alloc after reset");
    }

    inline void TestPM2_MultiThreadSys()
    {
        printf("[TestPM2_MultiThreadSys]\n");

        constexpr int thread_count = 4;
        constexpr int iterations = 10000;
        std::atomic<int> failed{ 0 };

        auto worker = [&failed]() {
            for (int i = 0; i < iterations; ++i)
            {
                void* p = sys_alloc(64, 8);
                if (p == nullptr)
                {
                    failed.fetch_add(1);
                    continue;
                }
                sys_free(p, 64, 8);
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(thread_count);
        for (int i = 0; i < thread_count; ++i)
            threads.emplace_back(worker);
        for (auto& t : threads)
            t.join();

        PM2_CHECK(failed.load() == 0, "multithread sys_alloc/sys_free");
    }

    inline void TestPM2_MultiThreadSpool()
    {
        printf("[TestPM2_MultiThreadSpool]\n");

        constexpr int thread_count = 4;
        constexpr int iterations = 10000;
        std::atomic<int> failed{ 0 };

        auto worker = [&failed]() {
            for (int i = 0; i < iterations; ++i)
            {
                void* p = spool_alloc(64, 8);
                if (p == nullptr)
                {
                    failed.fetch_add(1);
                    continue;
                }
                spool_free(p, 64, 8);
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(thread_count);
        for (int i = 0; i < thread_count; ++i)
            threads.emplace_back(worker);
        for (auto& t : threads)
            t.join();

        PM2_CHECK(failed.load() == 0, "multithread spool_alloc/spool_free");
    }

    inline void TestPM2_SysNewDelete()
    {
        using namespace test_privatememory2_detail;
        printf("[TestPM2_SysNewDelete]\n");

        reset_lifecycle();
        LifecycleProbe* p = SYS_NEW_ARRAY(LifecycleProbe, 123);
        PM2_CHECK(p != nullptr, "sys_new object");
        PM2_CHECK(p && p->value == 123, "sys_new ctor args");
        sys_delete(p);
        PM2_CHECK(LifecycleProbe::ctor_count == 1, "sys_new ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == 1, "sys_delete dtor count");

        reset_lifecycle();
        constexpr size_t kCount = 8;
        LifecycleProbe* arr = SYS_NEW_ARRAY(LifecycleProbe, kCount);
        PM2_CHECK(arr != nullptr, "sys_new_array");
        sys_delete_array(arr, kCount);
        PM2_CHECK(LifecycleProbe::ctor_count == static_cast<int>(kCount), "sys_new_array ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == static_cast<int>(kCount), "sys_delete_array dtor count");
    }

    inline void TestPM2_SpoolNewDelete()
    {
        using namespace test_privatememory2_detail;
        printf("[TestPM2_SpoolNewDelete]\n");

        reset_lifecycle();
        LifecycleProbe* p = SPOOL_NEW(LifecycleProbe, 456);
        PM2_CHECK(p != nullptr, "spool_new object");
        PM2_CHECK(p && p->value == 456, "spool_new ctor args");
        spool_delete(p);
        PM2_CHECK(LifecycleProbe::ctor_count == 1, "spool_new ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == 1, "spool_delete dtor count");

        reset_lifecycle();
        constexpr size_t kCount = 8;
        LifecycleProbe* arr = SPOOL_NEW_ARRAY(LifecycleProbe, kCount);
        PM2_CHECK(arr != nullptr, "spool_new_array");
        spool_delete_array(arr, kCount);
        PM2_CHECK(LifecycleProbe::ctor_count == static_cast<int>(kCount), "spool_new_array ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == static_cast<int>(kCount), "spool_delete_array dtor count");
    }

    inline void TestPM2_UpoolNewDelete()
    {
        using namespace test_privatememory2_detail;
        printf("[TestPM2_UpoolNewDelete]\n");

        reset_lifecycle();
        LifecycleProbe* p = UPOOL_NEW(LifecycleProbe, 789);
        PM2_CHECK(p != nullptr, "upool_new object");
        PM2_CHECK(p && p->value == 789, "upool_new ctor args");
        upool_delete(p);
        PM2_CHECK(LifecycleProbe::ctor_count == 1, "upool_new ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == 1, "upool_delete dtor count");

        reset_lifecycle();
        constexpr size_t kCount = 8;
        LifecycleProbe* arr = UPOOL_NEW_ARRAY(LifecycleProbe, kCount);
        PM2_CHECK(arr != nullptr, "upool_new_array");
        UPOOL_DELETE_ARRAY(LifecycleProbe, arr, kCount);
        PM2_CHECK(LifecycleProbe::ctor_count == static_cast<int>(kCount), "upool_new_array ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == static_cast<int>(kCount), "upool_delete_array dtor count");
    }

    inline void TestPM2_TempNewDelete()
    {
        using namespace test_privatememory2_detail;
        printf("[TestPM2_TempNewDelete]\n");

        reset_lifecycle();
        LifecycleProbe* p = TEMP_NEW(LifecycleProbe, 321);
        PM2_CHECK(p != nullptr, "temp_new object");
        PM2_CHECK(p && p->value == 321, "temp_new ctor args");
        TEMP_DELETE(LifecycleProbe, p);
        PM2_CHECK(LifecycleProbe::ctor_count == 1, "temp_new ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == 1, "temp_delete dtor count");

        reset_lifecycle();
        constexpr size_t kCount = 8;
        LifecycleProbe* arr = TEMP_NEW_ARRAY(LifecycleProbe, kCount);
        PM2_CHECK(arr != nullptr, "temp_new_array");
        TEMP_DELETE_ARRAY(LifecycleProbe, arr, kCount);
        PM2_CHECK(LifecycleProbe::ctor_count == static_cast<int>(kCount), "temp_new_array ctor count");
        PM2_CHECK(LifecycleProbe::dtor_count == static_cast<int>(kCount), "temp_delete_array dtor count");

        temp_reset();
    }

    inline void Test_PrivateMemory2()
    {
        using namespace test_privatememory2_detail;
        reset_counters();

        printf("========================================\n");
        printf("  PrivateMemory2 Test Suite\n");
        printf("========================================\n");

        TestPM2_InitShutdown();
        TestPM2_SysAllocFree();
        TestPM2_SpoolAllocFree();
        TestPM2_UpoolAllocFree();
        TestPM2_TempAllocReset();
        TestPM2_SysNewDelete();
        TestPM2_SpoolNewDelete();
        TestPM2_UpoolNewDelete();
        TestPM2_TempNewDelete();
        TestPM2_MultiThreadSys();
        TestPM2_MultiThreadSpool();

        shutdown_core_memory();

        print_summary();
    }
}
