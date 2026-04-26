#pragma once

#include "CoreHeap.h"
#include <cstdio>
#include <thread>
#include <vector>
#include <atomic>
#include <memory_resource>

namespace mark
{
    namespace test_coreheap_detail
    {
        // -----------------------------------------------------------------------
        // 생명주기 추적 구조체
        // -----------------------------------------------------------------------
        struct Probe
        {
            static int ctor_count;
            static int dtor_count;
            int value;

            Probe() : value(42) { ++ctor_count; }
            explicit Probe(int v) : value(v) { ++ctor_count; }
            ~Probe() { ++dtor_count; }
        };

        inline int Probe::ctor_count = 0;
        inline int Probe::dtor_count = 0;

        inline void reset_probe()
        {
            Probe::ctor_count = 0;
            Probe::dtor_count = 0;
        }

        // -----------------------------------------------------------------------
        // 테스트 카운터 & 유틸
        // -----------------------------------------------------------------------
        static int g_total  = 0;
        static int g_passed = 0;
        static int g_failed = 0;

        inline void reset_counters()
        {
            g_total = g_passed = g_failed = 0;
        }

        inline void check(bool cond, const char* tag)
        {
            ++g_total;
            if (cond) { ++g_passed; }
            else      { ++g_failed; printf("  [FAIL] %s\n", tag); }
        }

        inline void print_summary()
        {
            printf("========================================\n");
            printf("  Total : %d   Passed : %d   Failed : %d\n", g_total, g_passed, g_failed);
            printf("========================================\n\n");
        }

        inline bool is_aligned(const void* p, size_t align)
        {
            return (reinterpret_cast<uintptr_t>(p) % align) == 0;
        }

#define CH_CHECK(expr, tag) mark::test_coreheap_detail::check((expr), (tag))
    }

    // =========================================================================
    // 1. 초기화 / 종료
    // =========================================================================
    inline void TestCH_InitShutdown()
    {
        printf("[TestCH_InitShutdown]\n");

        shutdown_core_memory();

        bool ok = initialize_core_memory(256, 256, 1024, 1024, 1024 * 1024 * 4);
        CH_CHECK(ok, "initialize_core_memory success");

        shutdown_core_memory();
        CH_CHECK(true, "shutdown_core_memory no crash");

        // 재초기화
        ok = initialize_core_memory(256, 256, 1024, 1024, 1024 * 1024 * 4);
        CH_CHECK(ok, "re-initialize_core_memory success");
    }

    // =========================================================================
    // 2. 기본 alloc / free (정렬 검증 포함)
    // =========================================================================
    inline void TestCH_SysAllocFree()
    {
        printf("[TestCH_SysAllocFree]\n");
        using namespace test_coreheap_detail;

        void* p = sys_alloc(64, 8);
        CH_CHECK(p != nullptr,          "sys_alloc 64B not null");
        CH_CHECK(is_aligned(p, 8),      "sys_alloc align 8");
        sys_free(p, 64, 8);

        p = sys_alloc(256, 16);
        CH_CHECK(p != nullptr,          "sys_alloc 256B not null");
        CH_CHECK(is_aligned(p, 16),     "sys_alloc align 16");
        sys_free(p, 256, 16);

        p = sys_alloc(512, 64);
        CH_CHECK(p != nullptr,          "sys_alloc 512B not null");
        CH_CHECK(is_aligned(p, 64),     "sys_alloc align 64");
        sys_free(p, 512, 64);
    }

    inline void TestCH_SpoolAllocFree()
    {
        printf("[TestCH_SpoolAllocFree]\n");
        using namespace test_coreheap_detail;

        void* p = spool_alloc(64, 8);
        CH_CHECK(p != nullptr,          "spool_alloc 64B not null");
        CH_CHECK(is_aligned(p, 8),      "spool_alloc align 8");
        spool_free(p, 64, 8);

        p = spool_alloc(512, 16);
        CH_CHECK(p != nullptr,          "spool_alloc 512B not null");
        CH_CHECK(is_aligned(p, 16),     "spool_alloc align 16");
        spool_free(p, 512, 16);
    }

    inline void TestCH_UpoolAllocFree()
    {
        printf("[TestCH_UpoolAllocFree]\n");
        using namespace test_coreheap_detail;

        void* p = upool_alloc(64, 8);
        CH_CHECK(p != nullptr,          "upool_alloc 64B not null");
        CH_CHECK(is_aligned(p, 8),      "upool_alloc align 8");
        upool_free(p, 64, 8);

        p = upool_alloc(512, 16);
        CH_CHECK(p != nullptr,          "upool_alloc 512B not null");
        CH_CHECK(is_aligned(p, 16),     "upool_alloc align 16");
        upool_free(p, 512, 16);
    }

    inline void TestCH_TempAllocReset()
    {
        printf("[TestCH_TempAllocReset]\n");
        using namespace test_coreheap_detail;

        void* p1 = temp_alloc(128, 8);
        void* p2 = temp_alloc(128, 8);
        CH_CHECK(p1 != nullptr,         "temp_alloc first not null");
        CH_CHECK(p2 != nullptr,         "temp_alloc second not null");
        CH_CHECK(is_aligned(p1, 8),     "temp_alloc align 8 (first)");
        CH_CHECK(is_aligned(p2, 8),     "temp_alloc align 8 (second)");
        CH_CHECK(p1 != p2,              "temp_alloc sequential pointers differ");

        temp_reset();
        CH_CHECK(true,                  "temp_reset no crash");

        void* p3 = temp_alloc(128, 8);
        CH_CHECK(p3 != nullptr,         "temp_alloc after reset not null");
        temp_reset();
    }

    // =========================================================================
    // 3. source_location 버전 (항상 선언되어 있음)
    // =========================================================================
    inline void TestCH_SysAllocFreeWithLoc()
    {
        printf("[TestCH_SysAllocFreeWithLoc]\n");
        using namespace test_coreheap_detail;

        auto loc = std::source_location::current();
        void* p = sys_alloc(128, 16, loc);
        CH_CHECK(p != nullptr,          "sys_alloc(loc) not null");
        CH_CHECK(is_aligned(p, 16),     "sys_alloc(loc) align 16");
        sys_free(p, 128, 16);

        p = spool_alloc(128, 16, loc);
        CH_CHECK(p != nullptr,          "spool_alloc(loc) not null");
        CH_CHECK(is_aligned(p, 16),     "spool_alloc(loc) align 16");
        spool_free(p, 128, 16);

        p = upool_alloc(128, 16, loc);
        CH_CHECK(p != nullptr,          "upool_alloc(loc) not null");
        CH_CHECK(is_aligned(p, 16),     "upool_alloc(loc) align 16");
        upool_free(p, 128, 16);
    }

    // =========================================================================
    // 4. memory_resource 포인터 유효성
    // =========================================================================
    inline void TestCH_MemoryResourcePtrs()
    {
        printf("[TestCH_MemoryResourcePtrs]\n");

        CH_CHECK(get_default_system_memory_resource_ptr() != nullptr,  "sys  memory_resource not null");
        CH_CHECK(get_default_spool_memory_resource_ptr()  != nullptr,  "spool memory_resource not null");
        CH_CHECK(get_default_usync_pool_memory_resource_ptr() != nullptr, "upool memory_resource not null");
        CH_CHECK(get_default_temp_memory_resource_ptr()   != nullptr,  "temp  memory_resource not null");
    }

    // =========================================================================
    // 5. sys_new / sys_delete (기본 정렬)
    // =========================================================================
    inline void TestCH_SysNewDelete()
    {
        printf("[TestCH_SysNewDelete]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        Probe* p = SYS_NEW(Probe, 100);
        CH_CHECK(p != nullptr,                  "sys_new<Probe>(100) not null");
        CH_CHECK(p && p->value == 100,          "sys_new ctor arg forwarded");
        SYS_DELETE(Probe, p);
        CH_CHECK(Probe::ctor_count == 1,        "sys_new  ctor count == 1");
        CH_CHECK(Probe::dtor_count == 1,        "sys_delete dtor count == 1");

        // 기본 생성자
        reset_probe();
        Probe* p2 = SYS_NEW(Probe);
        CH_CHECK(p2 != nullptr,                 "sys_new<Probe>() not null");
        CH_CHECK(p2 && p2->value == 42,         "sys_new default ctor value");
        SYS_DELETE(Probe, p2);
        CH_CHECK(Probe::dtor_count == 1,        "sys_delete default ctor dtor");
    }

    // =========================================================================
    // 6. sys_new / sys_delete (커스텀 정렬)
    // =========================================================================
    inline void TestCH_SysNewDeleteAlign()
    {
        printf("[TestCH_SysNewDeleteAlign]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        Probe* p = SYS_NEW_A(Probe, 64, 200);
        CH_CHECK(p != nullptr,              "sys_new<Probe,64> not null");
        CH_CHECK(is_aligned(p, 64),         "sys_new<Probe,64> alignment");
        CH_CHECK(p && p->value == 200,      "sys_new<Probe,64> ctor arg");
        SYS_DELETE_A(Probe, 64, p);
        CH_CHECK(Probe::dtor_count == 1,    "sys_delete<Probe,64> dtor");
    }

    // =========================================================================
    // 7. sys_new_array / sys_delete_array
    // =========================================================================
    inline void TestCH_SysNewDeleteArray()
    {
        printf("[TestCH_SysNewDeleteArray]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        constexpr size_t N = 8;
        Probe* arr = SYS_NEW_ARRAY(Probe, N);
        CH_CHECK(arr != nullptr,                        "sys_new_array not null");
        CH_CHECK(is_aligned(arr, alignof(Probe)),       "sys_new_array alignment");
        SYS_DELETE_ARRAY(Probe, arr, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N), "sys_new_array ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "sys_delete_array dtor count");

        // 커스텀 정렬
        reset_probe();
        Probe* arr2 = SYS_NEW_ARRAY_A(Probe, 64, N);
        CH_CHECK(arr2 != nullptr,                       "sys_new_array<Probe,64> not null");
        CH_CHECK(is_aligned(arr2, 64),                  "sys_new_array<Probe,64> alignment");
        SYS_DELETE_ARRAY_A(Probe, 64, arr2, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "sys_delete_array<Probe,64> dtor count");
    }

    // =========================================================================
    // 8. spool_new / spool_delete
    // =========================================================================
    inline void TestCH_SpoolNewDelete()
    {
        printf("[TestCH_SpoolNewDelete]\n");
        using namespace test_coreheap_detail;

        reset_probe();
		Probe* p = SPOOL_NEW(Probe, 300);
        CH_CHECK(p != nullptr,              "SPOOL_NEW<Probe>(300) not null");
        CH_CHECK(p && p->value == 300,      "SPOOL_NEW ctor arg forwarded");
        SPOOL_DELETE(Probe, p);
        CH_CHECK(Probe::ctor_count == 1,    "SPOOL_NEW  ctor count == 1");
        CH_CHECK(Probe::dtor_count == 1,    "SPOOL_DELETE dtor count == 1");
    }

    inline void TestCH_SpoolNewDeleteArray()
    {
        printf("[TestCH_SpoolNewDeleteArray]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        constexpr size_t N = 6;
        Probe* arr = SPOOL_NEW_ARRAY(Probe, N);
        CH_CHECK(arr != nullptr,                            "spool_new_array not null");
        spool_delete_array(arr, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N), "spool_new_array ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "spool_delete_array dtor count");

        // 커스텀 정렬
        reset_probe();
        Probe* arr2 = SPOOL_NEW_ARRAY_A(Probe, 32, N);
        CH_CHECK(arr2 != nullptr,                          "spool_new_array<Probe,32> not null");
        CH_CHECK(is_aligned(arr2, 32),                     "spool_new_array<Probe,32> alignment");
        SPOOL_DELETE_ARRAY_A(Probe, 32, arr2, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "spool_delete_array<Probe,32> dtor count");
    }

    // =========================================================================
    // 9. upool_new / upool_delete
    // =========================================================================
    inline void TestCH_UpoolNewDelete()
    {
        printf("[TestCH_UpoolNewDelete]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        Probe* p = UPOOL_NEW(Probe, 400);
        CH_CHECK(p != nullptr,              "upool_new<Probe>(400) not null");
        CH_CHECK(p && p->value == 400,      "upool_new ctor arg forwarded");
        UPOOL_DELETE(Probe, p);
        CH_CHECK(Probe::ctor_count == 1,    "upool_new  ctor count == 1");
        CH_CHECK(Probe::dtor_count == 1,    "upool_delete dtor count == 1");
    }

    inline void TestCH_UpoolNewDeleteArray()
    {
        printf("[TestCH_UpoolNewDeleteArray]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        constexpr size_t N = 5;
        Probe* arr = UPOOL_NEW_ARRAY(Probe, N);
        CH_CHECK(arr != nullptr,                            "upool_new_array not null");
        UPOOL_DELETE_ARRAY(Probe, arr, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N), "upool_new_array ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "upool_delete_array dtor count");

        // 커스텀 정렬
        reset_probe();
        Probe* arr2 = UPOOL_NEW_ARRAY_A(Probe, 32, N);
        CH_CHECK(arr2 != nullptr,                          "upool_new_array<Probe,32> not null");
        CH_CHECK(is_aligned(arr2, 32),                     "upool_new_array<Probe,32> alignment");
        UPOOL_DELETE_ARRAY_A(Probe, 32, arr2, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "upool_delete_array<Probe,32> dtor count");
    }

    // =========================================================================
    // 10. temp_new / temp_delete
    // =========================================================================
    inline void TestCH_TempNewDelete()
    {
        printf("[TestCH_TempNewDelete]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        Probe* p = TEMP_NEW(Probe, 500);
        CH_CHECK(p != nullptr,              "temp_new<Probe>(500) not null");
        CH_CHECK(p && p->value == 500,      "temp_new ctor arg forwarded");
        TEMP_DELETE(Probe, p);
        CH_CHECK(Probe::ctor_count == 1,    "temp_new  ctor count == 1");
        CH_CHECK(Probe::dtor_count == 1,    "temp_delete dtor count == 1");

        temp_reset();
    }

    inline void TestCH_TempNewDeleteArray()
    {
        printf("[TestCH_TempNewDeleteArray]\n");
        using namespace test_coreheap_detail;

        reset_probe();
        constexpr size_t N = 4;
        Probe* arr = TEMP_NEW_ARRAY(Probe, N);
        CH_CHECK(arr != nullptr,                            "temp_new_array not null");
        TEMP_DELETE_ARRAY(Probe, arr, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N), "temp_new_array ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N), "temp_delete_array dtor count");

        temp_reset();
    }

    // =========================================================================
    // 11. #define 매크로 — SYS / SPOOL / UPOOL / TEMP (단일 객체)
    // =========================================================================
    inline void TestCH_MacroNewDelete()
    {
        printf("[TestCH_MacroNewDelete]\n");
        using namespace test_coreheap_detail;

        // SYS_NEW / SYS_DELETE
        reset_probe();
        Probe* ps = SYS_NEW(Probe, 10);
        CH_CHECK(ps != nullptr,             "SYS_NEW not null");
        CH_CHECK(ps && ps->value == 10,     "SYS_NEW ctor arg");
        SYS_DELETE(Probe, ps);
        CH_CHECK(Probe::dtor_count == 1,    "SYS_DELETE dtor");

        // SYS_NEW_A / SYS_DELETE_A (커스텀 정렬)
        reset_probe();
        Probe* psa = SYS_NEW_A(Probe, 64, 20);
        CH_CHECK(psa != nullptr,            "SYS_NEW_A not null");
        CH_CHECK(is_aligned(psa, 64),       "SYS_NEW_A alignment");
        CH_CHECK(psa && psa->value == 20,   "SYS_NEW_A ctor arg");
        SYS_DELETE_A(Probe, 64, psa);
        CH_CHECK(Probe::dtor_count == 1,    "SYS_DELETE_A dtor");

        // SPOOL_NEW / SPOOL_DELETE
        reset_probe();
        Probe* pp = SPOOL_NEW(Probe, 30);
        CH_CHECK(pp != nullptr,             "SPOOL_NEW not null");
        CH_CHECK(pp && pp->value == 30,     "SPOOL_NEW ctor arg");
        SPOOL_DELETE(Probe, pp);
        CH_CHECK(Probe::dtor_count == 1,    "SPOOL_DELETE dtor");

        // SPOOL_NEW_A / SPOOL_DELETE_A
        reset_probe();
        Probe* ppa = SPOOL_NEW_A(Probe, 32, 31);
        CH_CHECK(ppa != nullptr,            "SPOOL_NEW_A not null");
        CH_CHECK(is_aligned(ppa, 32),       "SPOOL_NEW_A alignment");
        SPOOL_DELETE_A(Probe, 32, ppa);
        CH_CHECK(Probe::dtor_count == 1,    "SPOOL_DELETE_A dtor");

        // UPOOL_NEW / UPOOL_DELETE
        reset_probe();
        Probe* pu = UPOOL_NEW(Probe, 40);
        CH_CHECK(pu != nullptr,             "UPOOL_NEW not null");
        CH_CHECK(pu && pu->value == 40,     "UPOOL_NEW ctor arg");
        UPOOL_DELETE(Probe, pu);
        CH_CHECK(Probe::dtor_count == 1,    "UPOOL_DELETE dtor");

        // UPOOL_NEW_A / UPOOL_DELETE_A
        reset_probe();
        Probe* pua = UPOOL_NEW_A(Probe, 32, 41);
        CH_CHECK(pua != nullptr,            "UPOOL_NEW_A not null");
        CH_CHECK(is_aligned(pua, 32),       "UPOOL_NEW_A alignment");
        UPOOL_DELETE_A(Probe, 32, pua);
        CH_CHECK(Probe::dtor_count == 1,    "UPOOL_DELETE_A dtor");

        // TEMP_NEW / TEMP_DELETE
        reset_probe();
        Probe* pt = TEMP_NEW(Probe, 50);
        CH_CHECK(pt != nullptr,             "TEMP_NEW not null");
        CH_CHECK(pt && pt->value == 50,     "TEMP_NEW ctor arg");
        TEMP_DELETE(Probe, pt);
        CH_CHECK(Probe::dtor_count == 1,    "TEMP_DELETE dtor");

        // TEMP_NEW_A / TEMP_DELETE_A
        reset_probe();
        Probe* pta = TEMP_NEW_A(Probe, 16, 51);
        CH_CHECK(pta != nullptr,            "TEMP_NEW_A not null");
        CH_CHECK(is_aligned(pta, 16),       "TEMP_NEW_A alignment");
        TEMP_DELETE_A(Probe, 16, pta);
        CH_CHECK(Probe::dtor_count == 1,    "TEMP_DELETE_A dtor");

        temp_reset();
    }

    // =========================================================================
    // 12. #define 매크로 — ARRAY 버전
    // =========================================================================
    inline void TestCH_MacroNewDeleteArray()
    {
        printf("[TestCH_MacroNewDeleteArray]\n");
        using namespace test_coreheap_detail;
        constexpr size_t N = 5;

        // SYS_NEW_ARRAY / SYS_DELETE_ARRAY
        reset_probe();
        Probe* ps = SYS_NEW_ARRAY(Probe, N);
        CH_CHECK(ps != nullptr,                                 "SYS_NEW_ARRAY not null");
        SYS_DELETE_ARRAY(Probe, ps, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N),     "SYS_NEW_ARRAY ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "SYS_DELETE_ARRAY dtor count");

        // SYS_NEW_ARRAY_A / SYS_DELETE_ARRAY_A
        reset_probe();
        Probe* psa = SYS_NEW_ARRAY_A(Probe, 64, N);
        CH_CHECK(psa != nullptr,                                "SYS_NEW_ARRAY_A not null");
        CH_CHECK(is_aligned(psa, 64),                           "SYS_NEW_ARRAY_A alignment");
        SYS_DELETE_ARRAY_A(Probe, 64, psa, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "SYS_DELETE_ARRAY_A dtor count");

        // SPOOL_NEW_ARRAY / SPOOL_DELETE_ARRAY
        reset_probe();
        Probe* pp = SPOOL_NEW_ARRAY(Probe, N);
        CH_CHECK(pp != nullptr,                                 "SPOOL_NEW_ARRAY not null");
        SPOOL_DELETE_ARRAY(Probe, pp, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N),     "SPOOL_NEW_ARRAY ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "SPOOL_DELETE_ARRAY dtor count");

        // SPOOL_NEW_ARRAY_A / SPOOL_DELETE_ARRAY_A
        reset_probe();
        Probe* ppa = SPOOL_NEW_ARRAY_A(Probe, 32, N);
        CH_CHECK(ppa != nullptr,                                "SPOOL_NEW_ARRAY_A not null");
        CH_CHECK(is_aligned(ppa, 32),                           "SPOOL_NEW_ARRAY_A alignment");
        SPOOL_DELETE_ARRAY_A(Probe, 32, ppa, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "SPOOL_DELETE_ARRAY_A dtor count");

        // UPOOL_NEW_ARRAY / UPOOL_DELETE_ARRAY
        reset_probe();
        Probe* pu = UPOOL_NEW_ARRAY(Probe, N);
        CH_CHECK(pu != nullptr,                                 "UPOOL_NEW_ARRAY not null");
        UPOOL_DELETE_ARRAY(Probe, pu, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N),     "UPOOL_NEW_ARRAY ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "UPOOL_DELETE_ARRAY dtor count");

        // UPOOL_NEW_ARRAY_A / UPOOL_DELETE_ARRAY_A
        reset_probe();
        Probe* pua = UPOOL_NEW_ARRAY_A(Probe, 32, N);
        CH_CHECK(pua != nullptr,                                "UPOOL_NEW_ARRAY_A not null");
        CH_CHECK(is_aligned(pua, 32),                           "UPOOL_NEW_ARRAY_A alignment");
        UPOOL_DELETE_ARRAY_A(Probe, 32, pua, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "UPOOL_DELETE_ARRAY_A dtor count");

        // TEMP_NEW_ARRAY / TEMP_DELETE_ARRAY
        reset_probe();
        Probe* pt = TEMP_NEW_ARRAY(Probe, N);
        CH_CHECK(pt != nullptr,                                 "TEMP_NEW_ARRAY not null");
        TEMP_DELETE_ARRAY(Probe, pt, N);
        CH_CHECK(Probe::ctor_count == static_cast<int>(N),     "TEMP_NEW_ARRAY ctor count");
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "TEMP_DELETE_ARRAY dtor count");

        // TEMP_NEW_ARRAY_A / TEMP_DELETE_ARRAY_A
        reset_probe();
        Probe* pta = TEMP_NEW_ARRAY_A(Probe, 16, N);
        CH_CHECK(pta != nullptr,                                "TEMP_NEW_ARRAY_A not null");
        CH_CHECK(is_aligned(pta, 16),                           "TEMP_NEW_ARRAY_A alignment");
        TEMP_DELETE_ARRAY_A(Probe, 16, pta, N);
        CH_CHECK(Probe::dtor_count == static_cast<int>(N),     "TEMP_DELETE_ARRAY_A dtor count");

        temp_reset();
    }

    // =========================================================================
    // 13. 멀티스레드 — sys / spool / upool
    // =========================================================================
    inline void TestCH_MultiThreadSys()
    {
        printf("[TestCH_MultiThreadSys]\n");
        constexpr int kThreads = 4;
        constexpr int kIter    = 10000;
        std::atomic<int> failed{ 0 };

        auto worker = [&failed]() {
            for (int i = 0; i < kIter; ++i)
            {
                void* p = sys_alloc(64, 8);
                if (!p) { failed.fetch_add(1); continue; }
                sys_free(p, 64, 8);
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(kThreads);
        for (int i = 0; i < kThreads; ++i) threads.emplace_back(worker);
        for (auto& t : threads) t.join();
        CH_CHECK(failed.load() == 0, "multithread sys_alloc/sys_free");
    }

    inline void TestCH_MultiThreadSpool()
    {
        printf("[TestCH_MultiThreadSpool]\n");
        constexpr int kThreads = 4;
        constexpr int kIter    = 10000;
        std::atomic<int> failed{ 0 };

        auto worker = [&failed]() {
            for (int i = 0; i < kIter; ++i)
            {
                void* p = spool_alloc(64, 8);
                if (!p) { failed.fetch_add(1); continue; }
                spool_free(p, 64, 8);
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(kThreads);
        for (int i = 0; i < kThreads; ++i) threads.emplace_back(worker);
        for (auto& t : threads) t.join();
        CH_CHECK(failed.load() == 0, "multithread spool_alloc/spool_free");
    }

    inline void TestCH_MultiThreadUpool()
    {
        printf("[TestCH_MultiThreadUpool]\n");
        constexpr int kThreads = 4;
        constexpr int kIter    = 10000;
        std::atomic<int> failed{ 0 };

        auto worker = [&failed]() {
            for (int i = 0; i < kIter; ++i)
            {
                void* p = upool_alloc(64, 8);
                if (!p) { failed.fetch_add(1); continue; }
                upool_free(p, 64, 8);
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(kThreads);
        for (int i = 0; i < kThreads; ++i) threads.emplace_back(worker);
        for (auto& t : threads) t.join();
        CH_CHECK(failed.load() == 0, "multithread upool_alloc/upool_free");
    }

    // =========================================================================
    // 14. POOL enum — 각 풀의 memory_resource를 통한 할당 검증
    // =========================================================================
    inline void TestCH_MemoryResourceAllocate()
    {
        printf("[TestCH_MemoryResourceAllocate]\n");
        using namespace test_coreheap_detail;

        auto* sys_res   = get_default_system_memory_resource_ptr();
        auto* spool_res = get_default_spool_memory_resource_ptr();
        auto* upool_res = get_default_usync_pool_memory_resource_ptr();
        auto* temp_res  = get_default_temp_memory_resource_ptr();

        void* p1 = sys_res->allocate(128, 16);
        CH_CHECK(p1 != nullptr,         "sys   memory_resource allocate");
        CH_CHECK(is_aligned(p1, 16),    "sys   memory_resource alignment");
        sys_res->deallocate(p1, 128, 16);

        void* p2 = spool_res->allocate(128, 16);
        CH_CHECK(p2 != nullptr,         "spool memory_resource allocate");
        CH_CHECK(is_aligned(p2, 16),    "spool memory_resource alignment");
        spool_res->deallocate(p2, 128, 16);

        void* p3 = upool_res->allocate(128, 16);
        CH_CHECK(p3 != nullptr,         "upool memory_resource allocate");
        CH_CHECK(is_aligned(p3, 16),    "upool memory_resource alignment");
        upool_res->deallocate(p3, 128, 16);

        void* p4 = temp_res->allocate(128, 16);
        CH_CHECK(p4 != nullptr,         "temp  memory_resource allocate");
        CH_CHECK(is_aligned(p4, 16),    "temp  memory_resource alignment");
        // temp는 deallocate 불필요, reset으로 해제
        temp_reset();
    }

    // =========================================================================
    // Entry point
    // =========================================================================
    inline void Test_CoreHeap()
    {
        using namespace test_coreheap_detail;
        reset_counters();

        printf("========================================\n");
        printf("  CoreHeap Test Suite\n");
        printf("========================================\n\n");

        TestCH_InitShutdown();
        TestCH_SysAllocFree();
        TestCH_SpoolAllocFree();
        TestCH_UpoolAllocFree();
        TestCH_TempAllocReset();
        TestCH_SysAllocFreeWithLoc();
        TestCH_MemoryResourcePtrs();
        TestCH_SysNewDelete();
        TestCH_SysNewDeleteAlign();
        TestCH_SysNewDeleteArray();
        TestCH_SpoolNewDelete();
        TestCH_SpoolNewDeleteArray();
        TestCH_UpoolNewDelete();
        TestCH_UpoolNewDeleteArray();
        TestCH_TempNewDelete();
        TestCH_TempNewDeleteArray();
        TestCH_MacroNewDelete();
        TestCH_MacroNewDeleteArray();
        TestCH_MemoryResourceAllocate();
        TestCH_MultiThreadSys();
        TestCH_MultiThreadSpool();
        //TestCH_MultiThreadUpool();

        shutdown_core_memory();

        print_summary();
    }
}
