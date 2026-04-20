#pragma once
#include "Unicode.h"
#include <cstdio>
#include <cstring>

namespace mark
{
    namespace test_unicode_detail
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

#define UNI_CHECK(expr, tag) mark::test_unicode_detail::check((expr), (tag))
    }

    // ========================================================================
    // detect_encoding - BOM detection
    // ========================================================================
    inline void TestUnicode_DetectEncoding()
    {
        printf("[TestUnicode_DetectEncoding]\n");

        // UTF-8 BOM
        {
            const char bom[] = { (char)0xEF, (char)0xBB, (char)0xBF, 'A', 'B' };
            auto enc = unicode::detect_encoding(bom, sizeof(bom));
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UTF8_BOM, "detect: UTF-8 BOM");
        }

        // UTF-16 LE BOM
        {
            const char bom[] = { (char)0xFF, (char)0xFE, 'A', 0 };
            auto enc = unicode::detect_encoding(bom, sizeof(bom));
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UTF16LE_BOM, "detect: UTF-16 LE BOM");
        }

        // UTF-16 BE BOM
        {
            const char bom[] = { (char)0xFE, (char)0xFF, 0, 'A' };
            auto enc = unicode::detect_encoding(bom, sizeof(bom));
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UTF16BE_BOM, "detect: UTF-16 BE BOM");
        }

        // UTF-32 LE BOM
        {
            const char bom[] = { (char)0xFF, (char)0xFE, 0, 0 };
            auto enc = unicode::detect_encoding(bom, sizeof(bom));
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UTF32LE_BOM, "detect: UTF-32 LE BOM");
        }

        // UTF-32 BE BOM
        {
            const char bom[] = { 0, 0, (char)0xFE, (char)0xFF };
            auto enc = unicode::detect_encoding(bom, sizeof(bom));
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UTF32BE_BOM, "detect: UTF-32 BE BOM");
        }

        // Plain ASCII -> UTF-8
        {
            const char data[] = "Hello";
            auto enc = unicode::detect_encoding(data, strlen(data));
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UTF8, "detect: plain ASCII -> UTF-8");
        }

        // nullptr / empty
        {
            auto enc = unicode::detect_encoding(nullptr, 0);
            UNI_CHECK(enc == unicode::ENCODING_TYPE::ENCODING_UNKNOWN, "detect: nullptr -> UNKNOWN");

            auto enc2 = unicode::detect_encoding("A", 0);
            UNI_CHECK(enc2 == unicode::ENCODING_TYPE::ENCODING_UNKNOWN, "detect: size 0 -> UNKNOWN");
        }
    }

    // ========================================================================
    // UTF-8 <-> UTF-16 round-trip
    // ========================================================================
    inline void TestUnicode_Utf8Utf16()
    {
        printf("[TestUnicode_Utf8Utf16]\n");

        const char* utf8_str = "Hello";
        size_t utf8_len = strlen(utf8_str);

        char16_t utf16_buf[64] = { 0 };
        bool ok = unicode::conv_utf8_to_utf16(utf8_str, utf8_len, utf16_buf, sizeof(utf16_buf));
        UNI_CHECK(ok, "utf8->utf16: success");
        UNI_CHECK(utf16_buf[0] == u'H', "utf8->utf16: first char");

        // back to utf8
        char utf8_back[64] = { 0 };
        ok = unicode::conv_utf16_to_utf8(utf16_buf, utf8_len * 2, utf8_back, sizeof(utf8_back));
        UNI_CHECK(ok, "utf16->utf8: success");
        UNI_CHECK(strcmp(utf8_back, utf8_str) == 0, "utf16->utf8: round-trip match");

        // null args
        UNI_CHECK(!unicode::conv_utf8_to_utf16(nullptr, 5, utf16_buf, sizeof(utf16_buf)), "utf8->utf16: null src");
        UNI_CHECK(!unicode::conv_utf8_to_utf16(utf8_str, utf8_len, nullptr, sizeof(utf16_buf)), "utf8->utf16: null dest");
        UNI_CHECK(!unicode::conv_utf8_to_utf16(utf8_str, 0, utf16_buf, sizeof(utf16_buf)), "utf8->utf16: zero size");
    }

    // ========================================================================
    // UTF-8 <-> UTF-32 round-trip
    // ========================================================================
    inline void TestUnicode_Utf8Utf32()
    {
        printf("[TestUnicode_Utf8Utf32]\n");

        const char* utf8_str = "ABC";
        size_t utf8_len = strlen(utf8_str);

        char32_t utf32_buf[64] = { 0 };
        bool ok = unicode::conv_utf8_to_utf32(utf8_str, utf8_len, utf32_buf, sizeof(utf32_buf));
        UNI_CHECK(ok, "utf8->utf32: success");
        UNI_CHECK(utf32_buf[0] == U'A', "utf8->utf32: first char");

        // back to utf8
        char utf8_back[64] = { 0 };
        ok = unicode::conv_utf32_to_utf8(utf32_buf, utf8_len * 4, utf8_back, sizeof(utf8_back));
        UNI_CHECK(ok, "utf32->utf8: success");
        UNI_CHECK(strcmp(utf8_back, utf8_str) == 0, "utf32->utf8: round-trip match");
    }

    // ========================================================================
    // conv (generic)
    // ========================================================================
    inline void TestUnicode_Conv()
    {
        printf("[TestUnicode_Conv]\n");

        const char* utf8_str = "Test";
        size_t utf8_len = strlen(utf8_str);

        char utf16_buf[64] = { 0 };
        bool ok = unicode::conv(
            unicode::ENCODING_TYPE::ENCODING_UTF8,
            unicode::ENCODING_TYPE::ENCODING_UTF16LE,
            utf8_str, utf8_len,
            utf16_buf, sizeof(utf16_buf)
        );
        UNI_CHECK(ok, "conv: UTF8 -> UTF16LE");

        // null args
        UNI_CHECK(!unicode::conv(
            unicode::ENCODING_TYPE::ENCODING_UTF8,
            unicode::ENCODING_TYPE::ENCODING_UTF16LE,
            nullptr, utf8_len,
            utf16_buf, sizeof(utf16_buf)
        ), "conv: null src");

        UNI_CHECK(!unicode::conv(
            unicode::ENCODING_TYPE::ENCODING_UTF8,
            unicode::ENCODING_TYPE::ENCODING_UTF16LE,
            utf8_str, utf8_len,
            nullptr, sizeof(utf16_buf)
        ), "conv: null dest");

        // unsupported encoding
        UNI_CHECK(!unicode::conv(
            unicode::ENCODING_TYPE::ENCODING_UNKNOWN,
            unicode::ENCODING_TYPE::ENCODING_UTF8,
            utf8_str, utf8_len,
            utf16_buf, sizeof(utf16_buf)
        ), "conv: unsupported src encoding");
    }

    // ========================================================================
    // UTF-8 <-> wchar_t
    // ========================================================================
    inline void TestUnicode_Utf8Wchar()
    {
        printf("[TestUnicode_Utf8Wchar]\n");

        const char* utf8_str = "Hello";
        size_t utf8_len = strlen(utf8_str);

        wchar_t wbuf[64] = { 0 };
        bool ok = unicode::conv_utf8_to_wchar(utf8_str, utf8_len, wbuf, sizeof(wbuf));
        UNI_CHECK(ok, "utf8->wchar: success");
        UNI_CHECK(wbuf[0] == L'H', "utf8->wchar: first char");

        char utf8_back[64] = { 0 };
        ok = unicode::conv_wchar_to_utf8(wbuf, utf8_len * sizeof(wchar_t), utf8_back, sizeof(utf8_back));
        UNI_CHECK(ok, "wchar->utf8: success");
        UNI_CHECK(strcmp(utf8_back, utf8_str) == 0, "wchar->utf8: round-trip match");
    }

    // ========================================================================
    // Entry Point
    // ========================================================================
    inline void Test_Unicode()
    {
        using namespace test_unicode_detail;
        reset_counters();

        printf("========================================\n");
        printf("  Unicode Test Suite\n");
        printf("========================================\n");

        TestUnicode_DetectEncoding();
        TestUnicode_Utf8Utf16();
        TestUnicode_Utf8Utf32();
        TestUnicode_Conv();
        TestUnicode_Utf8Wchar();

        print_summary();
    }

} // namespace mark
