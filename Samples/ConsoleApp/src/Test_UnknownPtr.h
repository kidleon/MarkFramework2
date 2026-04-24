/*
#pragma once
#include "UnknownPtr.h"
#include <cstdio>
#include <cstring>
#include <thread>

namespace mark
{
    namespace test_unknownptr_detail
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

#define UPTR_CHECK(expr, tag) mark::test_unknownptr_detail::check((expr), (tag))

        // Test object that inherits from unknown
        static int g_cleanup_count = 0;
        static int g_delete_count = 0;

        class TestObj : public unknown
        {
        public:
            int value;

            TestObj(int v = 0) : unknown(), value(v) {}

        protected:
            void on_cleanup() noexcept override
            {
                ++g_cleanup_count;
            }

            void on_delete() noexcept override
            {
                ++g_delete_count;
                delete this;
            }
        };

        class DerivedObj : public TestObj
        {
        public:
            int extra;
            DerivedObj(int v, int e) : TestObj(v), extra(e) {}
        };

        inline void reset_obj_counters()
        {
            g_cleanup_count = 0;
            g_delete_count = 0;
        }
    }

    // ========================================================================
    // u_ptr basic
    // ========================================================================
    inline void TestUPtr_Basic()
    {
        printf("[TestUPtr_Basic]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        // default constructor
        u_ptr<TestObj> p1;
        UPTR_CHECK(!p1, "default ctor: null");
        UPTR_CHECK(!p1.is_valid(), "default ctor: is_valid false");
        UPTR_CHECK(p1.get_ptr() == nullptr, "default ctor: get_ptr null");

        // construct from raw pointer
        {
            TestObj* raw = new TestObj(42);
            u_ptr<TestObj> p2(raw);
            UPTR_CHECK(p2.is_valid(), "raw ctor: is_valid true");
            UPTR_CHECK(p2->value == 42, "raw ctor: value == 42");
            UPTR_CHECK((*p2).value == 42, "raw ctor: deref value == 42");
        }
        UPTR_CHECK(g_cleanup_count == 1, "raw ctor: cleanup called on destroy");
        UPTR_CHECK(g_delete_count == 1, "raw ctor: delete called on destroy");
    }

    // ========================================================================
    // u_ptr copy / move
    // ========================================================================
    inline void TestUPtr_CopyMove()
    {
        printf("[TestUPtr_CopyMove]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        TestObj* raw = new TestObj(10);

        {
            u_ptr<TestObj> p1(raw);

            // copy constructor
            u_ptr<TestObj> p2(p1);
            UPTR_CHECK(p2.is_valid(), "copy ctor: valid");
            UPTR_CHECK(p2->value == 10, "copy ctor: value");
            UPTR_CHECK(p1.get_ptr() == p2.get_ptr(), "copy ctor: same ptr");

            // move constructor
            u_ptr<TestObj> p3(std::move(p2));
            UPTR_CHECK(p3.is_valid(), "move ctor: valid");
            UPTR_CHECK(!p2.is_valid(), "move ctor: source null");
            UPTR_CHECK(p3->value == 10, "move ctor: value");
        }
        UPTR_CHECK(g_cleanup_count == 1, "copy/move: single cleanup");
        UPTR_CHECK(g_delete_count == 1, "copy/move: single delete");
    }

    // ========================================================================
    // u_ptr assignment
    // ========================================================================
    inline void TestUPtr_Assignment()
    {
        printf("[TestUPtr_Assignment]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        {
            u_ptr<TestObj> p1(new TestObj(1));
            u_ptr<TestObj> p2(new TestObj(2));

            // copy assignment
            p2 = p1;
            UPTR_CHECK(p2->value == 1, "copy assign: value");
            UPTR_CHECK(p1.get_ptr() == p2.get_ptr(), "copy assign: same ptr");
        }
        // obj(2) cleaned up during assignment, obj(1) cleaned up at scope end
        UPTR_CHECK(g_cleanup_count == 2, "assign: two cleanups");

        reset_obj_counters();
        {
            u_ptr<TestObj> p1(new TestObj(3));
            u_ptr<TestObj> p2;

            // move assignment
            p2 = std::move(p1);
            UPTR_CHECK(p2->value == 3, "move assign: value");
            UPTR_CHECK(!p1.is_valid(), "move assign: source null");
        }
        UPTR_CHECK(g_cleanup_count == 1, "move assign: single cleanup");

        reset_obj_counters();
        {
            TestObj* raw = new TestObj(4);
            u_ptr<TestObj> p1;

            // raw pointer assignment
            p1 = raw;
            UPTR_CHECK(p1->value == 4, "raw assign: value");
        }
        UPTR_CHECK(g_cleanup_count == 1, "raw assign: cleanup");
    }

    // ========================================================================
    // u_ptr reset
    // ========================================================================
    inline void TestUPtr_Reset()
    {
        printf("[TestUPtr_Reset]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        {
            u_ptr<TestObj> p1(new TestObj(99));
            UPTR_CHECK(p1.is_valid(), "before reset: valid");

            p1.reset();
            UPTR_CHECK(!p1.is_valid(), "after reset: null");
            UPTR_CHECK(g_cleanup_count == 1, "reset: cleanup called");
        }

        // reset on null ptr - no crash
        {
            u_ptr<TestObj> p2;
            p2.reset();
            UPTR_CHECK(true, "reset null: no crash");
        }
    }

    // ========================================================================
    // u_ptr comparison operators
    // ========================================================================
    inline void TestUPtr_Comparison()
    {
        printf("[TestUPtr_Comparison]\n");
        using namespace test_unknownptr_detail;

        TestObj* raw = new TestObj(5);
        {
            u_ptr<TestObj> p1(raw);
            u_ptr<TestObj> p2(p1);
            u_ptr<TestObj> p3(new TestObj(6));

            UPTR_CHECK(p1 == p2, "== same ptr");
            UPTR_CHECK(p1 != p3, "!= different ptr");

            u_ptr<TestObj> p4;
            u_ptr<TestObj> p5;
            UPTR_CHECK(p4 == p5, "== both null");
        }
    }

    // ========================================================================
    // u_ptr cast
    // ========================================================================
    inline void TestUPtr_Cast()
    {
        printf("[TestUPtr_Cast]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        {
            u_ptr<DerivedObj> pd(new DerivedObj(10, 20));
            UPTR_CHECK(pd->value == 10, "derived: value");
            UPTR_CHECK(pd->extra == 20, "derived: extra");

            u_ptr<TestObj> pb = pd.cast<TestObj>();
            UPTR_CHECK(pb.is_valid(), "cast: valid");
            UPTR_CHECK(pb->value == 10, "cast: base value");
        }
        UPTR_CHECK(g_cleanup_count == 1, "cast: single cleanup");
    }

    // ========================================================================
    // uweak_ptr basic
    // ========================================================================
    inline void TestUWeakPtr_Basic()
    {
        printf("[TestUWeakPtr_Basic]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        // default constructor
        uweak_ptr<TestObj> w1;
        UPTR_CHECK(!w1, "weak default: expired");
        UPTR_CHECK(w1.expired(), "weak default: expired()");

        {
            u_ptr<TestObj> strong(new TestObj(77));

            uweak_ptr<TestObj> w2(strong);
            UPTR_CHECK(!w2.expired(), "weak from strong: not expired");
            UPTR_CHECK((bool)w2, "weak from strong: bool true");

            // lock
            u_ptr<TestObj> locked = w2.lock();
            UPTR_CHECK(locked.is_valid(), "lock: valid");
            UPTR_CHECK(locked->value == 77, "lock: value");
        }

        // after strong is destroyed, cleanup should have happened
        UPTR_CHECK(g_cleanup_count == 1, "weak: cleanup after strong dies");
    }

    // ========================================================================
    // uweak_ptr expires after strong dies
    // ========================================================================
    inline void TestUWeakPtr_Expiry()
    {
        printf("[TestUWeakPtr_Expiry]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        uweak_ptr<TestObj> w;

        {
            u_ptr<TestObj> strong(new TestObj(88));
            w = strong;
            UPTR_CHECK(!w.expired(), "before strong dies: not expired");
        }

        UPTR_CHECK(w.expired(), "after strong dies: expired");

        u_ptr<TestObj> locked = w.lock();
        UPTR_CHECK(!locked.is_valid(), "lock after expire: null");
    }

    // ========================================================================
    // uweak_ptr copy / move
    // ========================================================================
    inline void TestUWeakPtr_CopyMove()
    {
        printf("[TestUWeakPtr_CopyMove]\n");
        using namespace test_unknownptr_detail;

        u_ptr<TestObj> strong(new TestObj(55));
        uweak_ptr<TestObj> w1(strong);

        // copy
        uweak_ptr<TestObj> w2(w1);
        UPTR_CHECK(!w2.expired(), "weak copy: not expired");
        UPTR_CHECK(w1 == w2, "weak copy: equal");

        // move
        uweak_ptr<TestObj> w3(std::move(w2));
        UPTR_CHECK(!w3.expired(), "weak move: not expired");

        // copy assignment
        uweak_ptr<TestObj> w4;
        w4 = w1;
        UPTR_CHECK(!w4.expired(), "weak copy assign: not expired");

        // move assignment
        uweak_ptr<TestObj> w5;
        w5 = std::move(w4);
        UPTR_CHECK(!w5.expired(), "weak move assign: not expired");
    }

    // ========================================================================
    // uweak_ptr reset
    // ========================================================================
    inline void TestUWeakPtr_Reset()
    {
        printf("[TestUWeakPtr_Reset]\n");
        using namespace test_unknownptr_detail;

        u_ptr<TestObj> strong(new TestObj(33));
        uweak_ptr<TestObj> w(strong);
        UPTR_CHECK(!w.expired(), "before reset: not expired");

        w.reset();
        UPTR_CHECK(w.expired(), "after reset: expired");

        u_ptr<TestObj> locked = w.lock();
        UPTR_CHECK(!locked.is_valid(), "lock after reset: null");

        // reset on empty weak - no crash
        uweak_ptr<TestObj> w2;
        w2.reset();
        UPTR_CHECK(true, "reset empty weak: no crash");
    }

    // ========================================================================
    // uweak_ptr comparison
    // ========================================================================
    inline void TestUWeakPtr_Comparison()
    {
        printf("[TestUWeakPtr_Comparison]\n");
        using namespace test_unknownptr_detail;

        u_ptr<TestObj> s1(new TestObj(1));
        u_ptr<TestObj> s2(new TestObj(2));

        uweak_ptr<TestObj> w1(s1);
        uweak_ptr<TestObj> w2(s1);
        uweak_ptr<TestObj> w3(s2);

        UPTR_CHECK(w1 == w2, "weak ==: same strong");
        UPTR_CHECK(w1 != w3, "weak !=: different strong");
    }

    // ========================================================================
    // Thread safety: multiple threads lock() on weak_ptr
    // ========================================================================
    inline void TestUPtr_ThreadSafety()
    {
        printf("[TestUPtr_ThreadSafety]\n");
        using namespace test_unknownptr_detail;
        reset_obj_counters();

        u_ptr<TestObj> strong(new TestObj(100));
        uweak_ptr<TestObj> weak(strong);

        std::atomic<int> success_count{ 0 };

        auto worker = [&]() {
            for (int i = 0; i < 1000; ++i)
            {
                u_ptr<TestObj> locked = weak.lock();
                if (locked.is_valid())
                    success_count.fetch_add(1, std::memory_order_relaxed);
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

        UPTR_CHECK(success_count.load() == 4000, "thread safety: all locks succeeded");
        UPTR_CHECK(strong->value == 100, "thread safety: value intact");
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_UnknownPtr()
    {
        using namespace test_unknownptr_detail;
        reset_counters();

        printf("========================================\n");
        printf("  UnknownPtr Test Suite\n");
        printf("========================================\n");

        TestUPtr_Basic();
        TestUPtr_CopyMove();
        TestUPtr_Assignment();
        TestUPtr_Reset();
        TestUPtr_Comparison();
        TestUPtr_Cast();
        TestUWeakPtr_Basic();
        TestUWeakPtr_Expiry();
        TestUWeakPtr_CopyMove();
        TestUWeakPtr_Reset();
        TestUWeakPtr_Comparison();
        TestUPtr_ThreadSafety();

        print_summary();
    }

} // namespace mark

*/
