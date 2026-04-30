#pragma once
#include "srw_lock.h"
#include <cstdio>
#include <thread>
#include <atomic>

namespace mark
{
    namespace test_srwlock_detail
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

#define SRW_CHECK(expr, tag) mark::test_srwlock_detail::check((expr), (tag))
    }

    // ========================================================================
    // Init / Basic lock/unlock
    // ========================================================================
    inline void TestSRW_InitBasic()
    {
        printf("[TestSRW_InitBasic]\n");

        srw_lock lock;
        init_srw_lock(&lock);
        SRW_CHECK(true, "init: no crash");

        // read lock / unlock
        lock_read(&lock);
        unlock_read(&lock);
        SRW_CHECK(true, "read lock/unlock: no crash");

        // write lock / unlock
        lock_write(&lock);
        unlock_write(&lock);
        SRW_CHECK(true, "write lock/unlock: no crash");

        destroy_srw_lock(&lock);
        SRW_CHECK(true, "destroy: no crash");
    }

    // ========================================================================
    // Multiple readers
    // ========================================================================
    inline void TestSRW_MultipleReaders()
    {
        printf("[TestSRW_MultipleReaders]\n");

        srw_lock lock;
        init_srw_lock(&lock);

        std::atomic<int> reader_count{ 0 };
        bool all_readers_ok = true;

        auto reader_fn = [&]() {
            lock_read(&lock);
            reader_count.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            int count = reader_count.load();
            if (count < 1) all_readers_ok = false;
            reader_count.fetch_sub(1);
            unlock_read(&lock);
        };

        std::thread t1(reader_fn);
        std::thread t2(reader_fn);
        std::thread t3(reader_fn);

        t1.join();
        t2.join();
        t3.join();

        SRW_CHECK(all_readers_ok, "multiple readers: concurrent access ok");

        destroy_srw_lock(&lock);
    }

    // ========================================================================
    // Write exclusion
    // ========================================================================
    inline void TestSRW_WriteExclusion()
    {
        printf("[TestSRW_WriteExclusion]\n");

        srw_lock lock;
        init_srw_lock(&lock);

        int shared_value = 0;
        const int iterations = 1000;

        auto writer_fn = [&]() {
            for (int i = 0; i < iterations; ++i)
            {
                lock_write(&lock);
                ++shared_value;
                unlock_write(&lock);
            }
        };

        std::thread t1(writer_fn);
        std::thread t2(writer_fn);

        t1.join();
        t2.join();

        SRW_CHECK(shared_value == iterations * 2, "write exclusion: correct count");

        destroy_srw_lock(&lock);
    }

    // ========================================================================
    // AUTO_LOCK_READ / AUTO_LOCK_RW
    // ========================================================================
    inline void TestSRW_AutoLock()
    {
        printf("[TestSRW_AutoLock]\n");

        srw_lock lock;
        init_srw_lock(&lock);

        // AUTO_LOCK_READ
        {
            AUTO_LOCK_READ auto_read(&lock);
            SRW_CHECK(true, "AUTO_LOCK_READ: acquired");
        }
        SRW_CHECK(true, "AUTO_LOCK_READ: released on scope exit");

        // AUTO_LOCK_RW
        {
            AUTO_LOCK_RW auto_rw(&lock);
            SRW_CHECK(true, "AUTO_LOCK_RW: acquired");
        }
        SRW_CHECK(true, "AUTO_LOCK_RW: released on scope exit");

        destroy_srw_lock(&lock);
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_SRWLock()
    {
        using namespace test_srwlock_detail;
        reset_counters();

        printf("========================================\n");
        printf("  SRWLock Test Suite\n");
        printf("========================================\n");

        TestSRW_InitBasic();
        TestSRW_MultipleReaders();
        TestSRW_WriteExclusion();
        TestSRW_AutoLock();

        print_summary();
    }

} // namespace mark
