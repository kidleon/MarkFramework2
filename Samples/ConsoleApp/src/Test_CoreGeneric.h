#pragma once

#include "CoreGeneric.h"
#include "CoreHeap.h"
#include <cstdio>
#include <cstring>

namespace mark
{
    namespace test_coregeneric_detail
    {
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

#define CG_CHECK(expr, tag) mark::test_coregeneric_detail::check((expr), (tag))
    }

    // =========================================================================
    // 1. memory_resource 포인터 유효성
    // =========================================================================
    inline void TestCG_MemoryResourcePtrs()
    {
        printf("[TestCG_MemoryResourcePtrs]\n");

        CG_CHECK(get_core_system_memory_resource_ptr()    != nullptr, "get_core_system_memory_resource_ptr not null");
        CG_CHECK(get_core_sync_pool_memory_resource_ptr() != nullptr, "get_core_sync_pool_memory_resource_ptr not null");
        CG_CHECK(get_core_usync_pool_memory_resource_ptr()!= nullptr, "get_core_usync_pool_memory_resource_ptr not null");
        CG_CHECK(get_core_temp_memory_resource_ptr()      != nullptr, "get_core_temp_memory_resource_ptr not null");
    }

    // =========================================================================
    // 2. ALLOC 커스텀 얼로케이터 — allocate / deallocate
    // =========================================================================
    inline void TestCG_AllocatorSys()
    {
        printf("[TestCG_AllocatorSys]\n");

        ALLOC::SYS<int> alloc;
        int* p = alloc.allocate(4);
        CG_CHECK(p != nullptr, "ALLOC::SYS<int> allocate 4");
        p[0] = 10; p[1] = 20; p[2] = 30; p[3] = 40;
        CG_CHECK(p[0] == 10 && p[3] == 40, "ALLOC::SYS<int> write/read");
        alloc.deallocate(p, 4);
        CG_CHECK(true, "ALLOC::SYS<int> deallocate no crash");
    }

    inline void TestCG_AllocatorSpool()
    {
        printf("[TestCG_AllocatorSpool]\n");

        ALLOC::SPOOL<double> alloc;
        double* p = alloc.allocate(2);
        CG_CHECK(p != nullptr, "ALLOC::SPOOL<double> allocate 2");
        p[0] = 1.1; p[1] = 2.2;
        CG_CHECK(p[0] == 1.1 && p[1] == 2.2, "ALLOC::SPOOL<double> write/read");
        alloc.deallocate(p, 2);
        CG_CHECK(true, "ALLOC::SPOOL<double> deallocate no crash");
    }

    inline void TestCG_AllocatorUpool()
    {
        printf("[TestCG_AllocatorUpool]\n");

        ALLOC::UPOOL<char> alloc;
        char* p = alloc.allocate(8);
        CG_CHECK(p != nullptr, "ALLOC::UPOOL<char> allocate 8");
        std::memcpy(p, "hello", 6);
        CG_CHECK(std::strcmp(p, "hello") == 0, "ALLOC::UPOOL<char> write/read");
        alloc.deallocate(p, 8);
        CG_CHECK(true, "ALLOC::UPOOL<char> deallocate no crash");
    }

    inline void TestCG_AllocatorTemp()
    {
        printf("[TestCG_AllocatorTemp]\n");

        ALLOC::TEMP<int> alloc;
        int* p = alloc.allocate(3);
        CG_CHECK(p != nullptr, "ALLOC::TEMP<int> allocate 3");
        p[0] = 7; p[1] = 8; p[2] = 9;
        CG_CHECK(p[0] == 7 && p[2] == 9, "ALLOC::TEMP<int> write/read");
        // TEMP는 reset으로 해제
        temp_reset();
        CG_CHECK(true, "ALLOC::TEMP<int> temp_reset no crash");
    }

    // =========================================================================
    // 3. vector
    // =========================================================================
    inline void TestCG_Vector()
    {
        printf("[TestCG_Vector]\n");

        sys_vector<int> sv;
        sv.push_back(1); sv.push_back(2); sv.push_back(3);
        CG_CHECK(sv.size() == 3,           "sys_vector push_back size");
        CG_CHECK(sv[0] == 1 && sv[2] == 3, "sys_vector element access");

        spool_vector<int> pv;
        pv.push_back(10); pv.push_back(20);
        CG_CHECK(pv.size() == 2,            "spool_vector push_back size");
        CG_CHECK(pv[0] == 10 && pv[1] == 20,"spool_vector element access");

        upool_vector<int> uv;
        uv.push_back(100); uv.push_back(200);
        CG_CHECK(uv.size() == 2,              "upool_vector push_back size");
        CG_CHECK(uv[0] == 100 && uv[1] == 200,"upool_vector element access");

        temp_vector<int> tv;
        tv.push_back(7); tv.push_back(8); tv.push_back(9);
        CG_CHECK(tv.size() == 3,           "temp_vector push_back size");
        CG_CHECK(tv[1] == 8,               "temp_vector element access");
        tv.clear();
        temp_reset();
    }

    inline void TestCG_VectorReserveErase()
    {
        printf("[TestCG_VectorReserveErase]\n");

        sys_vector<int> v;
        v.reserve(10);
        CG_CHECK(v.capacity() >= 10, "sys_vector reserve capacity");
        for (int i = 0; i < 5; ++i) v.push_back(i);
        v.erase(v.begin());
        CG_CHECK(v.size() == 4,  "sys_vector erase size");
        CG_CHECK(v[0] == 1,      "sys_vector erase front value");

        // 커스텀 얼로케이터(generic vector)
        mark::vector<int, ALLOC::SYS<int>> gv;
        gv.push_back(42);
        CG_CHECK(gv.size() == 1 && gv[0] == 42, "mark::vector<int,SYS> push_back");
    }

    // =========================================================================
    // 4. list
    // =========================================================================
    inline void TestCG_List()
    {
        printf("[TestCG_List]\n");

        sys_list<int> sl;
        sl.push_back(1); sl.push_front(0);
        CG_CHECK(sl.size() == 2,    "sys_list push front/back size");
        CG_CHECK(sl.front() == 0,   "sys_list front");
        CG_CHECK(sl.back()  == 1,   "sys_list back");

        spool_list<int> pl;
        pl.push_back(10); pl.push_back(20); pl.push_back(30);
        CG_CHECK(pl.size() == 3,    "spool_list size");
        pl.pop_front();
        CG_CHECK(pl.front() == 20,  "spool_list pop_front");

        upool_list<int> ul;
        ul.push_back(100);
        CG_CHECK(ul.size() == 1 && ul.front() == 100, "upool_list push_back");

        temp_list<int> tl;
        tl.push_back(7); tl.push_back(8);
        CG_CHECK(tl.size() == 2 && tl.back() == 8, "temp_list push_back");
        tl.clear();
        temp_reset();

        // 커스텀 얼로케이터(generic list)
        mark::list<int, ALLOC::SYS<int>> gl;
        gl.push_back(99);
        CG_CHECK(gl.size() == 1 && gl.front() == 99, "mark::list<int,SYS> push_back");
    }

    // =========================================================================
    // 5. deque
    // =========================================================================
    inline void TestCG_Deque()
    {
        printf("[TestCG_Deque]\n");

        sys_deque<int> sd;
        sd.push_back(1); sd.push_front(0); sd.push_back(2);
        CG_CHECK(sd.size() == 3,                    "sys_deque push size");
        CG_CHECK(sd.front() == 0 && sd.back() == 2, "sys_deque front/back");
        sd.pop_front();
        CG_CHECK(sd.front() == 1,                   "sys_deque pop_front");

        spool_deque<int> pd;
        pd.push_back(10); pd.push_back(20);
        CG_CHECK(pd.size() == 2 && pd[1] == 20, "spool_deque push/access");

        upool_deque<int> ud;
        ud.push_back(100); ud.push_back(200);
        CG_CHECK(ud.size() == 2 && ud[0] == 100, "upool_deque push/access");

        temp_deque<int> td;
        td.push_back(7); td.push_back(8);
        CG_CHECK(td.size() == 2 && td[0] == 7,  "temp_deque push/access");
        td.clear();
        temp_reset();

        // 커스텀 얼로케이터(generic deque)
        mark::deque<int, ALLOC::SYS<int>> gd;
        gd.push_back(55);
        CG_CHECK(gd.size() == 1 && gd[0] == 55, "mark::deque<int,SYS> push_back");
    }

    // =========================================================================
    // 6. unordered_map
    // =========================================================================
    inline void TestCG_UnorderedMap()
    {
        printf("[TestCG_UnorderedMap]\n");

        sys_unordered_map<int, int> sm;
        sm[1] = 10; sm[2] = 20; sm[3] = 30;
        CG_CHECK(sm.size() == 3,         "sys_unordered_map insert size");
        CG_CHECK(sm.count(2) == 1,       "sys_unordered_map count");
        CG_CHECK(sm.at(2) == 20,         "sys_unordered_map at");
        sm.erase(2);
        CG_CHECK(sm.count(2) == 0,       "sys_unordered_map erase");

        spool_unordered_map<int, int> pm;
        pm[10] = 100; pm[20] = 200;
        CG_CHECK(pm.size() == 2 && pm[10] == 100, "spool_unordered_map insert/access");

        upool_unordered_map<int, int> um;
        um[1] = 1; um[2] = 2;
        CG_CHECK(um.size() == 2 && um[2] == 2, "upool_unordered_map insert/access");

        temp_unordered_map<int, int> tm;
        tm[5] = 50; tm[6] = 60;
        CG_CHECK(tm.size() == 2 && tm[6] == 60, "temp_unordered_map insert/access");
        tm.clear();
        temp_reset();

        // 커스텀 얼로케이터(generic unordered_map)
        mark::unordered_map<int, int, ALLOC::SYS<std::pair<const int, int>>> gm;
        gm[7] = 77;
        CG_CHECK(gm.size() == 1 && gm[7] == 77, "mark::unordered_map<int,int,SYS> insert");
    }

    // =========================================================================
    // 7. unordered_set
    // =========================================================================
    inline void TestCG_UnorderedSet()
    {
        printf("[TestCG_UnorderedSet]\n");

        sys_unordered_set<int> ss;
        ss.insert(1); ss.insert(2); ss.insert(3); ss.insert(2);
        CG_CHECK(ss.size() == 3,       "sys_unordered_set insert unique size");
        CG_CHECK(ss.count(2) == 1,     "sys_unordered_set count");
        ss.erase(2);
        CG_CHECK(ss.count(2) == 0,     "sys_unordered_set erase");

        spool_unordered_set<int> ps;
        ps.insert(10); ps.insert(20);
        CG_CHECK(ps.size() == 2 && ps.count(10) == 1, "spool_unordered_set insert/count");

        upool_unordered_set<int> us;
        us.insert(100); us.insert(200);
        CG_CHECK(us.size() == 2 && us.count(200) == 1, "upool_unordered_set insert/count");

        temp_unordered_set<int> ts;
        ts.insert(7); ts.insert(8);
        CG_CHECK(ts.size() == 2 && ts.count(8) == 1, "temp_unordered_set insert/count");
        ts.clear();
        temp_reset();

        // 커스텀 얼로케이터(generic unordered_set)
        mark::unordered_set<int, ALLOC::SYS<int>> gs;
        gs.insert(42);
        CG_CHECK(gs.size() == 1 && gs.count(42) == 1, "mark::unordered_set<int,SYS> insert");
    }

    // =========================================================================
    // 8. unordered_multimap
    // =========================================================================
    inline void TestCG_UnorderedMultimap()
    {
        printf("[TestCG_UnorderedMultimap]\n");

        sys_unordered_multimap<int, int> smm;
        smm.insert({1, 10}); smm.insert({1, 20}); smm.insert({2, 30});
        CG_CHECK(smm.size() == 3,      "sys_unordered_multimap insert size");
        CG_CHECK(smm.count(1) == 2,    "sys_unordered_multimap count duplicates");
        smm.erase(1);
        CG_CHECK(smm.count(1) == 0,    "sys_unordered_multimap erase");

        spool_unordered_multimap<int, int> pmm;
        pmm.insert({5, 50}); pmm.insert({5, 55});
        CG_CHECK(pmm.count(5) == 2, "spool_unordered_multimap duplicate count");

        upool_unordered_multimap<int, int> umm;
        umm.insert({3, 33}); umm.insert({4, 44});
        CG_CHECK(umm.size() == 2, "upool_unordered_multimap size");

        temp_unordered_multimap<int, int> tmm;
        tmm.insert({7, 70}); tmm.insert({7, 77});
        CG_CHECK(tmm.count(7) == 2, "temp_unordered_multimap duplicate count");
        tmm.clear();
        temp_reset();

        // 커스텀 얼로케이터(generic unordered_multimap)
        mark::unordered_multimap<int, int, ALLOC::SYS<std::pair<const int, int>>> gmm;
        gmm.insert({9, 99}); gmm.insert({9, 98});
        CG_CHECK(gmm.count(9) == 2, "mark::unordered_multimap<int,int,SYS> duplicate count");
    }

    // =========================================================================
    // 9. unordered_multiset
    // =========================================================================
    inline void TestCG_UnorderedMultiset()
    {
        printf("[TestCG_UnorderedMultiset]\n");

        sys_unordered_multiset<int> sms;
        sms.insert(1); sms.insert(1); sms.insert(2);
        CG_CHECK(sms.size() == 3,     "sys_unordered_multiset insert size");
        CG_CHECK(sms.count(1) == 2,   "sys_unordered_multiset duplicate count");
        sms.erase(1);
        CG_CHECK(sms.count(1) == 0,   "sys_unordered_multiset erase all");

        spool_unordered_multiset<int> pms;
        pms.insert(5); pms.insert(5);
        CG_CHECK(pms.count(5) == 2, "spool_unordered_multiset duplicate");

        upool_unordered_multiset<int> ums;
        ums.insert(3); ums.insert(4); ums.insert(3);
        CG_CHECK(ums.count(3) == 2, "upool_unordered_multiset duplicate");

        temp_unordered_multiset<int> tms;
        tms.insert(7); tms.insert(7);
        CG_CHECK(tms.count(7) == 2, "temp_unordered_multiset duplicate");
        tms.clear();
        temp_reset();

        // 커스텀 얼로케이터(generic unordered_multiset)
        mark::unordered_multiset<int, ALLOC::SYS<int>> gms;
        gms.insert(9); gms.insert(9);
        CG_CHECK(gms.count(9) == 2, "mark::unordered_multiset<int,SYS> duplicate");
    }

    // =========================================================================
    // 10. string (char)
    // =========================================================================
    inline void TestCG_String()
    {
        printf("[TestCG_String]\n");

        sys_string ss = "hello";
        ss += " world";
        CG_CHECK(ss == "hello world",    "sys_string concat");
        CG_CHECK(ss.size() == 11,        "sys_string size");
        CG_CHECK(ss.find("world") != sys_string::npos, "sys_string find");

        spool_string ps = "spool";
        ps.append("_test");
        CG_CHECK(ps == "spool_test",     "spool_string append");

        upool_string us = "upool";
        us += "_ok";
        CG_CHECK(us == "upool_ok",       "upool_string concat");

        temp_string ts = "temp";
        ts += "_str";
        CG_CHECK(ts == "temp_str",       "temp_string concat");

        // 커스텀 얼로케이터(generic string)
        mark::string<ALLOC::SYS<char>> gs = "generic";
        CG_CHECK(gs == "generic",        "mark::string<SYS> assign");
    }

    // =========================================================================
    // 11. wstring
    // =========================================================================
    inline void TestCG_WString()
    {
        printf("[TestCG_WString]\n");

        sys_wstring sw = L"hello";
        sw += L" world";
        CG_CHECK(sw == L"hello world",   "sys_wstring concat");
        CG_CHECK(sw.size() == 11,        "sys_wstring size");

        spool_wstring pw = L"spool";
        pw.append(L"_test");
        CG_CHECK(pw == L"spool_test",    "spool_wstring append");

        upool_wstring uw = L"upool";
        uw += L"_ok";
        CG_CHECK(uw == L"upool_ok",      "upool_wstring concat");

        temp_wstring tw = L"temp";
        tw += L"_str";
        CG_CHECK(tw == L"temp_str",      "temp_wstring concat");

        // 커스텀 얼로케이터(generic wstring)
        mark::wstring<ALLOC::SYS<wchar_t>> gw = L"generic";
        CG_CHECK(gw == L"generic",       "mark::wstring<SYS> assign");
    }

    // =========================================================================
    // 12. u16string
    // =========================================================================
    inline void TestCG_U16String()
    {
        printf("[TestCG_U16String]\n");

        sys_u16string s16 = u"hello";
        s16 += u"_sys";
        CG_CHECK(s16 == u"hello_sys",    "sys_u16string concat");
        CG_CHECK(s16.size() == 9,        "sys_u16string size");

        spool_u16string p16 = u"spool";
        p16.append(u"_test");
        CG_CHECK(p16 == u"spool_test",   "spool_u16string append");

        upool_u16string u16 = u"upool";
        u16 += u"_ok";
        CG_CHECK(u16 == u"upool_ok",     "upool_u16string concat");

        temp_u16string t16 = u"temp";
        t16 += u"_str";
        CG_CHECK(t16 == u"temp_str",     "temp_u16string concat");

        // 커스텀 얼로케이터(generic u16string)
        mark::u16string<ALLOC::SYS<char16_t>> g16 = u"generic";
        CG_CHECK(g16 == u"generic",      "mark::u16string<SYS> assign");
    }

    // =========================================================================
    // 13. u32string
    // =========================================================================
    inline void TestCG_U32String()
    {
        printf("[TestCG_U32String]\n");

        sys_u32string s32 = U"hello";
        s32 += U"_sys";
        CG_CHECK(s32 == U"hello_sys",    "sys_u32string concat");
        CG_CHECK(s32.size() == 9,        "sys_u32string size");

        spool_u32string p32 = U"spool";
        p32.append(U"_test");
        CG_CHECK(p32 == U"spool_test",   "spool_u32string append");

        upool_u32string u32 = U"upool";
        u32 += U"_ok";
        CG_CHECK(u32 == U"upool_ok",     "upool_u32string concat");

        temp_u32string t32 = U"temp";
        t32 += U"_str";
        CG_CHECK(t32 == U"temp_str",     "temp_u32string concat");

        // 커스텀 얼로케이터(generic u32string)
        mark::u32string<ALLOC::SYS<char32_t>> g32 = U"generic";
        CG_CHECK(g32 == U"generic",      "mark::u32string<SYS> assign");
    }

    // =========================================================================
    // 14. 컨테이너 이동(move) 시맨틱
    // =========================================================================
    inline void TestCG_MoveSemantics()
    {
        printf("[TestCG_MoveSemantics]\n");

        sys_vector<int> src;
        src.push_back(1); src.push_back(2); src.push_back(3);
        sys_vector<int> dst = std::move(src);
        CG_CHECK(dst.size() == 3,   "sys_vector move size");
        CG_CHECK(src.empty(),       "sys_vector moved-from empty");

        sys_string ss = "move_test";
        sys_string ds = std::move(ss);
        CG_CHECK(ds == "move_test", "sys_string move value");
        CG_CHECK(ss.empty(),        "sys_string moved-from empty");
    }

    // =========================================================================
    // 15. 컨테이너 swap
    // =========================================================================
    inline void TestCG_Swap()
    {
        printf("[TestCG_Swap]\n");

        sys_vector<int> a, b;
        a.push_back(1); b.push_back(2); b.push_back(3);
        a.swap(b);
        CG_CHECK(a.size() == 2 && a[0] == 2, "sys_vector swap a");
        CG_CHECK(b.size() == 1 && b[0] == 1, "sys_vector swap b");

        sys_string sa = "aaa", sb = "bbb";
        sa.swap(sb);
        CG_CHECK(sa == "bbb", "sys_string swap a");
        CG_CHECK(sb == "aaa", "sys_string swap b");
    }

    // =========================================================================
    // Entry point
    // =========================================================================
    inline void Test_CoreGeneric()
    {
        using namespace test_coregeneric_detail;
        reset_counters();

        // 이 테스트 스위트는 core memory가 초기화된 상태를 가정한다.
        // 미초기화 상태라면 여기서 초기화한다.
        initialize_core_memory(256, 256, 1024, 1024, 1024 * 1024 * 4);

        printf("========================================\n");
        printf("  CoreGeneric Test Suite\n");
        printf("========================================\n\n");

        TestCG_MemoryResourcePtrs();
        TestCG_AllocatorSys();
        TestCG_AllocatorSpool();
        TestCG_AllocatorUpool();
        TestCG_AllocatorTemp();
        TestCG_Vector();
        TestCG_VectorReserveErase();
        TestCG_List();
        TestCG_Deque();
        TestCG_UnorderedMap();
        TestCG_UnorderedSet();
        TestCG_UnorderedMultimap();
        TestCG_UnorderedMultiset();
        TestCG_String();
        TestCG_WString();
        TestCG_U16String();
        TestCG_U32String();
        TestCG_MoveSemantics();
        TestCG_Swap();

        print_summary();
    }
}
