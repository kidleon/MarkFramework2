#pragma once
#include "Hash.h"
#include <cstdio>
#include <cstring>

namespace mark
{
    namespace test_hash_detail
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

#define HASH_CHECK(expr, tag) mark::test_hash_detail::check((expr), (tag))
    }

    // ========================================================================
    // CRC32
    // ========================================================================
    inline void TestHash_CRC32()
    {
        printf("[TestHash_CRC32]\n");

        const char* data = "Hello, World!";
        size_t len = strlen(data);

        uint32_t h1 = hash::crc32(data, len);
        uint32_t h2 = hash::crc32(data, len);
        HASH_CHECK(h1 == h2, "crc32: deterministic");
        HASH_CHECK(h1 != 0, "crc32: non-zero result");

        // different data -> different hash
        const char* data2 = "Hello, World?";
        uint32_t h3 = hash::crc32(data2, strlen(data2));
        HASH_CHECK(h1 != h3, "crc32: different data -> different hash");

        // empty data
        uint32_t h4 = hash::crc32("", 0);
        HASH_CHECK(h4 != h1, "crc32: empty vs non-empty");

        // custom initial value
        uint32_t h5 = hash::crc32(data, len, 0);
        uint32_t h6 = hash::crc32(data, len, 0xFFFFFFFFu);
        HASH_CHECK(h5 != h6, "crc32: different initial values");
    }

    // ========================================================================
    // CRC64
    // ========================================================================
    inline void TestHash_CRC64()
    {
        printf("[TestHash_CRC64]\n");

        const char* data = "Hello, World!";
        size_t len = strlen(data);

        uint64_t h1 = hash::crc64(data, len);
        uint64_t h2 = hash::crc64(data, len);
        HASH_CHECK(h1 == h2, "crc64: deterministic");
        HASH_CHECK(h1 != 0, "crc64: non-zero result");

        const char* data2 = "Hello, World?";
        uint64_t h3 = hash::crc64(data2, strlen(data2));
        HASH_CHECK(h1 != h3, "crc64: different data -> different hash");

        uint64_t h5 = hash::crc64(data, len, 0);
        uint64_t h6 = hash::crc64(data, len, 0xFFFFFFFFFFFFFFFFu);
        HASH_CHECK(h5 != h6, "crc64: different initial values");
    }

    // ========================================================================
    // FNV32
    // ========================================================================
    inline void TestHash_FNV32()
    {
        printf("[TestHash_FNV32]\n");

        const char* data = "test string";
        size_t len = strlen(data);

        uint32_t h1 = hash::fnv32(data, len);
        uint32_t h2 = hash::fnv32(data, len);
        HASH_CHECK(h1 == h2, "fnv32: deterministic");

        const char* data2 = "test strinh";
        uint32_t h3 = hash::fnv32(data2, strlen(data2));
        HASH_CHECK(h1 != h3, "fnv32: different data -> different hash");

        // single byte
        uint32_t h4 = hash::fnv32("a", 1);
        uint32_t h5 = hash::fnv32("b", 1);
        HASH_CHECK(h4 != h5, "fnv32: single byte difference");
    }

    // ========================================================================
    // FNV64
    // ========================================================================
    inline void TestHash_FNV64()
    {
        printf("[TestHash_FNV64]\n");

        const char* data = "test string";
        size_t len = strlen(data);

        uint64_t h1 = hash::fnv64(data, len);
        uint64_t h2 = hash::fnv64(data, len);
        HASH_CHECK(h1 == h2, "fnv64: deterministic");

        const char* data2 = "test strinh";
        uint64_t h3 = hash::fnv64(data2, strlen(data2));
        HASH_CHECK(h1 != h3, "fnv64: different data -> different hash");
    }

    // ========================================================================
    // gen_guid
    // ========================================================================
    inline void TestHash_GenGUID()
    {
        printf("[TestHash_GenGUID]\n");

        char guid1[64] = { 0 };
        char guid2[64] = { 0 };

        HASH_CHECK(hash::gen_guid(guid1, sizeof(guid1)), "gen_guid: success");
        HASH_CHECK(strlen(guid1) == 36, "gen_guid: length == 36");

        HASH_CHECK(hash::gen_guid(guid2, sizeof(guid2)), "gen_guid: second success");
        HASH_CHECK(strcmp(guid1, guid2) != 0, "gen_guid: two guids differ");

        // buffer too small
        char small[10] = { 0 };
        // gen_guid needs at least 37 bytes, so small buffer should fail or produce truncated result
        // (implementation-dependent, just test it doesn't crash)
        hash::gen_guid(small, sizeof(small));
        HASH_CHECK(true, "gen_guid: small buffer no crash");

        // nullptr
        HASH_CHECK(!hash::gen_guid(nullptr, 64), "gen_guid: nullptr fails");
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_Hash()
    {
        using namespace test_hash_detail;
        reset_counters();

        printf("========================================\n");
        printf("  Hash Test Suite\n");
        printf("========================================\n");

        TestHash_CRC32();
        TestHash_CRC64();
        TestHash_FNV32();
        TestHash_FNV64();
        TestHash_GenGUID();

        print_summary();
    }

} // namespace mark
