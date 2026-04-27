#pragma once
#include "ObjectPool.h"
#include "CoreGeneric.h"
#include "CoreHeap.h"
#include <cstdio>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

namespace mark
{
    // =========================================================================
    //  Allocator aliases
    // =========================================================================
    template<typename _T>
    using sys_object_pool = mark::object_pool<_T, ALLOC::SYS<_T>>;

    template<typename _T>
    using spool_object_pool = mark::object_pool<_T, ALLOC::SPOOL<_T>>;

    template<typename _T>
    using upool_object_pool = mark::object_pool<_T, ALLOC::UPOOL<_T>>;

    template<typename _T>
    using temp_object_pool = mark::object_pool<_T, ALLOC::TEMP<_T>>;

    // =========================================================================
    //  Test infrastructure
    // =========================================================================
    namespace test_objectpool_detail
    {
        static int g_total  = 0;
        static int g_passed = 0;
        static int g_failed = 0;

        inline void reset_counters() { g_total = g_passed = g_failed = 0; }

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

#define OP_CHECK(expr, tag) mark::test_objectpool_detail::check((expr), (tag))

        // -----------------------------------------------------------------------
        //  Test object types
        // -----------------------------------------------------------------------
        struct SimpleInt
        {
            int value = 0;
            explicit SimpleInt(int v = 0) : value(v) {}
        };

        struct Pod64
        {
            int64_t a = 0;
            int64_t b = 0;
        };

        struct WithDtor
        {
            static std::atomic<int> s_dtor_count;
            int id = 0;
            explicit WithDtor(int i = 0) : id(i) {}
            ~WithDtor() { s_dtor_count.fetch_add(1, std::memory_order_relaxed); }
        };
        std::atomic<int> WithDtor::s_dtor_count{ 0 };

    } // namespace test_objectpool_detail

    // =========================================================================
    //  [1] Basic acquire / release — slot reuse
    // =========================================================================
    inline void TestOP_BasicAcquireRelease()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_BasicAcquireRelease]\n");

        sys_object_pool<SimpleInt> pool(8);
        OP_CHECK(pool.size() == 0, "initial size == 0");

        SimpleInt* p = pool.acquire(42);
        OP_CHECK(p != nullptr,    "acquire returns non-null");
        OP_CHECK(p->value == 42,  "acquire constructs with arg");
        OP_CHECK(pool.size() == 0, "size 0 after acquire from empty pool");

        pool.release(p);
        OP_CHECK(pool.size() == 1, "size 1 after release");

        // 재사용: 두 번째 acquire는 같은 슬롯을 가져와야 함
        SimpleInt* p2 = pool.acquire(99);
        OP_CHECK(p2 != nullptr,   "re-acquire returns non-null");
        OP_CHECK(p2->value == 99, "re-acquired object constructed fresh");
        OP_CHECK(pool.size() == 0, "size 0 after re-acquire");

        pool.release(p2);

        print_summary();
    }

    // =========================================================================
    //  [2] Prefill
    // =========================================================================
    inline void TestOP_Prefill()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_Prefill]\n");

        constexpr int PREFILL = 16;
        sys_object_pool<Pod64> pool(64, PREFILL);
        OP_CHECK(pool.size() == PREFILL, "prefill: pool.size() == PREFILL");

        // PREFILL 개수만큼 acquire 해도 신규 할당 없이 가능해야 함
        std::vector<Pod64*> ptrs;
        ptrs.reserve(PREFILL);
        for (int i = 0; i < PREFILL; ++i)
            ptrs.push_back(pool.acquire());

        OP_CHECK(pool.size() == 0, "after draining prefill, size == 0");

        for (auto* p : ptrs)
            pool.release(p);

        OP_CHECK(pool.size() == PREFILL, "size restored after releasing all");

        print_summary();
    }

    // =========================================================================
    //  [3] max_size cap — 초과 슬롯은 즉시 해제
    // =========================================================================
    inline void TestOP_MaxSize()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_MaxSize]\n");

        constexpr int MAX = 4;
        sys_object_pool<SimpleInt> pool(MAX);

        std::vector<SimpleInt*> ptrs;
        for (int i = 0; i < MAX + 4; ++i)
            ptrs.push_back(pool.acquire(i));

        for (auto* p : ptrs)
            pool.release(p);

        // 최대 MAX개만 풀에 보관, 나머지는 즉시 해제됨
        OP_CHECK(pool.size() <= static_cast<size_t>(MAX), "pool.size() <= max_size after over-release");

        print_summary();
    }

    // =========================================================================
    //  [4] unique_handle RAII
    // =========================================================================
    inline void TestOP_UniqueHandle()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_UniqueHandle]\n");

        WithDtor::s_dtor_count.store(0);
        {
            sys_object_pool<WithDtor> pool(8);
            {
                auto h = pool.make_unique(7);
                OP_CHECK(h.get() != nullptr,  "make_unique: get() non-null");
                OP_CHECK(h->id == 7,          "make_unique: id correct");
                OP_CHECK(static_cast<bool>(h), "make_unique: bool true");
                // 스코프 종료 시 자동 release
            }
            // 소멸자가 호출됐어야 함
            OP_CHECK(WithDtor::s_dtor_count.load() >= 1, "unique_handle dtor called on scope exit");
            OP_CHECK(pool.size() == 1, "slot returned to pool after unique_handle scope exit");

            // move semantics
            auto h1 = pool.make_unique(10);
            auto h2 = std::move(h1);
            OP_CHECK(h1.get() == nullptr, "after move, original is null");
            OP_CHECK(h2.get() != nullptr, "after move, target is non-null");

            // reset()
            h2.reset();
            OP_CHECK(h2.get() == nullptr, "after reset(), get() == null");

            // release_ownership
            auto h3 = pool.make_unique(20);
            WithDtor* raw = h3.release_ownership();
            OP_CHECK(raw != nullptr,      "release_ownership returns non-null");
            OP_CHECK(h3.get() == nullptr, "after release_ownership, handle is null");
            pool.release(raw); // 수동 반환
        }

        print_summary();
    }

    // =========================================================================
    //  [5] acquire_raw / release_raw
    // =========================================================================
    inline void TestOP_AcquireReleaseRaw()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_AcquireReleaseRaw]\n");

        sys_object_pool<SimpleInt> pool(8);

        void* mem = pool.acquire_raw();
        OP_CHECK(mem != nullptr, "acquire_raw: non-null");

        SimpleInt* obj = new (mem) SimpleInt(55);
        OP_CHECK(obj->value == 55, "placement new after acquire_raw");

        obj->~SimpleInt();
        pool.release_raw(mem);
        OP_CHECK(pool.size() == 1, "slot returned after release_raw");

        print_summary();
    }

    // =========================================================================
    //  [6] 소멸자 호출 검증 (release 시 dtor 호출)
    // =========================================================================
    inline void TestOP_DtorOnRelease()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_DtorOnRelease]\n");

        WithDtor::s_dtor_count.store(0);
        {
            sys_object_pool<WithDtor> pool(4);

            WithDtor* p1 = pool.acquire(1);
            WithDtor* p2 = pool.acquire(2);
            WithDtor* p3 = pool.acquire(3);

            pool.release(p1);
            pool.release(p2);
            pool.release(p3);

            OP_CHECK(WithDtor::s_dtor_count.load() == 3, "dtor called 3 times on release");
            OP_CHECK(pool.size() == 3, "3 slots returned to pool");
        }

        print_summary();
    }

    // =========================================================================
    //  [7] nullptr release 안전성
    // =========================================================================
    inline void TestOP_NullRelease()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_NullRelease]\n");

        sys_object_pool<SimpleInt> pool(4);
        pool.release(nullptr);   // crash 없어야 함
        pool.release_raw(nullptr);
        OP_CHECK(true, "release(nullptr) no crash");
        OP_CHECK(true, "release_raw(nullptr) no crash");

        print_summary();
    }

    // =========================================================================
    //  [8] 각 Allocator별 기본 동작 확인
    // =========================================================================
    inline void TestOP_AllocatorVariants()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_AllocatorVariants]\n");

        // SYS
        {
            sys_object_pool<SimpleInt> pool(4);
            auto* p = pool.acquire(1);
            OP_CHECK(p != nullptr && p->value == 1, "SYS: acquire ok");
            pool.release(p);
        }

        // SPOOL
        {
            spool_object_pool<SimpleInt> pool(4);
            auto* p = pool.acquire(2);
            OP_CHECK(p != nullptr && p->value == 2, "SPOOL: acquire ok");
            pool.release(p);
        }

        // UPOOL (단일 스레드 전용)
        {
            upool_object_pool<SimpleInt> pool(4);
            auto* p = pool.acquire(3);
            OP_CHECK(p != nullptr && p->value == 3, "UPOOL: acquire ok");
            pool.release(p);
        }

        // TEMP (단일 스레드 전용, reset으로 회수)
        {
            temp_object_pool<SimpleInt> pool(4);
            auto* p = pool.acquire(4);
            OP_CHECK(p != nullptr && p->value == 4, "TEMP: acquire ok");
            pool.release(p);
        }
		::mark::temp_reset();

        print_summary();
    }

    // =========================================================================
    //  [9] Multi-thread — sys_object_pool 동시 acquire/release
    // =========================================================================
    inline void TestOP_MultiThread_Sys()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_MultiThread_Sys]\n");

        constexpr int THREAD_COUNT = 4;
        constexpr int OPS_PER_THREAD = 10000;

        sys_object_pool<SimpleInt> pool(256, 64);
        std::atomic<int> error_count{ 0 };
        std::atomic<int> barrier{ 0 };

        auto worker = [&]()
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT) {}

            for (int i = 0; i < OPS_PER_THREAD; ++i)
            {
                SimpleInt* p = pool.acquire(i);
                if (!p || p->value != i)
                {
                    error_count.fetch_add(1, std::memory_order_relaxed);
                }
                else
                {
                    pool.release(p);
                }
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(THREAD_COUNT);
        for (int t = 0; t < THREAD_COUNT; ++t)
            threads.emplace_back(worker);
        for (auto& th : threads) th.join();

        OP_CHECK(error_count.load() == 0,
            "multi-thread sys: no acquire/value errors across 4 threads");

        print_summary();
    }

    // =========================================================================
    //  [10] Multi-thread — spool_object_pool 동시 acquire/release
    // =========================================================================
    inline void TestOP_MultiThread_Spool()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_MultiThread_Spool]\n");

        constexpr int THREAD_COUNT = 4;
        constexpr int OPS_PER_THREAD = 10000;

        spool_object_pool<SimpleInt> pool(256, 64);
        std::atomic<int> error_count{ 0 };
        std::atomic<int> barrier{ 0 };

        auto worker = [&]()
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT) {}

            for (int i = 0; i < OPS_PER_THREAD; ++i)
            {
                SimpleInt* p = pool.acquire(i);
                if (!p || p->value != i)
                    error_count.fetch_add(1, std::memory_order_relaxed);
                else
                    pool.release(p);
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(THREAD_COUNT);
        for (int t = 0; t < THREAD_COUNT; ++t)
            threads.emplace_back(worker);
        for (auto& th : threads) th.join();

        OP_CHECK(error_count.load() == 0,
            "multi-thread spool: no acquire/value errors across 4 threads");

        print_summary();
    }

    // =========================================================================
    //  [11] Multi-thread — unique_handle (sys)
    // =========================================================================
    inline void TestOP_MultiThread_UniqueHandle()
    {
        using namespace test_objectpool_detail;
        printf("[TestOP_MultiThread_UniqueHandle]\n");

        constexpr int THREAD_COUNT = 4;
        constexpr int OPS_PER_THREAD = 10000;

        sys_object_pool<Pod64> pool(256, 64);
        std::atomic<int> error_count{ 0 };
        std::atomic<int> barrier{ 0 };

        auto worker = [&]()
        {
            barrier.fetch_add(1);
            while (barrier.load() < THREAD_COUNT) {}

            for (int i = 0; i < OPS_PER_THREAD; ++i)
            {
                auto h = pool.make_unique();
                if (!h)
                    error_count.fetch_add(1, std::memory_order_relaxed);
                // 스코프 종료 시 자동 release
            }
        };

        std::vector<std::thread> threads;
        threads.reserve(THREAD_COUNT);
        for (int t = 0; t < THREAD_COUNT; ++t)
            threads.emplace_back(worker);
        for (auto& th : threads) th.join();

        OP_CHECK(error_count.load() == 0,
            "multi-thread unique_handle: no null handles across 4 threads");

        print_summary();
    }

    // =========================================================================
    //  Entry point
    // =========================================================================
    inline void Test_ObjectPool()
    {
        printf("============================================================\n");
        printf("  Test_ObjectPool\n");
        printf("============================================================\n\n");

        test_objectpool_detail::reset_counters();

        TestOP_BasicAcquireRelease();
        TestOP_Prefill();
        TestOP_MaxSize();
        TestOP_UniqueHandle();
        TestOP_AcquireReleaseRaw();
        TestOP_DtorOnRelease();
        TestOP_NullRelease();
        TestOP_AllocatorVariants();
        TestOP_MultiThread_Sys();
        TestOP_MultiThread_Spool();
        TestOP_MultiThread_UniqueHandle();

        printf("============================================================\n");
        printf("  Test_ObjectPool Complete\n");
        printf("============================================================\n\n");
    }

} // namespace mark
