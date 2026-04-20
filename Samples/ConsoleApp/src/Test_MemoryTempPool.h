#pragma once
#include "MemoryTempPool.h"
#include <cstdio>
#include <cstring>

namespace mark
{
    namespace test_temppool_detail
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

#define TP_CHECK(expr, tag) mark::test_temppool_detail::check((expr), (tag))
    }

    // ========================================================================
    // Create / Destroy
    // ========================================================================
    inline void TestTP_CreateDestroy()
    {
        printf("[TestTP_CreateDestroy]\n");

        HANDLE h = temppool_create(1024);
        TP_CHECK(h != nullptr, "create: non-null handle");

        temppool_destroy(h);
        TP_CHECK(true, "destroy: no crash");

        // destroy nullptr should not crash
        temppool_destroy(nullptr);
        TP_CHECK(true, "destroy nullptr: no crash");
    }

    // ========================================================================
    // Alloc
    // ========================================================================
    inline void TestTP_Alloc()
    {
        printf("[TestTP_Alloc]\n");

        HANDLE h = temppool_create(1024);
        TP_CHECK(h != nullptr, "alloc: create");

        // basic allocation
        void* p1 = temppool_alloc(h, 64);
        TP_CHECK(p1 != nullptr, "alloc: 64 bytes");

        void* p2 = temppool_alloc(h, 128);
        TP_CHECK(p2 != nullptr, "alloc: 128 bytes");
        TP_CHECK(p1 != p2, "alloc: different pointers");

        // write and verify
        memset(p1, 0xAB, 64);
        memset(p2, 0xCD, 128);
        TP_CHECK(((uint8_t*)p1)[0] == 0xAB, "alloc: p1 data intact");
        TP_CHECK(((uint8_t*)p2)[0] == 0xCD, "alloc: p2 data intact");

        // allocation exceeding pool size should fail
        void* p3 = temppool_alloc(h, 2048);
        TP_CHECK(p3 == nullptr, "alloc: exceeds pool -> nullptr");

        // nullptr handle
        void* p4 = temppool_alloc(nullptr, 64);
        TP_CHECK(p4 == nullptr, "alloc: nullptr handle -> nullptr");

        temppool_destroy(h);
    }

    // ========================================================================
    // Reset
    // ========================================================================
    inline void TestTP_Reset()
    {
        printf("[TestTP_Reset]\n");

        HANDLE h = temppool_create(256);
        TP_CHECK(h != nullptr, "reset: create");

        // fill up pool
        void* p1 = temppool_alloc(h, 128);
        TP_CHECK(p1 != nullptr, "reset: first alloc");

        void* p2 = temppool_alloc(h, 128);
        TP_CHECK(p2 != nullptr, "reset: second alloc");

        // pool should be full now
        void* p3 = temppool_alloc(h, 16);
        TP_CHECK(p3 == nullptr, "reset: pool full -> nullptr");

        // reset and allocate again
        temppool_reset(h);

        void* p4 = temppool_alloc(h, 128);
        TP_CHECK(p4 != nullptr, "reset: alloc after reset");

        // reset nullptr should not crash
        temppool_reset(nullptr);
        TP_CHECK(true, "reset nullptr: no crash");

        temppool_destroy(h);
    }

    // ========================================================================
    // Alignment
    // ========================================================================
    inline void TestTP_Alignment()
    {
        printf("[TestTP_Alignment]\n");

        HANDLE h = temppool_create(4096);
        TP_CHECK(h != nullptr, "align: create");

        // allocations should be 16-byte aligned
        void* p1 = temppool_alloc(h, 1);
        void* p2 = temppool_alloc(h, 1);
        TP_CHECK(((uintptr_t)p1 % 16) == 0, "align: p1 16-byte aligned");
        TP_CHECK(((uintptr_t)p2 % 16) == 0, "align: p2 16-byte aligned");

        // distance between allocations should be at least 16 (alignment)
        uintptr_t diff = (uintptr_t)p2 - (uintptr_t)p1;
        TP_CHECK(diff >= 16, "align: min gap is 16");

        temppool_destroy(h);
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_MemoryTempPool()
    {
        using namespace test_temppool_detail;
        reset_counters();

        printf("========================================\n");
        printf("  MemoryTempPool Test Suite\n");
        printf("========================================\n");

        TestTP_CreateDestroy();
        TestTP_Alloc();
        TestTP_Reset();
        TestTP_Alignment();

        print_summary();
    }

} // namespace mark
