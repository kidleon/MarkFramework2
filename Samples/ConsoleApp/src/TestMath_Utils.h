#pragma once

#include "MathLib.h"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace mark
{
    namespace test_math_utils_detail
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

#define UTIL_CHECK(expr, tag) mark::test_math_utils_detail::check((expr), (tag))
    }

    // ========================================================================
    // repeat
    // ========================================================================
    inline void TestUtil_Repeat()
    {
        printf("[TestUtil_Repeat]\n");

        // float repeat
        UTIL_CHECK(test_math_utils_detail::nearly_equal(repeat(0.0f, 1.0f), 0.0f, 0.001f), "repeat float 0.0 in [0,1)");
        UTIL_CHECK(test_math_utils_detail::nearly_equal(repeat(0.5f, 1.0f), 0.5f, 0.001f), "repeat float 0.5 in [0,1)");
        UTIL_CHECK(test_math_utils_detail::nearly_equal(repeat(1.5f, 1.0f), 0.5f, 0.001f), "repeat float 1.5 in [0,1)");
        UTIL_CHECK(test_math_utils_detail::nearly_equal(repeat(3.7f, 2.0f), 1.7f, 0.001f), "repeat float 3.7 in [0,2)");
        UTIL_CHECK(test_math_utils_detail::nearly_equal(repeat(-0.3f, 1.0f), 0.7f, 0.001f), "repeat float -0.3 in [0,1)");

        // FLOAT2 repeat
        FLOAT2 r2 = repeat(FLOAT2{ 1.5f, 3.7f }, FLOAT2{ 1.0f, 2.0f });
        UTIL_CHECK(test_math_utils_detail::nearly_equal(r2.x, 0.5f, 0.001f) && test_math_utils_detail::nearly_equal(r2.y, 1.7f, 0.001f), "repeat FLOAT2");

        // FLOAT3 repeat
        FLOAT3 r3 = repeat(FLOAT3{ 1.5f, 3.7f, 5.2f }, FLOAT3{ 1.0f, 2.0f, 3.0f });
        UTIL_CHECK(test_math_utils_detail::nearly_equal(r3.x, 0.5f, 0.001f) && test_math_utils_detail::nearly_equal(r3.y, 1.7f, 0.001f) && test_math_utils_detail::nearly_equal(r3.z, 2.2f, 0.001f), "repeat FLOAT3");
    }

    // ========================================================================
    // delta_angle
    // ========================================================================
    inline void TestUtil_DeltaAngle()
    {
        printf("[TestUtil_DeltaAngle]\n");

        // Same angle -> 0
        UTIL_CHECK(test_math_utils_detail::nearly_equal(delta_angle(0.0f, 0.0f), 0.0f, 0.001f), "delta_angle same angle");

        // Small positive delta
        UTIL_CHECK(test_math_utils_detail::nearly_equal(delta_angle(0.0f, 0.5f), 0.5f, 0.001f), "delta_angle 0 -> 0.5");

        // Small negative delta
        UTIL_CHECK(test_math_utils_detail::nearly_equal(delta_angle(0.5f, 0.0f), -0.5f, 0.001f), "delta_angle 0.5 -> 0");

        // Wrap around: 350deg -> 10deg should be +20deg (shortest path)
        float deg350 = 350.0f * M_PI / 180.0f;
        float deg10 = 10.0f * M_PI / 180.0f;
        float da = delta_angle(deg350, deg10);
        float expected = 20.0f * M_PI / 180.0f;
        UTIL_CHECK(test_math_utils_detail::nearly_equal(da, expected, 0.01f), "delta_angle 350deg -> 10deg == +20deg");

        // Wrap around: 10deg -> 350deg should be -20deg (shortest path)
        float da2 = delta_angle(deg10, deg350);
        UTIL_CHECK(test_math_utils_detail::nearly_equal(da2, -expected, 0.01f), "delta_angle 10deg -> 350deg == -20deg");

        // Opposite direction: 0 -> PI
        float da3 = delta_angle(0.0f, M_PI);
        UTIL_CHECK(std::fabs(da3) <= M_PI + 0.01f, "delta_angle 0 -> PI within range");
    }

    // ========================================================================
    // move_towards
    // ========================================================================
    inline void TestUtil_MoveTowards()
    {
        printf("[TestUtil_MoveTowards]\n");

        // FLOAT2: already at target
        {
            FLOAT2 cur = { 1.0f, 1.0f }, tgt = { 1.0f, 1.0f };
            FLOAT2 r = move_towards(cur, tgt, 1.0f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, 1.0f) && test_math_utils_detail::nearly_equal(r.y, 1.0f), "move_towards FLOAT2 at target");
        }

        // FLOAT2: reach target within max_delta
        {
            FLOAT2 cur = { 0.0f, 0.0f }, tgt = { 3.0f, 4.0f };
            FLOAT2 r = move_towards(cur, tgt, 10.0f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, 3.0f) && test_math_utils_detail::nearly_equal(r.y, 4.0f), "move_towards FLOAT2 overshoot returns target");
        }

        // FLOAT2: partial move
        {
            FLOAT2 cur = { 0.0f, 0.0f }, tgt = { 10.0f, 0.0f };
            FLOAT2 r = move_towards(cur, tgt, 3.0f);
            float dist = math_length(math_sub(r, cur));
            UTIL_CHECK(test_math_utils_detail::nearly_equal(dist, 3.0f, 0.01f), "move_towards FLOAT2 partial move distance");
        }

        // FLOAT3: reach target
        {
            FLOAT3 cur = { 0.0f, 0.0f, 0.0f }, tgt = { 1.0f, 2.0f, 2.0f };
            FLOAT3 r = move_towards(cur, tgt, 100.0f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, 1.0f) && test_math_utils_detail::nearly_equal(r.y, 2.0f) && test_math_utils_detail::nearly_equal(r.z, 2.0f), "move_towards FLOAT3 overshoot returns target");
        }

        // FLOAT3: partial move
        {
            FLOAT3 cur = { 0.0f, 0.0f, 0.0f }, tgt = { 10.0f, 0.0f, 0.0f };
            FLOAT3 r = move_towards(cur, tgt, 2.0f);
            float dist = math_length(math_sub(r, cur));
            UTIL_CHECK(test_math_utils_detail::nearly_equal(dist, 2.0f, 0.01f), "move_towards FLOAT3 partial move distance");
        }

        // FLOAT4: reach target
        {
            FLOAT4 cur = { 0.0f, 0.0f, 0.0f, 0.0f }, tgt = { 1.0f, 1.0f, 1.0f, 1.0f };
            FLOAT4 r = move_towards(cur, tgt, 100.0f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, 1.0f) && test_math_utils_detail::nearly_equal(r.w, 1.0f), "move_towards FLOAT4 overshoot returns target");
        }

        // FLOAT4: partial move
        {
            FLOAT4 cur = { 0.0f, 0.0f, 0.0f, 0.0f }, tgt = { 10.0f, 0.0f, 0.0f, 0.0f };
            FLOAT4 r = move_towards(cur, tgt, 2.0f);
            float dist = math_length(math_sub(r, cur));
            UTIL_CHECK(test_math_utils_detail::nearly_equal(dist, 2.0f, 0.01f), "move_towards FLOAT4 partial move distance");
        }
    }

    // ========================================================================
    // forward_to_quat
    // ========================================================================
    inline void TestUtil_ForwardToQuat()
    {
        printf("[TestUtil_ForwardToQuat]\n");

        // FLOAT2: forward (0,1) -> identity quaternion
        {
            QUAT q = forward_to_quat(FLOAT2{ 0.0f, 1.0f });
            UTIL_CHECK(test_math_utils_detail::nearly_equal(q.x, 0.0f) && test_math_utils_detail::nearly_equal(q.y, 0.0f) && test_math_utils_detail::nearly_equal(q.z, 0.0f) && test_math_utils_detail::nearly_equal(q.w, 1.0f), "forward_to_quat FLOAT2 (0,1) == identity");
        }

        // FLOAT2: forward (1,0) -> Z-axis rotation
        {
            QUAT q = forward_to_quat(FLOAT2{ 1.0f, 0.0f });
            float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(len, 1.0f, 0.001f), "forward_to_quat FLOAT2 (1,0) unit length");
            UTIL_CHECK(test_math_utils_detail::nearly_equal(q.x, 0.0f, 0.001f) && test_math_utils_detail::nearly_equal(q.y, 0.0f, 0.001f), "forward_to_quat FLOAT2 (1,0) only Z rotation");
        }

        // FLOAT2: forward (-1,0) -> Z-axis rotation
        {
            QUAT q = forward_to_quat(FLOAT2{ -1.0f, 0.0f });
            float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(len, 1.0f, 0.001f), "forward_to_quat FLOAT2 (-1,0) unit length");
        }

        // FLOAT3: forward (0,0,1), up (0,1,0) -> identity-like quaternion
        {
            QUAT q = forward_to_quat(FLOAT3{ 0.0f, 0.0f, 1.0f }, FLOAT3{ 0.0f, 1.0f, 0.0f });
            float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(len, 1.0f, 0.001f), "forward_to_quat FLOAT3 forward+up unit length");
        }

        // FLOAT3: forward (1,0,0), up (0,1,0) -> 90deg Y rotation
        {
            QUAT q = forward_to_quat(FLOAT3{ 1.0f, 0.0f, 0.0f }, FLOAT3{ 0.0f, 1.0f, 0.0f });
            float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(len, 1.0f, 0.001f), "forward_to_quat FLOAT3 (1,0,0) unit length");
        }

        // FLOAT3: round-trip test - applying quaternion to (0,0,1) should give back forward
        {
            FLOAT3 fwd = math_normalized(FLOAT3{ 1.0f, 0.0f, 1.0f });
            FLOAT3 up = { 0.0f, 1.0f, 0.0f };
            QUAT q = forward_to_quat(fwd, up);
            FLOAT3 result = quat_mul(q, FLOAT3{ 0.0f, 0.0f, 1.0f });
            UTIL_CHECK(test_math_utils_detail::nearly_equal(result.x, fwd.x, 0.01f) && test_math_utils_detail::nearly_equal(result.y, fwd.y, 0.01f) && test_math_utils_detail::nearly_equal(result.z, fwd.z, 0.01f), "forward_to_quat FLOAT3 round-trip");
        }
    }

    // ========================================================================
    // rotate_towards
    // ========================================================================
    inline void TestUtil_RotateTowards()
    {
        printf("[TestUtil_RotateTowards]\n");

        // FLOAT2: already aligned -> returns target
        {
            FLOAT2 cur = { 1.0f, 0.0f }, tgt = { 1.0f, 0.0f };
            FLOAT2 r = rotate_towards(cur, tgt, 0.1f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, 1.0f, 0.001f) && test_math_utils_detail::nearly_equal(r.y, 0.0f, 0.001f), "rotate_towards FLOAT2 same direction");
        }

        // FLOAT2: small angle, large max_angle -> returns target
        {
            float angle = 0.1f;
            FLOAT2 cur = { 1.0f, 0.0f };
            FLOAT2 tgt = { std::cos(angle), std::sin(angle) };
            FLOAT2 r = rotate_towards(cur, tgt, 1.0f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, tgt.x, 0.001f) && test_math_utils_detail::nearly_equal(r.y, tgt.y, 0.001f), "rotate_towards FLOAT2 within max_angle");
        }

        // FLOAT2: large angle, small max_angle -> partial rotation
        {
            FLOAT2 cur = { 1.0f, 0.0f };
            FLOAT2 tgt = { 0.0f, 1.0f }; // 90 degrees
            float max_angle = 0.1f;
            FLOAT2 r = rotate_towards(cur, tgt, max_angle);
            float result_len = std::sqrt(r.x * r.x + r.y * r.y);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(result_len, 1.0f, 0.01f), "rotate_towards FLOAT2 partial rotation unit length");
            // Should have rotated by max_angle from (1,0) CCW
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, std::cos(max_angle), 0.01f) && test_math_utils_detail::nearly_equal(r.y, std::sin(max_angle), 0.01f), "rotate_towards FLOAT2 partial rotation angle");
        }

        // FLOAT3: already aligned -> returns target
        {
            FLOAT3 cur = { 0.0f, 0.0f, 1.0f }, tgt = { 0.0f, 0.0f, 1.0f };
            FLOAT3 r = rotate_towards(cur, tgt, 0.1f);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, 0.0f, 0.001f) && test_math_utils_detail::nearly_equal(r.y, 0.0f, 0.001f) && test_math_utils_detail::nearly_equal(r.z, 1.0f, 0.001f), "rotate_towards FLOAT3 same direction");
        }

        // FLOAT3: small angle, large max_angle -> returns target
        {
            FLOAT3 cur = { 1.0f, 0.0f, 0.0f };
            FLOAT3 tgt = math_normalized(FLOAT3{ 0.99f, 0.1f, 0.0f });
            FLOAT3 r = rotate_towards(cur, tgt, M_PI);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(r.x, tgt.x, 0.01f) && test_math_utils_detail::nearly_equal(r.y, tgt.y, 0.01f) && test_math_utils_detail::nearly_equal(r.z, tgt.z, 0.01f), "rotate_towards FLOAT3 within max_angle");
        }

        // FLOAT3: large angle, small max_angle -> partial rotation, unit length
        {
            FLOAT3 cur = { 1.0f, 0.0f, 0.0f };
            FLOAT3 tgt = { 0.0f, 1.0f, 0.0f }; // 90 degrees
            float max_angle = 0.2f;
            FLOAT3 r = rotate_towards(cur, tgt, max_angle);
            float result_len = std::sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(result_len, 1.0f, 0.01f), "rotate_towards FLOAT3 partial rotation unit length");
            // Angle between cur and r should be approximately max_angle
            float dot = r.x * cur.x + r.y * cur.y + r.z * cur.z;
            float angle_result = std::acos(std::fmax(-1.0f, std::fmin(1.0f, dot)));
            UTIL_CHECK(test_math_utils_detail::nearly_equal(angle_result, max_angle, 0.01f), "rotate_towards FLOAT3 partial rotation angle");
        }

        // FLOAT3: opposite direction -> should still produce valid unit vector
        {
            FLOAT3 cur = { 1.0f, 0.0f, 0.0f };
            FLOAT3 tgt = { -1.0f, 0.0f, 0.0f }; // 180 degrees
            float max_angle = 0.5f;
            FLOAT3 r = rotate_towards(cur, tgt, max_angle);
            float result_len = std::sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
            UTIL_CHECK(test_math_utils_detail::nearly_equal(result_len, 1.0f, 0.01f), "rotate_towards FLOAT3 opposite direction unit length");
        }
    }

    // ========================================================================
    // Entry Point: TestMathLib_Util()
    // ========================================================================
    inline void TestMathLib_Util()
    {
        using namespace test_math_utils_detail;
        reset_counters();

        printf("========================================\n");
        printf("  MathLib Utils Test Suite\n");
        printf("========================================\n");

        TestUtil_Repeat();
        TestUtil_DeltaAngle();
        TestUtil_MoveTowards();
        TestUtil_ForwardToQuat();
        TestUtil_RotateTowards();

        print_summary();
    }

} // namespace mark


