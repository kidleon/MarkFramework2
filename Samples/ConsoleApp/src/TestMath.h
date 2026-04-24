#pragma once
#include "mathlib.h"
#include <cmath>
#include <cstdio>
#include <cstring>

// ============================================================================
// Helper Macros & Utilities
// ============================================================================
namespace mark
{
    namespace test_math_detail
    {
        static int g_total = 0;
        static int g_passed = 0;
        static int g_failed = 0;

        inline bool nearly_equal(float a, float b, float eps = M_TINY)
        {
            return std::fabs(a - b) <= eps;
        }

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

#define MATH_CHECK(expr, tag) mark::test_math_detail::check((expr), (tag))
    }

    // ========================================================================
    // FLOAT2 / FLOAT3 / FLOAT4 Basic Operations
    // ========================================================================
    inline void TestMath_Set()
    {
        printf("[TestMath_Set]\n");

        FLOAT2 f2 = math_set(1.0f, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(f2.x, 1.0f) && test_math_detail::nearly_equal(f2.y, 2.0f), "math_set FLOAT2");

        FLOAT3 f3 = math_set(3.0f, 4.0f, 5.0f);
        MATH_CHECK(test_math_detail::nearly_equal(f3.x, 3.0f) && test_math_detail::nearly_equal(f3.y, 4.0f) && test_math_detail::nearly_equal(f3.z, 5.0f), "math_set FLOAT3");

        FLOAT4 f4 = math_set(6.0f, 7.0f, 8.0f, 9.0f);
        MATH_CHECK(test_math_detail::nearly_equal(f4.x, 6.0f) && test_math_detail::nearly_equal(f4.y, 7.0f) && test_math_detail::nearly_equal(f4.z, 8.0f) && test_math_detail::nearly_equal(f4.w, 9.0f), "math_set FLOAT4");
    }

    inline void TestMath_Add()
    {
        printf("[TestMath_Add]\n");

        FLOAT2 a2 = { 1.0f, 2.0f }, b2 = { 3.0f, 4.0f };
        FLOAT2 r2 = math_add(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 4.0f) && test_math_detail::nearly_equal(r2.y, 6.0f), "math_add FLOAT2");

        FLOAT3 a3 = { 1.0f, 2.0f, 3.0f }, b3 = { 4.0f, 5.0f, 6.0f };
        FLOAT3 r3 = math_add(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 5.0f) && test_math_detail::nearly_equal(r3.y, 7.0f) && test_math_detail::nearly_equal(r3.z, 9.0f), "math_add FLOAT3");

        FLOAT4 a4 = { 1.0f, 2.0f, 3.0f, 4.0f }, b4 = { 5.0f, 6.0f, 7.0f, 8.0f };
        FLOAT4 r4 = math_add(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 6.0f) && test_math_detail::nearly_equal(r4.y, 8.0f) && test_math_detail::nearly_equal(r4.z, 10.0f) && test_math_detail::nearly_equal(r4.w, 12.0f), "math_add FLOAT4");
    }

    inline void TestMath_Sub()
    {
        printf("[TestMath_Sub]\n");

        FLOAT2 a2 = { 5.0f, 7.0f }, b2 = { 2.0f, 3.0f };
        FLOAT2 r2 = math_sub(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 3.0f) && test_math_detail::nearly_equal(r2.y, 4.0f), "math_sub FLOAT2");

        FLOAT3 a3 = { 10.0f, 20.0f, 30.0f }, b3 = { 1.0f, 2.0f, 3.0f };
        FLOAT3 r3 = math_sub(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 9.0f) && test_math_detail::nearly_equal(r3.y, 18.0f) && test_math_detail::nearly_equal(r3.z, 27.0f), "math_sub FLOAT3");

        FLOAT4 a4 = { 10.0f, 20.0f, 30.0f, 40.0f }, b4 = { 1.0f, 2.0f, 3.0f, 4.0f };
        FLOAT4 r4 = math_sub(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 9.0f) && test_math_detail::nearly_equal(r4.y, 18.0f) && test_math_detail::nearly_equal(r4.z, 27.0f) && test_math_detail::nearly_equal(r4.w, 36.0f), "math_sub FLOAT4");
    }

    inline void TestMath_Mul()
    {
        printf("[TestMath_Mul]\n");

        // Component-wise
        FLOAT2 a2 = { 2.0f, 3.0f }, b2 = { 4.0f, 5.0f };
        FLOAT2 r2 = math_mul(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 8.0f) && test_math_detail::nearly_equal(r2.y, 15.0f), "math_mul FLOAT2 component");

        FLOAT3 a3 = { 2.0f, 3.0f, 4.0f }, b3 = { 5.0f, 6.0f, 7.0f };
        FLOAT3 r3 = math_mul(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 10.0f) && test_math_detail::nearly_equal(r3.y, 18.0f) && test_math_detail::nearly_equal(r3.z, 28.0f), "math_mul FLOAT3 component");

        FLOAT4 a4 = { 1.0f, 2.0f, 3.0f, 4.0f }, b4 = { 5.0f, 6.0f, 7.0f, 8.0f };
        FLOAT4 r4 = math_mul(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 5.0f) && test_math_detail::nearly_equal(r4.y, 12.0f) && test_math_detail::nearly_equal(r4.z, 21.0f) && test_math_detail::nearly_equal(r4.w, 32.0f), "math_mul FLOAT4 component");

        // Scalar
        FLOAT2 s2 = math_mul(a2, 3.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s2.x, 6.0f) && test_math_detail::nearly_equal(s2.y, 9.0f), "math_mul FLOAT2 scalar");

        FLOAT3 s3 = math_mul(a3, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s3.x, 4.0f) && test_math_detail::nearly_equal(s3.y, 6.0f) && test_math_detail::nearly_equal(s3.z, 8.0f), "math_mul FLOAT3 scalar");

        FLOAT4 s4 = math_mul(a4, 0.5f);
        MATH_CHECK(test_math_detail::nearly_equal(s4.x, 0.5f) && test_math_detail::nearly_equal(s4.y, 1.0f) && test_math_detail::nearly_equal(s4.z, 1.5f) && test_math_detail::nearly_equal(s4.w, 2.0f), "math_mul FLOAT4 scalar");
    }

    inline void TestMath_Div()
    {
        printf("[TestMath_Div]\n");

        // Component-wise
        FLOAT2 a2 = { 10.0f, 20.0f }, b2 = { 2.0f, 5.0f };
        FLOAT2 r2 = math_div(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 5.0f) && test_math_detail::nearly_equal(r2.y, 4.0f), "math_div FLOAT2 component");

        FLOAT3 a3 = { 12.0f, 15.0f, 18.0f }, b3 = { 3.0f, 5.0f, 6.0f };
        FLOAT3 r3 = math_div(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 4.0f) && test_math_detail::nearly_equal(r3.y, 3.0f) && test_math_detail::nearly_equal(r3.z, 3.0f), "math_div FLOAT3 component");

        FLOAT4 a4 = { 8.0f, 16.0f, 24.0f, 32.0f }, b4 = { 2.0f, 4.0f, 8.0f, 16.0f };
        FLOAT4 r4 = math_div(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 4.0f) && test_math_detail::nearly_equal(r4.y, 4.0f) && test_math_detail::nearly_equal(r4.z, 3.0f) && test_math_detail::nearly_equal(r4.w, 2.0f), "math_div FLOAT4 component");

        // Scalar
        FLOAT2 s2 = math_div(a2, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s2.x, 5.0f) && test_math_detail::nearly_equal(s2.y, 10.0f), "math_div FLOAT2 scalar");

        FLOAT3 s3 = math_div(a3, 3.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s3.x, 4.0f) && test_math_detail::nearly_equal(s3.y, 5.0f) && test_math_detail::nearly_equal(s3.z, 6.0f), "math_div FLOAT3 scalar");

        FLOAT4 s4 = math_div(a4, 4.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s4.x, 2.0f) && test_math_detail::nearly_equal(s4.y, 4.0f) && test_math_detail::nearly_equal(s4.z, 6.0f) && test_math_detail::nearly_equal(s4.w, 8.0f), "math_div FLOAT4 scalar");
    }

    inline void TestMath_Dot()
    {
        printf("[TestMath_Dot]\n");

        FLOAT2 a2 = { 1.0f, 2.0f }, b2 = { 3.0f, 4.0f };
        float d2 = math_dot(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(d2, 11.0f), "math_dot FLOAT2");

        FLOAT3 a3 = { 1.0f, 2.0f, 3.0f }, b3 = { 4.0f, 5.0f, 6.0f };
        float d3 = math_dot(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(d3, 32.0f), "math_dot FLOAT3");

        FLOAT4 a4 = { 1.0f, 2.0f, 3.0f, 4.0f }, b4 = { 5.0f, 6.0f, 7.0f, 8.0f };
        float d4 = math_dot(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(d4, 70.0f), "math_dot FLOAT4");

        // Out param versions
        float out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        math_dot(a2, b2, out2);
        MATH_CHECK(test_math_detail::nearly_equal(out2, 11.0f), "math_dot FLOAT2 out");
        math_dot(a3, b3, out3);
        MATH_CHECK(test_math_detail::nearly_equal(out3, 32.0f), "math_dot FLOAT3 out");
        math_dot(a4, b4, out4);
        MATH_CHECK(test_math_detail::nearly_equal(out4, 70.0f), "math_dot FLOAT4 out");
    }

    inline void TestMath_Cross()
    {
        printf("[TestMath_Cross]\n");

        // 2D cross product (scalar)
        FLOAT2 a2 = { 1.0f, 0.0f }, b2 = { 0.0f, 1.0f };
        float c2 = math_cross(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(c2, 1.0f), "math_cross FLOAT2");

        // 3D cross product
        FLOAT3 a3 = { 1.0f, 0.0f, 0.0f }, b3 = { 0.0f, 1.0f, 0.0f };
        FLOAT3 c3 = math_cross(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(c3.x, 0.0f) && test_math_detail::nearly_equal(c3.y, 0.0f) && test_math_detail::nearly_equal(c3.z, 1.0f), "math_cross FLOAT3 (X x Y = Z)");

        // 4D cross product
        FLOAT4 va = { 1.0f, 0.0f, 0.0f, 0.0f };
        FLOAT4 vb = { 0.0f, 1.0f, 0.0f, 0.0f };
        FLOAT4 vc = { 0.0f, 0.0f, 1.0f, 0.0f };
        FLOAT4 c4 = math_cross(va, vb, vc);
        MATH_CHECK(test_math_detail::nearly_equal(c4.w, -1.0f, 0.001f), "math_cross FLOAT4 (triple)");

        // Out param versions
        float out_c2 = 0.0f;
        math_cross(a2, b2, out_c2);
        MATH_CHECK(test_math_detail::nearly_equal(out_c2, 1.0f), "math_cross FLOAT2 out");

        FLOAT3 out_c3 = {};
        math_cross(a3, b3, out_c3);
        MATH_CHECK(test_math_detail::nearly_equal(out_c3.z, 1.0f), "math_cross FLOAT3 out");

        FLOAT4 out_c4 = {};
        math_cross(va, vb, vc, out_c4);
        MATH_CHECK(test_math_detail::nearly_equal(out_c4.w, -1.0f, 0.001f), "math_cross FLOAT4 out");
    }

    inline void TestMath_Length()
    {
        printf("[TestMath_Length]\n");

        FLOAT2 v2 = { 3.0f, 4.0f };
        MATH_CHECK(test_math_detail::nearly_equal(math_length(v2), 5.0f), "math_length FLOAT2");

        FLOAT3 v3 = { 1.0f, 2.0f, 2.0f };
        MATH_CHECK(test_math_detail::nearly_equal(math_length(v3), 3.0f), "math_length FLOAT3");

        FLOAT4 v4 = { 1.0f, 2.0f, 2.0f, 0.0f };
        MATH_CHECK(test_math_detail::nearly_equal(math_length(v4), 3.0f), "math_length FLOAT4");
    }

    inline void TestMath_LengthSq()
    {
        printf("[TestMath_LengthSq]\n");

        FLOAT2 v2 = { 3.0f, 4.0f };
        MATH_CHECK(test_math_detail::nearly_equal(math_lengthsq(v2), 25.0f), "math_lengthsq FLOAT2");

        FLOAT3 v3 = { 1.0f, 2.0f, 3.0f };
        MATH_CHECK(test_math_detail::nearly_equal(math_lengthsq(v3), 14.0f), "math_lengthsq FLOAT3");

        FLOAT4 v4 = { 1.0f, 2.0f, 3.0f, 4.0f };
        MATH_CHECK(test_math_detail::nearly_equal(math_lengthsq(v4), 30.0f), "math_lengthsq FLOAT4");
    }

    inline void TestMath_Normalized()
    {
        printf("[TestMath_Normalized]\n");

        FLOAT2 v2 = { 3.0f, 4.0f };
        FLOAT2 n2 = math_normalized(v2);
        MATH_CHECK(test_math_detail::nearly_equal(math_length(n2), 1.0f, 0.0001f), "math_normalized FLOAT2 length==1");

        FLOAT3 v3 = { 1.0f, 2.0f, 2.0f };
        FLOAT3 n3 = math_normalized(v3);
        MATH_CHECK(test_math_detail::nearly_equal(math_length(n3), 1.0f, 0.0001f), "math_normalized FLOAT3 length==1");

        FLOAT4 v4 = { 1.0f, 2.0f, 3.0f, 4.0f };
        FLOAT4 n4 = math_normalized(v4);
        MATH_CHECK(test_math_detail::nearly_equal(math_length(n4), 1.0f, 0.0001f), "math_normalized FLOAT4 length==1");
    }

    inline void TestMath_Normalize()
    {
        printf("[TestMath_Normalize]\n");

        FLOAT2 v2 = { 3.0f, 4.0f };
        math_normalize(v2);
        MATH_CHECK(test_math_detail::nearly_equal(math_length(v2), 1.0f, 0.0001f), "math_normalize FLOAT2 in-place");

        FLOAT3 v3 = { 1.0f, 2.0f, 2.0f };
        math_normalize(v3);
        MATH_CHECK(test_math_detail::nearly_equal(math_length(v3), 1.0f, 0.0001f), "math_normalize FLOAT3 in-place");

        FLOAT4 v4 = { 1.0f, 2.0f, 3.0f, 4.0f };
        math_normalize(v4);
        MATH_CHECK(test_math_detail::nearly_equal(math_length(v4), 1.0f, 0.0001f), "math_normalize FLOAT4 in-place");
    }

    inline void TestMath_Lerp()
    {
        printf("[TestMath_Lerp]\n");

        FLOAT2 a2 = { 0.0f, 0.0f }, b2 = { 10.0f, 20.0f };
        FLOAT2 r2 = math_lerp(a2, b2, 0.5f);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 5.0f) && test_math_detail::nearly_equal(r2.y, 10.0f), "math_lerp FLOAT2 t=0.5");

        FLOAT3 a3 = { 0.0f, 0.0f, 0.0f }, b3 = { 10.0f, 20.0f, 30.0f };
        FLOAT3 r3 = math_lerp(a3, b3, 0.25f);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 2.5f) && test_math_detail::nearly_equal(r3.y, 5.0f) && test_math_detail::nearly_equal(r3.z, 7.5f), "math_lerp FLOAT3 t=0.25");

        FLOAT4 a4 = { 0.0f, 0.0f, 0.0f, 0.0f }, b4 = { 4.0f, 8.0f, 12.0f, 16.0f };
        FLOAT4 r4 = math_lerp(a4, b4, 1.0f);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 4.0f) && test_math_detail::nearly_equal(r4.y, 8.0f) && test_math_detail::nearly_equal(r4.z, 12.0f) && test_math_detail::nearly_equal(r4.w, 16.0f), "math_lerp FLOAT4 t=1.0");
    }

    inline void TestMath_Clamp()
    {
        printf("[TestMath_Clamp]\n");

        FLOAT2 v2 = { -1.0f, 5.0f };
        FLOAT2 r2 = math_clamp(v2, FLOAT2{ 0.0f, 0.0f }, FLOAT2{ 3.0f, 3.0f });
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 0.0f) && test_math_detail::nearly_equal(r2.y, 3.0f), "math_clamp FLOAT2");

        FLOAT3 v3 = { -2.0f, 1.5f, 10.0f };
        FLOAT3 r3 = math_clamp(v3, FLOAT3{ 0.0f, 0.0f, 0.0f }, FLOAT3{ 5.0f, 5.0f, 5.0f });
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 0.0f) && test_math_detail::nearly_equal(r3.y, 1.5f) && test_math_detail::nearly_equal(r3.z, 5.0f), "math_clamp FLOAT3");

        FLOAT4 v4 = { -1.0f, 0.5f, 3.0f, 10.0f };
        FLOAT4 r4 = math_clamp(v4, FLOAT4{ 0.0f, 0.0f, 0.0f, 0.0f }, FLOAT4{ 2.0f, 2.0f, 2.0f, 2.0f });
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 0.0f) && test_math_detail::nearly_equal(r4.y, 0.5f) && test_math_detail::nearly_equal(r4.z, 2.0f) && test_math_detail::nearly_equal(r4.w, 2.0f), "math_clamp FLOAT4");
    }

    inline void TestMath_Saturate()
    {
        printf("[TestMath_Saturate]\n");

        FLOAT2 v2 = { -0.5f, 1.5f };
        FLOAT2 r2 = math_saturate(v2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 0.0f) && test_math_detail::nearly_equal(r2.y, 1.0f), "math_saturate FLOAT2");

        FLOAT3 v3 = { -1.0f, 0.5f, 2.0f };
        FLOAT3 r3 = math_saturate(v3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 0.0f) && test_math_detail::nearly_equal(r3.y, 0.5f) && test_math_detail::nearly_equal(r3.z, 1.0f), "math_saturate FLOAT3");

        FLOAT4 v4 = { -2.0f, 0.3f, 0.7f, 3.0f };
        FLOAT4 r4 = math_saturate(v4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 0.0f) && test_math_detail::nearly_equal(r4.y, 0.3f) && test_math_detail::nearly_equal(r4.z, 0.7f) && test_math_detail::nearly_equal(r4.w, 1.0f), "math_saturate FLOAT4");
    }

    inline void TestMath_Min()
    {
        printf("[TestMath_Min]\n");

        FLOAT2 a2 = { 1.0f, 5.0f }, b2 = { 3.0f, 2.0f };
        FLOAT2 r2 = math_min(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 1.0f) && test_math_detail::nearly_equal(r2.y, 2.0f), "math_min FLOAT2");

        FLOAT3 a3 = { 1.0f, 5.0f, 3.0f }, b3 = { 3.0f, 2.0f, 4.0f };
        FLOAT3 r3 = math_min(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 1.0f) && test_math_detail::nearly_equal(r3.y, 2.0f) && test_math_detail::nearly_equal(r3.z, 3.0f), "math_min FLOAT3");

        FLOAT4 a4 = { 1.0f, 5.0f, 3.0f, 8.0f }, b4 = { 3.0f, 2.0f, 4.0f, 6.0f };
        FLOAT4 r4 = math_min(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 1.0f) && test_math_detail::nearly_equal(r4.y, 2.0f) && test_math_detail::nearly_equal(r4.z, 3.0f) && test_math_detail::nearly_equal(r4.w, 6.0f), "math_min FLOAT4");
    }

    inline void TestMath_Max()
    {
        printf("[TestMath_Max]\n");

        FLOAT2 a2 = { 1.0f, 5.0f }, b2 = { 3.0f, 2.0f };
        FLOAT2 r2 = math_max(a2, b2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 3.0f) && test_math_detail::nearly_equal(r2.y, 5.0f), "math_max FLOAT2");

        FLOAT3 a3 = { 1.0f, 5.0f, 3.0f }, b3 = { 3.0f, 2.0f, 4.0f };
        FLOAT3 r3 = math_max(a3, b3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 3.0f) && test_math_detail::nearly_equal(r3.y, 5.0f) && test_math_detail::nearly_equal(r3.z, 4.0f), "math_max FLOAT3");

        FLOAT4 a4 = { 1.0f, 5.0f, 3.0f, 8.0f }, b4 = { 3.0f, 2.0f, 4.0f, 6.0f };
        FLOAT4 r4 = math_max(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 3.0f) && test_math_detail::nearly_equal(r4.y, 5.0f) && test_math_detail::nearly_equal(r4.z, 4.0f) && test_math_detail::nearly_equal(r4.w, 8.0f), "math_max FLOAT4");
    }

    inline void TestMath_Ceil()
    {
        printf("[TestMath_Ceil]\n");

        FLOAT2 v2 = { 1.2f, 2.8f };
        FLOAT2 r2 = math_ceil(v2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 2.0f) && test_math_detail::nearly_equal(r2.y, 3.0f), "math_ceil FLOAT2");

        FLOAT3 v3 = { 1.1f, 2.5f, 3.9f };
        FLOAT3 r3 = math_ceil(v3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 2.0f) && test_math_detail::nearly_equal(r3.y, 3.0f) && test_math_detail::nearly_equal(r3.z, 4.0f), "math_ceil FLOAT3");

        FLOAT4 v4 = { -0.5f, 0.1f, 2.9f, 3.0f };
        FLOAT4 r4 = math_ceil(v4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 0.0f) && test_math_detail::nearly_equal(r4.y, 1.0f) && test_math_detail::nearly_equal(r4.z, 3.0f) && test_math_detail::nearly_equal(r4.w, 3.0f), "math_ceil FLOAT4");
    }

    inline void TestMath_Floor()
    {
        printf("[TestMath_Floor]\n");

        FLOAT2 v2 = { 1.9f, 2.1f };
        FLOAT2 r2 = math_floor(v2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 1.0f) && test_math_detail::nearly_equal(r2.y, 2.0f), "math_floor FLOAT2");

        FLOAT3 v3 = { 1.9f, 2.1f, 3.5f };
        FLOAT3 r3 = math_floor(v3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 1.0f) && test_math_detail::nearly_equal(r3.y, 2.0f) && test_math_detail::nearly_equal(r3.z, 3.0f), "math_floor FLOAT3");

        FLOAT4 v4 = { -0.1f, 0.9f, 2.0f, 3.7f };
        FLOAT4 r4 = math_floor(v4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, -1.0f) && test_math_detail::nearly_equal(r4.y, 0.0f) && test_math_detail::nearly_equal(r4.z, 2.0f) && test_math_detail::nearly_equal(r4.w, 3.0f), "math_floor FLOAT4");
    }

    inline void TestMath_Round()
    {
        printf("[TestMath_Round]\n");

        FLOAT2 v2 = { 1.4f, 2.6f };
        FLOAT2 r2 = math_round(v2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 1.0f) && test_math_detail::nearly_equal(r2.y, 3.0f), "math_round FLOAT2");

        FLOAT3 v3 = { 1.4f, 2.5f, 3.6f };
        FLOAT3 r3 = math_round(v3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 1.0f) && test_math_detail::nearly_equal(r3.z, 4.0f), "math_round FLOAT3");

        FLOAT4 v4 = { -0.6f, 0.4f, 2.5f, 3.7f };
        FLOAT4 r4 = math_round(v4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, -1.0f) && test_math_detail::nearly_equal(r4.y, 0.0f) && test_math_detail::nearly_equal(r4.w, 4.0f), "math_round FLOAT4");
    }

    inline void TestMath_Trunc()
    {
        printf("[TestMath_Trunc]\n");

        FLOAT2 v2 = { 1.9f, -2.9f };
        FLOAT2 r2 = math_trunc(v2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.x, 1.0f) && test_math_detail::nearly_equal(r2.y, -2.0f), "math_trunc FLOAT2");

        FLOAT3 v3 = { 1.9f, -2.9f, 3.1f };
        FLOAT3 r3 = math_trunc(v3);
        MATH_CHECK(test_math_detail::nearly_equal(r3.x, 1.0f) && test_math_detail::nearly_equal(r3.y, -2.0f) && test_math_detail::nearly_equal(r3.z, 3.0f), "math_trunc FLOAT3");

        FLOAT4 v4 = { 1.9f, -2.9f, 3.1f, -4.5f };
        FLOAT4 r4 = math_trunc(v4);
        MATH_CHECK(test_math_detail::nearly_equal(r4.x, 1.0f) && test_math_detail::nearly_equal(r4.y, -2.0f) && test_math_detail::nearly_equal(r4.z, 3.0f) && test_math_detail::nearly_equal(r4.w, -4.0f), "math_trunc FLOAT4");
    }

    // ========================================================================
    // Quaternion Operations
    // ========================================================================
    inline void TestQuat_Ident()
    {
        printf("[TestQuat_Ident]\n");

        QUAT q = quat_ident();
        MATH_CHECK(test_math_detail::nearly_equal(q.x, 0.0f) && test_math_detail::nearly_equal(q.y, 0.0f) && test_math_detail::nearly_equal(q.z, 0.0f) && test_math_detail::nearly_equal(q.w, 1.0f), "quat_ident");
    }

    inline void TestQuat_Mul()
    {
        printf("[TestQuat_Mul]\n");

        // Scalar multiply
        QUAT q = { 1.0f, 2.0f, 3.0f, 4.0f };
        QUAT qs = quat_mul(q, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(qs.x, 2.0f) && test_math_detail::nearly_equal(qs.y, 4.0f) && test_math_detail::nearly_equal(qs.z, 6.0f) && test_math_detail::nearly_equal(qs.w, 8.0f), "quat_mul scalar");

        // Quaternion * Quaternion : identity * q = q
        QUAT id = quat_ident();
        QUAT qq = quat_mul(id, q);
        MATH_CHECK(test_math_detail::nearly_equal(qq.x, q.x) && test_math_detail::nearly_equal(qq.y, q.y) && test_math_detail::nearly_equal(qq.z, q.z) && test_math_detail::nearly_equal(qq.w, q.w), "quat_mul identity*q == q");

        // Quaternion * Vector : identity rotation should not change the vector
        FLOAT3 v = { 1.0f, 0.0f, 0.0f };
        FLOAT3 rv = quat_mul(id, v);
        MATH_CHECK(test_math_detail::nearly_equal(rv.x, 1.0f, 0.001f) && test_math_detail::nearly_equal(rv.y, 0.0f, 0.001f) && test_math_detail::nearly_equal(rv.z, 0.0f, 0.001f), "quat_mul identity*vec");

        // 90-degree rotation around Z-axis: (1,0,0) -> (0,1,0)
        QUAT qz90 = quat_rotation_axis_angle(FLOAT3{ 0.0f, 0.0f, 1.0f }, M_PIDIV2);
        FLOAT3 rotated = quat_mul(qz90, v);
        MATH_CHECK(test_math_detail::nearly_equal(rotated.x, 0.0f, 0.01f) && test_math_detail::nearly_equal(rotated.y, 1.0f, 0.01f) && test_math_detail::nearly_equal(rotated.z, 0.0f, 0.01f), "quat_mul 90deg Z rotation");
    }

    inline void TestQuat_Inverse()
    {
        printf("[TestQuat_Inverse]\n");

        QUAT q = quat_normalize(QUAT{ 0.0f, 0.0f, 0.707f, 0.707f });
        QUAT qi = quat_inverse(q);
        QUAT prod = quat_mul(q, qi);
        MATH_CHECK(test_math_detail::nearly_equal(prod.x, 0.0f, 0.001f) && test_math_detail::nearly_equal(prod.y, 0.0f, 0.001f) && test_math_detail::nearly_equal(prod.z, 0.0f, 0.001f) && test_math_detail::nearly_equal(std::fabs(prod.w), 1.0f, 0.001f), "quat_inverse q*q^-1 == identity");
    }

    inline void TestQuat_Normalize()
    {
        printf("[TestQuat_Normalize]\n");

        QUAT q = { 1.0f, 2.0f, 3.0f, 4.0f };
        QUAT qn = quat_normalize(q);
        float len = std::sqrt(qn.x * qn.x + qn.y * qn.y + qn.z * qn.z + qn.w * qn.w);
        MATH_CHECK(test_math_detail::nearly_equal(len, 1.0f, 0.0001f), "quat_normalize return length==1");

        // In-place normalized
        QUAT q2 = { 1.0f, 2.0f, 3.0f, 4.0f };
        quat_normalized(q2);
        float len2 = std::sqrt(q2.x * q2.x + q2.y * q2.y + q2.z * q2.z + q2.w * q2.w);
        MATH_CHECK(test_math_detail::nearly_equal(len2, 1.0f, 0.0001f), "quat_normalized in-place length==1");
    }

    inline void TestQuat_Conjugate()
    {
        printf("[TestQuat_Conjugate]\n");

        QUAT q = { 1.0f, 2.0f, 3.0f, 4.0f };
        QUAT qc = quat_conjugate(q);
        MATH_CHECK(test_math_detail::nearly_equal(qc.x, -1.0f) && test_math_detail::nearly_equal(qc.y, -2.0f) && test_math_detail::nearly_equal(qc.z, -3.0f) && test_math_detail::nearly_equal(qc.w, 4.0f), "quat_conjugate return");
    }

    inline void TestQuat_Slerp()
    {
        printf("[TestQuat_Slerp]\n");

        QUAT q0 = quat_ident();
        QUAT q1 = quat_rotation_axis_angle(FLOAT3{ 0.0f, 0.0f, 1.0f }, M_PI);

        // t=0 -> q0
        QUAT r0 = quat_slerp(q0, q1, 0.0f);
        MATH_CHECK(test_math_detail::nearly_equal(r0.w, q0.w, 0.01f), "quat_slerp t=0");

        // t=1 -> q1 (or -q1, both represent the same rotation)
        QUAT r1 = quat_slerp(q0, q1, 1.0f);
        MATH_CHECK(test_math_detail::nearly_equal(std::fabs(r1.z), std::fabs(q1.z), 0.01f), "quat_slerp t=1");

        // t=0.5 -> halfway rotation
        QUAT r05 = quat_slerp(q0, q1, 0.5f);
        float len05 = std::sqrt(r05.x * r05.x + r05.y * r05.y + r05.z * r05.z + r05.w * r05.w);
        MATH_CHECK(test_math_detail::nearly_equal(len05, 1.0f, 0.01f), "quat_slerp t=0.5 unit length");
    }

    inline void TestQuat_RotationRollPitchYaw()
    {
        printf("[TestQuat_RotationRollPitchYaw]\n");

        QUAT q0 = quat_rotation_roll_pitch_yaw(0.0f, 0.0f, 0.0f);
        MATH_CHECK(test_math_detail::nearly_equal(q0.x, 0.0f, 0.001f) && test_math_detail::nearly_equal(q0.y, 0.0f, 0.001f) && test_math_detail::nearly_equal(q0.z, 0.0f, 0.001f) && test_math_detail::nearly_equal(q0.w, 1.0f, 0.001f), "quat_rotation_roll_pitch_yaw (0,0,0) == identity");

        QUAT q90z = quat_rotation_roll_pitch_yaw(0.0f, 0.0f, M_PIDIV2);
        float len = std::sqrt(q90z.x * q90z.x + q90z.y * q90z.y + q90z.z * q90z.z + q90z.w * q90z.w);
        MATH_CHECK(test_math_detail::nearly_equal(len, 1.0f, 0.001f), "quat_rotation_roll_pitch_yaw unit length");
    }

    inline void TestQuat_RotationAxisAngle()
    {
        printf("[TestQuat_RotationAxisAngle]\n");

        QUAT q = quat_rotation_axis_angle(FLOAT3{ 0.0f, 1.0f, 0.0f }, M_PI);
        float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
        MATH_CHECK(test_math_detail::nearly_equal(len, 1.0f, 0.001f), "quat_rotation_axis_angle unit length");

        // 180 degrees around Y: (1,0,0) -> (-1,0,0)
        FLOAT3 v = { 1.0f, 0.0f, 0.0f };
        FLOAT3 rv = quat_mul(q, v);
        MATH_CHECK(test_math_detail::nearly_equal(rv.x, -1.0f, 0.01f) && test_math_detail::nearly_equal(rv.y, 0.0f, 0.01f) && test_math_detail::nearly_equal(rv.z, 0.0f, 0.01f), "quat_rotation_axis_angle 180deg Y");
    }

    inline void TestQuat_RotationLookAt()
    {
        printf("[TestQuat_RotationLookAt]\n");

        QUAT q = quat_rotation_lookat(FLOAT3{ 0.0f, 0.0f, 1.0f }, FLOAT3{ 0.0f, 1.0f, 0.0f });
        float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
        MATH_CHECK(test_math_detail::nearly_equal(len, 1.0f, 0.01f), "quat_rotation_lookat unit length");
    }

    inline void TestQuat_RotationBetween()
    {
        printf("[TestQuat_RotationBetween]\n");

        FLOAT3 from = { 1.0f, 0.0f, 0.0f };
        FLOAT3 to = { 0.0f, 1.0f, 0.0f };
        QUAT q = quat_rotation_between(from, to);
        FLOAT3 result = quat_mul(q, from);
        MATH_CHECK(test_math_detail::nearly_equal(result.x, to.x, 0.01f) && test_math_detail::nearly_equal(result.y, to.y, 0.01f) && test_math_detail::nearly_equal(result.z, to.z, 0.01f), "quat_rotation_between X->Y");

        // Same direction -> identity
        QUAT qsame = quat_rotation_between(from, from);
        MATH_CHECK(test_math_detail::nearly_equal(std::fabs(qsame.w), 1.0f, 0.01f), "quat_rotation_between same direction");
    }

    inline void TestQuat_FromMatrix()
    {
        printf("[TestQuat_FromMatrix]\n");

        QUAT q_orig = quat_rotation_axis_angle(FLOAT3{ 0.0f, 1.0f, 0.0f }, M_PIDIV2);
        MATRIX4 m = quat_to_matrix(q_orig);
        QUAT q_back = quat_from_matrix(m);

        // q and -q represent the same rotation
        bool match = (test_math_detail::nearly_equal(q_orig.x, q_back.x, 0.01f) &&
            test_math_detail::nearly_equal(q_orig.y, q_back.y, 0.01f) &&
            test_math_detail::nearly_equal(q_orig.z, q_back.z, 0.01f) &&
            test_math_detail::nearly_equal(q_orig.w, q_back.w, 0.01f)) ||
            (test_math_detail::nearly_equal(q_orig.x, -q_back.x, 0.01f) &&
                test_math_detail::nearly_equal(q_orig.y, -q_back.y, 0.01f) &&
                test_math_detail::nearly_equal(q_orig.z, -q_back.z, 0.01f) &&
                test_math_detail::nearly_equal(q_orig.w, -q_back.w, 0.01f));
        MATH_CHECK(match, "quat_from_matrix round-trip");
    }

    inline void TestQuat_ToRollPitchYaw()
    {
        printf("[TestQuat_ToRollPitchYaw]\n");

        QUAT q = quat_ident();
        FLOAT3 rpy = quat_to_roll_pitch_yaw(q);
        MATH_CHECK(test_math_detail::nearly_equal(rpy.x, 0.0f, 0.001f) && test_math_detail::nearly_equal(rpy.y, 0.0f, 0.001f) && test_math_detail::nearly_equal(rpy.z, 0.0f, 0.001f), "quat_to_roll_pitch_yaw identity -> (0,0,0)");
    }

    inline void TestQuat_ToAxisAngle()
    {
        printf("[TestQuat_ToAxisAngle]\n");

        float angle_in = M_PIDIV2;
        FLOAT3 axis_in = { 0.0f, 1.0f, 0.0f };
        QUAT q = quat_rotation_axis_angle(axis_in, angle_in);
        FLOAT4 aa = quat_to_axis_angle(q);
        MATH_CHECK(test_math_detail::nearly_equal(aa.w, angle_in, 0.01f), "quat_to_axis_angle angle match");
        MATH_CHECK(test_math_detail::nearly_equal(std::fabs(aa.y), 1.0f, 0.01f), "quat_to_axis_angle axis Y");
    }

    inline void TestQuat_ToMatrix()
    {
        printf("[TestQuat_ToMatrix]\n");

        // Identity quaternion -> identity matrix
        QUAT qid = quat_ident();
        MATRIX4 mid = quat_to_matrix(qid);
        MATH_CHECK(mat4_is_ident(mid), "quat_to_matrix identity");

        // 90-degree rotation around Z should move X-axis to Y-axis
        QUAT qz = quat_rotation_axis_angle(FLOAT3{ 0.0f, 0.0f, 1.0f }, M_PIDIV2);
        MATRIX4 mz = quat_to_matrix(qz);
        FLOAT3 vx = { 1.0f, 0.0f, 0.0f };
        FLOAT3 rv = mat4_mul(mz, vx);
        MATH_CHECK(test_math_detail::nearly_equal(rv.x, 0.0f, 0.01f) && test_math_detail::nearly_equal(rv.y, 1.0f, 0.01f), "quat_to_matrix Z90 * X == Y");
    }

    // ========================================================================
    // Matrix4 Operations
    // ========================================================================
    inline void TestMat4_IsIdent()
    {
        printf("[TestMat4_IsIdent]\n");

        MATH_CHECK(mat4_is_ident(MATRIX4_IDENT), "mat4_is_ident identity");
        MATH_CHECK(!mat4_is_ident(MATRIX4_ZERO), "mat4_is_ident zero != ident");
    }

    inline void TestMat4_IsNan()
    {
        printf("[TestMat4_IsNan]\n");

        MATH_CHECK(!mat4_is_nan(MATRIX4_IDENT), "mat4_is_nan identity -> false");

        MATRIX4 mnan = MATRIX4_IDENT;
        mnan.m00 = std::nanf("");
        MATH_CHECK(mat4_is_nan(mnan), "mat4_is_nan with NaN -> true");
    }

    inline void TestMat4_IsInfinite()
    {
        printf("[TestMat4_IsInfinite]\n");

        MATH_CHECK(!mat4_is_infinite(MATRIX4_IDENT), "mat4_is_infinite identity -> false");

        MATRIX4 minf = MATRIX4_IDENT;
        minf.m00 = INFINITY;
        MATH_CHECK(mat4_is_infinite(minf), "mat4_is_infinite with INF -> true");
    }

    inline void TestMat4_Set()
    {
        printf("[TestMat4_Set]\n");

        MATRIX4 m = mat4_set(
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f);
        MATH_CHECK(test_math_detail::nearly_equal(m.m00, 1.0f) && test_math_detail::nearly_equal(m.m33, 16.0f), "mat4_set return");

        MATRIX4 m2;
        mat4_set(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f, m2);
        MATH_CHECK(mat4_is_ident(m2), "mat4_set out identity");
    }

    inline void TestMat4_Trans()
    {
        printf("[TestMat4_Trans]\n");

        FLOAT3 pos = { 10.0f, 20.0f, 30.0f };
        MATRIX4 t = mat4_trans(pos);
        MATH_CHECK(test_math_detail::nearly_equal(t.m30, 10.0f) && test_math_detail::nearly_equal(t.m31, 20.0f) && test_math_detail::nearly_equal(t.m32, 30.0f), "mat4_trans FLOAT3 return");

        MATRIX4 t2;
        mat4_trans(pos, t2);
        MATH_CHECK(test_math_detail::nearly_equal(t2.m30, 10.0f), "mat4_trans FLOAT3 out");

        MATRIX4 t3 = mat4_trans(1.0f, 2.0f, 3.0f);
        MATH_CHECK(test_math_detail::nearly_equal(t3.m30, 1.0f) && test_math_detail::nearly_equal(t3.m31, 2.0f) && test_math_detail::nearly_equal(t3.m32, 3.0f), "mat4_trans float xyz return");

        MATRIX4 t4;
        mat4_trans(4.0f, 5.0f, 6.0f, t4);
        MATH_CHECK(test_math_detail::nearly_equal(t4.m30, 4.0f), "mat4_trans float xyz out");
    }

    inline void TestMat4_GetTrans()
    {
        printf("[TestMat4_GetTrans]\n");

        MATRIX4 m = mat4_trans(7.0f, 8.0f, 9.0f);
        FLOAT3 p = mat4_get_trans(m);
        MATH_CHECK(test_math_detail::nearly_equal(p.x, 7.0f) && test_math_detail::nearly_equal(p.y, 8.0f) && test_math_detail::nearly_equal(p.z, 9.0f), "mat4_get_trans return");

        FLOAT3 p2;
        mat4_get_trans(m, p2);
        MATH_CHECK(test_math_detail::nearly_equal(p2.x, 7.0f), "mat4_get_trans out");
    }

    inline void TestMat4_Transpose()
    {
        printf("[TestMat4_Transpose]\n");

        MATRIX4 m = mat4_set(
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f);
        MATRIX4 t = mat4_transpose(m);
        MATH_CHECK(test_math_detail::nearly_equal(t.m01, 5.0f) && test_math_detail::nearly_equal(t.m10, 2.0f), "mat4_transpose return");

        MATRIX4 t2;
        mat4_transpose(m, t2);
        MATH_CHECK(test_math_detail::nearly_equal(t2.m01, 5.0f) && test_math_detail::nearly_equal(t2.m10, 2.0f), "mat4_transpose out");

        // Double transpose == original
        MATRIX4 tt = mat4_transpose(t);
        MATH_CHECK(test_math_detail::nearly_equal(tt.m00, m.m00) && test_math_detail::nearly_equal(tt.m33, m.m33), "mat4_transpose double == original");
    }

    inline void TestMat4_Inverse()
    {
        printf("[TestMat4_Inverse]\n");

        MATRIX4 t = mat4_trans(3.0f, 5.0f, 7.0f);
        MATRIX4 ti = mat4_inverse(t);
        MATRIX4 prod = mat4_mul(t, ti);
        MATH_CHECK(test_math_detail::nearly_equal(prod.m00, 1.0f, 0.001f) && test_math_detail::nearly_equal(prod.m30, 0.0f, 0.001f) && test_math_detail::nearly_equal(prod.m33, 1.0f, 0.001f), "mat4_inverse T*T^-1 == I");

        MATRIX4 ti2;
        mat4_inverse(t, ti2);
        MATH_CHECK(test_math_detail::nearly_equal(ti2.m30, -3.0f, 0.001f), "mat4_inverse out");
    }

    inline void TestMat4_Mul()
    {
        printf("[TestMat4_Mul]\n");

        // Matrix * Matrix : I * M = M
        MATRIX4 m = mat4_trans(1.0f, 2.0f, 3.0f);
        MATRIX4 r = mat4_mul(MATRIX4_IDENT, m);
        MATH_CHECK(test_math_detail::nearly_equal(r.m30, 1.0f) && test_math_detail::nearly_equal(r.m31, 2.0f) && test_math_detail::nearly_equal(r.m32, 3.0f), "mat4_mul I*M == M");

        MATRIX4 r2;
        mat4_mul(MATRIX4_IDENT, m, r2);
        MATH_CHECK(test_math_detail::nearly_equal(r2.m30, 1.0f), "mat4_mul I*M out");

        // Matrix * FLOAT3 : translation applied to origin
        FLOAT3 origin = { 0.0f, 0.0f, 0.0f };
        FLOAT3 rv3 = mat4_mul(m, origin);
        MATH_CHECK(test_math_detail::nearly_equal(rv3.x, 1.0f) && test_math_detail::nearly_equal(rv3.y, 2.0f) && test_math_detail::nearly_equal(rv3.z, 3.0f), "mat4_mul M*FLOAT3");

        FLOAT3 rv3out;
        mat4_mul(m, origin, rv3out);
        MATH_CHECK(test_math_detail::nearly_equal(rv3out.x, 1.0f), "mat4_mul M*FLOAT3 out");

        // Matrix * FLOAT4
        FLOAT4 v4 = { 0.0f, 0.0f, 0.0f, 1.0f };
        FLOAT4 rv4 = mat4_mul(m, v4);
        MATH_CHECK(test_math_detail::nearly_equal(rv4.x, 1.0f) && test_math_detail::nearly_equal(rv4.y, 2.0f) && test_math_detail::nearly_equal(rv4.z, 3.0f), "mat4_mul M*FLOAT4");

        FLOAT4 rv4out;
        mat4_mul(m, v4, rv4out);
        MATH_CHECK(test_math_detail::nearly_equal(rv4out.x, 1.0f), "mat4_mul M*FLOAT4 out");
    }

    inline void TestMat4_Scale()
    {
        printf("[TestMat4_Scale]\n");

        MATRIX4 s = mat4_scale(2.0f, 3.0f, 4.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s.m00, 2.0f) && test_math_detail::nearly_equal(s.m11, 3.0f) && test_math_detail::nearly_equal(s.m22, 4.0f), "mat4_scale float return");

        MATRIX4 s2;
        mat4_scale(5.0f, 6.0f, 7.0f, s2);
        MATH_CHECK(test_math_detail::nearly_equal(s2.m00, 5.0f), "mat4_scale float out");

        FLOAT3 sv = { 2.0f, 3.0f, 4.0f };
        MATRIX4 s3 = mat4_scale(sv);
        MATH_CHECK(test_math_detail::nearly_equal(s3.m00, 2.0f), "mat4_scale FLOAT3 return");

        MATRIX4 s4;
        mat4_scale(sv, s4);
        MATH_CHECK(test_math_detail::nearly_equal(s4.m00, 2.0f), "mat4_scale FLOAT3 out");

        // Scale applied to vector
        FLOAT3 v = { 1.0f, 1.0f, 1.0f };
        FLOAT3 scaled = mat4_mul(s, v);
        MATH_CHECK(test_math_detail::nearly_equal(scaled.x, 2.0f) && test_math_detail::nearly_equal(scaled.y, 3.0f) && test_math_detail::nearly_equal(scaled.z, 4.0f), "mat4_scale applied to vector");
    }

    inline void TestMat4_Rotation()
    {
        printf("[TestMat4_Rotation]\n");

        // Rotation X : 90 degrees -> (0,1,0) should become (0,0,1)
        {
            MATRIX4 rx = mat4_rotation_x(M_PIDIV2);
            FLOAT3 v = { 0.0f, 1.0f, 0.0f };
            FLOAT3 rv = mat4_mul(rx, v);
            MATH_CHECK(test_math_detail::nearly_equal(rv.y, 0.0f, 0.01f) && test_math_detail::nearly_equal(rv.z, 1.0f, 0.01f), "mat4_rotation_x 90deg");

            MATRIX4 rx2;
            mat4_rotation_x(M_PIDIV2, rx2);
            FLOAT3 rv2 = mat4_mul(rx2, v);
            MATH_CHECK(test_math_detail::nearly_equal(rv2.z, 1.0f, 0.01f), "mat4_rotation_x 90deg out");
        }

        // Rotation Y : 90 degrees -> (1,0,0) should become (0,0,-1)
        {
            MATRIX4 ry = mat4_rotation_y(M_PIDIV2);
            FLOAT3 v = { 1.0f, 0.0f, 0.0f };
            FLOAT3 rv = mat4_mul(ry, v);
            MATH_CHECK(test_math_detail::nearly_equal(rv.x, 0.0f, 0.01f) && test_math_detail::nearly_equal(rv.z, -1.0f, 0.01f), "mat4_rotation_y 90deg");

            MATRIX4 ry2;
            mat4_rotation_y(M_PIDIV2, ry2);
            FLOAT3 rv2 = mat4_mul(ry2, v);
            MATH_CHECK(test_math_detail::nearly_equal(rv2.z, -1.0f, 0.01f), "mat4_rotation_y 90deg out");
        }

        // Rotation Z : 90 degrees -> (1,0,0) should become (0,1,0)
        {
            MATRIX4 rz = mat4_rotation_z(M_PIDIV2);
            FLOAT3 v = { 1.0f, 0.0f, 0.0f };
            FLOAT3 rv = mat4_mul(rz, v);
            MATH_CHECK(test_math_detail::nearly_equal(rv.x, 0.0f, 0.01f) && test_math_detail::nearly_equal(rv.y, 1.0f, 0.01f), "mat4_rotation_z 90deg");

            MATRIX4 rz2;
            mat4_rotation_z(M_PIDIV2, rz2);
            FLOAT3 rv2 = mat4_mul(rz2, v);
            MATH_CHECK(test_math_detail::nearly_equal(rv2.y, 1.0f, 0.01f), "mat4_rotation_z 90deg out");
        }
    }

    inline void TestMat4_RotationYawPitchRoll()
    {
        printf("[TestMat4_RotationYawPitchRoll]\n");

        MATRIX4 m = mat4_rotation_yaw_pitch_roll(0.0f, 0.0f, 0.0f);
        MATH_CHECK(mat4_is_ident(m), "mat4_rotation_yaw_pitch_roll (0,0,0) == identity");

        MATRIX4 m2;
        mat4_rotation_yaw_pitch_roll(0.0f, 0.0f, 0.0f, m2);
        MATH_CHECK(mat4_is_ident(m2), "mat4_rotation_yaw_pitch_roll (0,0,0) out == identity");

        MATRIX4 m3 = mat4_rotation_yaw_pitch_roll(M_PIDIV2, 0.0f, 0.0f);
        MATH_CHECK(!mat4_is_ident(m3), "mat4_rotation_yaw_pitch_roll non-zero != identity");
    }

    inline void TestMat4_RotationAxis()
    {
        printf("[TestMat4_RotationAxis]\n");

        FLOAT3 axis = { 0.0f, 1.0f, 0.0f };
        MATRIX4 m = mat4_rotation_axis(axis, M_PI);
        FLOAT3 v = { 1.0f, 0.0f, 0.0f };
        FLOAT3 rv = mat4_mul(m, v);
        MATH_CHECK(test_math_detail::nearly_equal(rv.x, -1.0f, 0.01f), "mat4_rotation_axis Y 180deg");

        MATRIX4 m2;
        mat4_rotation_axis(axis, M_PI, m2);
        FLOAT3 rv2 = mat4_mul(m2, v);
        MATH_CHECK(test_math_detail::nearly_equal(rv2.x, -1.0f, 0.01f), "mat4_rotation_axis Y 180deg out");
    }

    inline void TestMat4_RotationQuat()
    {
        printf("[TestMat4_RotationQuat]\n");

        QUAT q = quat_rotation_axis_angle(FLOAT3{ 0.0f, 0.0f, 1.0f }, M_PIDIV2);
        MATRIX4 m = mat4_rotation_quat(q);
        FLOAT3 v = { 1.0f, 0.0f, 0.0f };
        FLOAT3 rv = mat4_mul(m, v);
        MATH_CHECK(test_math_detail::nearly_equal(rv.x, 0.0f, 0.01f) && test_math_detail::nearly_equal(rv.y, 1.0f, 0.01f), "mat4_rotation_quat return");

        MATRIX4 m2;
        mat4_rotation_quat(q, m2);
        FLOAT3 rv2 = mat4_mul(m2, v);
        MATH_CHECK(test_math_detail::nearly_equal(rv2.y, 1.0f, 0.01f), "mat4_rotation_quat out");
    }

    inline void TestMat4_LookAt()
    {
        printf("[TestMat4_LookAt]\n");

        FLOAT3 eye = { 0.0f, 0.0f, -5.0f };
        FLOAT3 dir = { 0.0f, 0.0f, 1.0f };
        FLOAT3 up = { 0.0f, 1.0f, 0.0f };

        MATRIX4 lh = mat4_lookat_lh(eye, dir, up);
        MATH_CHECK(!mat4_is_nan(lh) && !mat4_is_infinite(lh), "mat4_lookat_lh valid");

        MATRIX4 lh2;
        mat4_lookat_lh(eye, dir, up, lh2);
        MATH_CHECK(test_math_detail::nearly_equal(lh.m00, lh2.m00, 0.001f), "mat4_lookat_lh out matches return");
    }

    inline void TestMat4_LookTo()
    {
        printf("[TestMat4_LookTo]\n");

        FLOAT3 eye = { 0.0f, 0.0f, 0.0f };
        FLOAT3 target = { 0.0f, 0.0f, 10.0f };
        FLOAT3 up = { 0.0f, 1.0f, 0.0f };

        // LH
        MATRIX4 lh = mat4_lookto_lh(eye, target, up);
        MATH_CHECK(!mat4_is_nan(lh) && !mat4_is_infinite(lh), "mat4_lookto_lh valid");

        MATRIX4 lh2;
        mat4_lookto_lh(eye, target, up, lh2);
        MATH_CHECK(test_math_detail::nearly_equal(lh.m00, lh2.m00, 0.001f), "mat4_lookto_lh out matches return");

        // RH
        MATRIX4 rh = mat4_lookto_rh(eye, target, up);
        MATH_CHECK(!mat4_is_nan(rh) && !mat4_is_infinite(rh), "mat4_lookto_rh valid");

        MATRIX4 rh2;
        mat4_lookto_rh(eye, target, up, rh2);
        MATH_CHECK(test_math_detail::nearly_equal(rh.m00, rh2.m00, 0.001f), "mat4_lookto_rh out matches return");
    }

    inline void TestMat4_Perspective()
    {
        printf("[TestMat4_Perspective]\n");

        float fov = M_PI / 4.0f;
        float aspect = 16.0f / 9.0f;
        float nearZ = 0.1f;
        float farZ = 1000.0f;

        // FOV-based
        MATRIX4 plh = mat4_perspective_lh(fov, aspect, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(plh) && !mat4_is_infinite(plh), "mat4_perspective_lh fov valid");

        MATRIX4 plh2;
        mat4_perspective_lh(fov, aspect, nearZ, farZ, plh2);
        MATH_CHECK(test_math_detail::nearly_equal(plh.m00, plh2.m00, 0.001f), "mat4_perspective_lh fov out");

        MATRIX4 prh = mat4_perspective_rh(fov, aspect, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(prh) && !mat4_is_infinite(prh), "mat4_perspective_rh fov valid");

        MATRIX4 prh2;
        mat4_perspective_rh(fov, aspect, nearZ, farZ, prh2);
        MATH_CHECK(test_math_detail::nearly_equal(prh.m00, prh2.m00, 0.001f), "mat4_perspective_rh fov out");

        // LRTB-based
        MATRIX4 plh_lr = mat4_perspective_lh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(plh_lr), "mat4_perspective_lh LRTB valid");

        MATRIX4 plh_lr2;
        mat4_perspective_lh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ, plh_lr2);
        MATH_CHECK(test_math_detail::nearly_equal(plh_lr.m00, plh_lr2.m00, 0.001f), "mat4_perspective_lh LRTB out");

        MATRIX4 prh_lr = mat4_perspective_rh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(prh_lr), "mat4_perspective_rh LRTB valid");

        MATRIX4 prh_lr2;
        mat4_perspective_rh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ, prh_lr2);
        MATH_CHECK(test_math_detail::nearly_equal(prh_lr.m00, prh_lr2.m00, 0.001f), "mat4_perspective_rh LRTB out");
    }

    inline void TestMat4_Ortho()
    {
        printf("[TestMat4_Ortho]\n");

        float nearZ = 0.1f, farZ = 100.0f;

        // Width/Height based
        MATRIX4 olh = mat4_ortho_lh(800.0f, 600.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(olh), "mat4_ortho_lh WH valid");

        MATRIX4 olh2;
        mat4_ortho_lh(800.0f, 600.0f, nearZ, farZ, olh2);
        MATH_CHECK(test_math_detail::nearly_equal(olh.m00, olh2.m00, 0.001f), "mat4_ortho_lh WH out");

        MATRIX4 orh = mat4_ortho_rh(800.0f, 600.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(orh), "mat4_ortho_rh WH valid");

        MATRIX4 orh2;
        mat4_ortho_rh(800.0f, 600.0f, nearZ, farZ, orh2);
        MATH_CHECK(test_math_detail::nearly_equal(orh.m00, orh2.m00, 0.001f), "mat4_ortho_rh WH out");

        // LRTB based
        MATRIX4 olh_lr = mat4_ortho_lh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(olh_lr), "mat4_ortho_lh LRTB valid");

        MATRIX4 olh_lr2;
        mat4_ortho_lh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ, olh_lr2);
        MATH_CHECK(test_math_detail::nearly_equal(olh_lr.m00, olh_lr2.m00, 0.001f), "mat4_ortho_lh LRTB out");

        MATRIX4 orh_lr = mat4_ortho_rh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(orh_lr), "mat4_ortho_rh LRTB valid");

        MATRIX4 orh_lr2;
        mat4_ortho_rh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ, orh_lr2);
        MATH_CHECK(test_math_detail::nearly_equal(orh_lr.m00, orh_lr2.m00, 0.001f), "mat4_ortho_rh LRTB out");
    }

    inline void TestMat4_Frustum()
    {
        printf("[TestMat4_Frustum]\n");

        float nearZ = 0.1f, farZ = 100.0f;

        MATRIX4 flh = mat4_frustum_lh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(flh), "mat4_frustum_lh valid");

        MATRIX4 flh2;
        mat4_frustum_lh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ, flh2);
        MATH_CHECK(test_math_detail::nearly_equal(flh.m00, flh2.m00, 0.001f), "mat4_frustum_lh out");

        MATRIX4 frh = mat4_frustum_rh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ);
        MATH_CHECK(!mat4_is_nan(frh), "mat4_frustum_rh valid");

        MATRIX4 frh2;
        mat4_frustum_rh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ, frh2);
        MATH_CHECK(test_math_detail::nearly_equal(frh.m00, frh2.m00, 0.001f), "mat4_frustum_rh out");
    }

    // ========================================================================
    // SSE (XFLOAT / XQUAT / XMATRIX4) Operations
    // ========================================================================
#if defined(__TARGET_OS_WINDOWS) && defined(__SSE__)

    inline void TestXVec2()
    {
        printf("[TestXVec2]\n");

        XFLOAT2 z = xvec2_zero();
        MATH_CHECK(test_math_detail::nearly_equal(z.x, 0.0f) && test_math_detail::nearly_equal(z.y, 0.0f), "xvec2_zero");

        XFLOAT2 o = xvec2_one();
        MATH_CHECK(test_math_detail::nearly_equal(o.x, 1.0f) && test_math_detail::nearly_equal(o.y, 1.0f), "xvec2_one");

        XFLOAT2 ux = xvec2_unit_x();
        MATH_CHECK(test_math_detail::nearly_equal(ux.x, 1.0f) && test_math_detail::nearly_equal(ux.y, 0.0f), "xvec2_unit_x");

        XFLOAT2 uy = xvec2_unit_y();
        MATH_CHECK(test_math_detail::nearly_equal(uy.x, 0.0f) && test_math_detail::nearly_equal(uy.y, 1.0f), "xvec2_unit_y");

        XFLOAT2 s = xvec2_set(3.0f, 4.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s.x, 3.0f) && test_math_detail::nearly_equal(s.y, 4.0f), "xvec2_set");

        XFLOAT2 a = xvec2_set(1.0f, 2.0f), b = xvec2_set(3.0f, 4.0f);

        XFLOAT2 radd = xvec2_add(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(radd.x, 4.0f) && test_math_detail::nearly_equal(radd.y, 6.0f), "xvec2_add");

        XFLOAT2 rsub = xvec2_sub(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(rsub.x, -2.0f) && test_math_detail::nearly_equal(rsub.y, -2.0f), "xvec2_sub");

        XFLOAT2 rmul = xvec2_mul(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(rmul.x, 3.0f) && test_math_detail::nearly_equal(rmul.y, 8.0f), "xvec2_mul component");

        XFLOAT2 rmuls = xvec2_mul(a, 3.0f);
        MATH_CHECK(test_math_detail::nearly_equal(rmuls.x, 3.0f) && test_math_detail::nearly_equal(rmuls.y, 6.0f), "xvec2_mul scalar");

        XFLOAT2 rdiv = xvec2_div(b, a);
        MATH_CHECK(test_math_detail::nearly_equal(rdiv.x, 3.0f) && test_math_detail::nearly_equal(rdiv.y, 2.0f), "xvec2_div component");

        XFLOAT2 rdivs = xvec2_div(b, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(rdivs.x, 1.5f) && test_math_detail::nearly_equal(rdivs.y, 2.0f), "xvec2_div scalar");

        float df = xvec2_dot_f(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(df, 11.0f), "xvec2_dot_f");

        XFLOAT2 dv = xvec2_dot(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(dv.x, 11.0f), "xvec2_dot");

        XFLOAT2 c2 = xvec2_cross(xvec2_set(1.0f, 0.0f), xvec2_set(0.0f, 1.0f));
        MATH_CHECK(test_math_detail::nearly_equal(c2.x, 1.0f), "xvec2_cross");

        XFLOAT2 v34 = xvec2_set(3.0f, 4.0f);
        float lsqf = xvec2_lengthsq_f(v34);
        MATH_CHECK(test_math_detail::nearly_equal(lsqf, 25.0f), "xvec2_lengthsq_f");

        float lf = xvec2_length_f(v34);
        MATH_CHECK(test_math_detail::nearly_equal(lf, 5.0f), "xvec2_length_f");

        XFLOAT2 lv = xvec2_length(v34);
        MATH_CHECK(test_math_detail::nearly_equal(lv.x, 5.0f), "xvec2_length");

        XFLOAT2 n2 = xvec2_normalize(v34);
        float nlen = xvec2_length_f(n2);
        MATH_CHECK(test_math_detail::nearly_equal(nlen, 1.0f, 0.001f), "xvec2_normalize");

        XFLOAT2 la = xvec2_set(0.0f, 0.0f), lb = xvec2_set(10.0f, 20.0f);
        XFLOAT2 lr = xvec2_lerp(la, lb, 0.5f);
        MATH_CHECK(test_math_detail::nearly_equal(lr.x, 5.0f) && test_math_detail::nearly_equal(lr.y, 10.0f), "xvec2_lerp");

        XFLOAT2 vc = xvec2_set(-1.0f, 5.0f);
        XFLOAT2 vcr = xvec2_clamp(vc, xvec2_set(0.0f, 0.0f), xvec2_set(3.0f, 3.0f));
        MATH_CHECK(test_math_detail::nearly_equal(vcr.x, 0.0f) && test_math_detail::nearly_equal(vcr.y, 3.0f), "xvec2_clamp");

        XFLOAT2 vs = xvec2_set(-0.5f, 1.5f);
        XFLOAT2 vsr = xvec2_saturate(vs);
        MATH_CHECK(test_math_detail::nearly_equal(vsr.x, 0.0f) && test_math_detail::nearly_equal(vsr.y, 1.0f), "xvec2_saturate");

        XFLOAT2 vmin = xvec2_min(xvec2_set(1.0f, 5.0f), xvec2_set(3.0f, 2.0f));
        MATH_CHECK(test_math_detail::nearly_equal(vmin.x, 1.0f) && test_math_detail::nearly_equal(vmin.y, 2.0f), "xvec2_min");

        XFLOAT2 vmax = xvec2_max(xvec2_set(1.0f, 5.0f), xvec2_set(3.0f, 2.0f));
        MATH_CHECK(test_math_detail::nearly_equal(vmax.x, 3.0f) && test_math_detail::nearly_equal(vmax.y, 5.0f), "xvec2_max");

        XFLOAT2 vce = xvec2_ceil(xvec2_set(1.2f, 2.8f));
        MATH_CHECK(test_math_detail::nearly_equal(vce.x, 2.0f) && test_math_detail::nearly_equal(vce.y, 3.0f), "xvec2_ceil");

        XFLOAT2 vfl = xvec2_floor(xvec2_set(1.9f, 2.1f));
        MATH_CHECK(test_math_detail::nearly_equal(vfl.x, 1.0f) && test_math_detail::nearly_equal(vfl.y, 2.0f), "xvec2_floor");

        XFLOAT2 vro = xvec2_round(xvec2_set(1.4f, 2.6f));
        MATH_CHECK(test_math_detail::nearly_equal(vro.x, 1.0f) && test_math_detail::nearly_equal(vro.y, 3.0f), "xvec2_round");

        XFLOAT2 vtr = xvec2_trunc(xvec2_set(1.9f, -2.9f));
        MATH_CHECK(test_math_detail::nearly_equal(vtr.x, 1.0f) && test_math_detail::nearly_equal(vtr.y, -2.0f), "xvec2_trunc");
    }

    inline void TestXVec3()
    {
        printf("[TestXVec3]\n");

        XFLOAT3 z = xvec3_zero();
        MATH_CHECK(test_math_detail::nearly_equal(z.x, 0.0f) && test_math_detail::nearly_equal(z.y, 0.0f) && test_math_detail::nearly_equal(z.z, 0.0f), "xvec3_zero");

        MATH_CHECK(test_math_detail::nearly_equal(xvec3_unit_x().x, 1.0f), "xvec3_unit_x");
        MATH_CHECK(test_math_detail::nearly_equal(xvec3_unit_y().y, 1.0f), "xvec3_unit_y");
        MATH_CHECK(test_math_detail::nearly_equal(xvec3_unit_z().z, 1.0f), "xvec3_unit_z");

        XFLOAT3 s = xvec3_set(1.0f, 2.0f, 3.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s.x, 1.0f) && test_math_detail::nearly_equal(s.z, 3.0f), "xvec3_set");

        XFLOAT3 a = xvec3_set(1.0f, 2.0f, 3.0f), b = xvec3_set(4.0f, 5.0f, 6.0f);

        XFLOAT3 radd = xvec3_add(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(radd.x, 5.0f) && test_math_detail::nearly_equal(radd.y, 7.0f) && test_math_detail::nearly_equal(radd.z, 9.0f), "xvec3_add");

        XFLOAT3 rsub = xvec3_sub(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(rsub.x, -3.0f), "xvec3_sub");

        XFLOAT3 rmul = xvec3_mul(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(rmul.x, 4.0f) && test_math_detail::nearly_equal(rmul.y, 10.0f) && test_math_detail::nearly_equal(rmul.z, 18.0f), "xvec3_mul component");

        XFLOAT3 rmuls = xvec3_mul(a, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(rmuls.x, 2.0f) && test_math_detail::nearly_equal(rmuls.y, 4.0f) && test_math_detail::nearly_equal(rmuls.z, 6.0f), "xvec3_mul scalar");

        XFLOAT3 rmulo;
        xvec3_mul(a, 2.0f, rmulo);
        MATH_CHECK(test_math_detail::nearly_equal(rmulo.x, 2.0f), "xvec3_mul scalar out");

        XFLOAT3 rdiv = xvec3_div(b, a);
        MATH_CHECK(test_math_detail::nearly_equal(rdiv.x, 4.0f) && test_math_detail::nearly_equal(rdiv.y, 2.5f) && test_math_detail::nearly_equal(rdiv.z, 2.0f), "xvec3_div component");

        XFLOAT3 rdivs = xvec3_div(b, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(rdivs.x, 2.0f) && test_math_detail::nearly_equal(rdivs.y, 2.5f) && test_math_detail::nearly_equal(rdivs.z, 3.0f), "xvec3_div scalar");

        XFLOAT3 rdivo;
        xvec3_div(b, 2.0f, rdivo);
        MATH_CHECK(test_math_detail::nearly_equal(rdivo.x, 2.0f), "xvec3_div scalar out");

        float df3 = xvec3_dot_f(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(df3, 32.0f), "xvec3_dot_f");

        XFLOAT3 dv3 = xvec3_dot(a, b);
        MATH_CHECK(test_math_detail::nearly_equal(dv3.x, 32.0f), "xvec3_dot");

        XFLOAT3 cx = xvec3_cross(xvec3_set(1.0f, 0.0f, 0.0f), xvec3_set(0.0f, 1.0f, 0.0f));
        MATH_CHECK(test_math_detail::nearly_equal(cx.z, 1.0f), "xvec3_cross X x Y = Z");

        XFLOAT3 v122 = xvec3_set(1.0f, 2.0f, 2.0f);
        float lsq3 = xvec3_lengthsq_f(v122);
        MATH_CHECK(test_math_detail::nearly_equal(lsq3, 9.0f), "xvec3_lengthsq_f");

        XFLOAT3 lsqv3 = xvec3_lengthsq(v122);
        MATH_CHECK(test_math_detail::nearly_equal(lsqv3.x, 9.0f), "xvec3_lengthsq");

        float l3 = xvec3_length_f(v122);
        MATH_CHECK(test_math_detail::nearly_equal(l3, 3.0f), "xvec3_length_f");

        XFLOAT3 lv3 = xvec3_length(v122);
        MATH_CHECK(test_math_detail::nearly_equal(lv3.x, 3.0f), "xvec3_length");

        XFLOAT3 n3 = xvec3_normalize(v122);
        MATH_CHECK(test_math_detail::nearly_equal(xvec3_length_f(n3), 1.0f, 0.001f), "xvec3_normalize");

        XFLOAT3 lr3 = xvec3_lerp(xvec3_set(0.0f, 0.0f, 0.0f), xvec3_set(10.0f, 20.0f, 30.0f), 0.5f);
        MATH_CHECK(test_math_detail::nearly_equal(lr3.x, 5.0f) && test_math_detail::nearly_equal(lr3.y, 10.0f) && test_math_detail::nearly_equal(lr3.z, 15.0f), "xvec3_lerp");

        XFLOAT3 cl3 = xvec3_clamp(xvec3_set(-1.0f, 5.0f, 2.0f), xvec3_set(0.0f, 0.0f, 0.0f), xvec3_set(3.0f, 3.0f, 3.0f));
        MATH_CHECK(test_math_detail::nearly_equal(cl3.x, 0.0f) && test_math_detail::nearly_equal(cl3.y, 3.0f) && test_math_detail::nearly_equal(cl3.z, 2.0f), "xvec3_clamp");

        XFLOAT3 sa3 = xvec3_saturate(xvec3_set(-0.5f, 0.5f, 1.5f));
        MATH_CHECK(test_math_detail::nearly_equal(sa3.x, 0.0f) && test_math_detail::nearly_equal(sa3.y, 0.5f) && test_math_detail::nearly_equal(sa3.z, 1.0f), "xvec3_saturate");

        XFLOAT3 mn3 = xvec3_min(xvec3_set(1.0f, 5.0f, 3.0f), xvec3_set(3.0f, 2.0f, 4.0f));
        MATH_CHECK(test_math_detail::nearly_equal(mn3.x, 1.0f) && test_math_detail::nearly_equal(mn3.y, 2.0f) && test_math_detail::nearly_equal(mn3.z, 3.0f), "xvec3_min");

        XFLOAT3 mx3 = xvec3_max(xvec3_set(1.0f, 5.0f, 3.0f), xvec3_set(3.0f, 2.0f, 4.0f));
        MATH_CHECK(test_math_detail::nearly_equal(mx3.x, 3.0f) && test_math_detail::nearly_equal(mx3.y, 5.0f) && test_math_detail::nearly_equal(mx3.z, 4.0f), "xvec3_max");

        XFLOAT3 ce3 = xvec3_ceil(xvec3_set(1.2f, 2.8f, 3.1f));
        MATH_CHECK(test_math_detail::nearly_equal(ce3.x, 2.0f) && test_math_detail::nearly_equal(ce3.y, 3.0f) && test_math_detail::nearly_equal(ce3.z, 4.0f), "xvec3_ceil");

        XFLOAT3 fl3 = xvec3_floor(xvec3_set(1.9f, 2.1f, 3.5f));
        MATH_CHECK(test_math_detail::nearly_equal(fl3.x, 1.0f) && test_math_detail::nearly_equal(fl3.y, 2.0f) && test_math_detail::nearly_equal(fl3.z, 3.0f), "xvec3_floor");

        XFLOAT3 ro3 = xvec3_round(xvec3_set(1.4f, 2.6f, 3.5f));
        MATH_CHECK(test_math_detail::nearly_equal(ro3.x, 1.0f) && test_math_detail::nearly_equal(ro3.y, 3.0f), "xvec3_round");

        XFLOAT3 tr3 = xvec3_trunc(xvec3_set(1.9f, -2.9f, 3.1f));
        MATH_CHECK(test_math_detail::nearly_equal(tr3.x, 1.0f) && test_math_detail::nearly_equal(tr3.y, -2.0f) && test_math_detail::nearly_equal(tr3.z, 3.0f), "xvec3_trunc");
    }

    inline void TestXVec4()
    {
        printf("[TestXVec4]\n");

        XFLOAT4 z = xvec4_zero();
        MATH_CHECK(test_math_detail::nearly_equal(z.x, 0.0f) && test_math_detail::nearly_equal(z.w, 0.0f), "xvec4_zero");

        MATH_CHECK(test_math_detail::nearly_equal(xvec4_unit_x().x, 1.0f), "xvec4_unit_x");
        MATH_CHECK(test_math_detail::nearly_equal(xvec4_unit_y().y, 1.0f), "xvec4_unit_y");
        MATH_CHECK(test_math_detail::nearly_equal(xvec4_unit_z().z, 1.0f), "xvec4_unit_z");
        MATH_CHECK(test_math_detail::nearly_equal(xvec4_unit_w().w, 1.0f), "xvec4_unit_w");

        XFLOAT4 s4 = xvec4_set(1.0f, 2.0f, 3.0f, 4.0f);
        MATH_CHECK(test_math_detail::nearly_equal(s4.x, 1.0f) && test_math_detail::nearly_equal(s4.w, 4.0f), "xvec4_set");

        XFLOAT4 a4 = xvec4_set(1.0f, 2.0f, 3.0f, 4.0f), b4 = xvec4_set(5.0f, 6.0f, 7.0f, 8.0f);

        XFLOAT4 radd4 = xvec4_add(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(radd4.x, 6.0f) && test_math_detail::nearly_equal(radd4.w, 12.0f), "xvec4_add");

        XFLOAT4 rsub4 = xvec4_sub(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(rsub4.x, -4.0f) && test_math_detail::nearly_equal(rsub4.w, -4.0f), "xvec4_sub");

        XFLOAT4 rmul4 = xvec4_mul(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(rmul4.x, 5.0f) && test_math_detail::nearly_equal(rmul4.w, 32.0f), "xvec4_mul component");

        XFLOAT4 rmuls4 = xvec4_mul(a4, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(rmuls4.x, 2.0f) && test_math_detail::nearly_equal(rmuls4.w, 8.0f), "xvec4_mul scalar");

        XFLOAT4 rdiv4 = xvec4_div(b4, a4);
        MATH_CHECK(test_math_detail::nearly_equal(rdiv4.x, 5.0f) && test_math_detail::nearly_equal(rdiv4.y, 3.0f), "xvec4_div component");

        XFLOAT4 rdivs4 = xvec4_div(b4, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(rdivs4.x, 2.5f) && test_math_detail::nearly_equal(rdivs4.w, 4.0f), "xvec4_div scalar");

        float df4 = xvec4_dot_f(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(df4, 70.0f), "xvec4_dot_f");

        XFLOAT4 dv4 = xvec4_dot(a4, b4);
        MATH_CHECK(test_math_detail::nearly_equal(dv4.x, 70.0f), "xvec4_dot");

        XFLOAT4 va4 = xvec4_set(1.0f, 0.0f, 0.0f, 0.0f);
        XFLOAT4 vb4 = xvec4_set(0.0f, 1.0f, 0.0f, 0.0f);
        XFLOAT4 vc4 = xvec4_set(0.0f, 0.0f, 1.0f, 0.0f);
        XFLOAT4 cx4 = xvec4_cross(va4, vb4, vc4);
        MATH_CHECK(test_math_detail::nearly_equal(cx4.w, -1.0f, 0.001f), "xvec4_cross");

        XFLOAT4 v1234 = xvec4_set(1.0f, 2.0f, 3.0f, 4.0f);
        float lsq4 = xvec4_lengthsq_f(v1234);
        MATH_CHECK(test_math_detail::nearly_equal(lsq4, 30.0f), "xvec4_lengthsq_f");

        XFLOAT4 lsqv4 = xvec4_lengthsq(v1234);
        MATH_CHECK(test_math_detail::nearly_equal(lsqv4.x, 30.0f), "xvec4_lengthsq");

        float l4 = xvec4_length_f(v1234);
        MATH_CHECK(test_math_detail::nearly_equal(l4, std::sqrt(30.0f), 0.001f), "xvec4_length_f");

        XFLOAT4 lv4 = xvec4_length(v1234);
        MATH_CHECK(test_math_detail::nearly_equal(lv4.x, std::sqrt(30.0f), 0.001f), "xvec4_length");

        XFLOAT4 n4 = xvec4_normalize(v1234);
        MATH_CHECK(test_math_detail::nearly_equal(xvec4_length_f(n4), 1.0f, 0.001f), "xvec4_normalize");

        XFLOAT4 lr4 = xvec4_lerp(xvec4_set(0.0f, 0.0f, 0.0f, 0.0f), xvec4_set(4.0f, 8.0f, 12.0f, 16.0f), 0.5f);
        MATH_CHECK(test_math_detail::nearly_equal(lr4.x, 2.0f) && test_math_detail::nearly_equal(lr4.w, 8.0f), "xvec4_lerp");

        XFLOAT4 cl4 = xvec4_clamp(xvec4_set(-1.0f, 5.0f, 2.0f, 8.0f), xvec4_set(0.0f, 0.0f, 0.0f, 0.0f), xvec4_set(3.0f, 3.0f, 3.0f, 3.0f));
        MATH_CHECK(test_math_detail::nearly_equal(cl4.x, 0.0f) && test_math_detail::nearly_equal(cl4.y, 3.0f) && test_math_detail::nearly_equal(cl4.w, 3.0f), "xvec4_clamp");

        XFLOAT4 sa4 = xvec4_saturate(xvec4_set(-0.5f, 0.5f, 1.5f, 0.3f));
        MATH_CHECK(test_math_detail::nearly_equal(sa4.x, 0.0f) && test_math_detail::nearly_equal(sa4.y, 0.5f) && test_math_detail::nearly_equal(sa4.z, 1.0f) && test_math_detail::nearly_equal(sa4.w, 0.3f), "xvec4_saturate");

        XFLOAT4 mn4 = xvec4_min(xvec4_set(1.0f, 5.0f, 3.0f, 8.0f), xvec4_set(3.0f, 2.0f, 4.0f, 6.0f));
        MATH_CHECK(test_math_detail::nearly_equal(mn4.x, 1.0f) && test_math_detail::nearly_equal(mn4.y, 2.0f) && test_math_detail::nearly_equal(mn4.w, 6.0f), "xvec4_min");

        XFLOAT4 mx4 = xvec4_max(xvec4_set(1.0f, 5.0f, 3.0f, 8.0f), xvec4_set(3.0f, 2.0f, 4.0f, 6.0f));
        MATH_CHECK(test_math_detail::nearly_equal(mx4.x, 3.0f) && test_math_detail::nearly_equal(mx4.y, 5.0f) && test_math_detail::nearly_equal(mx4.w, 8.0f), "xvec4_max");

        XFLOAT4 ce4 = xvec4_ceil(xvec4_set(1.2f, 2.8f, -0.5f, 3.0f));
        MATH_CHECK(test_math_detail::nearly_equal(ce4.x, 2.0f) && test_math_detail::nearly_equal(ce4.y, 3.0f) && test_math_detail::nearly_equal(ce4.z, 0.0f), "xvec4_ceil");

        XFLOAT4 fl4 = xvec4_floor(xvec4_set(1.9f, 2.1f, -0.1f, 3.0f));
        MATH_CHECK(test_math_detail::nearly_equal(fl4.x, 1.0f) && test_math_detail::nearly_equal(fl4.y, 2.0f) && test_math_detail::nearly_equal(fl4.z, -1.0f), "xvec4_floor");

        XFLOAT4 ro4 = xvec4_round(xvec4_set(1.4f, 2.6f, 3.5f, 4.1f));
        MATH_CHECK(test_math_detail::nearly_equal(ro4.x, 1.0f) && test_math_detail::nearly_equal(ro4.y, 3.0f), "xvec4_round");

        XFLOAT4 tr4 = xvec4_trunc(xvec4_set(1.9f, -2.9f, 3.1f, -4.5f));
        MATH_CHECK(test_math_detail::nearly_equal(tr4.x, 1.0f) && test_math_detail::nearly_equal(tr4.y, -2.0f) && test_math_detail::nearly_equal(tr4.z, 3.0f) && test_math_detail::nearly_equal(tr4.w, -4.0f), "xvec4_trunc");
    }

    inline void TestXQuat()
    {
        printf("[TestXQuat]\n");

        XQUAT qz = xquat_zero();
        MATH_CHECK(test_math_detail::nearly_equal(qz.x, 0.0f) && test_math_detail::nearly_equal(qz.w, 0.0f), "xquat_zero");

        XQUAT qi = xquat_ident();
        MATH_CHECK(test_math_detail::nearly_equal(qi.x, 0.0f) && test_math_detail::nearly_equal(qi.y, 0.0f) && test_math_detail::nearly_equal(qi.z, 0.0f) && test_math_detail::nearly_equal(qi.w, 1.0f), "xquat_ident");

        XQUAT qs = xquat_set(1.0f, 2.0f, 3.0f, 4.0f);
        MATH_CHECK(test_math_detail::nearly_equal(qs.x, 1.0f) && test_math_detail::nearly_equal(qs.w, 4.0f), "xquat_set");

        // Quat * Quat: identity * q = q
        XQUAT qmul = xquat_mul(qi, qs);
        MATH_CHECK(test_math_detail::nearly_equal(qmul.x, qs.x) && test_math_detail::nearly_equal(qmul.w, qs.w), "xquat_mul identity*q == q");

        // Quat * scalar
        XQUAT qms = xquat_mul(qi, 2.0f);
        MATH_CHECK(test_math_detail::nearly_equal(qms.w, 2.0f), "xquat_mul scalar");

        // Quat * Vector: identity should not change vector
        XFLOAT3 v3 = xvec3_set(1.0f, 0.0f, 0.0f);
        XFLOAT3 rv3 = xquat_mul(qi, v3);
        MATH_CHECK(test_math_detail::nearly_equal(rv3.x, 1.0f, 0.001f) && test_math_detail::nearly_equal(rv3.y, 0.0f, 0.001f) && test_math_detail::nearly_equal(rv3.z, 0.0f, 0.001f), "xquat_mul identity*vec");

        // Inverse
        XQUAT qrot = xquat_rotation_axis(M_PIDIV2, xvec3_set(0.0f, 0.0f, 1.0f));
        XQUAT qinv = xquat_inverse(qrot);
        XQUAT qprod = xquat_mul(qrot, qinv);
        MATH_CHECK(test_math_detail::nearly_equal(std::fabs(qprod.w), 1.0f, 0.01f), "xquat_inverse q*q^-1 == identity");

        // Conjugate return
        XQUAT qc = xquat_conjugate(xquat_set(1.0f, 2.0f, 3.0f, 4.0f));
        MATH_CHECK(test_math_detail::nearly_equal(qc.x, -1.0f) && test_math_detail::nearly_equal(qc.w, 4.0f), "xquat_conjugate");

        // Conjugate out
        XQUAT qco;
        xquat_conjugate(xquat_set(1.0f, 2.0f, 3.0f, 4.0f), qco);
        MATH_CHECK(test_math_detail::nearly_equal(qco.x, -1.0f) && test_math_detail::nearly_equal(qco.w, 4.0f), "xquat_conjugate out");

        // Slerp
        XQUAT q0 = xquat_ident();
        XQUAT q1 = xquat_rotation_axis(M_PI, xvec3_set(0.0f, 0.0f, 1.0f));
        XQUAT qs0 = xquat_slerp(q0, q1, 0.0f);
        MATH_CHECK(test_math_detail::nearly_equal(qs0.w, q0.w, 0.01f), "xquat_slerp t=0");

        XQUAT qs05 = xquat_slerp(q0, q1, 0.5f);
        float slen = std::sqrt(qs05.x * qs05.x + qs05.y * qs05.y + qs05.z * qs05.z + qs05.w * qs05.w);
        MATH_CHECK(test_math_detail::nearly_equal(slen, 1.0f, 0.01f), "xquat_slerp t=0.5 unit length");

        // Rotation yaw/pitch/roll
        XQUAT qrpy = xquat_rotation_yaw_pitch_roll(0.0f, 0.0f, 0.0f);
        MATH_CHECK(test_math_detail::nearly_equal(qrpy.w, 1.0f, 0.001f), "xquat_rotation_yaw_pitch_roll (0,0,0)");

        XQUAT qrpy2 = xquat_rotation_yaw_pitch_roll(xvec3_set(0.0f, 0.0f, 0.0f));
        MATH_CHECK(test_math_detail::nearly_equal(qrpy2.w, 1.0f, 0.001f), "xquat_rotation_yaw_pitch_roll XFLOAT3(0,0,0)");

        // Rotation axis/angle
        XQUAT qra = xquat_rotation_axis(M_PIDIV2, xvec3_set(0.0f, 1.0f, 0.0f));
        float ralen = std::sqrt(qra.x * qra.x + qra.y * qra.y + qra.z * qra.z + qra.w * qra.w);
        MATH_CHECK(test_math_detail::nearly_equal(ralen, 1.0f, 0.001f), "xquat_rotation_axis XFLOAT3 unit length");

        XQUAT qra2 = xquat_rotation_axis(M_PIDIV2, 0.0f, 1.0f, 0.0f);
        MATH_CHECK(test_math_detail::nearly_equal(qra2.y, qra.y, 0.001f), "xquat_rotation_axis float xyz");

        // Rotation from matrix
        XMATRIX4 xm;
        xquat_to_matrix(qra, xm);
        XQUAT qfm = xquat_rotation_matrix(xm);
        bool match = (test_math_detail::nearly_equal(qra.x, qfm.x, 0.01f) && test_math_detail::nearly_equal(qra.y, qfm.y, 0.01f) &&
            test_math_detail::nearly_equal(qra.z, qfm.z, 0.01f) && test_math_detail::nearly_equal(qra.w, qfm.w, 0.01f)) ||
            (test_math_detail::nearly_equal(qra.x, -qfm.x, 0.01f) && test_math_detail::nearly_equal(qra.y, -qfm.y, 0.01f) &&
                test_math_detail::nearly_equal(qra.z, -qfm.z, 0.01f) && test_math_detail::nearly_equal(qra.w, -qfm.w, 0.01f));
        MATH_CHECK(match, "xquat_rotation_matrix round-trip");

        // From lookat
        XQUAT qlk = xquat_from_lookat(xvec3_set(0.0f, 0.0f, 1.0f));
        float lklen = std::sqrt(qlk.x * qlk.x + qlk.y * qlk.y + qlk.z * qlk.z + qlk.w * qlk.w);
        MATH_CHECK(test_math_detail::nearly_equal(lklen, 1.0f, 0.01f), "xquat_from_lookat forward unit length");

        XQUAT qlk2 = xquat_from_lookat(xvec3_set(0.0f, 0.0f, 1.0f), xvec3_set(0.0f, 1.0f, 0.0f));
        float lk2len = std::sqrt(qlk2.x * qlk2.x + qlk2.y * qlk2.y + qlk2.z * qlk2.z + qlk2.w * qlk2.w);
        MATH_CHECK(test_math_detail::nearly_equal(lk2len, 1.0f, 0.01f), "xquat_from_lookat forward+up unit length");

        // To yaw/pitch/roll
        XFLOAT3 ypr = xquat_to_yaw_pitch_roll(xquat_ident());
        MATH_CHECK(test_math_detail::nearly_equal(ypr.x, 0.0f, 0.001f) && test_math_detail::nearly_equal(ypr.y, 0.0f, 0.001f) && test_math_detail::nearly_equal(ypr.z, 0.0f, 0.001f), "xquat_to_yaw_pitch_roll identity");

        // To axis/angle
        XFLOAT3 axisOut;
        float angleOut = 0.0f;
        xquat_to_axis_angle(qra, axisOut, angleOut);
        MATH_CHECK(test_math_detail::nearly_equal(angleOut, M_PIDIV2, 0.01f), "xquat_to_axis_angle angle");

        // To matrix
        XMATRIX4 xm2 = xquat_to_matrix(xquat_ident());
        MATH_CHECK(xmat4_is_ident(xm2), "xquat_to_matrix identity");
    }

    inline void TestXMat4()
    {
        printf("[TestXMat4]\n");

        // Zero / Identity
        XMATRIX4 mz = xmat4_zero();
        MATH_CHECK(test_math_detail::nearly_equal(mz.m00, 0.0f) && test_math_detail::nearly_equal(mz.m33, 0.0f), "xmat4_zero");

        XMATRIX4 mi;
        xmat4_ident(mi);
        MATH_CHECK(xmat4_is_ident(mi), "xmat4_ident + xmat4_is_ident");

        MATH_CHECK(!xmat4_is_nan(mi), "xmat4_is_nan identity -> false");
        MATH_CHECK(!xmat4_is_infinite(mi), "xmat4_is_infinite identity -> false");

        // Set
        XMATRIX4 ms = xmat4_set(
            1.0f, 2.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f,
            9.0f, 10.0f, 11.0f, 12.0f,
            13.0f, 14.0f, 15.0f, 16.0f);
        MATH_CHECK(test_math_detail::nearly_equal(ms.m00, 1.0f) && test_math_detail::nearly_equal(ms.m33, 16.0f), "xmat4_set return");

        XMATRIX4 ms2;
        xmat4_set(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f, ms2);
        MATH_CHECK(xmat4_is_ident(ms2), "xmat4_set out identity");

        // Translation
        XMATRIX4 mt = xmat4_trans(xvec3_set(1.0f, 2.0f, 3.0f));
        XFLOAT3 mtp = xmat4_get_trans(mt);
        MATH_CHECK(test_math_detail::nearly_equal(mtp.x, 1.0f) && test_math_detail::nearly_equal(mtp.y, 2.0f) && test_math_detail::nearly_equal(mtp.z, 3.0f), "xmat4_trans + xmat4_get_trans");

        XMATRIX4 mt2;
        xmat4_trans(4.0f, 5.0f, 6.0f, mt2);
        XFLOAT3 mt2p;
        xmat4_get_trans(mt2, mt2p);
        MATH_CHECK(test_math_detail::nearly_equal(mt2p.x, 4.0f), "xmat4_trans float out + get_trans out");

        XMATRIX4 mt3;
        xmat4_trans(xvec3_set(7.0f, 8.0f, 9.0f), mt3);
        MATH_CHECK(test_math_detail::nearly_equal(xmat4_get_trans(mt3).x, 7.0f), "xmat4_trans XFLOAT3 out");

        // Transpose
        XMATRIX4 mtr = xmat4_transpose(ms);
        MATH_CHECK(test_math_detail::nearly_equal(mtr.m01, 5.0f) && test_math_detail::nearly_equal(mtr.m10, 2.0f), "xmat4_transpose return");

        XMATRIX4 mtr2;
        xmat4_transpose(ms, mtr2);
        MATH_CHECK(test_math_detail::nearly_equal(mtr2.m01, 5.0f), "xmat4_transpose out");

        // Inverse
        XMATRIX4 xt = xmat4_trans(xvec3_set(3.0f, 5.0f, 7.0f));
        XMATRIX4 xti = xmat4_inverse(xt);
        XMATRIX4 xprod = xmat4_mul(xt, xti);

        MATH_CHECK(test_math_detail::nearly_equal(xprod.m00, 1.0f, 0.001f) && test_math_detail::nearly_equal(xprod.m30, 0.0f, 0.001f), "xmat4_inverse T*T^-1 == I");

        XMATRIX4 xti2;
        xmat4_inverse(xt, xti2);
        MATH_CHECK(test_math_detail::nearly_equal(xti2.m30, -3.0f, 0.001f), "xmat4_inverse out");

        // Mul matrix * matrix
        XMATRIX4 xmr = xmat4_mul(mi, xt);
        MATH_CHECK(test_math_detail::nearly_equal(xmr.m30, 3.0f), "xmat4_mul I*M == M");

        XMATRIX4 xmr2;
        xmat4_mul(mi, xt, xmr2);
        MATH_CHECK(test_math_detail::nearly_equal(xmr2.m30, 3.0f), "xmat4_mul I*M out");

        // Mul matrix * XFLOAT3
        XFLOAT3 orig3 = xvec3_set(0.0f, 0.0f, 0.0f);
        XFLOAT3 xrv3 = xmat4_mul(xt, orig3);
        MATH_CHECK(test_math_detail::nearly_equal(xrv3.x, 3.0f) && test_math_detail::nearly_equal(xrv3.y, 5.0f) && test_math_detail::nearly_equal(xrv3.z, 7.0f), "xmat4_mul M*XFLOAT3");

        XFLOAT3 xrv3o;
        xmat4_mul(xt, orig3, xrv3o);
        MATH_CHECK(test_math_detail::nearly_equal(xrv3o.x, 3.0f), "xmat4_mul M*XFLOAT3 out");

        // Mul matrix * XFLOAT4
        XFLOAT4 v4w1 = xvec4_set(0.0f, 0.0f, 0.0f, 1.0f);
        XFLOAT4 xrv4;
        xmat4_mul(xt, v4w1, xrv4);
        MATH_CHECK(test_math_detail::nearly_equal(xrv4.x, 3.0f) && test_math_detail::nearly_equal(xrv4.y, 5.0f) && test_math_detail::nearly_equal(xrv4.z, 7.0f), "xmat4_mul M*XFLOAT4 out");

        // Scale
        XMATRIX4 xs = xmat4_scale(2.0f, 3.0f, 4.0f);
        MATH_CHECK(test_math_detail::nearly_equal(xs.m00, 2.0f) && test_math_detail::nearly_equal(xs.m11, 3.0f) && test_math_detail::nearly_equal(xs.m22, 4.0f), "xmat4_scale float return");

        XMATRIX4 xs2;
        xmat4_scale(5.0f, 6.0f, 7.0f, xs2);
        MATH_CHECK(test_math_detail::nearly_equal(xs2.m00, 5.0f), "xmat4_scale float out");

        XMATRIX4 xs3 = xmat4_scale(xvec3_set(2.0f, 3.0f, 4.0f));
        MATH_CHECK(test_math_detail::nearly_equal(xs3.m00, 2.0f), "xmat4_scale XFLOAT3 return");

        XMATRIX4 xs4;
        xmat4_scale(xvec3_set(2.0f, 3.0f, 4.0f), xs4);
        MATH_CHECK(test_math_detail::nearly_equal(xs4.m00, 2.0f), "xmat4_scale XFLOAT3 out");

        // Rotations
        XMATRIX4 xrx = xmat4_rotation_x(M_PIDIV2);
        MATH_CHECK(!xmat4_is_nan(xrx), "xmat4_rotation_x valid");

        XMATRIX4 xrx2;
        xmat4_rotation_x(M_PIDIV2, xrx2);
        MATH_CHECK(test_math_detail::nearly_equal(xrx.m11, xrx2.m11, 0.001f), "xmat4_rotation_x out");

        XMATRIX4 xry = xmat4_rotation_y(M_PIDIV2);
        MATH_CHECK(!xmat4_is_nan(xry), "xmat4_rotation_y valid");

        XMATRIX4 xry2;
        xmat4_rotation_y(M_PIDIV2, xry2);
        MATH_CHECK(test_math_detail::nearly_equal(xry.m00, xry2.m00, 0.001f), "xmat4_rotation_y out");

        XMATRIX4 xrz = xmat4_rotation_z(M_PIDIV2);
        MATH_CHECK(!xmat4_is_nan(xrz), "xmat4_rotation_z valid");

        XMATRIX4 xrz2;
        xmat4_rotation_z(M_PIDIV2, xrz2);
        MATH_CHECK(test_math_detail::nearly_equal(xrz.m00, xrz2.m00, 0.001f), "xmat4_rotation_z out");

        // YawPitchRoll
        XMATRIX4 xypr = xmat4_rotation_yaw_pitch_roll(0.0f, 0.0f, 0.0f);
        MATH_CHECK(xmat4_is_ident(xypr), "xmat4_rotation_yaw_pitch_roll (0,0,0) == identity");

        XMATRIX4 xypr2;
        xmat4_rotation_yaw_pitch_roll(0.0f, 0.0f, 0.0f, xypr2);
        MATH_CHECK(xmat4_is_ident(xypr2), "xmat4_rotation_yaw_pitch_roll out");

        // Rotation Axis
        XMATRIX4 xra = xmat4_rotation_axis(xvec3_set(0.0f, 1.0f, 0.0f), M_PI);
        MATH_CHECK(!xmat4_is_nan(xra), "xmat4_rotation_axis valid");

        XMATRIX4 xra2;
        xmat4_rotation_axis(xvec3_set(0.0f, 1.0f, 0.0f), M_PI, xra2);
        MATH_CHECK(test_math_detail::nearly_equal(xra.m00, xra2.m00, 0.001f), "xmat4_rotation_axis out");

        // LookTo LH / RH
        XFLOAT3 eye = xvec3_set(0.0f, 0.0f, 0.0f);
        XFLOAT3 target = xvec3_set(0.0f, 0.0f, 10.0f);
        XFLOAT3 up = xvec3_set(0.0f, 1.0f, 0.0f);

        XMATRIX4 xltlh = xmat4_lookto_lh(eye, target, up);
        MATH_CHECK(!xmat4_is_nan(xltlh), "xmat4_lookto_lh valid");

        XMATRIX4 xltlh2;
        xmat4_lookto_lh(eye, target, up, xltlh2);
        MATH_CHECK(test_math_detail::nearly_equal(xltlh.m00, xltlh2.m00, 0.001f), "xmat4_lookto_lh out");

        XMATRIX4 xltrh = xmat4_lookto_rh(eye, target, up);
        MATH_CHECK(!xmat4_is_nan(xltrh), "xmat4_lookto_rh valid");

        XMATRIX4 xltrh2;
        xmat4_lookto_rh(eye, target, up, xltrh2);
        MATH_CHECK(test_math_detail::nearly_equal(xltrh.m00, xltrh2.m00, 0.001f), "xmat4_lookto_rh out");

        // Perspective LH / RH (FOV)
        float fov = M_PI / 4.0f, aspect = 16.0f / 9.0f, nearZ = 0.1f, farZ = 1000.0f;

        XMATRIX4 xplh = xmat4_perspective_lh(fov, aspect, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xplh), "xmat4_perspective_lh fov valid");

        XMATRIX4 xplh2;
        xmat4_perspective_lh(fov, aspect, nearZ, farZ, xplh2);
        MATH_CHECK(test_math_detail::nearly_equal(xplh.m00, xplh2.m00, 0.001f), "xmat4_perspective_lh fov out");

        XMATRIX4 xprh = xmat4_perspective_rh(fov, aspect, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xprh), "xmat4_perspective_rh fov valid");

        XMATRIX4 xprh2;
        xmat4_perspective_rh(fov, aspect, nearZ, farZ, xprh2);
        MATH_CHECK(test_math_detail::nearly_equal(xprh.m00, xprh2.m00, 0.001f), "xmat4_perspective_rh fov out");

        // Perspective LH / RH (LRTB)
        XMATRIX4 xplr = xmat4_perspective_lh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xplr), "xmat4_perspective_lh LRTB valid");

        XMATRIX4 xplr2;
        xmat4_perspective_lh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ, xplr2);
        MATH_CHECK(test_math_detail::nearly_equal(xplr.m00, xplr2.m00, 0.001f), "xmat4_perspective_lh LRTB out");

        XMATRIX4 xprr = xmat4_perspective_rh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xprr), "xmat4_perspective_rh LRTB valid");

        XMATRIX4 xprr2;
        xmat4_perspective_rh(-1.0f, 1.0f, 1.0f, -1.0f, nearZ, farZ, xprr2);
        MATH_CHECK(test_math_detail::nearly_equal(xprr.m00, xprr2.m00, 0.001f), "xmat4_perspective_rh LRTB out");

        // Ortho LH / RH (WH)
        XMATRIX4 xolh = xmat4_ortho_lh(800.0f, 600.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xolh), "xmat4_ortho_lh WH valid");

        XMATRIX4 xolh2;
        xmat4_ortho_lh(800.0f, 600.0f, nearZ, farZ, xolh2);
        MATH_CHECK(test_math_detail::nearly_equal(xolh.m00, xolh2.m00, 0.001f), "xmat4_ortho_lh WH out");

        XMATRIX4 xorh = xmat4_ortho_rh(800.0f, 600.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xorh), "xmat4_ortho_rh WH valid");

        XMATRIX4 xorh2;
        xmat4_ortho_rh(800.0f, 600.0f, nearZ, farZ, xorh2);
        MATH_CHECK(test_math_detail::nearly_equal(xorh.m00, xorh2.m00, 0.001f), "xmat4_ortho_rh WH out");

        // Ortho LH / RH (LRTB)
        XMATRIX4 xolr = xmat4_ortho_lh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xolr), "xmat4_ortho_lh LRTB valid");

        XMATRIX4 xolr2;
        xmat4_ortho_lh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ, xolr2);
        MATH_CHECK(test_math_detail::nearly_equal(xolr.m00, xolr2.m00, 0.001f), "xmat4_ortho_lh LRTB out");

        XMATRIX4 xorr = xmat4_ortho_rh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xorr), "xmat4_ortho_rh LRTB valid");

        XMATRIX4 xorr2;
        xmat4_ortho_rh(0.0f, 800.0f, 600.0f, 0.0f, nearZ, farZ, xorr2);
        MATH_CHECK(test_math_detail::nearly_equal(xorr.m00, xorr2.m00, 0.001f), "xmat4_ortho_rh LRTB out");

        // Frustum LH / RH
        XMATRIX4 xflh = xmat4_frustum_lh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xflh), "xmat4_frustum_lh valid");

        XMATRIX4 xflh2;
        xmat4_frustum_lh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ, xflh2);
        MATH_CHECK(test_math_detail::nearly_equal(xflh.m00, xflh2.m00, 0.001f), "xmat4_frustum_lh out");

        XMATRIX4 xfrh = xmat4_frustum_rh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ);
        MATH_CHECK(!xmat4_is_nan(xfrh), "xmat4_frustum_rh valid");

        XMATRIX4 xfrh2;
        xmat4_frustum_rh(-1.0f, 1.0f, -1.0f, 1.0f, nearZ, farZ, xfrh2);
        MATH_CHECK(test_math_detail::nearly_equal(xfrh.m00, xfrh2.m00, 0.001f), "xmat4_frustum_rh out");
    }

#endif // #if defined(__TARGET_OS_WINDOWS) && defined(__SSE__)

    // ========================================================================
    // Entry Point: TestMathLib()
    // ========================================================================
    inline void TestMathLib()
    {
        using namespace test_math_detail;
        reset_counters();

        printf("========================================\n");
        printf("  MathLib Test Suite\n");
        printf("========================================\n");

        // --- FLOAT2 / FLOAT3 / FLOAT4 ---
        TestMath_Set();
        TestMath_Add();
        TestMath_Sub();
        TestMath_Mul();
        TestMath_Div();
        TestMath_Dot();
        TestMath_Cross();
        TestMath_Length();
        TestMath_LengthSq();
        TestMath_Normalized();
        TestMath_Normalize();
        TestMath_Lerp();
        TestMath_Clamp();
        TestMath_Saturate();
        TestMath_Min();
        TestMath_Max();
        TestMath_Ceil();
        TestMath_Floor();
        TestMath_Round();
        TestMath_Trunc();

        // --- Quaternion ---
        TestQuat_Ident();
        TestQuat_Mul();
        TestQuat_Inverse();
        TestQuat_Normalize();
        TestQuat_Conjugate();
        TestQuat_Slerp();
        TestQuat_RotationRollPitchYaw();
        TestQuat_RotationAxisAngle();
        TestQuat_RotationLookAt();
        TestQuat_RotationBetween();
        TestQuat_FromMatrix();
        TestQuat_ToRollPitchYaw();
        TestQuat_ToAxisAngle();
        TestQuat_ToMatrix();

        // --- Matrix4 ---
        TestMat4_IsIdent();
        TestMat4_IsNan();
        TestMat4_IsInfinite();
        TestMat4_Set();
        TestMat4_Trans();
        TestMat4_GetTrans();
        TestMat4_Transpose();
        TestMat4_Inverse();
        TestMat4_Mul();
        TestMat4_Scale();
        TestMat4_Rotation();
        TestMat4_RotationYawPitchRoll();
        TestMat4_RotationAxis();
        TestMat4_RotationQuat();
        TestMat4_LookAt();
        TestMat4_LookTo();
        TestMat4_Perspective();
        TestMat4_Ortho();
        TestMat4_Frustum();

        // --- SSE / XFLOAT / XQUAT / XMATRIX4 ---
#if defined(__TARGET_OS_WINDOWS) && defined(__SSE__)
        TestXVec2();
        TestXVec3();
        TestXVec4();
        TestXQuat();
        TestXMat4();
#else
        printf("[SSE tests skipped - not available on this platform]\n");
#endif

        print_summary();
    }

} // namespace mark
