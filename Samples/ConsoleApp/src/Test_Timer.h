#pragma once
#include "Timer.h"
#include <cstdio>
#include <cstring>
#include <thread>

namespace mark
{
    namespace test_timer_detail
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

#define TMR_CHECK(expr, tag) mark::test_timer_detail::check((expr), (tag))
    }

    // ========================================================================
    // Create / Destroy
    // ========================================================================
    inline void TestTimer_CreateDestroy()
    {
        printf("[TestTimer_CreateDestroy]\n");

        HANDLE h = timer::highrestimer_create();
        TMR_CHECK(h != nullptr, "create: non-null");

        timer::highrestimer_destroy(h);
        TMR_CHECK(true, "destroy: no crash");

        // nullptr destroy
        timer::highrestimer_destroy(nullptr);
        TMR_CHECK(true, "destroy nullptr: no crash");
    }

    // ========================================================================
    // Start / GetTime
    // ========================================================================
    inline void TestTimer_StartGetTime()
    {
        printf("[TestTimer_StartGetTime]\n");

        HANDLE h = timer::highrestimer_create();
        TMR_CHECK(h != nullptr, "gettime: create");

        timer::highrestimer_start(h);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        double t = timer::highrestimer_gettime(h);
        TMR_CHECK(t > 0.0, "gettime: time > 0 after create");

        timer::highrestimer_destroy(h);

        // nullptr
        TMR_CHECK(timer::highrestimer_gettime(nullptr) == 0.0, "gettime: nullptr == 0");
    }

    // ========================================================================
    // Stop / Resume
    // ========================================================================
    inline void TestTimer_StopResume()
    {
        printf("[TestTimer_StopResume]\n");

        HANDLE h = timer::highrestimer_create();
        timer::highrestimer_start(h);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        timer::highrestimer_stop(h);

        // stop nullptr - no crash
        timer::highrestimer_stop(nullptr);
        TMR_CHECK(true, "stop nullptr: no crash");

        timer::highrestimer_resume(h);

        // resume nullptr - no crash
        timer::highrestimer_resume(nullptr);
        TMR_CHECK(true, "resume nullptr: no crash");

        timer::highrestimer_destroy(h);
    }

    // ========================================================================
    // Reset
    // ========================================================================
    inline void TestTimer_Reset()
    {
        printf("[TestTimer_Reset]\n");

        HANDLE h = timer::highrestimer_create();
        timer::highrestimer_start(h);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        timer::highrestimer_reset(h);

        // after reset, start nullptr - no crash
        timer::highrestimer_reset(nullptr);
        TMR_CHECK(true, "reset nullptr: no crash");

        timer::highrestimer_destroy(h);
    }

    // ========================================================================
    // ElapsedTime
    // ========================================================================
    inline void TestTimer_ElapsedTime()
    {
        printf("[TestTimer_ElapsedTime]\n");

        HANDLE h = timer::highrestimer_create();
        timer::highrestimer_start(h);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        double e1 = timer::highrestimer_getelapsedtime(h);
        TMR_CHECK(e1 > 0.0, "elapsed: first call > 0");

        double e2 = timer::highrestimer_getelapsedtime(h);
        TMR_CHECK(e2 >= 0.0, "elapsed: second call >= 0");
        TMR_CHECK(e2 < e1, "elapsed: second call < first (shorter interval)");

        TMR_CHECK(timer::highrestimer_getelapsedtime(nullptr) == 0.0, "elapsed: nullptr == 0");

        timer::highrestimer_destroy(h);
    }

    // ========================================================================
    // GetTimestamp
    // ========================================================================
    inline void TestTimer_Timestamp()
    {
        printf("[TestTimer_Timestamp]\n");

        char buf[64] = { 0 };
        timer::get_timestamp(buf, sizeof(buf));

        // format: "YYYY-MM-DD HH:MM:SS" -> length 19
        TMR_CHECK(strlen(buf) == 19, "timestamp: length == 19");
        TMR_CHECK(buf[4] == '-', "timestamp: buf[4] == '-'");
        TMR_CHECK(buf[7] == '-', "timestamp: buf[7] == '-'");
        TMR_CHECK(buf[10] == ' ', "timestamp: buf[10] == ' '");
        TMR_CHECK(buf[13] == ':', "timestamp: buf[13] == ':'");
        TMR_CHECK(buf[16] == ':', "timestamp: buf[16] == ':'");

        // nullptr / zero size - no crash
        timer::get_timestamp(nullptr, 64);
        TMR_CHECK(true, "timestamp: nullptr no crash");

        timer::get_timestamp(buf, 0);
        TMR_CHECK(true, "timestamp: zero size no crash");
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_Timer()
    {
        using namespace test_timer_detail;
        reset_counters();

        printf("========================================\n");
        printf("  Timer Test Suite\n");
        printf("========================================\n");

        TestTimer_CreateDestroy();
        TestTimer_StartGetTime();
        TestTimer_StopResume();
        TestTimer_Reset();
        TestTimer_ElapsedTime();
        TestTimer_Timestamp();

        print_summary();
    }

} // namespace mark
