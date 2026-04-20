#pragma once
#include "Compress.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace mark
{
    namespace test_compress_detail
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

#define CMP_CHECK(expr, tag) mark::test_compress_detail::check((expr), (tag))
    }

    // ========================================================================
    // compress_size_lz4
    // ========================================================================
    inline void TestCompress_CompressSize()
    {
        printf("[TestCompress_CompressSize]\n");

        // valid size
        size_t bound = compress::compress_size_lz4(1024);
        CMP_CHECK(bound > 0, "compress_size_lz4 1024 > 0");
        CMP_CHECK(bound >= 1024, "compress_size_lz4 1024 >= src_size");

        // small size
        size_t bound1 = compress::compress_size_lz4(1);
        CMP_CHECK(bound1 > 0, "compress_size_lz4 1 > 0");

        // zero size
        CMP_CHECK(compress::compress_size_lz4(0) == 0, "compress_size_lz4 0 == 0");
    }

    // ========================================================================
    // compress / uncompress round-trip (small data)
    // ========================================================================
    inline void TestCompress_RoundTripSmall()
    {
        printf("[TestCompress_RoundTripSmall]\n");

        const char src[] = "Hello, LZ4 Compression Test!";
        size_t src_size = strlen(src);

        // get compress bound
        size_t comp_bound = compress::compress_size_lz4(src_size);
        CMP_CHECK(comp_bound > 0, "small: compress_size_lz4 > 0");

        // compress
        char* comp_buf = (char*)malloc(comp_bound);
        size_t comp_size = compress::compress_lz4(src, src_size, comp_buf, comp_bound);
        CMP_CHECK(comp_size > 0, "small: compress_lz4 success");
        CMP_CHECK(comp_size <= comp_bound, "small: compressed size <= bound");

        // uncompress
        char* decomp_buf = (char*)malloc(src_size + 1);
        memset(decomp_buf, 0, src_size + 1);
        size_t decomp_size = compress::uncompress_lz4(comp_buf, comp_size, decomp_buf, src_size);
        CMP_CHECK(decomp_size == src_size, "small: decompressed size matches");
        CMP_CHECK(memcmp(decomp_buf, src, src_size) == 0, "small: decompressed data matches");

        free(comp_buf);
        free(decomp_buf);
    }

    // ========================================================================
    // compress / uncompress round-trip (large repetitive data)
    // ========================================================================
    inline void TestCompress_RoundTripLarge()
    {
        printf("[TestCompress_RoundTripLarge]\n");

        // repetitive data compresses well
        const size_t src_size = 64 * 1024; // 64KB
        char* src = (char*)malloc(src_size);
        for (size_t i = 0; i < src_size; ++i)
            src[i] = (char)(i % 26 + 'A');

        size_t comp_bound = compress::compress_size_lz4(src_size);
        CMP_CHECK(comp_bound > 0, "large: compress_size_lz4 > 0");

        char* comp_buf = (char*)malloc(comp_bound);
        size_t comp_size = compress::compress_lz4(src, src_size, comp_buf, comp_bound);
        CMP_CHECK(comp_size > 0, "large: compress_lz4 success");
        CMP_CHECK(comp_size < src_size, "large: compressed smaller than original (repetitive)");

        char* decomp_buf = (char*)malloc(src_size);
        size_t decomp_size = compress::uncompress_lz4(comp_buf, comp_size, decomp_buf, src_size);
        CMP_CHECK(decomp_size == src_size, "large: decompressed size matches");
        CMP_CHECK(memcmp(decomp_buf, src, src_size) == 0, "large: decompressed data matches");

        free(src);
        free(comp_buf);
        free(decomp_buf);
    }

    // ========================================================================
    // compress / uncompress round-trip (random data)
    // ========================================================================
    inline void TestCompress_RoundTripRandom()
    {
        printf("[TestCompress_RoundTripRandom]\n");

        const size_t src_size = 4096;
        char* src = (char*)malloc(src_size);
        srand(12345);
        for (size_t i = 0; i < src_size; ++i)
            src[i] = (char)(rand() % 256);

        size_t comp_bound = compress::compress_size_lz4(src_size);
        char* comp_buf = (char*)malloc(comp_bound);
        size_t comp_size = compress::compress_lz4(src, src_size, comp_buf, comp_bound);
        CMP_CHECK(comp_size > 0, "random: compress_lz4 success");

        char* decomp_buf = (char*)malloc(src_size);
        size_t decomp_size = compress::uncompress_lz4(comp_buf, comp_size, decomp_buf, src_size);
        CMP_CHECK(decomp_size == src_size, "random: decompressed size matches");
        CMP_CHECK(memcmp(decomp_buf, src, src_size) == 0, "random: decompressed data matches");

        free(src);
        free(comp_buf);
        free(decomp_buf);
    }

    // ========================================================================
    // single byte round-trip
    // ========================================================================
    inline void TestCompress_SingleByte()
    {
        printf("[TestCompress_SingleByte]\n");

        const char src = 'Z';
        size_t comp_bound = compress::compress_size_lz4(1);
        CMP_CHECK(comp_bound > 0, "1byte: compress_size_lz4 > 0");

        char* comp_buf = (char*)malloc(comp_bound);
        size_t comp_size = compress::compress_lz4(&src, 1, comp_buf, comp_bound);
        CMP_CHECK(comp_size > 0, "1byte: compress_lz4 success");

        char decomp = 0;
        size_t decomp_size = compress::uncompress_lz4(comp_buf, comp_size, &decomp, 1);
        CMP_CHECK(decomp_size == 1, "1byte: decompressed size == 1");
        CMP_CHECK(decomp == 'Z', "1byte: decompressed data matches");

        free(comp_buf);
    }

    // ========================================================================
    // compress_lz4 invalid arguments
    // ========================================================================
    inline void TestCompress_CompressInvalid()
    {
        printf("[TestCompress_CompressInvalid]\n");

        char src[16] = { 0 };
        char dst[256] = { 0 };

        CMP_CHECK(compress::compress_lz4(nullptr, 16, dst, 256) == 0, "compress_lz4 src nullptr");
        CMP_CHECK(compress::compress_lz4(src, 0, dst, 256) == 0, "compress_lz4 src_size 0");
        CMP_CHECK(compress::compress_lz4(src, 16, nullptr, 256) == 0, "compress_lz4 dst nullptr");
        CMP_CHECK(compress::compress_lz4(src, 16, dst, 0) == 0, "compress_lz4 dst_size 0");

        // dst buffer too small
        CMP_CHECK(compress::compress_lz4(src, 16, dst, 1) == 0, "compress_lz4 dst_size too small");
    }

    // ========================================================================
    // uncompress_lz4 invalid arguments
    // ========================================================================
    inline void TestCompress_UncompressInvalid()
    {
        printf("[TestCompress_UncompressInvalid]\n");

        char src[16] = { 0 };
        char dst[256] = { 0 };

        CMP_CHECK(compress::uncompress_lz4(nullptr, 16, dst, 256) == 0, "uncompress_lz4 src nullptr");
        CMP_CHECK(compress::uncompress_lz4(src, 0, dst, 256) == 0, "uncompress_lz4 src_size 0");
        CMP_CHECK(compress::uncompress_lz4(src, 16, nullptr, 256) == 0, "uncompress_lz4 dst nullptr");
        CMP_CHECK(compress::uncompress_lz4(src, 16, dst, 0) == 0, "uncompress_lz4 dst_size 0");

        // corrupted compressed data
        char garbage[32] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        CMP_CHECK(compress::uncompress_lz4(garbage, 8, dst, 256) == 0, "uncompress_lz4 corrupted data");
    }

    // ========================================================================
    // uncompress with insufficient dst buffer
    // ========================================================================
    inline void TestCompress_UncompressInsufficientBuffer()
    {
        printf("[TestCompress_UncompressInsufficientBuffer]\n");

        const char src[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"; // 40 bytes
        size_t src_size = strlen(src);

        size_t comp_bound = compress::compress_size_lz4(src_size);
        char* comp_buf = (char*)malloc(comp_bound);
        size_t comp_size = compress::compress_lz4(src, src_size, comp_buf, comp_bound);
        CMP_CHECK(comp_size > 0, "insuffbuf: compress success");

        // decompress into a buffer that's too small
        char small_buf[4] = { 0 };
        size_t decomp_size = compress::uncompress_lz4(comp_buf, comp_size, small_buf, 4);
        CMP_CHECK(decomp_size == 0, "uncompress_lz4 insufficient dst buffer returns 0");

        free(comp_buf);
    }

    // ========================================================================
    // Entry Point: Test_Compress()
    // ========================================================================
    inline void Test_Compress()
    {
        using namespace test_compress_detail;
        reset_counters();

        printf("========================================\n");
        printf("  Compress Test Suite\n");
        printf("========================================\n");

        TestCompress_CompressSize();
        TestCompress_RoundTripSmall();
        TestCompress_RoundTripLarge();
        TestCompress_RoundTripRandom();
        TestCompress_SingleByte();
        TestCompress_CompressInvalid();
        TestCompress_UncompressInvalid();
        TestCompress_UncompressInsufficientBuffer();

        print_summary();
    }

} // namespace mark
