#pragma once
#include "SpinLock.h"
#include <cstdio>
#include <thread>
#include <atomic>

namespace mark
{
    namespace test_spinlock_detail
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

#define SPIN_CHECK(expr, tag) mark::test_spinlock_detail::check((expr), (tag))
    }

    // ========================================================================
    // Init / Basic acquire / release
    // ========================================================================
    inline void TestSpin_InitBasic()
    {
        printf("[TestSpin_InitBasic]\n");

        spin_lock_t sl;
        init_spin_lock(&sl);
        SPIN_CHECK(sl.stat == 0, "init: stat == 0");

        acquire_spin_lock(&sl);
        SPIN_CHECK(sl.stat != 0, "acquire: stat != 0");

        release_spin_lock(&sl);
        SPIN_CHECK(sl.stat == 0, "release: stat == 0");
    }

    // ========================================================================
    // Thread safety - mutual exclusion
    // ========================================================================
    inline void TestSpin_MutualExclusion()
    {
        printf("[TestSpin_MutualExclusion]\n");

        spin_lock_t sl;
        init_spin_lock(&sl);

        int shared_value = 0;
        const int iterations = 5000;

        auto worker = [&]() {
            for (int i = 0; i < iterations; ++i)
            {
                acquire_spin_lock(&sl);
                ++shared_value;
                release_spin_lock(&sl);
            }
        };

        std::thread t1(worker);
        std::thread t2(worker);
        std::thread t3(worker);
        std::thread t4(worker);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        SPIN_CHECK(shared_value == iterations * 4, "mutual exclusion: correct count");
    }

    // ========================================================================
    // AUTO_SPIN_LOCK
    // ========================================================================
    inline void TestSpin_AutoLock()
    {
        printf("[TestSpin_AutoLock]\n");

        spin_lock_t sl;
        init_spin_lock(&sl);

        {
            AUTO_SPIN_LOCK auto_sl(&sl);
            SPIN_CHECK(sl.stat != 0, "AUTO_SPIN_LOCK: locked inside scope");
        }
        SPIN_CHECK(sl.stat == 0, "AUTO_SPIN_LOCK: unlocked after scope");
    }

    // ========================================================================
    // AUTO_SPIN_LOCK with threads
    // ========================================================================
    inline void TestSpin_AutoLockThreads()
    {
        printf("[TestSpin_AutoLockThreads]\n");

        spin_lock_t sl;
        init_spin_lock(&sl);

        int shared_value = 0;
        const int iterations = 2000;

        auto worker = [&]() {
            for (int i = 0; i < iterations; ++i)
            {
                AUTO_SPIN_LOCK auto_sl(&sl);
                ++shared_value;
            }
        };

        std::thread t1(worker);
        std::thread t2(worker);

        t1.join();
        t2.join();

        SPIN_CHECK(shared_value == iterations * 2, "auto lock threads: correct count");
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_SpinLock()
    {
        using namespace test_spinlock_detail;
        reset_counters();

        printf("========================================\n");
        printf("  SpinLock Test Suite\n");
        printf("========================================\n");

        TestSpin_InitBasic();
        TestSpin_MutualExclusion();
        TestSpin_AutoLock();
        TestSpin_AutoLockThreads();

        print_summary();
    }

} // namespace mark
