#pragma once

#include "CoreGeneric.h"
#include "CoreHeap.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <atomic>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <unordered_map>
#include <unordered_set>

namespace mark
{
    namespace benchmark_coregeneric_detail
    {
        using Clock = std::chrono::high_resolution_clock;

        inline double elapsed_ms(Clock::time_point s, Clock::time_point e)
        {
            return std::chrono::duration<double, std::milli>(e - s).count();
        }

        // -----------------------------------------------------------------------
        // 출력 헬퍼
        // -----------------------------------------------------------------------
        inline void print_header(const char* title)
        {
            printf("------------------------------------------------------------\n");
            printf("  %s\n", title);
            printf("  %-12s | %10s ms | %10s ms | %10s ms | %10s ms\n",
                "ops", "std", "sys", "spool", "upool");
            printf("------------------------------------------------------------\n");
        }

        inline void print_row(const char* op,
            double std_ms, double sys_ms, double spool_ms, double upool_ms)
        {
            printf("  %-12s | %10.3f    | %10.3f    | %10.3f    | %10.3f\n",
                op, std_ms, sys_ms, spool_ms, upool_ms);

            auto ratio_str = [](double base, double target) -> double {
                return (base > 0.0) ? (target / base) : 0.0;
            };
            printf("  %-12s | %10s    | %10.3fx   | %10.3fx   | %10.3fx\n",
                "(ratio)", "base",
                ratio_str(std_ms, sys_ms),
                ratio_str(std_ms, spool_ms),
                ratio_str(std_ms, upool_ms));
        }

        inline void print_footer() { printf("\n"); }

        static constexpr int ITER      = 100'000;   // 기본 반복 횟수
        static constexpr int STR_ITER  =  50'000;   // 문자열은 상대적으로 무거우므로 절반

        // -----------------------------------------------------------------------
        // vector — push_back
        // -----------------------------------------------------------------------
        template<typename Vec>
        inline double bench_vector_push(int iters)
        {
            auto t = Clock::now();
            Vec v;
            v.reserve(iters);
            for (int i = 0; i < iters; ++i)
                v.push_back(i);
            return elapsed_ms(t, Clock::now());
        }

        // vector — sequential read
        template<typename Vec>
        inline double bench_vector_read(int iters)
        {
            Vec v;
            v.reserve(iters);
            for (int i = 0; i < iters; ++i) v.push_back(i);

            volatile long long sum = 0;
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
                sum += v[i];
            (void)sum;
            return elapsed_ms(t, Clock::now());
        }

        // vector — push_back + clear (할당/해제 반복)
        template<typename Vec>
        inline double bench_vector_alloc_free(int iters)
        {
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                Vec v;
                v.push_back(i);
                v.push_back(i + 1);
            }
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        // list — push_back
        // -----------------------------------------------------------------------
        template<typename List>
        inline double bench_list_push(int iters)
        {
            auto t = Clock::now();
            List l;
            for (int i = 0; i < iters; ++i)
                l.push_back(i);
            return elapsed_ms(t, Clock::now());
        }

        // list — iterate
        template<typename List>
        inline double bench_list_iter(int iters)
        {
            List l;
            for (int i = 0; i < iters; ++i) l.push_back(i);

            volatile long long sum = 0;
            auto t = Clock::now();
            for (auto& v : l) sum += v;
            (void)sum;
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        // deque — push_back
        // -----------------------------------------------------------------------
        template<typename Deque>
        inline double bench_deque_push(int iters)
        {
            auto t = Clock::now();
            Deque d;
            for (int i = 0; i < iters; ++i)
                d.push_back(i);
            return elapsed_ms(t, Clock::now());
        }

        // deque — mixed push_front / push_back
        template<typename Deque>
        inline double bench_deque_mixed(int iters)
        {
            auto t = Clock::now();
            Deque d;
            for (int i = 0; i < iters; ++i)
            {
                if (i & 1) d.push_back(i);
                else       d.push_front(i);
            }
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        // unordered_map — insert / find / erase
        // -----------------------------------------------------------------------
        template<typename Map>
        inline double bench_map_insert(int iters)
        {
            auto t = Clock::now();
            Map m;
            m.reserve(iters);
            for (int i = 0; i < iters; ++i)
                m.emplace(i, i * 2);
            return elapsed_ms(t, Clock::now());
        }

        template<typename Map>
        inline double bench_map_find(int iters)
        {
            Map m;
            m.reserve(iters);
            for (int i = 0; i < iters; ++i) m.emplace(i, i * 2);

            volatile long long sum = 0;
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                auto it = m.find(i);
                if (it != m.end()) sum += it->second;
            }
            (void)sum;
            return elapsed_ms(t, Clock::now());
        }

        template<typename Map>
        inline double bench_map_erase(int iters)
        {
            Map m;
            m.reserve(iters);
            for (int i = 0; i < iters; ++i) m.emplace(i, i);

            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
                m.erase(i);
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        // unordered_set — insert / count
        // -----------------------------------------------------------------------
        template<typename Set>
        inline double bench_set_insert(int iters)
        {
            auto t = Clock::now();
            Set s;
            s.reserve(iters);
            for (int i = 0; i < iters; ++i)
                s.insert(i);
            return elapsed_ms(t, Clock::now());
        }

        template<typename Set>
        inline double bench_set_count(int iters)
        {
            Set s;
            s.reserve(iters);
            for (int i = 0; i < iters; ++i) s.insert(i);

            volatile size_t cnt = 0;
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
                cnt += s.count(i);
            (void)cnt;
            return elapsed_ms(t, Clock::now());
        }

        // -----------------------------------------------------------------------
        // string — append
        // -----------------------------------------------------------------------
        template<typename Str>
        inline double bench_string_append(int iters)
        {
            using C = typename Str::value_type;
            // 문자 단위 append — char/wchar_t/char16_t/char32_t 모두 호환
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
            {
                Str s;
                for (int j = 0; j < 18; ++j)
                    s += static_cast<C>('a' + (j % 26));
            }
            return elapsed_ms(t, Clock::now());
        }

        template<typename Str>
        inline double bench_string_find(int iters)
        {
            Str base;
            for (int i = 0; i < 64; ++i)
                base += typename Str::value_type('a' + (i % 26));

            volatile size_t pos = 0;
            auto t = Clock::now();
            for (int i = 0; i < iters; ++i)
                pos = base.find(typename Str::value_type('z'));
            (void)pos;
            return elapsed_ms(t, Clock::now());
        }

    } // namespace benchmark_coregeneric_detail


    // =========================================================================
    // Entry point
    // =========================================================================
    inline void Benchmark_CoreGeneric()
    {
        using namespace benchmark_coregeneric_detail;

        initialize_core_memory(512, 512, 4096, 4096, 1024 * 1024 * 64);

        printf("============================================================\n");
        printf("  CoreGeneric Benchmark  —  STL vs mark allocators\n");
        printf("  iterations = %d  (string = %d)\n", ITER, STR_ITER);
        printf("  ratio: target_ms / std_ms  (1.0x = same, <1.0x = faster)\n");
        printf("============================================================\n\n");

        // -----------------------------------------------------------------
        // vector<int>
        // -----------------------------------------------------------------
        using std_vec  = std::vector<int>;
        using sys_vec  = mark::sys_vector<int>;
        using sp_vec   = mark::spool_vector<int>;
        using up_vec   = mark::upool_vector<int>;

        print_header("vector<int>");
        print_row("push_back",
            bench_vector_push<std_vec>(ITER),
            bench_vector_push<sys_vec>(ITER),
            bench_vector_push<sp_vec>(ITER),
            bench_vector_push<up_vec>(ITER));
        print_row("seq read",
            bench_vector_read<std_vec>(ITER),
            bench_vector_read<sys_vec>(ITER),
            bench_vector_read<sp_vec>(ITER),
            bench_vector_read<up_vec>(ITER));
        print_row("alloc+free",
            bench_vector_alloc_free<std_vec>(ITER),
            bench_vector_alloc_free<sys_vec>(ITER),
            bench_vector_alloc_free<sp_vec>(ITER),
            bench_vector_alloc_free<up_vec>(ITER));
        print_footer();

        // -----------------------------------------------------------------
        // list<int>
        // -----------------------------------------------------------------
        using std_list = std::list<int>;
        using sys_lst  = mark::sys_list<int>;
        using sp_lst   = mark::spool_list<int>;
        using up_lst   = mark::upool_list<int>;

        print_header("list<int>");
        print_row("push_back",
            bench_list_push<std_list>(ITER),
            bench_list_push<sys_lst>(ITER),
            bench_list_push<sp_lst>(ITER),
            bench_list_push<up_lst>(ITER));
        print_row("iterate",
            bench_list_iter<std_list>(ITER),
            bench_list_iter<sys_lst>(ITER),
            bench_list_iter<sp_lst>(ITER),
            bench_list_iter<up_lst>(ITER));
        print_footer();

        // -----------------------------------------------------------------
        // deque<int>
        // -----------------------------------------------------------------
        using std_dq = std::deque<int>;
        using sys_dq = mark::sys_deque<int>;
        using sp_dq  = mark::spool_deque<int>;
        using up_dq  = mark::upool_deque<int>;

        print_header("deque<int>");
        print_row("push_back",
            bench_deque_push<std_dq>(ITER),
            bench_deque_push<sys_dq>(ITER),
            bench_deque_push<sp_dq>(ITER),
            bench_deque_push<up_dq>(ITER));
        print_row("push mix",
            bench_deque_mixed<std_dq>(ITER),
            bench_deque_mixed<sys_dq>(ITER),
            bench_deque_mixed<sp_dq>(ITER),
            bench_deque_mixed<up_dq>(ITER));
        print_footer();

        // -----------------------------------------------------------------
        // unordered_map<int,int>
        // -----------------------------------------------------------------
        using std_map = std::unordered_map<int, int>;
        using sys_map = mark::sys_unordered_map<int, int>;
        using sp_map  = mark::spool_unordered_map<int, int>;
        using up_map  = mark::upool_unordered_map<int, int>;

        print_header("unordered_map<int,int>");
        print_row("insert",
            bench_map_insert<std_map>(ITER),
            bench_map_insert<sys_map>(ITER),
            bench_map_insert<sp_map>(ITER),
            bench_map_insert<up_map>(ITER));
        print_row("find",
            bench_map_find<std_map>(ITER),
            bench_map_find<sys_map>(ITER),
            bench_map_find<sp_map>(ITER),
            bench_map_find<up_map>(ITER));
        print_row("erase",
            bench_map_erase<std_map>(ITER),
            bench_map_erase<sys_map>(ITER),
            bench_map_erase<sp_map>(ITER),
            bench_map_erase<up_map>(ITER));
        print_footer();

        // -----------------------------------------------------------------
        // unordered_set<int>
        // -----------------------------------------------------------------
        using std_set = std::unordered_set<int>;
        using sys_set = mark::sys_unordered_set<int>;
        using sp_set  = mark::spool_unordered_set<int>;
        using up_set  = mark::upool_unordered_set<int>;

        print_header("unordered_set<int>");
        print_row("insert",
            bench_set_insert<std_set>(ITER),
            bench_set_insert<sys_set>(ITER),
            bench_set_insert<sp_set>(ITER),
            bench_set_insert<up_set>(ITER));
        print_row("count",
            bench_set_count<std_set>(ITER),
            bench_set_count<sys_set>(ITER),
            bench_set_count<sp_set>(ITER),
            bench_set_count<up_set>(ITER));
        print_footer();

        // -----------------------------------------------------------------
        // string (char)
        // -----------------------------------------------------------------
        using std_str = std::string;
        using sys_str = mark::sys_string;
        using sp_str  = mark::spool_string;
        using up_str  = mark::upool_string;

        print_header("string (char)");
        print_row("append",
            bench_string_append<std_str>(STR_ITER),
            bench_string_append<sys_str>(STR_ITER),
            bench_string_append<sp_str>(STR_ITER),
            bench_string_append<up_str>(STR_ITER));
        print_row("find",
            bench_string_find<std_str>(STR_ITER),
            bench_string_find<sys_str>(STR_ITER),
            bench_string_find<sp_str>(STR_ITER),
            bench_string_find<up_str>(STR_ITER));
        print_footer();

        // -----------------------------------------------------------------
        // wstring
        // -----------------------------------------------------------------
        using std_wstr = std::wstring;
        using sys_wstr = mark::sys_wstring;
        using sp_wstr  = mark::spool_wstring;
        using up_wstr  = mark::upool_wstring;

        print_header("wstring (wchar_t)");
        print_row("append",
            bench_string_append<std_wstr>(STR_ITER),
            bench_string_append<sys_wstr>(STR_ITER),
            bench_string_append<sp_wstr>(STR_ITER),
            bench_string_append<up_wstr>(STR_ITER));
        print_row("find",
            bench_string_find<std_wstr>(STR_ITER),
            bench_string_find<sys_wstr>(STR_ITER),
            bench_string_find<sp_wstr>(STR_ITER),
            bench_string_find<up_wstr>(STR_ITER));
        print_footer();

        // -----------------------------------------------------------------
        // u16string
        // -----------------------------------------------------------------
        using std_u16 = std::u16string;
        using sys_u16 = mark::sys_u16string;
        using sp_u16  = mark::spool_u16string;
        using up_u16  = mark::upool_u16string;

        print_header("u16string (char16_t)");
        print_row("append",
            bench_string_append<std_u16>(STR_ITER),
            bench_string_append<sys_u16>(STR_ITER),
            bench_string_append<sp_u16>(STR_ITER),
            bench_string_append<up_u16>(STR_ITER));
        print_row("find",
            bench_string_find<std_u16>(STR_ITER),
            bench_string_find<sys_u16>(STR_ITER),
            bench_string_find<sp_u16>(STR_ITER),
            bench_string_find<up_u16>(STR_ITER));
        print_footer();

        // -----------------------------------------------------------------
        // u32string
        // -----------------------------------------------------------------
        using std_u32 = std::u32string;
        using sys_u32 = mark::sys_u32string;
        using sp_u32  = mark::spool_u32string;
        using up_u32  = mark::upool_u32string;

        print_header("u32string (char32_t)");
        print_row("append",
            bench_string_append<std_u32>(STR_ITER),
            bench_string_append<sys_u32>(STR_ITER),
            bench_string_append<sp_u32>(STR_ITER),
            bench_string_append<up_u32>(STR_ITER));
        print_row("find",
            bench_string_find<std_u32>(STR_ITER),
            bench_string_find<sys_u32>(STR_ITER),
            bench_string_find<sp_u32>(STR_ITER),
            bench_string_find<up_u32>(STR_ITER));
        print_footer();

        shutdown_core_memory();

        printf("============================================================\n");
        printf("  CoreGeneric Benchmark Complete\n");
        printf("============================================================\n\n");
    }
}
