__m128 xmvec2_dot(__m128 v0, __m128 v1)
{
	return _mm_dp_ps(v0, v1, 0x3F);
}

__m128 xmvec3_dot(__m128 v0, __m128 v1)
{
	return _mm_dp_ps(v0, v1, 0x7F);
}

__m128 xmvec4_dot(__m128 v0, __m128 v1)
{
	return _mm_dp_ps(v0, v1, 0xFF);
}

__m128 xmvec2_cross(__m128 v0, __m128 v1)
{
    // a.x * b.y
    __m128 xy = _mm_mul_ss(_mm_shuffle_ps(v0, v0, _MM_SHUFFLE(0, 0, 0, 0)),
        _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(0, 0, 0, 1)));

    // a.y * b.x
    __m128 yx = _mm_mul_ss(_mm_shuffle_ps(v0, v0, _MM_SHUFFLE(0, 0, 0, 1)),
        _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(0, 0, 0, 0)));

    // a.x * b.y - a.y * b.x
    __m128 result = _mm_sub_ss(xy, yx);

    float scale;
    _mm_store_ss(&scale, result);

    __m128 r = _mm_set_ps(0, 0, scale, scale);

    return r;
}

__m128 xmvec3_cross(__m128 v0, __m128 v1)
{
    // v0을 셔플링하여 v0을 (y,z,x,w) 순서로 재배열
    __m128 v0yzx = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 0, 2, 1));

    // v1을 셔플링하여 v1을 (z,x,y,w) 순서로 재배열
    __m128 v1zxy = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2));

    // v0yzx * v1zxy 계산
    __m128 prod1 = _mm_mul_ps(v0yzx, v1zxy);

    // v1을 (y,z,x,w) 순서로 재배열
    __m128 v1yzx = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));

    // v0을 (z,x,y,w) 순서로 재배열
    __m128 v0zxy = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 1, 0, 2));

    // v1yzx * v0zxy 계산
    __m128 prod2 = _mm_mul_ps(v1yzx, v0zxy);

    // 외적 계산: prod1 - prod2
    return _mm_sub_ps(prod1, prod2);
}

__m128 xmvec4_cross(__m128 v0, __m128 v1, __m128 v2)
{
    // 각 성분의 조합을 위한 셔플 마스크 상수
    const int MASK_YZW = _MM_SHUFFLE(3, 2, 1, 1);  // y, z, w 성분
    const int MASK_XZW = _MM_SHUFFLE(3, 2, 0, 0);  // x, z, w 성분
    const int MASK_XYW = _MM_SHUFFLE(3, 1, 0, 0);  // x, y, w 성분
    const int MASK_XYZ = _MM_SHUFFLE(2, 1, 0, 0);  // x, y, z 성분

    // x 성분 계산 - 첫 번째 3x3 행렬식
    // | v1.y v1.z v1.w |
    // | v2.y v2.z v2.w |
    // | v3.y v3.z v3.w |

    // v1의 (y,z,w) 성분
    __m128 v0_yzw = _mm_shuffle_ps(v0, v0, MASK_YZW);
    __m128 v1_yzw = _mm_shuffle_ps(v1, v1, MASK_YZW);
    __m128 v2_yzw = _mm_shuffle_ps(v2, v2, MASK_YZW);

    // 첫 번째 행렬식 계산을 위한 부분 계산
    __m128 temp1, temp2, temp3, result_x;

    // 첫 번째 부분 계산: v1.y * (v2.z * v3.w - v2.w * v3.z)
    temp1 = _mm_shuffle_ps(v1_yzw, v1_yzw, _MM_SHUFFLE(0, 0, 0, 0));
    temp2 = _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(1, 2, 1, 1));
    temp3 = _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(2, 1, 2, 2));
    __m128 v2zw_v2wz = _mm_mul_ps(temp2, _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(1, 0, 1, 1)));
    result_x = _mm_mul_ps(temp1, _mm_sub_ps(v2zw_v2wz, _mm_mul_ps(temp3, _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(0, 1, 0, 0)))));

    // 나머지 행렬식 부분 계산 (y, z, w 성분도 유사하게)
    // 이 부분은 계산이 복잡하고 많은 셔플과 곱셈 연산이 필요합니다

    // 간소화를 위해 여기서는 스칼라 방식으로 계산 후 SSE 레지스터에 저장합니다
    // 실제 구현에서는 더 많은 SSE 연산을 활용할 수 있습니다

    // 행렬식 계산을 위한 임시 변수
    //float a1 = v0->x, a2 = v0->y, a3 = v0->z, a4 = v0->w;
    float a1 = v0.m128_f32[0], a2 = v0.m128_f32[1], a3 = v0.m128_f32[2], a4 = v0.m128_f32[3];
    float b1 = v1.m128_f32[0], b2 = v1.m128_f32[1], b3 = v1.m128_f32[2], b4 = v1.m128_f32[3];
    float c1 = v2.m128_f32[0], c2 = v2.m128_f32[1], c3 = v2.m128_f32[2], c4 = v2.m128_f32[3];


    // x 성분 계산 (부호: +)
    float x = a2 * (b3 * c4 - b4 * c3) - a3 * (b2 * c4 - b4 * c2) + a4 * (b2 * c3 - b3 * c2);

    // y 성분 계산 (부호: -)
    float y = -(a1 * (b3 * c4 - b4 * c3) - a3 * (b1 * c4 - b4 * c1) + a4 * (b1 * c3 - b3 * c1));

    // z 성분 계산 (부호: +)
    float z = a1 * (b2 * c4 - b4 * c2) - a2 * (b1 * c4 - b4 * c1) + a4 * (b1 * c2 - b2 * c1);

    // w 성분 계산 (부호: -)
    float w = -(a1 * (b2 * c3 - b3 * c2) - a2 * (b1 * c3 - b3 * c1) + a3 * (b1 * c2 - b2 * c1));

    // 결과를 SSE 레지스터에 저장
    return _mm_set_ps(w, z, y, x);
}

__m128 xmvec2_lengthsq(__m128 v)
{
    return xmvec2_dot(v, v);
}

__m128 xmvec3_lengthsq(__m128 v)
{
    return xmvec3_dot(v, v);
}

__m128 xmvec4_lengthsq(__m128 v)
{
    return xmvec4_dot(v, v);
}

__m128 xmvec2_length(__m128 v)
{
    __m128 lengthSq = xmvec2_lengthsq(v);
    return _mm_sqrt_ps(lengthSq);
}

__m128 xmvec3_length(__m128 v)
{
    __m128 lengthSq = xmvec3_lengthsq(v);
    return _mm_sqrt_ps(lengthSq);
}

__m128 xmvec4_length(__m128 v)
{
    __m128 lengthSq = xmvec4_lengthsq(v);
    return _mm_sqrt_ps(lengthSq);
}

__m128 xmvec2_normalize(__m128 v)
{
    __m128 length = xmvec2_length(v);
    if (length.m128_f32[0] < TINY_NUMBER)
        return v;
    __m128 inv_len = _mm_div_ps(XMM_GLOBAL_ONE, length);
    return _mm_mul_ps(v, inv_len);
}

__m128 xmvec3_normalize(__m128 v)
{
    __m128 length = xmvec3_length(v);
    if (length.m128_f32[0] < TINY_NUMBER)
        return v;
    __m128 inv_len = _mm_div_ps(XMM_GLOBAL_ONE, length);
    return _mm_mul_ps(v, inv_len);
}

__m128 xmvec4_normalize(__m128 v)
{
    __m128 length = xmvec4_length(v);
    if (length.m128_f32[0] < TINY_NUMBER)
        return v;
    __m128 inv_len = _mm_div_ps(XMM_GLOBAL_ONE, length);
    return _mm_mul_ps(v, inv_len);
}

__m128 xmvec_lerp(__m128 v0, __m128 v1, float t)
{
    // V0 + ((V1 - V0) * t)
    __m128 tv = _mm_set_ps1(t);
    __m128 diff = _mm_sub_ps(v1, v0);
    __m128 scaled = _mm_mul_ps(diff, tv);
    __m128 result = _mm_add_ps(v0, scaled);

    return result;
}
__m128 xmvec_clamp(__m128 v, __m128 vmin, __m128 vmax)
{
    return _mm_min_ps(_mm_max_ps(v, vmin), vmax);
}

__m128 xmvec_saturate(__m128 v)
{
    return _mm_min_ps(_mm_max_ps(v, XMM_GLOBAL_ZERO), XMM_GLOBAL_ONE);
}

__m128 xmvec_min(__m128 v0, __m128 v1)
{
    return _mm_min_ps(v0, v1);
}

__m128 xmvec_max(__m128 v0, __m128 v1)
{
    return _mm_max_ps(v0, v1);
}
__m128 xmvec_ceil(__m128 v) // 올림
{
    return _mm_ceil_ps(v);
}
__m128 xmvec_floor(__m128 v) // 버림
{
    return _mm_floor_ps(v);
}
__m128 xmvec_round(__m128 v) // 반올림
{
    return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

__m128 xmvec_trunc(__m128 v) // 버림
{
    return _mm_trunc_ps(v);
}



//------------------------------------------------------
// XFLOAT2 Function Implementation (SSE Optimization)
//------------------------------------------------------

inline XFLOAT2 SIMD_CALL xvec2_zero()
{
    XFLOAT2 r;
    r.m = _mm_setzero_ps();
    return r;
}

inline void SIMD_CALL xvec2_zero(XFLOAT2* v)
{
    v->m = _mm_setzero_ps();
}

inline XFLOAT2 SIMD_CALL xvec2_unit_x()
{
    XFLOAT2 r;
    r.m = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    return r;
}

inline XFLOAT2 SIMD_CALL xvec2_unit_y()
{
    XFLOAT2 r;
    r.m = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    return r;
}

inline XFLOAT2 SIMD_CALL xvec2_set(float x, float y)
{
    XFLOAT2 r;
    xvec2_set(x, y, &r);
    return r;
}

inline void SIMD_CALL xvec2_set(float x, float y, XFLOAT2* out)
{
    out->m = _mm_set_ps(0, 0, y, x);
}

inline void SIMD_CALL xvec2_add(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = _mm_add_ps(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_add(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_add(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec2_sub(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = _mm_sub_ps(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_sub(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    r.m = _mm_sub_ps(v0->m, v1->m);
    return r;
}

inline void SIMD_CALL xvec2_mul(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = _mm_mul_ps(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_mul(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    r.m = _mm_mul_ps(v0->m, v1->m);
    return r;
}

inline void SIMD_CALL xvec2_mul(XFLOAT2* v, float f, XFLOAT2* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_mul_ps(v->m, scalar);
}

inline XFLOAT2 SIMD_CALL xvec2_mul(XFLOAT2* v, float f)
{
    XFLOAT2 r;
    xvec2_mul(v, f, &r);
    return r;
}

inline void SIMD_CALL xvec2_div(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = _mm_div_ps(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_div(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_div(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec2_div(XFLOAT2* v, float f, XFLOAT2* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_div_ps(v->m, scalar);
}

inline XFLOAT2 SIMD_CALL xvec2_div(XFLOAT2* v, float f)
{
    XFLOAT2 r;
    xvec2_div(v, f, &r);
    return r;
}

inline float SIMD_CALL xvec2_dot_f(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_dot(v0, v1, &r);
    return r.x;
}

inline void SIMD_CALL xvec2_dot(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = xmvec2_dot(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_dot(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_dot(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec2_cross(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = xmvec2_cross(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_cross(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_cross(v0, v1, &r);
    return r;
}

inline float SIMD_CALL xvec2_lengthsq_f(XFLOAT2* v)
{
    return xvec2_dot_f(v, v);
}

inline void SIMD_CALL xvec2_lengthsq(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec2_dot(v->m, v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_lengthsq(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_lengthsq(v, &r);
    return r;
}

inline float SIMD_CALL xvec2_length_f(XFLOAT2* v)
{
    __m128 r = xmvec2_length(v->m);
    return _mm_cvtss_f32(r);
}

inline void SIMD_CALL xvec2_length(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec2_length(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_length(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_length(v, &r);
    return r;
}

inline void SIMD_CALL xvec2_normalize(XFLOAT2* v)
{
    v->m = xmvec2_normalize(v->m);
}

inline void SIMD_CALL xvec2_normalized(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec2_normalize(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_normalized(XFLOAT2* v)
{
    XFLOAT2 result = *v;
    xvec2_normalize(&result);
    return result;
}

inline void SIMD_CALL xvec2_lerp(XFLOAT2* v0, XFLOAT2* v1, float t, XFLOAT2* out)
{
    // V0 + ((V1 - V0) * t)
    out->m = xmvec_lerp(v0->m, v1->m, t);
}

inline XFLOAT2 SIMD_CALL xvec2_lerp(XFLOAT2* v0, XFLOAT2* v1, float t)
{
    // V0 + ((V1 - V0) * t)
    XFLOAT2 r;
    xvec2_lerp(v0, v1, t, &r);
    return r;
}

inline void SIMD_CALL xvec2_clamp(XFLOAT2* v, XFLOAT2* vmin, XFLOAT2* vmax, XFLOAT2* out)
{
    out->m = xmvec_clamp(v->m, vmin->m, vmax->m);
}

inline XFLOAT2 SIMD_CALL xvec2_clamp(XFLOAT2* v, XFLOAT2* vmin, XFLOAT2* vmax)
{
    XFLOAT2 r;
    xvec2_clamp(v, vmin, vmax, &r);
    return r;
}

inline void SIMD_CALL xvec2_saturate(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec_saturate(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_saturate(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_saturate(v, &r);
    return r;
}

inline void SIMD_CALL xvec2_min(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = xmvec_min(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_min(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_min(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec2_max(XFLOAT2* v0, XFLOAT2* v1, XFLOAT2* out)
{
    out->m = xmvec_max(v0->m, v1->m);
}

inline XFLOAT2 SIMD_CALL xvec2_max(XFLOAT2* v0, XFLOAT2* v1)
{
    XFLOAT2 r;
    xvec2_max(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec2_ceil(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec_ceil(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_ceil(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_ceil(v, &r);
    return r;
}

inline void SIMD_CALL xvec2_floor(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec_floor(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_floor(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_floor(v, &r);
    return r;
}

inline void SIMD_CALL xvec2_round(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec_round(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_round(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_round(v, &r);
    return r;
}

inline void SIMD_CALL xvec2_trunc(XFLOAT2* v, XFLOAT2* out)
{
    out->m = xmvec_trunc(v->m);
}

inline XFLOAT2 SIMD_CALL xvec2_trunc(XFLOAT2* v)
{
    XFLOAT2 r;
    xvec2_trunc(v, &r);
    return r;
}


//------------------------------------------------------
// XFLOAT3 Function Implementation (SSE Optimization)
//------------------------------------------------------
inline XFLOAT3 SIMD_CALL xvec3_zero()
{
    XFLOAT3 r;
    xvec3_zero(&r);
    return r;
}

inline void SIMD_CALL xvec3_zero(XFLOAT3* v)
{
    v->m = _mm_setzero_ps();
}

inline XFLOAT3 SIMD_CALL xvec3_unit_x()
{
    XFLOAT3 r;
    r.m = _mm_set_ps(0, 0, 0, 1.0f);
    return r;
}

inline XFLOAT3 SIMD_CALL xvec3_unit_y()
{
    XFLOAT3 r;
    r.m = _mm_set_ps(0, 0, 1.0f, 0);
    return r;
}

inline XFLOAT3 SIMD_CALL xvec3_unit_z()
{
    XFLOAT3 r;
    r.m = _mm_set_ps(0, 1.0f, 0, 0);
    return r;
}

inline XFLOAT3 SIMD_CALL xvec3_set(float x, float y)
{
    XFLOAT3 r;
    xvec3_set(x, y, &r);
    return r;
}

inline XFLOAT3 SIMD_CALL xvec3_set(float x, float y, float z)
{
    XFLOAT3 r;
    xvec3_set(x, y, z, &r);
    return r;
}

inline void SIMD_CALL xvec3_set(float x, float y, XFLOAT3* out)
{
    out->m = _mm_set_ps(0.0f, 0.0f, y, x);
}

inline void SIMD_CALL xvec3_set(float x, float y, float z, XFLOAT3* out)
{
    out->m = _mm_set_ps(0.0f, z, y, x);
}

inline XFLOAT3 SIMD_CALL xvec3_add(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_add(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_add(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = _mm_add_ps(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_sub(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_sub(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_sub(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = _mm_sub_ps(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_mul(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_mul(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_mul(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = _mm_mul_ps(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_mul(XFLOAT3* v, float f)
{
    XFLOAT3 r;
    xvec3_mul(v, f, &r);
    return r;
}

inline void SIMD_CALL xvec3_mul(XFLOAT3* v, float f, XFLOAT3* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_mul_ps(v->m, scalar);
}

inline XFLOAT3 SIMD_CALL xvec3_div(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_div(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_div(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = _mm_div_ps(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_div(XFLOAT3* v, float f)
{
    XFLOAT3 r;
    xvec3_div(v, f, &r);
    return r;
}

inline void SIMD_CALL xvec3_div(XFLOAT3* v, float f, XFLOAT3* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_div_ps(v->m, scalar);
}

inline float SIMD_CALL xvec3_dot_f(XFLOAT3* v0, XFLOAT3* v1)
{
    __m128 r = xmvec3_dot(v0->m, v1->m);
    return _mm_cvtss_f32(r);
}

inline XFLOAT3 SIMD_CALL xvec3_dot(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_dot(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_dot(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = xmvec3_dot(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_cross(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_cross(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_cross(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = xmvec3_cross(v0->m, v1->m);
}


inline float SIMD_CALL xvec3_lengthsq_f(XFLOAT3* v)
{
    __m128 r = xmvec3_lengthsq(v->m);
    return _mm_cvtss_f32(r);
}

inline XFLOAT3 SIMD_CALL xvec3_lengthsq(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_lengthsq(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_lengthsq(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec2_lengthsq(v->m);
}

inline float SIMD_CALL xvec3_length_f(XFLOAT3* v)
{
    __m128 r = xmvec3_length(v->m);
    return _mm_cvtss_f32(r);
}

inline XFLOAT3 SIMD_CALL xvec3_length(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_length(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_length(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec3_length(v->m);
}

inline void SIMD_CALL xvec3_normalize(XFLOAT3* v)
{
    xmvec3_normalize(v->m);
}

inline XFLOAT3 SIMD_CALL xvec3_normalized(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_normalized(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_normalized(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec3_normalize(v->m);
}

inline XFLOAT3 SIMD_CALL xvec3_lerp(XFLOAT3* v0, XFLOAT3* v1, float t)
{
    XFLOAT3 r;
    xvec3_lerp(v0, v1, t, &r);
    return r;
}

inline void SIMD_CALL xvec3_lerp(XFLOAT3* v0, XFLOAT3* v1, float t, XFLOAT3* out)
{
    // V0 + ((V1 - V0) * t)
    out->m = xmvec_lerp(v0->m, v1->m, t);
}

inline XFLOAT3 SIMD_CALL xvec3_clamp(XFLOAT3* v, XFLOAT3* vmin, XFLOAT3* vmax)
{
    XFLOAT3 r;
    xvec3_clamp(v, vmin, vmax, &r);
    return r;
}

inline void SIMD_CALL xvec3_clamp(XFLOAT3* v, XFLOAT3* vmin, XFLOAT3* vmax, XFLOAT3* out)
{
    out->m = xmvec_clamp(v->m, vmin->m, vmax->m);
}

inline XFLOAT3 SIMD_CALL xvec3_saturate(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_saturate(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_saturate(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec_saturate(v->m);
}

inline XFLOAT3 SIMD_CALL xvec3_min(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_min(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_min(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = xmvec_min(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_max(XFLOAT3* v0, XFLOAT3* v1)
{
    XFLOAT3 r;
    xvec3_max(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec3_max(XFLOAT3* v0, XFLOAT3* v1, XFLOAT3* out)
{
    out->m = xmvec_max(v0->m, v1->m);
}

inline XFLOAT3 SIMD_CALL xvec3_ceil(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_ceil(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_ceil(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec_ceil(v->m);
}

inline XFLOAT3 SIMD_CALL xvec3_floor(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_floor(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_floor(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec_floor(v->m);
}

inline XFLOAT3 SIMD_CALL xvec3_round(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_round(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_round(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec_round(v->m);
}

inline XFLOAT3 SIMD_CALL xvec3_trunc(XFLOAT3* v)
{
    XFLOAT3 r;
    xvec3_trunc(v, &r);
    return r;
}

inline void SIMD_CALL xvec3_trunc(XFLOAT3* v, XFLOAT3* out)
{
    out->m = xmvec_trunc(v->m);
}



//------------------------------------------------------
// XFLOAT4 Function Implementation (SSE Optimization)
//------------------------------------------------------

inline XFLOAT4 SIMD_CALL xvec4_zero()
{
    XFLOAT4 r;
    xvec4_zero(&r);
    return r;
}

inline void SIMD_CALL xvec4_zero(XFLOAT4* v)
{
    v->m = _mm_setzero_ps();
}

inline XFLOAT4 SIMD_CALL xvec4_unit_x()
{
    XFLOAT4 r;
    r.m = _mm_set_ps(0, 0, 0, 1.0f);
    return r;
}

inline XFLOAT4 SIMD_CALL xvec4_unit_y()
{
    XFLOAT4 r;
    r.m = _mm_set_ps(0, 0, 1.0f, 0.0f);
    return r;
}

inline XFLOAT4 SIMD_CALL xvec4_unit_z()
{
    XFLOAT4 r;
    r.m = _mm_set_ps(0, 1.0f, 0, 0);
    return r;
}

inline XFLOAT4 SIMD_CALL xvec4_unit_w()
{
    XFLOAT4 r;
    r.m = _mm_set_ps(1.0f, 0, 0, 0);
    return r;
}

inline XFLOAT4 SIMD_CALL xvec4_set(float x, float y, float z, float w)
{
    XFLOAT4 r;
    xvec4_set(x, y, z, w, &r);
    return r;
}

inline XFLOAT4 SIMD_CALL xvec4_set(float x, float y, float z)
{
    XFLOAT4 r;
    xvec4_set(x, y, z, &r);
    return r;
}

inline void SIMD_CALL xvec4_set(float x, float y, float z, float w, XFLOAT4* out)
{
    out->m = _mm_set_ps(w, z, y, x);
}

inline void SIMD_CALL xvec4_set(float x, float y, float z, XFLOAT4* out)
{
    out->m = _mm_set_ps(0, z, y, x);
}

inline XFLOAT4 SIMD_CALL xvec4_add(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_add(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_add(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = _mm_add_ps(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_sub(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_sub(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_sub(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = _mm_sub_ps(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_mul(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_mul(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_mul(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = _mm_mul_ps(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_mul(XFLOAT4* v, float f)
{
    XFLOAT4 r;
    xvec4_mul(v, f, &r);
    return r;
}

inline void SIMD_CALL xvec4_mul(XFLOAT4* v, float f, XFLOAT4* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_mul_ps(v->m, scalar);
}

inline XFLOAT4 SIMD_CALL xvec4_div(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_div(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_div(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = _mm_div_ps(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_div(XFLOAT4* v, float f)
{
    XFLOAT4 r;
    xvec4_div(v, f, &r);
    return r;
}

inline void SIMD_CALL xvec4_div(XFLOAT4* v, float f, XFLOAT4* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_div_ps(v->m, scalar);
}

inline float SIMD_CALL xvec4_dot_f(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_dot(v0, v1, &r);
    return _mm_cvtss_f32(r.m);
}

inline XFLOAT4 SIMD_CALL xvec4_dot(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_dot(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_dot(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = xmvec4_dot(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_cross(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* v2)
{
    XFLOAT4 r;
    xvec4_cross(v0, v1, v2, &r);
    return r;
}

inline void SIMD_CALL xvec4_cross(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* v2, XFLOAT4* out)
{
    out->m = xmvec4_cross(v0->m, v1->m, v2->m);
}

inline float SIMD_CALL xvec4_lengthsq_f(XFLOAT4* v)
{
    return xvec4_dot_f(v, v);
}

inline XFLOAT4 SIMD_CALL xvec4_lengthsq(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_lengthsq(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_lengthsq(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec4_dot(v->m, v->m);
}

inline float SIMD_CALL xvec4_length_f(XFLOAT4* v)
{
    __m128 r = xmvec4_length(v->m);
    return _mm_cvtss_f32(r);
}

inline XFLOAT4 SIMD_CALL xvec4_length(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_length(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_length(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec4_length(v->m);
}

inline void SIMD_CALL xvec4_normalize(XFLOAT4* v)
{
    v->m = xmvec4_normalize(v->m);
}

inline XFLOAT4 SIMD_CALL xvec4_normalized(XFLOAT4* v)
{
    XFLOAT4 result = *v;
    xvec4_normalize(&result);
    return result;
}

inline void SIMD_CALL xvec4_normalized(XFLOAT4* v, XFLOAT4* out)
{
    *out = *v;
    xvec4_normalize(out);
}

inline XFLOAT4 SIMD_CALL xvec4_lerp(XFLOAT4* v0, XFLOAT4* v1, float t)
{
    XFLOAT4 r;
    xvec4_lerp(v0, v1, t, &r);
    return r;
}

inline void SIMD_CALL xvec4_lerp(XFLOAT4* v0, XFLOAT4* v1, float t, XFLOAT4* out)
{
    out->m = xmvec_lerp(v0->m, v1->m, t);
}

inline XFLOAT4 SIMD_CALL xvec4_clamp(XFLOAT4* v, XFLOAT4* vmin, XFLOAT4* vmax)
{
    XFLOAT4 r;
    xvec4_clamp(v, vmin, vmax, &r);
    return r;
}

inline void SIMD_CALL xvec4_clamp(XFLOAT4* v, XFLOAT4* vmin, XFLOAT4* vmax, XFLOAT4* out)
{
    out->m = xmvec_clamp(v->m, vmin->m, vmax->m);
}

inline XFLOAT4 SIMD_CALL xvec4_saturate(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_saturate(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_saturate(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec_saturate(v->m);
}

inline XFLOAT4 SIMD_CALL xvec4_min(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_min(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_min(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = xmvec_min(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_max(XFLOAT4* v0, XFLOAT4* v1)
{
    XFLOAT4 r;
    xvec4_max(v0, v1, &r);
    return r;
}

inline void SIMD_CALL xvec4_max(XFLOAT4* v0, XFLOAT4* v1, XFLOAT4* out)
{
    out->m = xmvec_max(v0->m, v1->m);
}

inline XFLOAT4 SIMD_CALL xvec4_ceil(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_ceil(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_ceil(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec_ceil(v->m);
}

inline XFLOAT4 SIMD_CALL xvec4_floor(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_floor(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_floor(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec_floor(v->m);
}

inline XFLOAT4 SIMD_CALL xvec4_round(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_round(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_round(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec_round(v->m);
}

inline XFLOAT4 SIMD_CALL xvec4_trunc(XFLOAT4* v)
{
    XFLOAT4 r;
    xvec4_trunc(v, &r);
    return r;
}

inline void SIMD_CALL xvec4_trunc(XFLOAT4* v, XFLOAT4* out)
{
    out->m = xmvec_trunc(v->m);
}


//------------------------------------------------------
// XQUAT Functions Implementation (SSE Optimized)
//------------------------------------------------------

inline XQUAT SIMD_CALL xquat_zero()
{
    XQUAT r;
    xquat_zero(&r);
    return r;
}

inline void SIMD_CALL xquat_zero(XQUAT* q)
{
    q->m = _mm_setzero_ps();
}

inline XQUAT SIMD_CALL xquat_ident()
{
    // Identity quaternion (0, 0, 0, 1)
    XQUAT r;
    xquat_ident(&r);
    return r;
}

inline void SIMD_CALL xquat_ident(XQUAT* q)
{
    // Identity quaternion (0, 0, 0, 1)
    q->m = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
}

inline XQUAT SIMD_CALL xquat_set(float x, float y, float z, float w)
{
    XQUAT r;
    xquat_set(x, y, z, w, &r);
    return r;
}

inline void SIMD_CALL xquat_set(float x, float y, float z, float w, XQUAT* out)
{
    out->m = _mm_set_ps(w, z, y, x);
}

inline XQUAT SIMD_CALL xquat_mul(XQUAT* q0, XQUAT* q1)
{
    XQUAT r;
    xquat_mul(q0, q1, &r);
    return r;
}

inline void SIMD_CALL xquat_mul(XQUAT* q0, XQUAT* q1, XQUAT* out)
{
    // 쿼터니언 곱셈 공식에 따라 직접 계산
     // out.x = q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y
     // out.y = q0.w * q1.y - q0.x * q1.z + q0.y * q1.w + q0.z * q1.x
     // out.z = q0.w * q1.z + q0.x * q1.y - q0.y * q1.x + q0.z * q1.w
     // out.w = q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z

     // 각 성분을 SSE 레지스터에 로드
    __m128 Q0 = _mm_set_ps(q0->w, q0->z, q0->y, q0->x);
    __m128 Q1 = _mm_set_ps(q1->w, q1->z, q1->y, q1->x);

    // 각 성분을 모든 위치에 복제
    __m128 q0w = _mm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(3, 3, 3, 3));
    __m128 q0x = _mm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 q0y = _mm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 q0z = _mm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(2, 2, 2, 2));

    // q1 셔플링
    __m128 q1w = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(3, 3, 3, 3));
    __m128 q1x = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 q1y = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 q1z = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(2, 2, 2, 2));

    // 각 항의 계산
    __m128 term1 = _mm_mul_ps(q0w, Q1);  // q0.w * (q1.x, q1.y, q1.z, q1.w)

    // x 항: q0.x * (q1.w, -q1.z, q1.y, -q1.x)
    __m128 x_terms = _mm_mul_ps(q0x, _mm_set_ps(-q1->x, q1->y, -q1->z, q1->w));

    // y 항: q0.y * (q1.z, q1.w, -q1.x, -q1.y)
    __m128 y_terms = _mm_mul_ps(q0y, _mm_set_ps(-q1->y, -q1->x, q1->w, q1->z));

    // z 항: q0.z * (-q1.y, q1.x, q1.w, -q1.z)
    __m128 z_terms = _mm_mul_ps(q0z, _mm_set_ps(-q1->z, q1->w, q1->x, -q1->y));

    // 모든 항 합산
    __m128 result = _mm_add_ps(term1, x_terms);
    result = _mm_add_ps(result, y_terms);
    result = _mm_add_ps(result, z_terms);

    // 결과를 출력 벡터에 저장
    _mm_store_ps(out->v, result);
}

inline XQUAT SIMD_CALL xquat_mul(XQUAT* q, float f)
{
    XQUAT r;
    xquat_mul(q, f, &r);
    return r;
}

inline void SIMD_CALL xquat_mul(XQUAT* q, float f, XQUAT* out)
{
    __m128 scalar = _mm_set1_ps(f);
    out->m = _mm_mul_ps(q->m, scalar);
}

inline XFLOAT3 SIMD_CALL xquat_mul(XQUAT* q, XFLOAT3* v)
{
    XFLOAT3 r;
    xquat_mul(q, v, &r);
    return r;
}

inline void SIMD_CALL xquat_mul(XQUAT* q, XFLOAT3* v, XFLOAT3* out)
{
    // nVidia SDK implementation reference
    // uv = cross(q.xyz, v)
    // uuv = cross(q.xyz, uv)
    // result = v + 2 * (uv * q.w + uuv)

    XFLOAT3 t{ q->x, q->y, q->z };
    XFLOAT3 uv, uuv;
    xvec3_cross(&t, v, &uv);
    xvec3_cross(&t, &uv, &uuv);

    __m128 q_sse = _mm_load_ps(q->v);
    __m128 uv_sse = _mm_load_ps(uv.v);
    __m128 uuv_sse = _mm_load_ps(uuv.v);
    __m128 v_sse = _mm_load_ps(v->v);

    // Multiply uv by q.w
    __m128 qW = _mm_shuffle_ps(q_sse, q_sse, _MM_SHUFFLE(3, 3, 3, 3)); // [q.w, q.w, q.w, q.w]
    __m128 uvByQw = _mm_mul_ps(uv_sse, qW);

    // Calculate 2 * (uv * q.w + uuv)
    __m128 two = _mm_set1_ps(2.0f);
    __m128 combined = _mm_add_ps(uvByQw, uuv_sse);
    __m128 scaled = _mm_mul_ps(combined, two);

    // Calculate final result: v + scaled
    __m128 result = _mm_add_ps(v_sse, scaled);

    _mm_store_ps(out->v, result);
}

inline XQUAT SIMD_CALL xquat_inverse(XQUAT* q)
{
    XQUAT r;
    xquat_inverse(q, &r);
    return r;
}

inline void SIMD_CALL xquat_inverse(XQUAT* q, XQUAT* out)
{
    // Calculate conjugate and normalize
    // Conjugate: (-x, -y, -z, w)
    __m128 conjugate = _mm_xor_ps(q->m, _mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f));

    // Calculate squared norm
    float norm = xvec4_dot_f(q, q);

    // Check if norm is close to zero
    if (norm < TINY_NUMBER)
    {
        *out = xquat_ident();
        return;
    }

    // Normalize the conjugate
    __m128 invNorm = _mm_set1_ps(1.0f / norm);
    out->m = _mm_mul_ps(conjugate, invNorm);
}

inline XQUAT SIMD_CALL xquat_conjugate(XQUAT* q)
{
    // Conjugate: (-x, -y, -z, w)
    XQUAT r;
    xquat_conjugate(q, &r);
    return r;
}

inline void SIMD_CALL xquat_conjugate(XQUAT* q, XQUAT* out)
{
    // Conjugate: (-x, -y, -z, w)
    out->m = _mm_xor_ps(q->m, _mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f));
}

inline XQUAT SIMD_CALL xquat_slerp(XQUAT* q0, XQUAT* q1, float t)
{
    XQUAT r;
    xquat_slerp(q0, q1, t, &r);
    return r;
}

inline void SIMD_CALL xquat_slerp(XQUAT* q0, XQUAT* q1, float t, XQUAT* out)
{
    // Calculate cosine of angle between quaternions
    float cosTheta = xvec4_dot_f(q0, q1);

    // If quaternions are very close, use linear interpolation
    if (cosTheta > 0.9995f)
    {
        xvec4_lerp(q0, q1, t, out);
        return;
    }

    // Adjust sign and angle direction
    XQUAT q1Adj = *q1;
    if (cosTheta < 0.0f)
    {
        q1Adj.m = _mm_xor_ps(q1->m, _mm_set1_ps(-0.0f)); // Change sign
        cosTheta = -cosTheta;
    }

    // Calculate angle and its sine
    float theta = acosf(cosTheta);
    float sinTheta = sinf(theta);

    // Check if sine of angle is too small
    if (fabsf(sinTheta) < TINY_NUMBER)
    {
        *out = *q0;
        return;
    }

    // Calculate interpolation weights
    float w0 = sinf((1.0f - t) * theta) / sinTheta;
    float w1 = sinf(t * theta) / sinTheta;

    // Calculate interpolation
    __m128 weight0 = _mm_set1_ps(w0);
    __m128 weight1 = _mm_set1_ps(w1);

    __m128 weighted0 = _mm_mul_ps(q0->m, weight0);
    __m128 weighted1 = _mm_mul_ps(q1Adj.m, weight1);

    out->m = _mm_add_ps(weighted0, weighted1);
}

inline XQUAT SIMD_CALL xquat_rotation_yaw_pitch_roll(float yaw, float pitch, float roll)
{
    XQUAT r;
    xquat_rotation_yaw_pitch_roll(yaw, pitch, roll, &r);
    return r;
}

inline void SIMD_CALL xquat_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, XQUAT* out)
{
    // Calculate rotation quaternion for each axis
    float halfRoll = roll * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfYaw = yaw * 0.5f;

    float sinRoll = sinf(halfRoll);
    float cosRoll = cosf(halfRoll);
    float sinPitch = sinf(halfPitch);
    float cosPitch = cosf(halfPitch);
    float sinYaw = sinf(halfYaw);
    float cosYaw = cosf(halfYaw);

    // Rotation order: Yaw(Y-axis), Pitch(X-axis), Roll(Z-axis)
    // Combine using quaternion multiplication
    float x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
    float y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
    float z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
    float w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);

    xquat_set(x, y, z, w, out);
}

inline XQUAT SIMD_CALL xquat_rotation_yaw_pitch_roll(XFLOAT3* v)
{
    return xquat_rotation_yaw_pitch_roll(v->y, v->x, v->z);
}

inline void SIMD_CALL xquat_rotation_yaw_pitch_roll(XFLOAT3* v, XQUAT* out)
{
    xquat_rotation_yaw_pitch_roll(v->y, v->x, v->z, out);
}

inline XQUAT SIMD_CALL xquat_rotation_axis(float angle, XFLOAT3* v)
{
    XQUAT r;
    xquat_rotation_axis(angle, v, &r);
    return r;
}

inline void SIMD_CALL xquat_rotation_axis(float angle, XFLOAT3* v, XQUAT* out)
{
    // Normalize axis vector
    XFLOAT3 axis;
    xvec3_normalized(v, &axis);

    // Calculate sine and cosine of half-angle
    float halfAngle = angle * 0.5f;
    float sinHalfAngle = sinf(halfAngle);
    float cosHalfAngle = cosf(halfAngle);

    // Set quaternion values: q = (x*sin(a/2), y*sin(a/2), z*sin(a/2), cos(a/2))
    out->m = _mm_set_ps(
        cosHalfAngle,
        axis.z * sinHalfAngle,
        axis.y * sinHalfAngle,
        axis.x * sinHalfAngle
    );
}

inline XQUAT SIMD_CALL xquat_rotation_axis(float angle, float x, float y, float z)
{
    XFLOAT3 axis;
    xvec3_set(x, y, z, &axis);
    return xquat_rotation_axis(angle, &axis);
}

inline void SIMD_CALL xquat_rotation_axis(float angle, float x, float y, float z, XQUAT* out)
{
    XFLOAT3 axis;
    xvec3_set(x, y, z, &axis);
    xquat_rotation_axis(angle, &axis, out);
}

inline XQUAT SIMD_CALL xquat_rotation_matrix(XMATRIX4* m)
{
    XQUAT r;
    xquat_rotation_matrix(m, &r);
    return r;
}

inline void SIMD_CALL xquat_rotation_matrix(XMATRIX4* m, XQUAT* out)
{
    // Convert matrix to quaternion using Ken Shoemake's algorithm
    float trace = m->m00 + m->m11 + m->m22;

    // Choose best method based on the matrix diagonal
    if (trace > 0.0f)
    {
        // When trace is positive
        float s = sqrtf(trace + 1.0f) * 2.0f;
        float invS = 1.0f / s;

        float x = (m->m21 - m->m12) * invS;
        float y = (m->m02 - m->m20) * invS;
        float z = (m->m10 - m->m01) * invS;
        float w = 0.25f * s;

        xquat_set(x, y, z, w, out);
    }
    else if ((m->m00 > m->m11) && (m->m00 > m->m22))
    {
        // When m00 is largest
        float s = sqrtf(1.0f + m->m00 - m->m11 - m->m22) * 2.0f;
        float invS = 1.0f / s;

        float x = 0.25f * s;
        float y = (m->m01 + m->m10) * invS;
        float z = (m->m02 + m->m20) * invS;
        float w = (m->m21 - m->m12) * invS;

        xquat_set(x, y, z, w, out);
    }
    else if (m->m11 > m->m22)
    {
        // When m11 is largest
        float s = sqrtf(1.0f + m->m11 - m->m00 - m->m22) * 2.0f;
        float invS = 1.0f / s;

        float x = (m->m01 + m->m10) * invS;
        float y = 0.25f * s;
        float z = (m->m12 + m->m21) * invS;
        float w = (m->m02 - m->m20) * invS;

        xquat_set(x, y, z, w, out);
    }
    else
    {
        // When m22 is largest
        float s = sqrtf(1.0f + m->m22 - m->m00 - m->m11) * 2.0f;
        float invS = 1.0f / s;

        float x = (m->m02 + m->m20) * invS;
        float y = (m->m12 + m->m21) * invS;
        float z = 0.25f * s;
        float w = (m->m10 - m->m01) * invS;

        xquat_set(x, y, z, w, out);
    }
}

inline XQUAT SIMD_CALL xquat_from_lookat(XFLOAT3* forward)
{
    XQUAT r;
    xquat_from_lookat(forward, &r);
    return r;
}

inline void SIMD_CALL xquat_from_lookat(XFLOAT3* forward, XQUAT* out)
{
    // Create rotation from [0,0,1] to the forward vector
    // Normalize the vector
    XFLOAT3 normal;
    xvec3_normalized(forward, &normal);

    // Calculate dot product with [0,0,1]
    float dot = xvec3_dot_f(&normal, &xvec3_unit_z());

    // Handle special cases
    // If vectors are nearly parallel
    if (dot > 0.999999f)
    {
        xquat_ident(out);
        return;
    }
    // If vectors are nearly opposite
    else if (dot < -0.999999f)
    {
        // Rotate 180 degrees around Y-axis
        xquat_set(0.0f, 1.0f, 0.0f, 0.0f, out);
        return;
    }
    else
    {
        // Calculate rotation axis (cross product)
        XFLOAT3 axis;
        XFLOAT3 unitZ = xvec3_unit_z();
        xvec3_cross(&unitZ, &normal, &axis);

        // Calculate rotation angle
        float angle = acosf(dot);

        // Create quaternion from axis-angle rotation
        xquat_rotation_axis(angle, &axis, out);
    }
}

inline XQUAT SIMD_CALL xquat_from_lookat(XFLOAT3* forward, XFLOAT3* up)
{
    XQUAT r;
    xquat_from_lookat(forward, up, &r);
    return r;
}

inline void SIMD_CALL xquat_from_lookat(XFLOAT3* forward, XFLOAT3* up, XQUAT* out)
{
    // Normalize forward vector
    XFLOAT3 zAxis;
    xvec3_normalized(forward, &zAxis);

    // Calculate right vector (cross product of up and forward)
    XFLOAT3 xAxis;
    xvec3_cross(up, &zAxis, &xAxis);
    xvec3_normalized(&xAxis, &xAxis);

    // Calculate new up vector (cross product of forward and right)
    XFLOAT3 yAxis;
    xvec3_cross(&zAxis, &xAxis, &yAxis);

    // Create rotation matrix
    XMATRIX4 rotMatrix;

    rotMatrix.m00 = xAxis.x; rotMatrix.m01 = xAxis.y; rotMatrix.m02 = xAxis.z; rotMatrix.m03 = 0.0f;
    rotMatrix.m10 = yAxis.x; rotMatrix.m11 = yAxis.y; rotMatrix.m12 = yAxis.z; rotMatrix.m13 = 0.0f;
    rotMatrix.m20 = zAxis.x; rotMatrix.m21 = zAxis.y; rotMatrix.m22 = zAxis.z; rotMatrix.m23 = 0.0f;
    rotMatrix.m30 = 0.0f;     rotMatrix.m31 = 0.0f;     rotMatrix.m32 = 0.0f;     rotMatrix.m33 = 1.0f;

    // Convert matrix to quaternion
    xquat_rotation_matrix(&rotMatrix, out);
}

inline XFLOAT3 SIMD_CALL xquat_to_yaw_pitch_roll(XQUAT* q)
{
    XFLOAT3 r;
    xquat_to_yaw_pitch_roll(q, &r);
    return r;
}

inline void SIMD_CALL xquat_to_yaw_pitch_roll(XQUAT* q, XFLOAT3* out)
{
    // Convert quaternion to Euler angles (roll, pitch, yaw in ZXY order)

    // Check for gimbal lock
    float test = q->x * q->y + q->z * q->w;
    if (test > 0.499f)
    {
        // Singularity at north pole
        out->x = 2.0f * atan2f(q->x, q->w);  // pitch
        out->y = M_PI_2;                     // yaw
        out->z = 0.0f;                       // roll
        return;
    }
    if (test < -0.499f)
    {
        // Singularity at south pole
        out->x = -2.0f * atan2f(q->x, q->w); // pitch
        out->y = -M_PI_2;                    // yaw
        out->z = 0.0f;                      // roll
        return;
    }

    // Normal case
    float sqx = q->x * q->x;
    float sqy = q->y * q->y;
    float sqz = q->z * q->z;

    // Roll (z-axis rotation)
    out->z = atan2f(2.0f * (q->x * q->y + q->z * q->w), 1.0f - 2.0f * (sqy + sqz));

    // Pitch (x-axis rotation)
    out->x = asinf(2.0f * test);

    // Yaw (y-axis rotation)
    out->y = atan2f(2.0f * (q->y * q->w - q->x * q->z), 1.0f - 2.0f * (sqy + sqx));
}

inline XFLOAT4 SIMD_CALL xquat_to_axis_angle(XQUAT* q)
{
    XFLOAT3 axis;
    float angle;
    xquat_to_axis_angle(q, &axis, &angle);

    XFLOAT4 axis_angle = { axis.x, axis.y, axis.z, angle };
    return axis_angle;
}

inline void SIMD_CALL xquat_to_axis_angle(XQUAT* q, XFLOAT3* axis, float* angle)
{
    // Convert quaternion to axis-angle representation
    *angle = 2.0f * acosf(q->w);

    // If rotation is nearly zero
    if (*angle < TINY_NUMBER)
    {
        axis->x = 1.0f;
        axis->y = 0.0f;
        axis->z = 0.0f;
        *angle = 0.0f;
        return;
    }

    // Calculate rotation axis
    float sinHalfAngle = sqrtf(1.0f - q->w * q->w);

    // Avoid division by zero
    if (fabsf(sinHalfAngle) < TINY_NUMBER)
    {
        sinHalfAngle = 1.0f;
    }

    float invSinHalfAngle = 1.0f / sinHalfAngle;
    axis->x = q->x * invSinHalfAngle;
    axis->y = q->y * invSinHalfAngle;
    axis->z = q->z * invSinHalfAngle;
}

inline XMATRIX4 SIMD_CALL xquat_to_matrix(XQUAT* q)
{
    XMATRIX4 r;
    xquat_to_matrix(q, &r);
    return r;
}

inline void SIMD_CALL xquat_to_matrix(XQUAT* q, XMATRIX4* out)
{
    // [ 1 - 2*(y^2 + z^2), 2*(x*y - z*w), 2*(x*z + y*w), 0.0f ]
    // [ 2*(x*y + z*w), 1 - 2*(x^2 + z^2), 2*(y*z - x*w), 0.0f ]
    // [ 2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x^2 + y^2), 0.0f ]
    // [ 0.0f, 0.0f, 0.0f, 1.0f ]
    out->m00 = (1.0f - (2.0f * (q->y * q->y))) - (2.0f * (q->z * q->z));
    out->m10 = (2.0f * (q->x * q->y)) - (2.0f * (q->z * q->w));
    out->m20 = (2.0f * (q->x * q->z)) + (2.0f * (q->y * q->w));
    out->m30 = 0.0f;
    out->m01 = (2.0f * (q->x * q->y)) + (2.0f * (q->z * q->w));
    out->m11 = (1.0f - (2.0f * (q->x * q->x))) - (2.0f * (q->z * q->z));
    out->m21 = (2.0f * (q->y * q->z)) - (2.0f * (q->x * q->w));
    out->m31 = 0.0f;
    out->m02 = (2.0f * (q->x * q->z)) - (2.0f * (q->y * q->w));
    out->m12 = (2.0f * (q->y * q->z)) + (2.0f * (q->x * q->w));
    out->m22 = (1.0f - (2.0f * (q->x * q->x))) - (2.0f * (q->y * q->y));
    out->m32 = 0.0f;
    out->m03 = 0.0f;
    out->m13 = 0.0f;
    out->m23 = 0.0f;
    out->m33 = 1.0f;
}


//------------------------------------------------------
// XMATRIX4 Functions Implementation (SSE Optimized)
//------------------------------------------------------
inline XMATRIX4 SIMD_CALL xmat4_zero()
{
    XMATRIX4 r;
    xmat4_zero(&r);
    return r;
}

inline void SIMD_CALL xmat4_zero(XMATRIX4* m)
{
    __m128 zero = _mm_setzero_ps();
    m->r[0] = zero;
    m->r[1] = zero;
    m->r[2] = zero;
    m->r[3] = zero;
}

inline XMATRIX4 SIMD_CALL xmat4_ident()
{
    XMATRIX4 r;
    xmat4_ident(&r);
    return r;
}

inline void SIMD_CALL xmat4_ident(XMATRIX4* m)
{
    // Set diagonal to 1
    m->r[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    m->r[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    m->r[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    m->r[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
}

inline bool SIMD_CALL xmat4_is_ident(XMATRIX4* m)
{
    // Check if matrix is identity
    __m128 row0 = _mm_cmpeq_ps(m->r[0], _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f));
    __m128 row1 = _mm_cmpeq_ps(m->r[1], _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f));
    __m128 row2 = _mm_cmpeq_ps(m->r[2], _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f));
    __m128 row3 = _mm_cmpeq_ps(m->r[3], _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));

    // Combine all comparison results with AND operation
    __m128 and1 = _mm_and_ps(row0, row1);
    __m128 and2 = _mm_and_ps(row2, row3);
    __m128 result = _mm_and_ps(and1, and2);

    // All bits must be 1 for identity matrix
    return (_mm_movemask_ps(result) == 0xF);
}

inline bool SIMD_CALL xmat4_is_nan(XMATRIX4* m)
{
    // Check if any element is NaN
    __m128 row0nan = _mm_cmpunord_ps(m->r[0], m->r[0]);
    __m128 row1nan = _mm_cmpunord_ps(m->r[1], m->r[1]);
    __m128 row2nan = _mm_cmpunord_ps(m->r[2], m->r[2]);
    __m128 row3nan = _mm_cmpunord_ps(m->r[3], m->r[3]);

    // Combine all results with OR operation
    __m128 or1 = _mm_or_ps(row0nan, row1nan);
    __m128 or2 = _mm_or_ps(row2nan, row3nan);
    __m128 result = _mm_or_ps(or1, or2);

    // If any bit is 1, NaN exists
    return (_mm_movemask_ps(result) != 0);
}

inline bool SIMD_CALL xmat4_is_infinite(XMATRIX4* m)
{
    // Create infinity mask (exponent bits all 1, other bits 0)
    __m128 infinity = _mm_set1_ps(INFINITY);
    __m128 neg_infinity = _mm_set1_ps(-INFINITY);

    // Check for infinity in each row
    __m128 row0inf = _mm_or_ps(_mm_cmpeq_ps(m->r[0], infinity), _mm_cmpeq_ps(m->r[0], neg_infinity));
    __m128 row1inf = _mm_or_ps(_mm_cmpeq_ps(m->r[1], infinity), _mm_cmpeq_ps(m->r[1], neg_infinity));
    __m128 row2inf = _mm_or_ps(_mm_cmpeq_ps(m->r[2], infinity), _mm_cmpeq_ps(m->r[2], neg_infinity));
    __m128 row3inf = _mm_or_ps(_mm_cmpeq_ps(m->r[3], infinity), _mm_cmpeq_ps(m->r[3], neg_infinity));

    // Combine all results with OR operation
    __m128 or1 = _mm_or_ps(row0inf, row1inf);
    __m128 or2 = _mm_or_ps(row2inf, row3inf);
    __m128 result = _mm_or_ps(or1, or2);

    // If any bit is 1, infinity exists
    return (_mm_movemask_ps(result) != 0);
}

inline XMATRIX4 SIMD_CALL xmat4_set(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
)
{
    XMATRIX4 r;
    xmat4_set(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33, &r);
    return r;
}

inline void SIMD_CALL xmat4_set(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33,
    XMATRIX4* out
)
{
    out->r[0] = _mm_set_ps(m03, m02, m01, m00);
    out->r[1] = _mm_set_ps(m13, m12, m11, m10);
    out->r[2] = _mm_set_ps(m23, m22, m21, m20);
    out->r[3] = _mm_set_ps(m33, m32, m31, m30);
}

inline XMATRIX4 SIMD_CALL xmat4_trans(XFLOAT3* pos)
{
    XMATRIX4 r;
    xmat4_trans(pos, &r);
    return r;
}

inline void SIMD_CALL xmat4_trans(XFLOAT3* pos, XMATRIX4* out)
{
    xmat4_ident(out);
    out->r[3] = _mm_set_ps(1.0f, pos->z, pos->y, pos->x);
}

inline XMATRIX4 SIMD_CALL xmat4_trans(float x, float y, float z)
{
    XMATRIX4 r;
    xmat4_trans(x, y, z, &r);
    return r;
}

inline void SIMD_CALL xmat4_trans(float x, float y, float z, XMATRIX4* out)
{
    xmat4_ident(out);
    out->r[3] = _mm_set_ps(1.0f, z, y, x);
}

inline XFLOAT3 SIMD_CALL xmat4_get_trans(XMATRIX4* m)
{
    XFLOAT3 r;
    xmat4_get_trans(m, &r);
    return r;
}

inline void SIMD_CALL xmat4_get_trans(XMATRIX4* m, XFLOAT3* out)
{
    out->x = m->m30;
    out->y = m->m31;
    out->z = m->m32;
}

inline XMATRIX4 SIMD_CALL xmat4_transpose(XMATRIX4* m)
{
    XMATRIX4 r;
    xmat4_transpose(m, &r);
    return r;
}

inline void SIMD_CALL xmat4_transpose(XMATRIX4* m, XMATRIX4* out)
{
    // Transpose matrix using _MM_TRANSPOSE4_PS
    // row0: [m00, m01, m02, m03]
    // row1: [m10, m11, m12, m13]
    // row2: [m20, m21, m22, m23]
    // row3: [m30, m31, m32, m33]

    // After transpose:
    // row0: [m00, m10, m20, m30]
    // row1: [m01, m11, m21, m31]
    // row2: [m02, m12, m22, m32]
    // row3: [m03, m13, m23, m33]
    __m128 row0 = m->r[0];
    __m128 row1 = m->r[1];
    __m128 row2 = m->r[2];
    __m128 row3 = m->r[3];

    _MM_TRANSPOSE4_PS(row0, row1, row2, row3);

    out->r[0] = row0;
    out->r[1] = row1;
    out->r[2] = row2;
    out->r[3] = row3;

    /*
    _mm_store_ps(out->row[0], row0);
    _mm_store_ps(out->row[1], row1);
    _mm_store_ps(out->row[2], row2);
    _mm_store_ps(out->row[3], row3);
    */
}

inline XMATRIX4 SIMD_CALL xmat4_inverse(XMATRIX4* m)
{
    XMATRIX4 r;
    xmat4_inverse(m, &r);
    return r;
}

inline void SIMD_CALL xmat4_inverse(XMATRIX4* m, XMATRIX4* out)
{
    XMATRIX4 XM;
    XM.r[0] = _mm_load_ps(m->row[0]);
    XM.r[1] = _mm_load_ps(m->row[1]);
    XM.r[2] = _mm_load_ps(m->row[2]);
    XM.r[3] = _mm_load_ps(m->row[3]);

    __m128 T0 = _mm_shuffle_ps(XM.r[0], XM.r[1], _MM_SHUFFLE(1, 0, 1, 0)); // T0: 00, 01, 10, 11	// 0, 1, 0, 1
    __m128 T2 = _mm_shuffle_ps(XM.r[0], XM.r[1], _MM_SHUFFLE(3, 2, 3, 2)); // T2: 02, 03, 12, 13	// 2, 3, 2, 3
    __m128 T1 = _mm_shuffle_ps(XM.r[2], XM.r[3], _MM_SHUFFLE(1, 0, 1, 0)); // T1: 20, 21, 30, 31	// 0, 1, 0, 1
    __m128 T3 = _mm_shuffle_ps(XM.r[2], XM.r[3], _MM_SHUFFLE(3, 2, 3, 2)); // T3: 22, 23, 32, 33	// 2, 3, 2, 3

    XMATRIX4 MT;
    MT.r[0] = _mm_shuffle_ps(T0, T1, _MM_SHUFFLE(2, 0, 2, 0));
    MT.r[1] = _mm_shuffle_ps(T0, T1, _MM_SHUFFLE(3, 1, 3, 1));
    MT.r[2] = _mm_shuffle_ps(T2, T3, _MM_SHUFFLE(2, 0, 2, 0));
    MT.r[3] = _mm_shuffle_ps(T2, T3, _MM_SHUFFLE(3, 1, 3, 1));

    __m128 V00 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(1, 1, 0, 0));
    __m128 V10 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(3, 2, 3, 2));
    __m128 V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(1, 1, 0, 0));
    __m128 V11 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(3, 2, 3, 2));
    __m128 V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(2, 0, 2, 0));
    __m128 V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(3, 1, 3, 1));

    __m128 D0 = _mm_mul_ps(V00, V10);
    __m128 D1 = _mm_mul_ps(V01, V11);
    __m128 D2 = _mm_mul_ps(V02, V12);

    V00 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(3, 2, 3, 2));
    V10 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(1, 1, 0, 0));
    V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(3, 2, 3, 2));
    V11 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(1, 1, 0, 0));
    V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(3, 1, 3, 1));
    V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(2, 0, 2, 0));

    D0 = _mm_sub_ps(D0, _mm_mul_ps(V00, V10));
    D1 = _mm_sub_ps(D1, _mm_mul_ps(V01, V11));
    D2 = _mm_sub_ps(D2, _mm_mul_ps(V02, V12));

    // V11 = D0Y,D0W,D2Y,D2Y
    V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
    V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(1, 0, 2, 1));
    V10 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(0, 3, 0, 2));
    V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(0, 1, 0, 2));
    V11 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(2, 1, 2, 1));

    // V13 = D1Y,D1W,D2W,D2W
    __m128 V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
    V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(1, 0, 2, 1));
    V12 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(0, 3, 0, 2));
    __m128 V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(0, 1, 0, 2));
    V13 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(2, 1, 2, 1));

    __m128 Sin = _mm_mul_ps(V00, V10);
    __m128 ICos = _mm_mul_ps(V01, V11);
    __m128 C4 = _mm_mul_ps(V02, V12);
    __m128 C6 = _mm_mul_ps(V03, V13);

    // V11 = D0X,D0Y,D2X,D2X
    V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
    V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(2, 1, 3, 2));
    V10 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(2, 1, 0, 3));
    V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(1, 3, 2, 3));
    V11 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(0, 2, 1, 2));

    // V13 = D1X,D1Y,D2Z,D2Z
    V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
    V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(2, 1, 3, 2));
    V12 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(2, 1, 0, 3));
    V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(1, 3, 2, 3));
    V13 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(0, 2, 1, 2));

    Sin = _mm_sub_ps(Sin, _mm_mul_ps(V00, V10));
    ICos = _mm_sub_ps(ICos, _mm_mul_ps(V01, V11));
    C4 = _mm_sub_ps(C4, _mm_mul_ps(V02, V12));
    C6 = _mm_sub_ps(C6, _mm_mul_ps(V03, V13));

    V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(0, 3, 0, 3));

    // V10 = D0Z,D0Z,D2X,D2Y
    V10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
    V10 = _mm_shuffle_ps(V10, V10, _MM_SHUFFLE(0, 2, 3, 0));
    V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(2, 0, 3, 1));

    // V11 = D0X,D0W,D2X,D2Y
    V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
    V11 = _mm_shuffle_ps(V11, V11, _MM_SHUFFLE(2, 1, 0, 3));
    V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(0, 3, 0, 3));

    // V12 = D1Z,D1Z,D2Z,D2W
    V12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
    V12 = _mm_shuffle_ps(V12, V12, _MM_SHUFFLE(0, 2, 3, 0));
    V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(2, 0, 3, 1));

    // V13 = D1X,D1W,D2Z,D2W
    V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
    V13 = _mm_shuffle_ps(V13, V13, _MM_SHUFFLE(2, 1, 0, 3));

    V00 = _mm_mul_ps(V00, V10);
    V01 = _mm_mul_ps(V01, V11);
    V02 = _mm_mul_ps(V02, V12);
    V03 = _mm_mul_ps(V03, V13);

    __m128 Cos = _mm_sub_ps(Sin, V00);
    Sin = _mm_add_ps(Sin, V00);
    __m128 C3 = _mm_add_ps(ICos, V01);
    ICos = _mm_sub_ps(ICos, V01);
    __m128 C5 = _mm_sub_ps(C4, V02);
    C4 = _mm_add_ps(C4, V02);
    __m128 C7 = _mm_add_ps(C6, V03);
    C6 = _mm_sub_ps(C6, V03);

    Sin = _mm_shuffle_ps(Sin, Cos, _MM_SHUFFLE(3, 1, 2, 0));
    ICos = _mm_shuffle_ps(ICos, C3, _MM_SHUFFLE(3, 1, 2, 0));
    C4 = _mm_shuffle_ps(C4, C5, _MM_SHUFFLE(3, 1, 2, 0));
    C6 = _mm_shuffle_ps(C6, C7, _MM_SHUFFLE(3, 1, 2, 0));
    Sin = _mm_shuffle_ps(Sin, Sin, _MM_SHUFFLE(3, 1, 2, 0));
    ICos = _mm_shuffle_ps(ICos, ICos, _MM_SHUFFLE(3, 1, 2, 0));
    C4 = _mm_shuffle_ps(C4, C4, _MM_SHUFFLE(3, 1, 2, 0));
    C6 = _mm_shuffle_ps(C6, C6, _MM_SHUFFLE(3, 1, 2, 0));

    // Get the determinant
    __m128 vTemp = _mm_dp_ps(Sin, MT.r[0], 0xff);

    __m128 XMASK_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
    vTemp = _mm_div_ps(XMASK_ONE, vTemp);

    XMATRIX4 mResult;
    mResult.r[0] = _mm_mul_ps(Sin, vTemp);
    mResult.r[1] = _mm_mul_ps(ICos, vTemp);
    mResult.r[2] = _mm_mul_ps(C4, vTemp);
    mResult.r[3] = _mm_mul_ps(C6, vTemp);

    /*
    _mm_store_ps(out->row[0], mResult.r[0]);
    _mm_store_ps(out->row[1], mResult.r[1]);
    _mm_store_ps(out->row[2], mResult.r[2]);
    _mm_store_ps(out->row[3], mResult.r[3]);
    */
    out->r[0] = mResult.r[0];
    out->r[1] = mResult.r[1];
    out->r[2] = mResult.r[2];
    out->r[3] = mResult.r[3];
}

inline XMATRIX4 SIMD_CALL xmat4_mul(XMATRIX4* m0, XMATRIX4* m1)
{
    XMATRIX4 r;
    xmat4_mul(m0, m1, &r);
    return r;
}

inline void SIMD_CALL xmat4_mul(XMATRIX4* m0, XMATRIX4* m1, XMATRIX4* out)
{
    // 1) m1 을 transpose
    XMATRIX4 m1Trans;
    xmat4_transpose(m1, &m1Trans);

    // 2) 각 row 에 대해
    for (int i = 0; i < 4; ++i) {
        __m128 row = m0->r[i];

        // 3) row 의 각 성분을 브로드캐스트해 미리 저장
        __m128 row0 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 row1 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 row2 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 row3 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(3, 3, 3, 3));

        // 4) 4-way dot-product (row × each column of m1)
        __m128 sum01 = _mm_add_ps(
            _mm_mul_ps(row0, m1Trans.r[0]),
            _mm_mul_ps(row1, m1Trans.r[1])
        );
        __m128 sum23 = _mm_add_ps(
            _mm_mul_ps(row2, m1Trans.r[2]),
            _mm_mul_ps(row3, m1Trans.r[3])
        );
        __m128 result = _mm_add_ps(sum01, sum23);

        // 5) 결과 저장
        //_mm_store_ps(out->row[i], result);
        out->r[i] = result;
    }
}

inline XFLOAT3 SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT3* v)
{
    XFLOAT3 r;
    xmat4_mul(m, v, &r);
    return r;
}

inline void SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT3* v, XFLOAT3* out)
{
    // 3차원 벡터를 동차 좌표계 (x, y, z, 1)로 확장
    __m128 vec = _mm_set_ps(1.0f, v->z, v->y, v->x);

    // 행렬의 각 행을 SSE 레지스터에 로드
    __m128 row0 = _mm_load_ps(m->row[0]); // 첫 번째 행
    __m128 row1 = _mm_load_ps(m->row[1]); // 두 번째 행
    __m128 row2 = _mm_load_ps(m->row[2]); // 세 번째 행
    __m128 row3 = _mm_load_ps(m->row[3]); // 네 번째 행 (이동 성분 포함)

    // 각 행과 벡터의 내적 계산
    __m128 result;

    // 모든 행과 벡터의 곱을 한 번에 계산하는 더 효율적인 방법
    // 행렬과 벡터의 곱을 통해 결과 계산
    __m128 xxxx = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 yyyy = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 zzzz = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 wwww = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3));

    // 각 벡터 성분에 대응하는 행렬 열과 곱셈
    __m128 res = _mm_mul_ps(row0, xxxx);       // row0 * x
    res = _mm_add_ps(res, _mm_mul_ps(row1, yyyy));   // + row1 * y
    res = _mm_add_ps(res, _mm_mul_ps(row2, zzzz));   // + row2 * z
    res = _mm_add_ps(res, _mm_mul_ps(row3, wwww));   // + row3 * w (w=1)

    // 결과는 동차 좌표 (x', y', z', w')
    // 여기서는 일반적인 변환만 가정하므로 w'=1
    // 필요한 경우 w'로 나누는 과정을 추가할 수 있음 (투영 변환 등에서)

    // 첫 3개 성분만 추출하여 출력 벡터에 저장
    //_mm_store_ps(out->v, res);
    out->m = res;
}

inline XFLOAT4 SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT4* v)
{
    XFLOAT4 r;
    xmat4_mul(m, v, &r);
    return r;
}

inline void SIMD_CALL xmat4_mul(XMATRIX4* m, XFLOAT4* v, XFLOAT4* out)
{
    // Matrix-vector multiplication
    __m128 vVec = v->m;

    __m128 row0 = _mm_mul_ps(m->r[0], vVec);
    __m128 row1 = _mm_mul_ps(m->r[1], vVec);
    __m128 row2 = _mm_mul_ps(m->r[2], vVec);
    __m128 row3 = _mm_mul_ps(m->r[3], vVec);

    // Horizontal sum
    __m128 sum01 = _mm_hadd_ps(row0, row1);
    __m128 sum23 = _mm_hadd_ps(row2, row3);
    __m128 sum = _mm_hadd_ps(sum01, sum23);

    //_mm_store_ps(out->v, sum);
    out->m = sum;
}

inline XMATRIX4 SIMD_CALL xmat4_scale(float sx, float sy, float sz)
{
    XMATRIX4 r;
    xmat4_scale(sx, sy, sz, &r);
    return r;
}

inline void SIMD_CALL xmat4_scale(float sx, float sy, float sz, XMATRIX4* out)
{
    xmat4_ident(out);

    // Set scale factors on diagonal
    out->m00 = sx;
    out->m11 = sy;
    out->m22 = sz;
}

inline XMATRIX4 SIMD_CALL xmat4_scale(XFLOAT3* v)
{
    XMATRIX4 r;
    xmat4_scale(v, &r);
    return r;
}

inline void SIMD_CALL xmat4_scale(XFLOAT3* v, XMATRIX4* out)
{
    xmat4_scale(v->x, v->y, v->z, out);
}

inline XMATRIX4 SIMD_CALL xmat4_rotation_x(float angle)
{
    XMATRIX4 r;
    xmat4_rotation_x(angle, &r);
    return r;
}

inline void SIMD_CALL xmat4_rotation_x(float angle, XMATRIX4* out)
{
    xmat4_ident(out);

    // Calculate X-axis rotation matrix
    float c = cosf(angle);
    float s = sinf(angle);

    out->m11 = c;
    out->m12 = s;
    out->m21 = -s;
    out->m22 = c;
}

inline XMATRIX4 SIMD_CALL xmat4_rotation_y(float angle)
{
    XMATRIX4 r;
    xmat4_rotation_y(angle, &r);
    return r;
}

inline void SIMD_CALL xmat4_rotation_y(float angle, XMATRIX4* out)
{
    xmat4_ident(out);

    // Calculate Y-axis rotation matrix
    float c = cosf(angle);
    float s = sinf(angle);

    out->m00 = c;
    out->m02 = -s;
    out->m20 = s;
    out->m22 = c;
}

inline XMATRIX4 SIMD_CALL xmat4_rotation_z(float angle)
{
    XMATRIX4 r;
    xmat4_rotation_z(angle, &r);
    return r;
}

inline void SIMD_CALL xmat4_rotation_z(float angle, XMATRIX4* out)
{
    xmat4_ident(out);

    // Calculate Z-axis rotation matrix
    float c = cosf(angle);
    float s = sinf(angle);

    out->m00 = c;
    out->m01 = s;
    out->m10 = -s;
    out->m11 = c;
}

inline XMATRIX4 SIMD_CALL xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll)
{
    XMATRIX4 r;
    xmat4_rotation_yaw_pitch_roll(yaw, pitch, roll, &r);
    return r;
}

inline void SIMD_CALL xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, XMATRIX4* out)
{
    // Convert to quaternion then to matrix (more stable method)
    XQUAT q;
    xquat_rotation_yaw_pitch_roll(yaw, pitch, roll, &q);
    xquat_to_matrix(&q, out);
}

inline XMATRIX4 SIMD_CALL xmat4_rotation_axis(XFLOAT3* axis, float angle)
{
    XMATRIX4 r;
    xmat4_rotation_axis(axis, angle, &r);
    return r;
}

inline void SIMD_CALL xmat4_rotation_axis(XFLOAT3* axis, float angle, XMATRIX4* out)
{
    // Convert to quaternion then to matrix (more stable method)
    XQUAT q;
    xquat_rotation_axis(angle, axis, &q);
    xquat_to_matrix(&q, out);
}

inline XMATRIX4 SIMD_CALL xmat4_rotation_quat(XQUAT* q)
{
    XMATRIX4 r;
    xmat4_rotation_quat(q, &r);
    return r;
}

inline void SIMD_CALL xmat4_rotation_quat(XQUAT* q, XMATRIX4* out)
{
    xquat_to_matrix(q, out);
}

inline XMATRIX4 SIMD_CALL xmat4_lookto_lh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up)
{
    XMATRIX4 r;
    xmat4_lookto_lh(eye, to, up, &r);
    return r;
}

inline void SIMD_CALL xmat4_lookto_lh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up, XMATRIX4* out)
{
    // SSE optimized lookto matrix calculation (left-handed coordinate system)
    // Create SSE vectors
    __m128 eyeVec = _mm_set_ps(0.0f, eye->z, eye->y, eye->x);
    __m128 toVec = _mm_set_ps(0.0f, to->z, to->y, to->x);
    __m128 upVec = _mm_set_ps(0.0f, up->z, up->y, up->x);

    // zaxis = normalize(to - eye)
    __m128 zaxis = _mm_sub_ps(toVec, eyeVec);

    // Normalize zaxis
    __m128 zaxis_dot = _mm_dp_ps(zaxis, zaxis, 0x7F); // 0x7F = 0111 1111, dot product of lowest 3 elements
    __m128 zaxis_length = _mm_sqrt_ps(zaxis_dot);

    // Prevent division by zero
    __m128 zaxis_is_zero = _mm_cmplt_ps(zaxis_dot, _mm_set1_ps(TINY_NUMBER));
    __m128 zaxis_safe_length = _mm_or_ps(_mm_andnot_ps(zaxis_is_zero, zaxis_length),
        _mm_and_ps(zaxis_is_zero, _mm_set1_ps(1.0f)));

    zaxis = _mm_div_ps(zaxis, zaxis_safe_length);

    // xaxis = normalize(cross(up, zaxis))
    __m128 upYZX = _mm_shuffle_ps(upVec, upVec, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 upZXY = _mm_shuffle_ps(upVec, upVec, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 zaxisYZX = _mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 zaxisZXY = _mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(3, 1, 0, 2));

    __m128 mul1 = _mm_mul_ps(upYZX, zaxisZXY);
    __m128 mul2 = _mm_mul_ps(upZXY, zaxisYZX);
    __m128 xaxis = _mm_sub_ps(mul1, mul2);

    // Normalize xaxis
    __m128 xaxis_dot = _mm_dp_ps(xaxis, xaxis, 0x7F);
    __m128 xaxis_length = _mm_sqrt_ps(xaxis_dot);

    // Prevent division by zero
    __m128 xaxis_is_zero = _mm_cmplt_ps(xaxis_dot, _mm_set1_ps(TINY_NUMBER));
    __m128 xaxis_safe_length = _mm_or_ps(_mm_andnot_ps(xaxis_is_zero, xaxis_length),
        _mm_and_ps(xaxis_is_zero, _mm_set1_ps(1.0f)));

    xaxis = _mm_div_ps(xaxis, xaxis_safe_length);

    // yaxis = cross(zaxis, xaxis)
    __m128 zaxisYZX2 = _mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 zaxisZXY2 = _mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 xaxisYZX = _mm_shuffle_ps(xaxis, xaxis, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 xaxisZXY = _mm_shuffle_ps(xaxis, xaxis, _MM_SHUFFLE(3, 1, 0, 2));

    __m128 mul3 = _mm_mul_ps(zaxisYZX2, xaxisZXY);
    __m128 mul4 = _mm_mul_ps(zaxisZXY2, xaxisYZX);
    __m128 yaxis = _mm_sub_ps(mul3, mul4);

    // Calculate dot products (matrix's translation part)
    __m128 neg_eye_dot_xaxis = _mm_xor_ps(_mm_dp_ps(eyeVec, xaxis, 0x7F), _mm_set1_ps(-0.0f));
    __m128 neg_eye_dot_yaxis = _mm_xor_ps(_mm_dp_ps(eyeVec, yaxis, 0x7F), _mm_set1_ps(-0.0f));
    __m128 neg_eye_dot_zaxis = _mm_xor_ps(_mm_dp_ps(eyeVec, zaxis, 0x7F), _mm_set1_ps(-0.0f));

    // Construct matrix
    // row0: [xaxis.x, yaxis.x, zaxis.x, 0]
    // row1: [xaxis.y, yaxis.y, zaxis.y, 0]
    // row2: [xaxis.z, yaxis.z, zaxis.z, 0]
    // row3: [-dot(xaxis,eye), -dot(yaxis,eye), -dot(zaxis,eye), 1]

    // Set matrix elements (careful with row-major order)
    out->m00 = _mm_cvtss_f32(xaxis);
    out->m01 = _mm_cvtss_f32(yaxis);
    out->m02 = _mm_cvtss_f32(zaxis);
    out->m03 = 0.0f;

    out->m10 = _mm_cvtss_f32(_mm_shuffle_ps(xaxis, xaxis, _MM_SHUFFLE(0, 0, 0, 1)));
    out->m11 = _mm_cvtss_f32(_mm_shuffle_ps(yaxis, yaxis, _MM_SHUFFLE(0, 0, 0, 1)));
    out->m12 = _mm_cvtss_f32(_mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(0, 0, 0, 1)));
    out->m13 = 0.0f;

    out->m20 = _mm_cvtss_f32(_mm_shuffle_ps(xaxis, xaxis, _MM_SHUFFLE(0, 0, 0, 2)));
    out->m21 = _mm_cvtss_f32(_mm_shuffle_ps(yaxis, yaxis, _MM_SHUFFLE(0, 0, 0, 2)));
    out->m22 = _mm_cvtss_f32(_mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(0, 0, 0, 2)));
    out->m23 = 0.0f;

    out->m30 = _mm_cvtss_f32(neg_eye_dot_xaxis);
    out->m31 = _mm_cvtss_f32(neg_eye_dot_yaxis);
    out->m32 = _mm_cvtss_f32(neg_eye_dot_zaxis);
    out->m33 = 1.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_lookto_rh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up)
{
    XMATRIX4 r;
    xmat4_lookto_rh(eye, to, up, &r);
    return r;
}

inline void SIMD_CALL xmat4_lookto_rh(XFLOAT3* eye, XFLOAT3* to, XFLOAT3* up, XMATRIX4* out)
{
    // SSE optimized lookto matrix calculation (right-handed coordinate system)
    // Only direction is flipped (zaxis = eye - to)
    XFLOAT3 negTo;
    negTo.x = -to->x;
    negTo.y = -to->y;
    negTo.z = -to->z;

    // Reuse left-handed function, just invert direction
    xmat4_lookto_lh(eye, &negTo, up, out);

    // Flip Z axis direction (right-handed coordinate system)
    out->m02 = -out->m02;
    out->m12 = -out->m12;
    out->m22 = -out->m22;
    out->m32 = -out->m32;
}

inline XMATRIX4 SIMD_CALL xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_perspective_lh(fov, aspect, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ, XMATRIX4* out)
{
    float tanHalfFov = tanf(fov * 0.5f);
    float yScale = 1.0f / tanHalfFov;
    float xScale = yScale / aspect;

    out->m00 = xScale;
    out->m01 = 0.0f;
    out->m02 = 0.0f;
    out->m03 = 0.0f;

    out->m10 = 0.0f;
    out->m11 = yScale;
    out->m12 = 0.0f;
    out->m13 = 0.0f;

    out->m20 = 0.0f;
    out->m21 = 0.0f;
    out->m22 = farZ / (farZ - nearZ);
    out->m23 = 1.0f;

    out->m30 = 0.0f;
    out->m31 = 0.0f;
    out->m32 = -nearZ * farZ / (farZ - nearZ);
    out->m33 = 0.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_perspective_rh(fov, aspect, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ, XMATRIX4* out)
{
    // Calculate values for projection matrix
    float tanHalfFov = tanf(fov * 0.5f);
    float yScale = 1.0f / tanHalfFov;
    float xScale = yScale / aspect;

    // Calculate near and far plane values
    float rangeInv = 1.0f / (nearZ - farZ);

    // Explicitly set matrix elements
    // Right-handed perspective projection matrix
    out->m00 = xScale;
    out->m01 = 0.0f;
    out->m02 = 0.0f;
    out->m03 = 0.0f;

    out->m10 = 0.0f;
    out->m11 = yScale;
    out->m12 = 0.0f;
    out->m13 = 0.0f;

    out->m20 = 0.0f;
    out->m21 = 0.0f;
    out->m22 = farZ * rangeInv;  // Negative value in right-handed system
    out->m23 = -1.0f;            // w component is -1 in right-handed system

    out->m30 = 0.0f;
    out->m31 = 0.0f;
    out->m32 = nearZ * farZ * rangeInv;
    out->m33 = 0.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_perspective_lh(left, right, top, bottom, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out)
{
    // Calculate values for projection matrix
    // Calculate width and height
    float width = right - left;
    float height = top - bottom;
    float depth = farZ - nearZ;

    // Error check - prevent division by zero
    if (fabs(width) < TINY_NUMBER || fabs(height) < TINY_NUMBER || fabs(depth) < TINY_NUMBER)
    {
        xmat4_ident(out);
        return;
    }

    // Prepare temporary values for SSE calculation
    __m128 vWidth = _mm_set_ss(width);
    __m128 vHeight = _mm_set_ss(height);
    __m128 vDepth = _mm_set_ss(depth);
    __m128 vNearZ = _mm_set_ss(nearZ);
    __m128 vTwoNearZ = _mm_mul_ss(_mm_set_ss(2.0f), vNearZ);

    // Calculate matrix elements
    // m00 = (2 * nearZ) / width
    __m128 vM00 = _mm_div_ss(vTwoNearZ, vWidth);

    // m11 = (2 * nearZ) / height
    __m128 vM11 = _mm_div_ss(vTwoNearZ, vHeight);

    // m20 = (left + right) / width
    __m128 vLeftRight = _mm_add_ss(_mm_set_ss(left), _mm_set_ss(right));
    __m128 vM20 = _mm_div_ss(vLeftRight, vWidth);

    // m21 = (top + bottom) / height
    __m128 vTopBottom = _mm_add_ss(_mm_set_ss(top), _mm_set_ss(bottom));
    __m128 vM21 = _mm_div_ss(vTopBottom, vHeight);

    // m22 = farZ / depth
    __m128 vM22 = _mm_div_ss(_mm_set_ss(farZ), vDepth);

    // m32 = -(farZ * nearZ) / depth
    __m128 vFarNear = _mm_mul_ss(vNearZ, _mm_set_ss(farZ));
    __m128 vM32 = _mm_xor_ps(_mm_div_ss(vFarNear, vDepth), _mm_set_ss(-0.0f));

    // Construct matrix
    out->r[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, _mm_cvtss_f32(vM00));
    out->r[1] = _mm_set_ps(0.0f, 0.0f, _mm_cvtss_f32(vM11), 0.0f);
    out->r[2] = _mm_set_ps(1.0f, _mm_cvtss_f32(vM22), _mm_cvtss_f32(vM21), _mm_cvtss_f32(vM20));
    out->r[3] = _mm_set_ps(0.0f, _mm_cvtss_f32(vM32), 0.0f, 0.0f);

    // Explicitly set elements for clarity
    out->m00 = _mm_cvtss_f32(vM00);   // (2 * nearZ) / width
    out->m01 = 0.0f;
    out->m02 = _mm_cvtss_f32(vM20);   // (left + right) / width
    out->m03 = 0.0f;

    out->m10 = 0.0f;
    out->m11 = _mm_cvtss_f32(vM11);   // (2 * nearZ) / height
    out->m12 = _mm_cvtss_f32(vM21);   // (top + bottom) / height
    out->m13 = 0.0f;

    out->m20 = 0.0f;
    out->m21 = 0.0f;
    out->m22 = _mm_cvtss_f32(vM22);   // farZ / depth
    out->m23 = 1.0f;

    out->m30 = 0.0f;
    out->m31 = 0.0f;
    out->m32 = _mm_cvtss_f32(vM32);   // -(farZ * nearZ) / depth
    out->m33 = 0.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_perspective_rh(left, right, top, bottom, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out)
{
    // Calculate values for projection matrix
    // Calculate width and height
    float width = right - left;
    float height = top - bottom;
    float depth = nearZ - farZ;  // Right-handed uses nearZ - farZ (note: opposite of left-handed)

    // Error check - prevent division by zero
    if (fabs(width) < TINY_NUMBER || fabs(height) < TINY_NUMBER || fabs(depth) < TINY_NUMBER)
    {
        xmat4_ident(out);
        return;
    }

    // Prepare temporary values for SSE calculation
    __m128 vWidth = _mm_set_ss(width);
    __m128 vHeight = _mm_set_ss(height);
    __m128 vDepth = _mm_set_ss(depth);
    __m128 vNearZ = _mm_set_ss(nearZ);
    __m128 vTwoNearZ = _mm_mul_ss(_mm_set_ss(2.0f), vNearZ);

    // Calculate matrix elements
    // m00 = (2 * nearZ) / width
    __m128 vM00 = _mm_div_ss(vTwoNearZ, vWidth);

    // m11 = (2 * nearZ) / height
    __m128 vM11 = _mm_div_ss(vTwoNearZ, vHeight);

    // m20 = (left + right) / width
    __m128 vLeftRight = _mm_add_ss(_mm_set_ss(left), _mm_set_ss(right));
    __m128 vM20 = _mm_div_ss(vLeftRight, vWidth);

    // m21 = (top + bottom) / height
    __m128 vTopBottom = _mm_add_ss(_mm_set_ss(top), _mm_set_ss(bottom));
    __m128 vM21 = _mm_div_ss(vTopBottom, vHeight);

    // m22 = farZ / depth (sign differs in right-handed system)
    __m128 vM22 = _mm_div_ss(_mm_set_ss(farZ), vDepth);

    // m32 = (nearZ * farZ) / depth (sign differs in right-handed system)
    __m128 vFarNear = _mm_mul_ss(vNearZ, _mm_set_ss(farZ));
    __m128 vM32 = _mm_div_ss(vFarNear, vDepth);

    // Construct matrix (m23 = -1 in right-handed system)
    // First row: [m00, 0, 0, 0]
    out->r[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, _mm_cvtss_f32(vM00));

    // Second row: [0, m11, 0, 0]
    out->r[1] = _mm_set_ps(0.0f, 0.0f, _mm_cvtss_f32(vM11), 0.0f);

    // Third row: [0, 0, m22, -1] (m23 = -1 in right-handed system)
    out->r[2] = _mm_set_ps(-1.0f, _mm_cvtss_f32(vM22), _mm_cvtss_f32(vM21), _mm_cvtss_f32(vM20));

    // Fourth row: [0, 0, m32, 0]
    out->r[3] = _mm_set_ps(0.0f, _mm_cvtss_f32(vM32), 0.0f, 0.0f);

    // Explicitly set elements for clarity
    out->m00 = _mm_cvtss_f32(vM00);   // (2 * nearZ) / width
    out->m01 = 0.0f;
    out->m02 = _mm_cvtss_f32(vM20);   // (left + right) / width
    out->m03 = 0.0f;

    out->m10 = 0.0f;
    out->m11 = _mm_cvtss_f32(vM11);   // (2 * nearZ) / height
    out->m12 = _mm_cvtss_f32(vM21);   // (top + bottom) / height
    out->m13 = 0.0f;

    out->m20 = 0.0f;
    out->m21 = 0.0f;
    out->m22 = _mm_cvtss_f32(vM22);   // farZ / depth (different sign in right-handed system)
    out->m23 = -1.0f;                 // -1.0f in right-handed system (different from left-handed)

    out->m30 = 0.0f;
    out->m31 = 0.0f;
    out->m32 = _mm_cvtss_f32(vM32);   // (nearZ * farZ) / depth (different sign in right-handed system)
    out->m33 = 0.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_ortho_lh(float width, float height, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_ortho_lh(width, height, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_ortho_lh(float width, float height, float nearZ, float farZ, XMATRIX4* out)
{
    // 1) Broadcast parameters to __m128
    __m128 vW = _mm_set1_ps(width);
    __m128 vH = _mm_set1_ps(height);
    __m128 vNear = _mm_set1_ps(nearZ);
    __m128 vFar = _mm_set1_ps(farZ);
    __m128 vTwo = _mm_set1_ps(2.0f);
    __m128 vOne = _mm_set1_ps(1.0f);
    __m128 vZero = _mm_setzero_ps();

    // 2) m11 =  2 / width
    //    m22 =  2 / height
    __m128 m11_4 = _mm_div_ps(vTwo, vW);
    __m128 m22_4 = _mm_div_ps(vTwo, vH);

    // 3) m33 = 1 / (farZ - nearZ)
    __m128 vDepth = _mm_sub_ps(vFar, vNear);
    __m128 invDepth = _mm_div_ps(vOne, vDepth);
    __m128 m33_4 = invDepth;

    // 4) m43 = –nearZ / (farZ - nearZ)
    __m128 m43_4 = _mm_mul_ps(_mm_sub_ps(vZero, vNear), invDepth);

    // 5) Extract only the scalar values needed
    float m11 = _mm_cvtss_f32(m11_4);
    float m22 = _mm_cvtss_f32(m22_4);
    float m33 = _mm_cvtss_f32(m33_4);
    float m43 = _mm_cvtss_f32(m43_4);

    // 6) Fill the output matrix in row-major format
    out->m00 = m11;  out->m01 = 0.0f; out->m02 = 0.0f; out->m03 = 0.0f;
    out->m10 = 0.0f; out->m11 = m22;  out->m12 = 0.0f; out->m13 = 0.0f;
    out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = m33;  out->m23 = 0.0f;
    out->m30 = 0.0f; out->m31 = 0.0f; out->m32 = m43;  out->m33 = 1.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_ortho_rh(float width, float height, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_ortho_rh(width, height, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_ortho_rh(float width, float height, float nearZ, float farZ, XMATRIX4* out)
{
    // Broadcast parameters
    __m128 vW = _mm_set1_ps(width);
    __m128 vH = _mm_set1_ps(height);
    __m128 vNear = _mm_set1_ps(nearZ);
    __m128 vFar = _mm_set1_ps(farZ);
    __m128 vTwo = _mm_set1_ps(2.0f);
    __m128 vOne = _mm_set1_ps(1.0f);
    __m128 vZero = _mm_setzero_ps();

    // m00, m11
    __m128 m11_v = _mm_div_ps(vTwo, vW);   // 2/width
    __m128 m22_v = _mm_div_ps(vTwo, vH);   // 2/height

    // depth = farZ - nearZ
    __m128 depth = _mm_sub_ps(vFar, vNear);
    __m128 invDepth = _mm_div_ps(vOne, depth);

    // RH Z scale: -1/(farZ-nearZ)
    __m128 m33_v = _mm_sub_ps(vZero, invDepth);

    // Z translation: -nearZ/(farZ-nearZ) (same for LH/RH)
    __m128 m43_v = _mm_mul_ps(_mm_sub_ps(vZero, vNear), invDepth);

    // Extract first components
    float m11 = _mm_cvtss_f32(m11_v);
    float m12 = _mm_cvtss_f32(m22_v);
    float m22 = _mm_cvtss_f32(m33_v);
    float m32 = _mm_cvtss_f32(m43_v);

    // Fill in row-major format
    out->m00 = m11;  out->m01 = 0.0f;  out->m02 = 0.0f;  out->m03 = 0.0f;
    out->m10 = 0.0f; out->m11 = m12;   out->m12 = 0.0f;  out->m13 = 0.0f;
    out->m20 = 0.0f; out->m21 = 0.0f;  out->m22 = m22;   out->m23 = 0.0f;
    out->m30 = 0.0f; out->m31 = 0.0f;  out->m32 = m32;   out->m33 = 1.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_ortho_lh(left, right, top, bottom, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out)
{
    // 1) Broadcast parameters
    __m128 vLeft = _mm_set1_ps(left);
    __m128 vRight = _mm_set1_ps(right);
    __m128 vTop = _mm_set1_ps(top);
    __m128 vBottom = _mm_set1_ps(bottom);
    __m128 vNear = _mm_set1_ps(nearZ);
    __m128 vFar = _mm_set1_ps(farZ);
    __m128 vTwo = _mm_set1_ps(2.0f);
    __m128 vOne = _mm_set1_ps(1.0f);
    __m128 vZero = _mm_setzero_ps();

    // 2) Calculate width, height, depth
    __m128 vWidth = _mm_sub_ps(vRight, vLeft);   // right - left
    __m128 vHeight = _mm_sub_ps(vTop, vBottom);  // top - bottom
    __m128 vDepth = _mm_sub_ps(vFar, vNear);     // farZ - nearZ

    // 3) Calculate diagonal elements
    __m128 m00_v = _mm_div_ps(vTwo, vWidth);     // 2/(right-left)
    __m128 m11_v = _mm_div_ps(vTwo, vHeight);    // 2/(top-bottom)
    __m128 invD = _mm_div_ps(vOne, vDepth);      // 1/(farZ-nearZ)
    __m128 m22_v = invD;                         // m22

    // 4) Calculate translation elements
    __m128 sumLR = _mm_add_ps(vLeft, vRight);    // left+right
    __m128 sumTB = _mm_add_ps(vTop, vBottom);    // top+bottom
    __m128 m30_v = _mm_div_ps(_mm_sub_ps(vZero, sumLR), vWidth);   // -(l+r)/(r-l)
    __m128 m31_v = _mm_div_ps(_mm_sub_ps(vZero, sumTB), vHeight);  // -(t+b)/(t-b)
    __m128 m32_v = _mm_mul_ps(_mm_sub_ps(vZero, vNear), invD);     // -nearZ/(farZ-nearZ)

    // 5) Extract scalar components
    float m00 = _mm_cvtss_f32(m00_v);
    float m11 = _mm_cvtss_f32(m11_v);
    float m22 = _mm_cvtss_f32(m22_v);
    float m30 = _mm_cvtss_f32(m30_v);
    float m31 = _mm_cvtss_f32(m31_v);
    float m32 = _mm_cvtss_f32(m32_v);

    // 6) Fill matrix in row-major format
    out->m00 = m00; out->m01 = 0.0f; out->m02 = 0.0f; out->m03 = 0.0f;
    out->m10 = 0.0f; out->m11 = m11; out->m12 = 0.0f; out->m13 = 0.0f;
    out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = m22; out->m23 = 0.0f;
    out->m30 = m30; out->m31 = m31; out->m32 = m32; out->m33 = 1.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_ortho_rh(left, right, top, bottom, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4* out)
{
    // 1) Broadcast parameters
    __m128 vLeft = _mm_set1_ps(left);
    __m128 vRight = _mm_set1_ps(right);
    __m128 vTop = _mm_set1_ps(top);
    __m128 vBottom = _mm_set1_ps(bottom);
    __m128 vNear = _mm_set1_ps(nearZ);
    __m128 vFar = _mm_set1_ps(farZ);
    __m128 vTwo = _mm_set1_ps(2.0f);
    __m128 vOne = _mm_set1_ps(1.0f);
    __m128 vZero = _mm_setzero_ps();

    // 2) Calculate width and height
    __m128 vWidth = _mm_sub_ps(vRight, vLeft);    // (r - l)
    __m128 vHeight = _mm_sub_ps(vTop, vBottom);   // (t - b)

    // 3) Calculate scale factors
    __m128 invWidth = _mm_div_ps(vTwo, vWidth);    // 2/(r-l)
    __m128 invHeight = _mm_div_ps(vTwo, vHeight);  // 2/(t-b)

    // 4) Calculate depth values (RH): invDepth = 1/(near - far)
    __m128 vDepth = _mm_sub_ps(vNear, vFar);      // (n - f)
    __m128 invDepth = _mm_div_ps(vOne, vDepth);    // 1/(n-f)
    __m128 m22_v = invDepth;                      // m22 = 1/(n-f)
    __m128 m32_v = _mm_mul_ps(vNear, invDepth);   // m32 = n/(n-f)

    // 5) Calculate offset elements
    __m128 sumLR = _mm_add_ps(vLeft, vRight);    // (l + r)
    __m128 negLR = _mm_sub_ps(vZero, sumLR);      // -(l+r)
    __m128 m30_v = _mm_div_ps(negLR, vWidth);    // m30 = -(l+r)/(r-l)

    __m128 sumTB = _mm_add_ps(vTop, vBottom);   // (t + b)
    __m128 negTB = _mm_sub_ps(vZero, sumTB);      // -(t+b)
    __m128 m31_v = _mm_div_ps(negTB, vHeight);   // m31 = -(t+b)/(t-b)

    // 6) Extract scalar values
    float m00 = _mm_cvtss_f32(invWidth);
    float m11 = _mm_cvtss_f32(invHeight);
    float m22 = _mm_cvtss_f32(m22_v);
    float m30 = _mm_cvtss_f32(m30_v);
    float m31 = _mm_cvtss_f32(m31_v);
    float m32 = _mm_cvtss_f32(m32_v);

    // 7) Fill matrix in row-major format
    out->m00 = m00;  out->m01 = 0.0f; out->m02 = 0.0f;  out->m03 = 0.0f;
    out->m10 = 0.0f; out->m11 = m11;  out->m12 = 0.0f;  out->m13 = 0.0f;
    out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = m22;   out->m23 = 0.0f;
    out->m30 = m30;  out->m31 = m31;  out->m32 = m32;   out->m33 = 1.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_frustum_lh(left, right, bottom, top, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4* out)
{
    // Broadcast parameters
    __m128 vL = _mm_set1_ps(left);
    __m128 vR = _mm_set1_ps(right);
    __m128 vB = _mm_set1_ps(bottom);
    __m128 vT = _mm_set1_ps(top);
    __m128 vN = _mm_set1_ps(nearZ);
    __m128 vF = _mm_set1_ps(farZ);
    __m128 vTwo = _mm_set1_ps(2.0f);
    __m128 vZero = _mm_setzero_ps();

    // (r-l), (t-b)
    __m128 vW = _mm_sub_ps(vR, vL);
    __m128 vH = _mm_sub_ps(vT, vB);

    // m00 = 2n/(r−l), m11 = 2n/(t−b)
    __m128 m00_v = _mm_div_ps(_mm_mul_ps(vTwo, vN), vW);
    __m128 m11_v = _mm_div_ps(_mm_mul_ps(vTwo, vN), vH);

    // m20 = (l+r)/(l−r)
    __m128 sumLR = _mm_add_ps(vL, vR);
    __m128 denLR = _mm_sub_ps(vL, vR);
    __m128 m20_v = _mm_div_ps(sumLR, denLR);

    // m21 = (t+b)/(b−t)
    __m128 sumTB = _mm_add_ps(vT, vB);
    __m128 denBT = _mm_sub_ps(vB, vT);
    __m128 m21_v = _mm_div_ps(sumTB, denBT);

    // Depth calculations
    __m128 vD = _mm_sub_ps(vF, vN);           // f−n
    // m22 = f/(f−n)
    __m128 m22_v = _mm_div_ps(vF, vD);
    // m32 = n·f/(n−f)  (== zn*zf/(zn−zf) == −zn*zf/(f−n))
    __m128 vNF = _mm_mul_ps(vN, vF);
    __m128 m32_v = _mm_div_ps(vNF, _mm_sub_ps(vN, vF));

    // Extract scalar values
    float m00 = _mm_cvtss_f32(m00_v);
    float m11 = _mm_cvtss_f32(m11_v);
    float m20 = _mm_cvtss_f32(m20_v);
    float m21 = _mm_cvtss_f32(m21_v);
    float m22 = _mm_cvtss_f32(m22_v);
    float m32 = _mm_cvtss_f32(m32_v);

    // Fill matrix
    out->m00 = m00;  out->m01 = 0.0f; out->m02 = m20;  out->m03 = 0.0f;
    out->m10 = 0.0f; out->m11 = m11;  out->m12 = m21;  out->m13 = 0.0f;
    out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = m22;  out->m23 = 1.0f;
    out->m30 = 0.0f; out->m31 = 0.0f; out->m32 = m32;  out->m33 = 0.0f;
}

inline XMATRIX4 SIMD_CALL xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ)
{
    XMATRIX4 r;
    xmat4_frustum_rh(left, right, bottom, top, nearZ, farZ, &r);
    return r;
}

inline void SIMD_CALL xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4* out)
{
    // This function is missing in the original code, but follows the same pattern as other perspective functions

    // Broadcast parameters
    __m128 vL = _mm_set1_ps(left);
    __m128 vR = _mm_set1_ps(right);
    __m128 vB = _mm_set1_ps(bottom);
    __m128 vT = _mm_set1_ps(top);
    __m128 vN = _mm_set1_ps(nearZ);
    __m128 vF = _mm_set1_ps(farZ);
    __m128 vTwo = _mm_set1_ps(2.0f);
    __m128 vZero = _mm_setzero_ps();

    // Width and height
    __m128 vW = _mm_sub_ps(vR, vL);
    __m128 vH = _mm_sub_ps(vT, vB);

    // m00 = 2n/(r−l), m11 = 2n/(t−b)
    __m128 m00_v = _mm_div_ps(_mm_mul_ps(vTwo, vN), vW);
    __m128 m11_v = _mm_div_ps(_mm_mul_ps(vTwo, vN), vH);

    // m20 = (l+r)/(r−l)
    __m128 sumLR = _mm_add_ps(vL, vR);
    __m128 denRL = _mm_sub_ps(vR, vL);
    __m128 m20_v = _mm_div_ps(sumLR, denRL);

    // m21 = (t+b)/(t−b)
    __m128 sumTB = _mm_add_ps(vT, vB);
    __m128 denTB = _mm_sub_ps(vT, vB);
    __m128 m21_v = _mm_div_ps(sumTB, denTB);

    // Depth calculations (Right-handed)
    __m128 vD = _mm_sub_ps(vN, vF);           // n−f
    // m22 = f/(n−f)
    __m128 m22_v = _mm_div_ps(vF, vD);
    // m32 = n·f/(n−f)
    __m128 vNF = _mm_mul_ps(vN, vF);
    __m128 m32_v = _mm_div_ps(vNF, vD);

    // Extract scalar values
    float m00 = _mm_cvtss_f32(m00_v);
    float m11 = _mm_cvtss_f32(m11_v);
    float m20 = _mm_cvtss_f32(m20_v);
    float m21 = _mm_cvtss_f32(m21_v);
    float m22 = _mm_cvtss_f32(m22_v);
    float m32 = _mm_cvtss_f32(m32_v);

    // Fill matrix (note m23 = -1 for right-handed)
    out->m00 = m00;  out->m01 = 0.0f; out->m02 = m20;  out->m03 = 0.0f;
    out->m10 = 0.0f; out->m11 = m11;  out->m12 = m21;  out->m13 = 0.0f;
    out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = m22;  out->m23 = -1.0f;
    out->m30 = 0.0f; out->m31 = 0.0f; out->m32 = m32;  out->m33 = 0.0f;
}