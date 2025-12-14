#ifndef __MATHLIB_SSE_H__
#define __MATHLIB_SSE_H__

#include "predefine.h"

#ifdef __TARGET_OS_WINDOWS
#include <immintrin.h>  // SSE/SSE2/SSE3/SSE4 명령어 집합 포함
#include "mathlib.h"

#define SIMD_CALL __vectorcall

// SIMD 최적화를 위한 호출 규약 설정
static constexpr __m128 XMM_GLOBAL_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
static constexpr __m128 XMM_GLOBAL_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };

inline __m128 xmvec2_dot(__m128 v0, __m128 v1);
inline __m128 xmvec3_dot(__m128 v0, __m128 v1);
inline __m128 xmvec4_dot(__m128 v0, __m128 v1);
inline __m128 xmvec2_cross(__m128 v0, __m128 v1);
inline __m128 xmvec3_cross(__m128 v0, __m128 v1);
inline __m128 xmvec4_cross(__m128 v0, __m128 v1, __m128 v2);
inline __m128 xmvec2_lengthsq(__m128 v);
inline __m128 xmvec3_lengthsq(__m128 v);
inline __m128 xmvec4_lengthsq(__m128 v);
inline __m128 xmvec2_length(__m128 v);
inline __m128 xmvec3_length(__m128 v);
inline __m128 xmvec4_length(__m128 v);
inline __m128 xmvec2_normalize(__m128 v);
inline __m128 xmvec3_normalize(__m128 v);
inline __m128 xmvec4_normalize(__m128 v);
inline __m128 xmvec_lerp(__m128 v0, __m128 v1, float t);
inline __m128 xmvec_clamp(__m128 v, __m128 vmin, __m128 vmax);
inline __m128 xmvec_saturate(__m128 v);
inline __m128 xmvec_saturate(__m128 v);
inline __m128 xmvec_saturate(__m128 v);
inline __m128 xmvec_min(__m128 v0, __m128 v1);
inline __m128 xmvec_min(__m128 v0, __m128 v1);
inline __m128 xmvec_min(__m128 v0, __m128 v1);
inline __m128 xmvec_max(__m128 v0, __m128 v1);
inline __m128 xmvec_max(__m128 v0, __m128 v1);
inline __m128 xmvec_max(__m128 v0, __m128 v1);
inline __m128 xmvec_ceil(__m128 v); // 올림
inline __m128 xmvec_ceil(__m128 v);
inline __m128 xmvec_ceil(__m128 v);
inline __m128 xmvec_floor(__m128 v); // 버림
inline __m128 xmvec_floor(__m128 v);
inline __m128 xmvec_floor(__m128 v);
inline __m128 xmvec_round(__m128 v); // 반올림
inline __m128 xmvec_round(__m128 v);
inline __m128 xmvec_round(__m128 v);
inline __m128 xmvec_trunc(__m128 v); // 버림
inline __m128 xmvec_trunc(__m128 v);
inline __m128 xmvec_trunc(__m128 v);


struct alignas(16) XFLOAT2
{
    union
    {
        struct { float x, y; };
        float v[2];
        __m128 m;  // SSE 벡터 (하위 2개 요소만 사용)
    };
};

struct alignas(16) XFLOAT3
{
    union
    {
        struct { float x, y, z; };
        float v[3];
        __m128 m;  // SSE 벡터 (하위 3개 요소만 사용)
    };
};

struct alignas(16) XFLOAT4
{
    union
    {
        struct { float x, y, z, w; };
        float v[4];
        __m128 m;  // SSE 벡터
    };
};

// QUAT을 XFLOAT4와 동일하게 정의
typedef XFLOAT4 XQUAT;

struct alignas(16) XMATRIX4
{
    union
    {
        struct
        {
            __m128 r[4];  // 4개의 행을 SSE 벡터로 표현
        };

        float m[16];
        float row[4][4];

        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };
};


//-------------------------------------------------------------------------------
// XFLOAT2
inline XFLOAT2 SIMD_CALL xvec2_zero();
inline void SIMD_CALL xvec2_zero(XFLOAT2* v);
inline XFLOAT2 SIMD_CALL xvec2_unit_x();
inline XFLOAT2 SIMD_CALL xvec2_unit_y();
inline XFLOAT2 SIMD_CALL xvec2_set(float x, float y);
inline void SIMD_CALL xvec2_set(float x, float y, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_add(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_add(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_sub(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_sub(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_mul(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_mul(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_mul(XFLOAT2* v, float f);
inline void SIMD_CALL xvec2_mul(XFLOAT2* v, float f, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_div(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_div(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_div(XFLOAT2* v, float f);
inline void SIMD_CALL xvec2_div(XFLOAT2* v, float f, XFLOAT2* out);
inline float SIMD_CALL xvec2_dot_f(XFLOAT2* v0, XFLOAT2* v1);
inline XFLOAT2 SIMD_CALL xvec2_dot(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_dot(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_cross(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_cross(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline float SIMD_CALL xvec2_lengthsq_f(XFLOAT2* v);
inline XFLOAT2 SIMD_CALL xvec2_lengthsq(XFLOAT2* v);
inline void SIMD_CALL xvec2_lengthsq(XFLOAT2* v, XFLOAT2* out);
inline float SIMD_CALL xvec2_length_f(XFLOAT2* v);
inline XFLOAT2 SIMD_CALL xvec2_length(XFLOAT2* v);
inline void SIMD_CALL xvec2_length(XFLOAT2* v, XFLOAT2* out);
inline void SIMD_CALL xvec2_normalize(XFLOAT2* v);
inline XFLOAT2 SIMD_CALL xvec2_normalized(XFLOAT2* v);
inline void SIMD_CALL xvec2_normalized(XFLOAT2* v, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_lerp(XFLOAT2* v0, XFLOAT2* v1, float t);
inline void SIMD_CALL xvec2_lerp(XFLOAT2* v0, XFLOAT2* v1, float t, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_clamp(XFLOAT2* v, XFLOAT2* vmin, XFLOAT2* vmax);
inline void SIMD_CALL xvec2_clamp(XFLOAT2* v, XFLOAT2* vmin, XFLOAT2* vmax, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_saturate(XFLOAT2* v);
inline void SIMD_CALL xvec2_saturate(XFLOAT2* v, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_min(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_min(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_max(XFLOAT2* v0, XFLOAT2* v1);
inline void SIMD_CALL xvec2_max(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_ceil(XFLOAT2* v);
inline void SIMD_CALL xvec2_ceil(XFLOAT2* v, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_floor(XFLOAT2* v);
inline void SIMD_CALL xvec2_floor(XFLOAT2* v, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_round(XFLOAT2* v);
inline void SIMD_CALL xvec2_round(XFLOAT2* v, XFLOAT2* out);
inline XFLOAT2 SIMD_CALL xvec2_trunc(XFLOAT2* v);
inline void SIMD_CALL xvec2_trunc(XFLOAT2* v, XFLOAT2* out);


//-------------------------------------------------------------------------------
// XFLOAT3
inline XFLOAT3 SIMD_CALL xvec3_zero();
inline void SIMD_CALL xvec3_zero(XFLOAT3* v);
inline XFLOAT3 SIMD_CALL xvec3_unit_x();
inline XFLOAT3 SIMD_CALL xvec3_unit_y();
inline XFLOAT3 SIMD_CALL xvec3_unit_z();
inline XFLOAT3 SIMD_CALL xvec3_set(float x, float y);
inline XFLOAT3 SIMD_CALL xvec3_set(float x, float y, float z);
inline void SIMD_CALL xvec3_set(float x, float y, XFLOAT3* out);
inline void SIMD_CALL xvec3_set(float x, float y, float z, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_add(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_add(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_sub(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_sub(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_mul(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_mul(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_mul(XFLOAT3* v, float f);
inline void SIMD_CALL xvec3_mul(XFLOAT3* v, float f, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_div(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_div(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_div(XFLOAT3* v, float f);
inline void SIMD_CALL xvec3_div(XFLOAT3* v, float f, XFLOAT3* out);
inline float SIMD_CALL xvec3_dot_f(XFLOAT3* v0, XFLOAT3* v1);
inline XFLOAT3 SIMD_CALL xvec3_dot(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_dot(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_cross(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_cross(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline float SIMD_CALL xvec3_lengthsq_f(XFLOAT3* v);
inline XFLOAT3 SIMD_CALL xvec3_lengthsq(XFLOAT3* v);
inline void SIMD_CALL xvec3_lengthsq(XFLOAT3* v, XFLOAT3* out);
inline float SIMD_CALL xvec3_length_f(XFLOAT3* v);
inline XFLOAT3 SIMD_CALL xvec3_length(XFLOAT3* v);
inline void SIMD_CALL xvec3_length(XFLOAT3* v, XFLOAT3* out);
inline void SIMD_CALL xvec3_normalize(XFLOAT3* v);
inline XFLOAT3 SIMD_CALL xvec3_normalized(XFLOAT3* v);
inline void SIMD_CALL xvec3_normalized(XFLOAT3* v, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_lerp(XFLOAT3* v0, XFLOAT3* v1, float t);
inline void SIMD_CALL xvec3_lerp(XFLOAT3* v0, XFLOAT3* v1, float t, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_clamp(XFLOAT3* v, XFLOAT3* vmin, XFLOAT3* vmax);
inline void SIMD_CALL xvec3_clamp(XFLOAT3* v, XFLOAT3* vmin, XFLOAT3* vmax, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_saturate(XFLOAT3* v);
inline void SIMD_CALL xvec3_saturate(XFLOAT3* v, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_min(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_min(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_max(XFLOAT3* v0, XFLOAT3* v1);
inline void SIMD_CALL xvec3_max(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_ceil(XFLOAT3* v);
inline void SIMD_CALL xvec3_ceil(XFLOAT3* v, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_floor(XFLOAT3* v);
inline void SIMD_CALL xvec3_floor(XFLOAT3* v, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_round(XFLOAT3* v);
inline void SIMD_CALL xvec3_round(XFLOAT3* v, XFLOAT3* out);
inline XFLOAT3 SIMD_CALL xvec3_trunc(XFLOAT3* v);
inline void SIMD_CALL xvec3_trunc(XFLOAT3* v, XFLOAT3* out);


//-------------------------------------------------------------------------------
// XFLOAT4
inline XFLOAT4 SIMD_CALL xvec4_zero();
inline void SIMD_CALL xvec4_zero(XFLOAT4* v);
inline XFLOAT4 SIMD_CALL xvec4_unit_x();
inline XFLOAT4 SIMD_CALL xvec4_unit_y();
inline XFLOAT4 SIMD_CALL xvec4_unit_z();
inline XFLOAT4 SIMD_CALL xvec4_unit_w();
inline XFLOAT4 SIMD_CALL xvec4_set(float x, float y, float z, float w);
inline XFLOAT4 SIMD_CALL xvec4_set(float x, float y, float z);
inline void SIMD_CALL xvec4_set(float x, float y, float z, float w, XFLOAT4* out);
inline void SIMD_CALL xvec4_set(float x, float y, float z, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_add(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_add(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_sub(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_sub(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_mul(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_mul(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_mul(XFLOAT4* v, float f);
inline void SIMD_CALL xvec4_mul(XFLOAT4* v, float f, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_div(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_div(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_div(XFLOAT4* v, float f);
inline void SIMD_CALL xvec4_div(XFLOAT4* v, float f, XFLOAT4* out);
inline float SIMD_CALL xvec4_dot_f(XFLOAT4* v0, XFLOAT4* v1);
inline XFLOAT4 SIMD_CALL xvec4_dot(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_dot(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_cross(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* v2);
inline void SIMD_CALL xvec4_cross(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* v2, XFLOAT4* out);
inline float SIMD_CALL xvec4_lengthsq_f(XFLOAT4* v);
inline XFLOAT4 SIMD_CALL xvec4_lengthsq(XFLOAT4* v);
inline void SIMD_CALL xvec4_lengthsq(XFLOAT4* v, XFLOAT4* out);
inline float SIMD_CALL xvec4_length_f(XFLOAT4* v);
inline XFLOAT4 SIMD_CALL xvec4_length(XFLOAT4* v);
inline void SIMD_CALL xvec4_length(XFLOAT4* v, XFLOAT4* out);
inline void SIMD_CALL xvec4_normalize(XFLOAT4* v);
inline XFLOAT4 SIMD_CALL xvec4_normalized(XFLOAT4* v);
inline void SIMD_CALL xvec4_normalized(XFLOAT4* v, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_lerp(XFLOAT4* v0, XFLOAT4* v1, float t);
inline void SIMD_CALL xvec4_lerp(XFLOAT4* v0, XFLOAT4* v1, float t, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_clamp(XFLOAT4* v, XFLOAT4* vmin, XFLOAT4* vmax);
inline void SIMD_CALL xvec4_clamp(XFLOAT4* v, XFLOAT4* vmin, XFLOAT4* vmax, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_saturate(XFLOAT4* v);
inline void SIMD_CALL xvec4_saturate(XFLOAT4* v, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_min(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_min(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_max(XFLOAT4* v0, XFLOAT4* v1);
inline void SIMD_CALL xvec4_max(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_ceil(XFLOAT4* v);
inline void SIMD_CALL xvec4_ceil(XFLOAT4* v, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_floor(XFLOAT4* v);
inline void SIMD_CALL xvec4_floor(XFLOAT4* v, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_round(XFLOAT4* v);
inline void SIMD_CALL xvec4_round(XFLOAT4* v, XFLOAT4* out);
inline XFLOAT4 SIMD_CALL xvec4_trunc(XFLOAT4* v);
inline void SIMD_CALL xvec4_trunc(XFLOAT4* v, XFLOAT4* out);


//-------------------------------------------------------------------------------
// XQUAT
inline XQUAT SIMD_CALL xquat_zero();
inline void SIMD_CALL xquat_zero(XQUAT* q);
inline XQUAT SIMD_CALL xquat_ident();
inline void SIMD_CALL xquat_ident(XQUAT* q);
inline XQUAT SIMD_CALL xquat_set(float x, float y, float z, float w);
inline void SIMD_CALL xquat_set(float x, float y, float z, float w, XQUAT* out);
inline XQUAT SIMD_CALL xquat_mul(XQUAT* q0, XQUAT* q1);
inline void SIMD_CALL xquat_mul(XQUAT* q0, XQUAT* q1, XQUAT* out);
inline XQUAT SIMD_CALL xquat_mul(XQUAT* q, float f);
inline void SIMD_CALL xquat_mul(XQUAT* q, float f, XQUAT* out);
inline XFLOAT3 SIMD_CALL xquat_mul(XQUAT* q, XFLOAT3* v);
inline void SIMD_CALL xquat_mul(XQUAT* q, XFLOAT3* v, XFLOAT3* out);
inline XQUAT SIMD_CALL xquat_inverse(XQUAT* q);
inline void SIMD_CALL xquat_inverse(XQUAT* q, XQUAT* out);
inline XQUAT SIMD_CALL xquat_conjugate(XQUAT* q);
inline void SIMD_CALL xquat_conjugate(XQUAT* q, XQUAT* out);
inline XQUAT SIMD_CALL xquat_slerp(XQUAT* q0, XQUAT* q1, float t);
inline void SIMD_CALL xquat_slerp(XQUAT* q0, XQUAT* q1, float t, XQUAT* out);
inline XQUAT SIMD_CALL xquat_rotation_yaw_pitch_roll(float yaw, float pitch, float roll);
inline void SIMD_CALL xquat_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, XQUAT* out);
inline XQUAT SIMD_CALL xquat_rotation_yaw_pitch_roll(XFLOAT3* v);
inline void SIMD_CALL xquat_rotation_yaw_pitch_roll(XFLOAT3* v, XQUAT* out);
inline XQUAT SIMD_CALL xquat_rotation_axis(float angle, XFLOAT3* v);
inline void SIMD_CALL xquat_rotation_axis(float angle, XFLOAT3* v, XQUAT* out);
inline XQUAT SIMD_CALL xquat_rotation_axis(float angle, float x, float y, float z);
inline void SIMD_CALL xquat_rotation_axis(float angle, float x, float y, float z, XQUAT* out);
inline XQUAT SIMD_CALL xquat_rotation_matrix(XMATRIX4* m);
inline void SIMD_CALL xquat_rotation_matrix(XMATRIX4* m, XQUAT* out);
inline XQUAT SIMD_CALL xquat_from_lookat(XFLOAT3* forward);
inline void SIMD_CALL xquat_from_lookat(XFLOAT3* forward, XQUAT* out);
inline XQUAT SIMD_CALL xquat_from_lookat(XFLOAT3* forward, XFLOAT3* up);
inline void SIMD_CALL xquat_from_lookat(XFLOAT3* forward, XFLOAT3* up, XQUAT* out);
inline XFLOAT3 SIMD_CALL xquat_to_yaw_pitch_roll(XQUAT* q);
inline void SIMD_CALL xquat_to_yaw_pitch_roll(XQUAT* q, XFLOAT3* out);
inline XFLOAT4 SIMD_CALL xquat_to_axis_angle(XQUAT* q);
inline void SIMD_CALL xquat_to_axis_angle(XQUAT* q, XFLOAT3* axis, float* angle);
inline XMATRIX4 SIMD_CALL xquat_to_matrix(XQUAT* q);
inline void SIMD_CALL xquat_to_matrix(XQUAT* q, XMATRIX4* out);


//-------------------------------------------------------------------------------
// XMATRIX4
inline XMATRIX4 SIMD_CALL xmat4_zero();
inline void SIMD_CALL xmat4_zero(XMATRIX4* m);
inline XMATRIX4 SIMD_CALL xmat4_ident();
inline void SIMD_CALL xmat4_ident(XMATRIX4* m);
inline bool SIMD_CALL xmat4_is_ident(XMATRIX4* m);
inline bool SIMD_CALL xmat4_is_nan(XMATRIX4* m);
inline bool SIMD_CALL xmat4_is_infinite(XMATRIX4* m);
inline XMATRIX4 SIMD_CALL xmat4_set(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
);
inline void SIMD_CALL xmat4_set(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33,
    XMATRIX4* out
);
inline XMATRIX4 SIMD_CALL xmat4_trans(XFLOAT3* pos);
inline void SIMD_CALL xmat4_trans(XFLOAT3* pos, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_trans(float x, float y, float z);
inline void SIMD_CALL xmat4_trans(float x, float y, float z, XMATRIX4* out);
inline XFLOAT3 SIMD_CALL xmat4_get_trans(XMATRIX4* m);
inline void SIMD_CALL xmat4_get_trans(XMATRIX4* m, XFLOAT3* out);
inline XMATRIX4 SIMD_CALL xmat4_transpose(XMATRIX4* m);
inline void SIMD_CALL xmat4_transpose(XMATRIX4* m, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_inverse(XMATRIX4* m);
inline void SIMD_CALL xmat4_inverse(XMATRIX4* m, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_mul(XMATRIX4* m0, XMATRIX4* m1);
inline void SIMD_CALL xmat4_mul(XMATRIX4* m0, XMATRIX4* m1, XMATRIX4* out);
inline XFLOAT3 SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT3* v);
inline void SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT3* v, XFLOAT3* out);
inline XFLOAT4 SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT4* v);
inline void SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT4* v, XFLOAT4* out);
inline XMATRIX4 SIMD_CALL xmat4_scale(float sx, float sy, float sz);
inline void SIMD_CALL xmat4_scale(float sx, float sy, float sz, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_scale(XFLOAT3* v);
inline void SIMD_CALL xmat4_scale(XFLOAT3* v, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_rotation_x(float angle);
inline void SIMD_CALL xmat4_rotation_x(float angle, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_rotation_y(float angle);
inline void SIMD_CALL xmat4_rotation_y(float angle, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_rotation_z(float angle);
inline void SIMD_CALL xmat4_rotation_z(float angle, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll);
inline void SIMD_CALL xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_rotation_axis(XFLOAT3* axis, float angle);
inline void SIMD_CALL xmat4_rotation_axis(XFLOAT3* axis, float angle, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_rotation_quat(XQUAT* q);
inline void SIMD_CALL xmat4_rotation_quat(XQUAT* q, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_lookto_lh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up);
inline void SIMD_CALL xmat4_lookto_lh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_lookto_rh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up);
inline void SIMD_CALL xmat4_lookto_rh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ);
inline void SIMD_CALL xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ);
inline void SIMD_CALL xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void SIMD_CALL xmat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void SIMD_CALL xmat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_ortho_lh(float width, float height, float nearZ, float farZ);
inline void SIMD_CALL xmat4_ortho_lh(float width, float height, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_ortho_rh(float width, float height, float nearZ, float farZ);
inline void SIMD_CALL xmat4_ortho_rh(float width, float height, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void SIMD_CALL xmat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void SIMD_CALL xmat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ);
inline void SIMD_CALL xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4* out);
inline XMATRIX4 SIMD_CALL xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ);
inline void SIMD_CALL xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4* out);

#include "mathlib_sse.inl"
#endif // __TARGET_OS_WINDOWS

#endif // __MATHLIB_SSE_H__