#if defined(__TARGET_OS_WINDOWS) && defined(__SSE__)

namespace mark
{
    __FORCEINLINE __m128 xmvec2_dot(__m128 v0, __m128 v1) noexcept
    {
        //return _mm_dp_ps(v0, v1, 0x3F);
		__m128 mul = _mm_mul_ps(v0, v1);
		__m128 shuf = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(0, 1, 0, 1));
		__m128 sum = _mm_add_ps(mul, shuf);

		return _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 0, 0, 0));
    }

    __FORCEINLINE __m128 xmvec3_dot(__m128 v0, __m128 v1) noexcept
    {
        //return _mm_dp_ps(v0, v1, 0x7F);
		__m128 mul = _mm_mul_ps(v0, v1);
		__m128 shuf1 = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(3, 2, 0, 1));
		__m128 add1 = _mm_add_ps(mul, shuf1);

		__m128 shuf2 = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(3, 2, 1, 2));
		__m128 add2 = _mm_add_ps(add1, shuf2);

		return _mm_shuffle_ps(add2, add2, _MM_SHUFFLE(0, 0, 0, 0));
    }

    __FORCEINLINE __m128 xmvec4_dot(__m128 v0, __m128 v1) noexcept
    {
        //return _mm_dp_ps(v0, v1, 0xFF);
		__m128 mul = _mm_mul_ps(v0, v1);

		__m128 shuf1 = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
		__m128 add1 = _mm_add_ps(mul, shuf1);

		__m128 shuf2 = _mm_shuffle_ps(add1, add1, _MM_SHUFFLE(1, 0, 3, 2));

		return _mm_add_ps(add1, shuf2);
    }

    __FORCEINLINE __m128 xmvec2_cross(__m128 v0, __m128 v1) noexcept
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

    __FORCEINLINE __m128 xmvec3_cross(__m128 v0, __m128 v1) noexcept
    {
        // v0를 셔플하여 y,z,x,w 순서로 만듦
        __m128 v0yzx = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 0, 2, 1));

        // v1를 셔플하여 z,x,y,w 순서로 만듦
        __m128 v1zxy = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2));

        // v0yzx * v1zxy 계산
        __m128 prod1 = _mm_mul_ps(v0yzx, v1zxy);

        // v1를 y,z,x,w 순서로 만듦
        __m128 v1yzx = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));

        // v0를 z,x,y,w 순서로 만듦
        __m128 v0zxy = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 1, 0, 2));

        // v1yzx * v0zxy 계산
        __m128 prod2 = _mm_mul_ps(v1yzx, v0zxy);

        // 외적 계산: prod1 - prod2
        return _mm_sub_ps(prod1, prod2);
    }

    __FORCEINLINE __m128 xmvec4_cross(__m128 v0, __m128 v1, __m128 v2) noexcept
    {
        // 각 성분 조합을 위한 셔플 마스크 상수
        const int MASK_YZW = _MM_SHUFFLE(3, 2, 1, 1);  // y, z, w 성분
        const int MASK_XZW = _MM_SHUFFLE(3, 2, 0, 0);  // x, z, w 성분
        const int MASK_XYW = _MM_SHUFFLE(3, 1, 0, 0);  // x, y, w 성분
        const int MASK_XYZ = _MM_SHUFFLE(2, 1, 0, 0);  // x, y, z 성분

        // x 성분 계산 - 첫 번째 3x3 행렬식
        // | v1.y v1.z v1.w |
        // | v2.y v2.z v2.w |
        // | v3.y v3.z v3.w |

        // v1의 y,z,w 성분
        __m128 v0_yzw = _mm_shuffle_ps(v0, v0, MASK_YZW);
        __m128 v1_yzw = _mm_shuffle_ps(v1, v1, MASK_YZW);
        __m128 v2_yzw = _mm_shuffle_ps(v2, v2, MASK_YZW);

        // 첫 번째 행렬식 부분 계산
        __m128 temp1, temp2, temp3, result_x;

        // 첫 번째 부분 계산
        temp1 = _mm_shuffle_ps(v1_yzw, v1_yzw, _MM_SHUFFLE(0, 0, 0, 0));
        temp2 = _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(1, 2, 1, 1));
        temp3 = _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(2, 1, 2, 2));
        __m128 v2zw_v2wz = _mm_mul_ps(temp2, _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(1, 0, 1, 1)));
        result_x = _mm_mul_ps(temp1, _mm_sub_ps(v2zw_v2wz, _mm_mul_ps(temp3, _mm_shuffle_ps(v2_yzw, v2_yzw, _MM_SHUFFLE(0, 1, 0, 0)))));

        // 나머지 행렬식 부분 계산
        // 각 부분을 계산한 복잡한 셔플과 곱셈 연산이 필요합니다

        // 간소화를 위해 스칼라 방식으로 계산 후 SSE 레지스터에 저장합니다
        // 실제 구현에서는 더 많은 SSE 연산을 사용할 수 있습니다

        // 행렬식 계산을 위한 임시 변수
        //float a1 = v0.x, a2 = v0.y, a3 = v0.z, a4 = v0.w;
        float a1 = v0.m128_f32[0], a2 = v0.m128_f32[1], a3 = v0.m128_f32[2], a4 = v0.m128_f32[3];
        float b1 = v1.m128_f32[0], b2 = v1.m128_f32[1], b3 = v1.m128_f32[2], b4 = v1.m128_f32[3];
        float c1 = v2.m128_f32[0], c2 = v2.m128_f32[1], c3 = v2.m128_f32[2], c4 = v2.m128_f32[3];


        // x 성분 계산 (부호 +)
        float x = a2 * (b3 * c4 - b4 * c3) - a3 * (b2 * c4 - b4 * c2) + a4 * (b2 * c3 - b3 * c2);

        // y 성분 계산 (부호 -)
        float y = -(a1 * (b3 * c4 - b4 * c3) - a3 * (b1 * c4 - b4 * c1) + a4 * (b1 * c3 - b3 * c1));

        // z 성분 계산 (부호 +)
        float z = a1 * (b2 * c4 - b4 * c2) - a2 * (b1 * c4 - b4 * c1) + a4 * (b1 * c2 - b2 * c1);

        // w 성분 계산 (부호 -)
        float w = -(a1 * (b2 * c3 - b3 * c2) - a2 * (b1 * c3 - b3 * c1) + a3 * (b1 * c2 - b2 * c1));

        // 결과를 SSE 레지스터에 저장
        return _mm_set_ps(w, z, y, x);
    }

    __FORCEINLINE __m128 xmvec2_lengthsq(__m128 v) noexcept
    {
        return xmvec2_dot(v, v);
    }

    __FORCEINLINE __m128 xmvec3_lengthsq(__m128 v) noexcept
    {
        return xmvec3_dot(v, v);
    }

    __FORCEINLINE __m128 xmvec4_lengthsq(__m128 v) noexcept
    {
        return xmvec4_dot(v, v);
    }

    __FORCEINLINE __m128 xmvec2_length(__m128 v) noexcept
    {
        __m128 lengthSq = xmvec2_lengthsq(v);
        return _mm_sqrt_ps(lengthSq);
    }

    __FORCEINLINE __m128 xmvec3_length(__m128 v) noexcept
    {
        __m128 lengthSq = xmvec3_lengthsq(v);
        return _mm_sqrt_ps(lengthSq);
    }

    __FORCEINLINE __m128 xmvec4_length(__m128 v) noexcept
    {
        __m128 lengthSq = xmvec4_lengthsq(v);
        return _mm_sqrt_ps(lengthSq);
    }

    __FORCEINLINE __m128 xmvec2_normalize(__m128 v) noexcept
    {
        __m128 length = xmvec2_length(v);
        if (length.m128_f32[0] < M_TINY)
            return v;
        __m128 inv_len = _mm_div_ps(XFLOAT4_ONE.xmm, length);
        return _mm_mul_ps(v, inv_len);
    }

    __FORCEINLINE __m128 xmvec3_normalize(__m128 v) noexcept
    {
        __m128 length = xmvec3_length(v);
        if (length.m128_f32[0] < M_TINY)
            return v;
        __m128 inv_len = _mm_div_ps(XFLOAT4_ONE.xmm, length);
        return _mm_mul_ps(v, inv_len);
    }

    __FORCEINLINE __m128 xmvec4_normalize(__m128 v) noexcept
    {
        __m128 length = xmvec4_length(v);
        if (length.m128_f32[0] < M_TINY)
            return v;
        __m128 inv_len = _mm_div_ps(XFLOAT4_ONE.xmm, length);
        return _mm_mul_ps(v, inv_len);
    }

    __FORCEINLINE __m128 xmvec_lerp(__m128 v0, __m128 v1, float t) noexcept
    {
        // V0 + ((V1 - V0) * t)
        __m128 tv = _mm_set_ps1(t);
        __m128 diff = _mm_sub_ps(v1, v0);
        __m128 scaled = _mm_mul_ps(diff, tv);
        __m128 result = _mm_add_ps(v0, scaled);

        return result;
    }

    __FORCEINLINE __m128 xmvec_clamp(__m128 v, __m128 vmin, __m128 vmax) noexcept
    {
        return _mm_min_ps(_mm_max_ps(v, vmin), vmax);
    }

    __FORCEINLINE __m128 xmvec_saturate(__m128 v) noexcept
    {
        return _mm_min_ps(_mm_max_ps(v, XFLOAT4_ZERO.xmm), XFLOAT4_ONE.xmm);
    }

    __FORCEINLINE __m128 xmvec_min(__m128 v0, __m128 v1) noexcept
    {
        return _mm_min_ps(v0, v1);
    }

    __FORCEINLINE __m128 xmvec_max(__m128 v0, __m128 v1) noexcept
    {
        return _mm_max_ps(v0, v1);
    }

    __FORCEINLINE __m128 xmvec_ceil(__m128 v) noexcept // 올림
    {
        return _mm_ceil_ps(v);
    }

    __FORCEINLINE __m128 xmvec_floor(__m128 v) noexcept // 내림
    {
        return _mm_floor_ps(v);
    }

    __FORCEINLINE __m128 xmvec_round(__m128 v) noexcept // 반올림
    {
        return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    }

    __FORCEINLINE __m128 xmvec_trunc(__m128 v) noexcept // 버림
    {
        return _mm_trunc_ps(v);
    }

    //------------------------------------------------------
    // XFLOAT2 Function Implementation (SSE Optimization)
    //------------------------------------------------------
    inline XFLOAT2 __vectorcall xvec2_zero() noexcept
    {
        return XFLOAT2{ _mm_setzero_ps() };
	}

    inline XFLOAT2 __vectorcall xvec2_one() noexcept
    {
        return XFLOAT2{ _mm_set_ps(0, 0, 1.0f, 1.0f) };
    }

    inline XFLOAT2 __vectorcall xvec2_unit_x() noexcept
    {
        return XFLOAT2{ _mm_set_ps(0, 0, 0, 1.0f) };
	}

    inline XFLOAT2 __vectorcall xvec2_unit_y() noexcept
    {
        return XFLOAT2{ _mm_set_ps(0, 0, 1.0f, 0) };
    }

    inline XFLOAT2 __vectorcall xvec2_set(float x, float y) noexcept
    {
        return XFLOAT2{ _mm_set_ps(0, 0, y, x) };
	}

    inline XFLOAT2 __vectorcall xvec2_add(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ _mm_add_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_sub(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ _mm_sub_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_mul(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2({ _mm_mul_ps(v0.xmm, v1.xmm) });
    }

    inline XFLOAT2 __vectorcall xvec2_mul(XFLOAT2 v, float f) noexcept
    {
        __m128 scalar = _mm_set1_ps(f);
        return XFLOAT2(_mm_mul_ps(v.xmm, scalar));
    }

    inline XFLOAT2 __vectorcall xvec2_div(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ _mm_div_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_div(XFLOAT2 v, float f) noexcept
    {
        __m128 scalar = _mm_set1_ps(f);
        return XFLOAT2{ _mm_div_ps(v.xmm, scalar) };
    }

    inline float __vectorcall xvec2_dot_f(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		XFLOAT2 r{ xmvec2_dot(v0.xmm, v1.xmm) };
        return _mm_cvtss_f32(r.xmm);
    }

    inline XFLOAT2 __vectorcall xvec2_dot(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ xmvec2_dot(v0.xmm, v1.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_cross(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ xmvec2_cross(v0.xmm, v1.xmm) };
    }

    inline float __vectorcall xvec2_lengthsq_f(XFLOAT2 v) noexcept
    {
        __m128 r = xmvec2_lengthsq(v.xmm);
        return _mm_cvtss_f32(r);
    }

    inline XFLOAT2 __vectorcall xvec2_lengthsq(XFLOAT2 v, XFLOAT2 out) noexcept
    {
		return XFLOAT2{ xmvec2_lengthsq(v.xmm) };
    }

    inline float __vectorcall xvec2_length_f(XFLOAT2 v) noexcept
    {
        __m128 r = xmvec2_length(v.xmm);
        return _mm_cvtss_f32(r);
    }

    inline XFLOAT2 __vectorcall xvec2_length(XFLOAT2 v) noexcept
    {
		return XFLOAT2{ xmvec2_length(v.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_normalize(XFLOAT2 v) noexcept
    {
		return XFLOAT2{ xmvec2_normalize(v.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_lerp(XFLOAT2 v0, XFLOAT2 v1, float t) noexcept
    {
        // V0 + ((V1 - V0) * t)
		return XFLOAT2{ xmvec_lerp(v0.xmm, v1.xmm, t) };
    }

    inline XFLOAT2 __vectorcall xvec2_clamp(XFLOAT2 v, XFLOAT2 vmin, XFLOAT2 vmax) noexcept
    {
        return XFLOAT2{ xmvec_clamp(v.xmm, vmin.xmm, vmax.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_saturate(XFLOAT2 v) noexcept
    {
		return XFLOAT2{ xmvec_saturate(v.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_min(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ xmvec_min(v0.xmm, v1.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_max(XFLOAT2 v0, XFLOAT2 v1) noexcept
    {
		return XFLOAT2{ xmvec_max(v0.xmm, v1.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_ceil(XFLOAT2 v) noexcept
    {
		return XFLOAT2{ xmvec_ceil(v.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_floor(XFLOAT2 v) noexcept
    {
		return XFLOAT2{ xmvec_floor(v.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_round(XFLOAT2 v) noexcept
    {
		return XFLOAT2{ xmvec_round(v.xmm) };
    }

    inline XFLOAT2 __vectorcall xvec2_trunc(XFLOAT2 v) noexcept
    {
        return XFLOAT2{ xmvec_trunc(v.xmm) };
    }


    //------------------------------------------------------
    // XFLOAT3 Function Implementation (SSE Optimization)
    //------------------------------------------------------
    inline XFLOAT3 __vectorcall xvec3_zero() noexcept
    {
		return XFLOAT3{ _mm_setzero_ps() };
    }

    inline XFLOAT3 __vectorcall xvec3_unit_x() noexcept
    {
		return XFLOAT3{ _mm_set_ps(0, 0, 0, 1.0f) };
    }

    inline XFLOAT3 __vectorcall xvec3_unit_y() noexcept
    {
		return XFLOAT3{ _mm_set_ps(0, 0, 1.0f, 0) };
    }

    inline XFLOAT3 __vectorcall xvec3_unit_z() noexcept
    {
		return XFLOAT3{ _mm_set_ps(0, 1.0f, 0, 0) };
    }

    inline XFLOAT3 __vectorcall xvec3_set(float x, float y, float z) noexcept
    {
		return XFLOAT3{ _mm_set_ps(0, z, y, x) };
    }

    inline XFLOAT3 __vectorcall xvec3_add(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ _mm_add_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_sub(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ _mm_sub_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_mul(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ _mm_mul_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_mul(XFLOAT3 v, float f) noexcept
    {
		return XFLOAT3{ _mm_mul_ps(v.xmm, _mm_set1_ps(f)) };
    }

    inline void __vectorcall xvec3_mul(XFLOAT3 v, float f, XFLOAT3& out) noexcept
    {
        __m128 scalar = _mm_set1_ps(f);
        out.xmm = _mm_mul_ps(v.xmm, scalar);
    }

    inline XFLOAT3 __vectorcall xvec3_div(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ _mm_div_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_div(XFLOAT3 v, float f) noexcept
    {
		return XFLOAT3{ _mm_div_ps(v.xmm, _mm_set1_ps(f)) };
    }

    inline void __vectorcall xvec3_div(XFLOAT3 v, float f, XFLOAT3& out) noexcept
    {
        __m128 scalar = _mm_set1_ps(f);
        out.xmm = _mm_div_ps(v.xmm, scalar);
    }

    inline float __vectorcall xvec3_dot_f(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
        __m128 r = xmvec3_dot(v0.xmm, v1.xmm);
        return _mm_cvtss_f32(r);
    }

    inline XFLOAT3 __vectorcall xvec3_dot(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ xmvec3_dot(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_cross(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ xmvec3_cross(v0.xmm, v1.xmm) };
    }

    inline float __vectorcall xvec3_lengthsq_f(XFLOAT3 v) noexcept
    {
        __m128 r = xmvec3_lengthsq(v.xmm);
        return _mm_cvtss_f32(r);
    }

    inline XFLOAT3 __vectorcall xvec3_lengthsq(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec3_lengthsq(v.xmm) };
    }

    inline float __vectorcall xvec3_length_f(XFLOAT3 v) noexcept
    {
        __m128 r = xmvec3_length(v.xmm);
        return _mm_cvtss_f32(r);
    }

    inline XFLOAT3 __vectorcall xvec3_length(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec3_length(v.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_normalize(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec3_normalize(v.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_lerp(XFLOAT3 v0, XFLOAT3 v1, float t) noexcept
    {
		return XFLOAT3{ xmvec_lerp(v0.xmm, v1.xmm, t) };
    }

    inline XFLOAT3 __vectorcall xvec3_clamp(XFLOAT3 v, XFLOAT3 vmin, XFLOAT3 vmax) noexcept
    {
		return XFLOAT3{ xmvec_clamp(v.xmm, vmin.xmm, vmax.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_saturate(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec_saturate(v.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_min(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ xmvec_min(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_max(XFLOAT3 v0, XFLOAT3 v1) noexcept
    {
		return XFLOAT3{ xmvec_max(v0.xmm, v1.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_ceil(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec_ceil(v.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_floor(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec_floor(v.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_round(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec_round(v.xmm) };
    }

    inline XFLOAT3 __vectorcall xvec3_trunc(XFLOAT3 v) noexcept
    {
		return XFLOAT3{ xmvec_trunc(v.xmm) };
    }


    //------------------------------------------------------
    // XFLOAT4 Function Implementation (SSE Optimization)
    //------------------------------------------------------
    inline XFLOAT4 __vectorcall xvec4_zero() noexcept
    {
		return XFLOAT4{ _mm_setzero_ps() };
    }

    inline void __vectorcall xvec4_zero(XFLOAT4 v) noexcept
    {
        v.xmm = _mm_setzero_ps();
    }

    inline XFLOAT4 __vectorcall xvec4_unit_x() noexcept
    {
		return XFLOAT4{ _mm_set_ps(0, 0, 0, 1.0f) };
    }

    inline XFLOAT4 __vectorcall xvec4_unit_y() noexcept
    {
		return XFLOAT4{ _mm_set_ps(0, 0, 1.0f, 0) };
    }

    inline XFLOAT4 __vectorcall xvec4_unit_z() noexcept
    {
		return XFLOAT4{ _mm_set_ps(0, 1.0f, 0, 0) };
    }

    inline XFLOAT4 __vectorcall xvec4_unit_w() noexcept
    {
		return XFLOAT4{ _mm_set_ps(1.0f, 0, 0, 0) };
    }

    inline XFLOAT4 __vectorcall xvec4_set(float x, float y, float z, float w) noexcept
    {
		return XFLOAT4{ _mm_set_ps(w, z, y, x) };
    }

    inline XFLOAT4 __vectorcall xvec4_add(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ _mm_add_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_sub(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ _mm_sub_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_mul(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ _mm_mul_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_mul(XFLOAT4 v, float f) noexcept
    {
		return XFLOAT4{ _mm_mul_ps(v.xmm, _mm_set1_ps(f)) };
    }

    inline XFLOAT4 __vectorcall xvec4_div(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ _mm_div_ps(v0.xmm, v1.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_div(XFLOAT4 v, float f) noexcept
    {
		return XFLOAT4{ _mm_div_ps(v.xmm, _mm_set1_ps(f)) };
    }

    inline float __vectorcall xvec4_dot_f(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return _mm_cvtss_f32(xmvec4_dot(v0.xmm, v1.xmm));
    }

	inline XFLOAT4 __vectorcall xvec4_dot(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ xmvec4_dot(v0.xmm, v1.xmm) };
    }

	inline XFLOAT4 __vectorcall xvec4_cross(XFLOAT4 v0, XFLOAT4 v1, XFLOAT4 v2) noexcept
    {
		return XFLOAT4{ xmvec4_cross(v0.xmm, v1.xmm, v2.xmm) };
    }

    inline float __vectorcall xvec4_lengthsq_f(XFLOAT4 v) noexcept
    {
        return xvec4_dot_f(v, v);
    }

    inline XFLOAT4 __vectorcall xvec4_lengthsq(XFLOAT4 v)  noexcept
    {
		return XFLOAT4{ xmvec4_lengthsq(v.xmm) };
    }

    inline float __vectorcall xvec4_length_f(XFLOAT4 v) noexcept
    {
        return _mm_cvtss_f32(xmvec4_length(v.xmm));
    }

    inline XFLOAT4 __vectorcall xvec4_length(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec4_length(v.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_normalize(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec4_normalize(v.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_lerp(XFLOAT4 v0, XFLOAT4 v1, float t) noexcept
    {
		return XFLOAT4{ xmvec_lerp(v0.xmm, v1.xmm, t) };
    }

    inline XFLOAT4 __vectorcall xvec4_clamp(XFLOAT4 v, XFLOAT4 vmin, XFLOAT4 vmax) noexcept
    {
		return XFLOAT4{ xmvec_clamp(v.xmm, vmin.xmm, vmax.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_saturate(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec_saturate(v.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_min(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ xmvec_min(v0.xmm, v1.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_max(XFLOAT4 v0, XFLOAT4 v1) noexcept
    {
		return XFLOAT4{ xmvec_max(v0.xmm, v1.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_ceil(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec_ceil(v.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_floor(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec_floor(v.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_round(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec_round(v.xmm) };
    }

    inline XFLOAT4 __vectorcall xvec4_trunc(XFLOAT4 v) noexcept
    {
		return XFLOAT4{ xmvec_trunc(v.xmm) };
    }


    //------------------------------------------------------
    // XQUAT Functions Implementation (SSE Optimized)
    //------------------------------------------------------

    inline XQUAT __vectorcall xquat_zero() noexcept
    {
		return XQUAT{ _mm_setzero_ps() };
    }

    inline XQUAT __vectorcall xquat_ident() noexcept
    {
        // Identity quaternion (0, 0, 0, 1)
		return XQUAT{ _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f) };
    }

    inline XQUAT __vectorcall xquat_set(float x, float y, float z, float w) noexcept
    {
		return XQUAT{ _mm_set_ps(w, z, y, x) };
    }

    inline XQUAT __vectorcall xquat_mul(XQUAT q0, XQUAT q1) noexcept
    {
        // 쿼터니언 곱셈 공식에 따라 직접 계산
         // out.x = q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y
         // out.y = q0.w * q1.y - q0.x * q1.z + q0.y * q1.w + q0.z * q1.x
         // out.z = q0.w * q1.z + q0.x * q1.y - q0.y * q1.x + q0.z * q1.w
         // out.w = q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z

        // 각 성분을 SSE 레지스터에 로드
        __m128 Q0 = _mm_set_ps(q0.w, q0.z, q0.y, q0.x);
        __m128 Q1 = _mm_set_ps(q1.w, q1.z, q1.y, q1.x);

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
                                                                                            __m128 term1 = _mm_mul_ps(q0w, Q1);  // q0.w * (q1.x, q1.y, q1.w)      // x ?? q0.x * (q1.w, -q1.z, q1.y, -q1.x)
        __m128 x_terms = _mm_mul_ps(q0x, _mm_set_ps(-q1.x, q1.y, -q1.z, q1.w));

        // y ?? q0.y * (q1.z, q1.w, -q1.x, -q1.y)
        __m128 y_terms = _mm_mul_ps(q0y, _mm_set_ps(-q1.y, -q1.x, q1.w, q1.z));

        // z ?? q0.z * (-q1.y, q1.x, q1.w, -q1.z)
        __m128 z_terms = _mm_mul_ps(q0z, _mm_set_ps(-q1.z, q1.w, q1.x, -q1.y));

        // 모든 항 합산
        __m128 result = _mm_add_ps(term1, x_terms);
        result = _mm_add_ps(result, y_terms);
        result = _mm_add_ps(result, z_terms);

        // 결과를 출력 벡터에 저장
		return XQUAT{ result };
    }

    inline XQUAT __vectorcall xquat_mul(XQUAT q, float f) noexcept
    {
		return XQUAT{ _mm_mul_ps(q.xmm, _mm_set1_ps(f)) };
    }

    inline XFLOAT3 __vectorcall xquat_mul(XQUAT q, XFLOAT3 v) noexcept
    {
        // nVidia SDK implementation reference
        // uv = cross(q.xyz, v)
        // uuv = cross(q.xyz, uv)
        // result = v + 2 * (uv * q.w + uuv)

        XFLOAT3 t{ q.x, q.y, q.z };
        XFLOAT3 uv, uuv;
        uv = xvec3_cross(t, v);
        uuv = xvec3_cross(t, uv);

        __m128 q_sse = _mm_load_ps(q.v);
        __m128 uv_sse = _mm_load_ps(uv.v);
        __m128 uuv_sse = _mm_load_ps(uuv.v);
        __m128 v_sse = _mm_load_ps(v.v);

        // Multiply uv by q.w
        __m128 qW = _mm_shuffle_ps(q_sse, q_sse, _MM_SHUFFLE(3, 3, 3, 3)); // [q.w, q.w, q.w, q.w]
        __m128 uvByQw = _mm_mul_ps(uv_sse, qW);

        // Calculate 2 * (uv * q.w + uuv)
        __m128 two = _mm_set1_ps(2.0f);
        __m128 combined = _mm_add_ps(uvByQw, uuv_sse);
        __m128 scaled = _mm_mul_ps(combined, two);

        // Calculate final result: v + scaled
        __m128 result = _mm_add_ps(v_sse, scaled);

		return XFLOAT3{ result };
    }

    inline XQUAT __vectorcall xquat_inverse(XQUAT q) noexcept
    {
        // Calculate conjugate and normalize
         // Conjugate: (-x, -y, -z, w)
        __m128 conjugate = _mm_xor_ps(q.xmm, _mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f));

        // Calculate squared norm
        float norm = xvec4_dot_f(q, q);

        // Check if norm is close to zero
        if (norm < M_TINY)
        {
            return xquat_ident();
        }

        // Normalize the conjugate
        __m128 invNorm = _mm_set1_ps(1.0f / norm);

        return XQUAT{ _mm_mul_ps(conjugate, invNorm) };
    }

    inline XQUAT __vectorcall xquat_conjugate(XQUAT q) noexcept
    {
        // Conjugate: (-x, -y, -z, w)
        return XQUAT{ _mm_xor_ps(q.xmm, _mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f)) };
    }

    inline void __vectorcall xquat_conjugate(XQUAT q, XQUAT& out) noexcept
    {
        // Conjugate: (-x, -y, -z, w)
        out.xmm = _mm_xor_ps(q.xmm, _mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f));
    }

    inline XQUAT __vectorcall xquat_slerp(XQUAT q0, XQUAT q1, float t) noexcept
    {
        // Calculate cosine of angle between quaternions
        float cosTheta = xvec4_dot_f(q0, q1);

        // If quaternions are very close, use linear interpolation
        if (cosTheta > 0.9995f)
        {
            return xvec4_lerp(q0, q1, t);
        }

        // Adjust sign and angle direction
        XQUAT q1Adj = q1;
        if (cosTheta < 0.0f)
        {
            q1Adj.xmm = _mm_xor_ps(q1.xmm, _mm_set1_ps(-0.0f)); // Change sign
            cosTheta = -cosTheta;
        }   

        // Calculate angle and its sine
        float theta = acosf(cosTheta);
        float sinTheta = sinf(theta);

        // Check if sine of angle is too small
        if (fabsf(sinTheta) < M_TINY)
        {
			return xvec4_lerp(q0, q1Adj, t);
        }

        // Calculate interpolation weights
        float w0 = sinf((1.0f - t) * theta) / sinTheta;
        float w1 = sinf(t * theta) / sinTheta;

        // Calculate interpolation
        __m128 weight0 = _mm_set1_ps(w0);
        __m128 weight1 = _mm_set1_ps(w1);

        __m128 weighted0 = _mm_mul_ps(q0.xmm, weight0);
        __m128 weighted1 = _mm_mul_ps(q1Adj.xmm, weight1);

        return XQUAT{ _mm_add_ps(weighted0, weighted1) };
    }

    inline XQUAT __vectorcall xquat_rotation_yaw_pitch_roll(float yaw, float pitch, float roll) noexcept
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

        return XQUAT{ xquat_set(x, y, z, w) };
    }

    inline XQUAT __vectorcall xquat_rotation_yaw_pitch_roll(XFLOAT3 v) noexcept
    {
        return xquat_rotation_yaw_pitch_roll(v.y, v.x, v.z);
    }

    inline XQUAT __vectorcall xquat_rotation_axis(float angle, XFLOAT3 v) noexcept
    {
        // Normalize axis vector
		XFLOAT3 axis = xvec3_normalize(v);

        // Calculate sine and cosine of half-angle
        float halfAngle = angle * 0.5f;
        float sinHalfAngle = sinf(halfAngle);
        float cosHalfAngle = cosf(halfAngle);

        // Set quaternion values: q = (x*sin(a/2), y*sin(a/2), z*sin(a/2), cos(a/2))
        return XQUAT{ _mm_set_ps(
            cosHalfAngle,
            axis.z * sinHalfAngle,
            axis.y * sinHalfAngle,
            axis.x * sinHalfAngle
		) };
    }

    inline XQUAT __vectorcall xquat_rotation_axis(float angle, float x, float y, float z) noexcept
    {
        XFLOAT3 axis = xvec3_set(x, y, z);
        return xquat_rotation_axis(angle, axis);
    }

    inline XQUAT __vectorcall xquat_rotation_matrix(XMATRIX4& m) noexcept
    {
        // Convert matrix to quaternion using Ken Shoemake's algorithm
        float trace = m.m00 + m.m11 + m.m22;

        // Choose best method based on the matrix diagonal
        if (trace > 0.0f)
        {
            // When trace is positive
            float s = sqrtf(trace + 1.0f) * 2.0f;
            float invS = 1.0f / s;

            float x = (m.m12 - m.m21) * invS;
            float y = (m.m20 - m.m02) * invS;
            float z = (m.m01 - m.m10) * invS;
            float w = 0.25f * s;

            return xquat_set(x, y, z, w);
        }
        else if ((m.m00 > m.m11) && (m.m00 > m.m22))
        {
            // When m00 is largest
            float s = sqrtf(1.0f + m.m00 - m.m11 - m.m22) * 2.0f;
            float invS = 1.0f / s;

            float x = 0.25f * s;
            float y = (m.m01 + m.m10) * invS;
            float z = (m.m02 + m.m20) * invS;
            float w = (m.m12 - m.m21) * invS;

            return xquat_set(x, y, z, w);
        }
        else if (m.m11 > m.m22)
        {
            // When m11 is largest
            float s = sqrtf(1.0f + m.m11 - m.m00 - m.m22) * 2.0f;
            float invS = 1.0f / s;

            float x = (m.m01 + m.m10) * invS;
            float y = 0.25f * s;
            float z = (m.m12 + m.m21) * invS;
            float w = (m.m20 - m.m02) * invS;

            return xquat_set(x, y, z, w);
        }
        else
        {
            // When m22 is largest
            float s = sqrtf(1.0f + m.m22 - m.m00 - m.m11) * 2.0f;
            float invS = 1.0f / s;

            float x = (m.m02 + m.m20) * invS;
            float y = (m.m12 + m.m21) * invS;
            float z = 0.25f * s;
            float w = (m.m01 - m.m10) * invS;

            return xquat_set(x, y, z, w);
        }
    }

    inline XQUAT __vectorcall xquat_from_lookat(XFLOAT3 forward) noexcept
    {
        // Create rotation from [0,0,1] to the forward vector
        // Normalize the vector
        XFLOAT3 normal = xvec3_normalize(forward);

        // Calculate dot product with [0,0,1]
        float dot = xvec3_dot_f(normal, xvec3_unit_z());

        // Handle special cases
        // If vectors are nearly parallel
        if (dot > 0.999999f)
        {
            return xquat_ident();
        }
        // If vectors are nearly opposite
        else if (dot < -0.999999f)
        {
            // Rotate 180 degrees around Y-axis
            return xquat_set(0.0f, 1.0f, 0.0f, 0.0f);
        }
        else
        {
            // Calculate rotation axis (cross product)
            XFLOAT3 unitZ = xvec3_unit_z();
            XFLOAT3 axis = xvec3_cross(unitZ, normal);

            // Calculate rotation angle
            float angle = acosf(dot);

            // Create quaternion from axis-angle rotation
            return xquat_rotation_axis(angle, axis);
        }
    }

    inline XQUAT __vectorcall xquat_from_lookat(XFLOAT3 forward, XFLOAT3 up) noexcept
    {
        // Normalize forward vector
		XFLOAT3 zAxis = xvec3_normalize(forward);

        // Calculate right vector (cross product of up and forward)
        XFLOAT3 xAxis = xvec3_cross(up, zAxis);
        xAxis = xvec3_normalize(xAxis);

        // Calculate new up vector (cross product of forward and right)
        XFLOAT3 yAxis = xvec3_cross(zAxis, xAxis);

        // Create rotation matrix
        XMATRIX4 rotMatrix;

        rotMatrix.m00 = xAxis.x; rotMatrix.m01 = xAxis.y; rotMatrix.m02 = xAxis.z; rotMatrix.m03 = 0.0f;
        rotMatrix.m10 = yAxis.x; rotMatrix.m11 = yAxis.y; rotMatrix.m12 = yAxis.z; rotMatrix.m13 = 0.0f;
        rotMatrix.m20 = zAxis.x; rotMatrix.m21 = zAxis.y; rotMatrix.m22 = zAxis.z; rotMatrix.m23 = 0.0f;
        rotMatrix.m30 = 0.0f;     rotMatrix.m31 = 0.0f;     rotMatrix.m32 = 0.0f;     rotMatrix.m33 = 1.0f;

        // Convert matrix to quaternion
        return xquat_rotation_matrix(rotMatrix);
    }

    inline XFLOAT3 __vectorcall xquat_to_yaw_pitch_roll(XQUAT q) noexcept
    {
        // Convert quaternion to Euler angles (roll, pitch, yaw in ZXY order)

        // Check for gimbal lock
        float test = q.x * q.y + q.z * q.w;
        if (test > 0.499f)
        {
            // Singularity at north pole
			return xvec3_set(2.0f * atan2f(q.x, q.w), M_PIDIV2, 0.0f); // pitch, yaw, roll
        }
        if (test < -0.499f)
        {
            // Singularity at south pole
            return xvec3_set(-2.0f * atan2f(q.x, q.w), -M_PIDIV2, 0.0f); // pitch, yaw, roll
        }

        // Normal case
        float sqx = q.x * q.x;
        float sqy = q.y * q.y;
        float sqz = q.z * q.z;

        // Roll (z-axis rotation)
        return xvec3_set(
             asinf(2.0f * test), // pitch (x-axis rotation)
             atan2f(2.0f * (q.y * q.w - q.x * q.z), 1.0f - 2.0f * (sqy + sqx)), // yaw (y-axis rotation)
             atan2f(2.0f * (q.x * q.y + q.z * q.w), 1.0f - 2.0f * (sqy + sqz)) // roll (z-axis rotation)
        );
    }

    inline void __vectorcall xquat_to_axis_angle(XQUAT q, XFLOAT3& axis, float& angle) noexcept
    {
        // Convert quaternion to axis-angle representation
        angle = 2.0f * acosf(q.w);

        // If rotation is nearly zero
        if (angle < M_TINY)
        {
			axis = xvec3_unit_x(); // Default axis
            angle = 0.0f;
            return;
        }

        // Calculate rotation axis
        float sinHalfAngle = sqrtf(1.0f - q.w * q.w);

        // Avoid division by zero
        if (fabsf(sinHalfAngle) < M_TINY)
        {
            sinHalfAngle = 1.0f;
        }

        float invSinHalfAngle = 1.0f / sinHalfAngle;
        axis.x = q.x * invSinHalfAngle;
        axis.y = q.y * invSinHalfAngle;
        axis.z = q.z * invSinHalfAngle;
    }

    inline void __vectorcall xquat_to_matrix(XQUAT q, XMATRIX4& out) noexcept
    {
        // SSE-optimized quaternion to rotation matrix (row-major)
        // row0: [1-2(yy+zz),  2(xy+zw),  2(xz-yw), 0]
        // row1: [2(xy-zw),  1-2(xx+zz),  2(yz+xw), 0]
        // row2: [2(xz+yw),  2(yz-xw),  1-2(xx+yy), 0]
        // row3: [0, 0, 0, 1]

        // Compute all 2*qi*qj products via SSE
        __m128 vq = _mm_set_ps(q.w, q.z, q.y, q.x);
        __m128 v2q = _mm_add_ps(vq, vq);

        // 2x*[x,y,z,w] = [2xx, 2xy, 2xz, 2xw]
        __m128 tx = _mm_mul_ps(_mm_shuffle_ps(v2q, v2q, _MM_SHUFFLE(0, 0, 0, 0)), vq);
        // 2y*[x,y,z,w] = [2yx, 2yy, 2yz, 2yw]
        __m128 ty = _mm_mul_ps(_mm_shuffle_ps(v2q, v2q, _MM_SHUFFLE(1, 1, 1, 1)), vq);
        // 2z*[x,y,z,w] = [2zx, 2zy, 2zz, 2zw]
        __m128 tz = _mm_mul_ps(_mm_shuffle_ps(v2q, v2q, _MM_SHUFFLE(2, 2, 2, 2)), vq);

        // Store to temp arrays for row assembly
        __declspec(align(16)) float ftx[4], fty[4], ftz[4];
        _mm_store_ps(ftx, tx);  // ftx = [2xx, 2xy, 2xz, 2xw]
        _mm_store_ps(fty, ty);  // fty = [2yx, 2yy, 2yz, 2yw]
        _mm_store_ps(ftz, tz);  // ftz = [2zx, 2zy, 2zz, 2zw]

        // Assemble rows using SSE set + add/sub
        // Row 0: [1-2yy-2zz, 2xy+2zw, 2xz-2yw, 0]
        __m128 r0_pos = _mm_set_ps(0.0f, ftx[2], ftx[1], 1.0f);
        __m128 r0_neg = _mm_set_ps(0.0f, fty[3], 0.0f,   fty[1] + ftz[2]);
        __m128 r0_add = _mm_set_ps(0.0f, 0.0f,   ftz[3], 0.0f);
        out.xmm_row[0] = _mm_add_ps(_mm_sub_ps(r0_pos, r0_neg), r0_add);

        // Row 1: [2xy-2zw, 1-2xx-2zz, 2yz+2xw, 0]
        __m128 r1_pos = _mm_set_ps(0.0f, fty[2], 1.0f,   ftx[1]);
        __m128 r1_neg = _mm_set_ps(0.0f, 0.0f,   ftx[0] + ftz[2], ftz[3]);
        __m128 r1_add = _mm_set_ps(0.0f, ftx[3], 0.0f,   0.0f);
        out.xmm_row[1] = _mm_add_ps(_mm_sub_ps(r1_pos, r1_neg), r1_add);

        // Row 2: [2xz+2yw, 2yz-2xw, 1-2xx-2yy, 0]
        __m128 r2_pos = _mm_set_ps(0.0f, 1.0f,   fty[2], ftx[2]);
        __m128 r2_neg = _mm_set_ps(0.0f, ftx[0] + fty[1], ftx[3], 0.0f);
        __m128 r2_add = _mm_set_ps(0.0f, 0.0f,   0.0f,   fty[3]);
        out.xmm_row[2] = _mm_add_ps(_mm_sub_ps(r2_pos, r2_neg), r2_add);

        // Row 3: [0, 0, 0, 1]
        out.xmm_row[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
    }

    inline XMATRIX4 __vectorcall xquat_to_matrix(XQUAT q) noexcept
    {
        // [ 1 - 2*(y^2 + z^2), 2*(x*y - z*w), 2*(x*z + y*w), 0.0f ]
        // [ 2*(x*y + z*w), 1 - 2*(x^2 + z^2), 2*(y*z - x*w), 0.0f ]
        // [ 2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x^2 + y^2), 0.0f ]
        // [ 0.0f, 0.0f, 0.0f, 1.0f ]

		XMATRIX4 out;
		xquat_to_matrix(q, out);
        return out;
    }


    //------------------------------------------------------
    // XMATRIX4 Functions Implementation (SSE Optimized)
    //------------------------------------------------------
    inline void __vectorcall xmat4_zero(XMATRIX4& m) noexcept
    {
        __m128 zero = _mm_setzero_ps();
        m.xmm_row[0] = zero;
        m.xmm_row[1] = zero;
        m.xmm_row[2] = zero;
        m.xmm_row[3] = zero;
    }

    inline XMATRIX4 __vectorcall xmat4_zero() noexcept
    {
        XMATRIX4 m;
        xmat4_zero(m);
        return m;
    }

    inline void __vectorcall xmat4_ident(XMATRIX4& m) noexcept
    {
        // Set diagonal to 1
        m.xmm_row[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
        m.xmm_row[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
        m.xmm_row[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
        m.xmm_row[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
    }

    inline bool __vectorcall xmat4_is_ident(const XMATRIX4& m) noexcept
    {
        // Check if matrix is identity
        __m128 row0 = _mm_cmpeq_ps(m.xmm_row[0], _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f));
        __m128 row1 = _mm_cmpeq_ps(m.xmm_row[1], _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f));
        __m128 row2 = _mm_cmpeq_ps(m.xmm_row[2], _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f));
        __m128 row3 = _mm_cmpeq_ps(m.xmm_row[3], _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));
        // Combine all comparison results with AND operation
        __m128 and1 = _mm_and_ps(row0, row1);
        __m128 and2 = _mm_and_ps(row2, row3);
        __m128 result = _mm_and_ps(and1, and2);

        // All bits must be 1 for identity matrix
        return (_mm_movemask_ps(result) == 0xF);
    }

    inline bool __vectorcall xmat4_is_nan(const XMATRIX4& m) noexcept
    {
        // Check if any element is NaN
        __m128 row0nan = _mm_cmpunord_ps(m.xmm_row[0], m.xmm_row[0]);
        __m128 row1nan = _mm_cmpunord_ps(m.xmm_row[1], m.xmm_row[1]);
        __m128 row2nan = _mm_cmpunord_ps(m.xmm_row[2], m.xmm_row[2]);
        __m128 row3nan = _mm_cmpunord_ps(m.xmm_row[3], m.xmm_row[3]);

        // Combine all results with OR operation
        __m128 or1 = _mm_or_ps(row0nan, row1nan);
        __m128 or2 = _mm_or_ps(row2nan, row3nan);
        __m128 result = _mm_or_ps(or1, or2);

        // If any bit is 1, NaN exists
        return (_mm_movemask_ps(result) != 0);
    }

    inline bool __vectorcall xmat4_is_infinite(const XMATRIX4& m) noexcept
    {
        // Create infinity mask (exponent bits all 1, other bits 0)
        __m128 infinity = _mm_set1_ps(INFINITY);
        __m128 neg_infinity = _mm_set1_ps(-INFINITY);

        // Check for infinity in each row
        __m128 row0inf = _mm_or_ps(_mm_cmpeq_ps(m.xmm_row[0], infinity), _mm_cmpeq_ps(m.xmm_row[0], neg_infinity));
        __m128 row1inf = _mm_or_ps(_mm_cmpeq_ps(m.xmm_row[1], infinity), _mm_cmpeq_ps(m.xmm_row[1], neg_infinity));
        __m128 row2inf = _mm_or_ps(_mm_cmpeq_ps(m.xmm_row[2], infinity), _mm_cmpeq_ps(m.xmm_row[2], neg_infinity));
        __m128 row3inf = _mm_or_ps(_mm_cmpeq_ps(m.xmm_row[3], infinity), _mm_cmpeq_ps(m.xmm_row[3], neg_infinity));

        // Combine all results with OR operation
        __m128 or1 = _mm_or_ps(row0inf, row1inf);
        __m128 or2 = _mm_or_ps(row2inf, row3inf);
        __m128 result = _mm_or_ps(or1, or2);

        // If any bit is 1, infinity exists
        return (_mm_movemask_ps(result) != 0);
    }

    inline void __vectorcall xmat4_set(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33,
        XMATRIX4& out
    ) noexcept
    {
        out.xmm_row[0] = _mm_set_ps(m03, m02, m01, m00);
        out.xmm_row[1] = _mm_set_ps(m13, m12, m11, m10);
        out.xmm_row[2] = _mm_set_ps(m23, m22, m21, m20);
        out.xmm_row[3] = _mm_set_ps(m33, m32, m31, m30);
    }

    inline XMATRIX4 __vectorcall xmat4_set(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) noexcept
    {
        XMATRIX4 r;
        xmat4_set(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33,
            r
        );
        return r;
    }

    inline void __vectorcall xmat4_trans(XFLOAT3 pos, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.xmm_row[3] = _mm_set_ps(1.0f, pos.z, pos.y, pos.x);
    }

    inline void __vectorcall xmat4_trans(float x, float y, float z, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.xmm_row[3] = _mm_set_ps(1.0f, z, y, x);
	}

    inline XMATRIX4 __vectorcall xmat4_trans(XFLOAT3 pos) noexcept
    {
        XMATRIX4 r;
        xmat4_trans(pos.x, pos.y, pos.z, r);
        return r;
    }

    inline void __vectorcall xmat4_get_trans(const XMATRIX4& m, XFLOAT3& out) noexcept
    {
        out.x = m.m30;
        out.y = m.m31;
        out.z = m.m32;
    }

    inline XFLOAT3 __vectorcall xmat4_get_trans(const XMATRIX4& m) noexcept
    {
        XFLOAT3 r;
        xmat4_get_trans(m, r);
        return r;
    }
    
    inline void __vectorcall xmat4_transpose(const XMATRIX4& m, XMATRIX4& out) noexcept
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
        __m128 row0 = m.xmm_row[0];
        __m128 row1 = m.xmm_row[1];
        __m128 row2 = m.xmm_row[2];
        __m128 row3 = m.xmm_row[3];

        _MM_TRANSPOSE4_PS(row0, row1, row2, row3);

        out.xmm_row[0] = row0;
        out.xmm_row[1] = row1;
        out.xmm_row[2] = row2;
        out.xmm_row[3] = row3;
    }

    inline XMATRIX4 __vectorcall xmat4_transpose(const XMATRIX4& m) noexcept
    {
        XMATRIX4 r;
        xmat4_transpose(m, r);
        return r;
    }

    inline void __vectorcall xmat4_inverse(const XMATRIX4& m, XMATRIX4& out) noexcept
    {
        // SSE-optimized 4x4 inverse via Laplace expansion with 2x2 sub-determinants
        float m00=m.m00, m01=m.m01, m02=m.m02, m03=m.m03;
        float m10=m.m10, m11=m.m11, m12=m.m12, m13=m.m13;
        float m20=m.m20, m21=m.m21, m22=m.m22, m23=m.m23;
        float m30=m.m30, m31=m.m31, m32=m.m32, m33=m.m33;

        // 2x2 sub-determinants from rows 0,1
        float s0 = m00*m11 - m01*m10;
        float s1 = m00*m12 - m02*m10;
        float s2 = m00*m13 - m03*m10;
        float s3 = m01*m12 - m02*m11;
        float s4 = m01*m13 - m03*m11;
        float s5 = m02*m13 - m03*m12;

        // 2x2 sub-determinants from rows 2,3
        float c5 = m22*m33 - m23*m32;
        float c4 = m21*m33 - m23*m31;
        float c3 = m21*m32 - m22*m31;
        float c2 = m20*m33 - m23*m30;
        float c1 = m20*m32 - m22*m30;
        float c0 = m20*m31 - m21*m30;

        float det = s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0;
        if (fabsf(det) < 1e-6f) { xmat4_ident(out); return; }

        __m128 vDet = _mm_set1_ps(1.0f / det);

        // Adjugate rows computed with SSE: 4 elements per mul_ps
        // Row 0: adj = [m11*c5-m12*c4+m13*c3, -(m01*c5-m02*c4+m03*c3), m31*s5-m32*s4+m33*s3, -(m21*s5-m22*s4+m23*s3)]
        out.xmm_row[0] = _mm_mul_ps(vDet, _mm_add_ps(_mm_sub_ps(
            _mm_mul_ps(_mm_set_ps(-m21, m31, -m01, m11), _mm_set1_ps(c5)),
            _mm_mul_ps(_mm_set_ps(-m22, m32, -m02, m12), _mm_set1_ps(c4))),
            _mm_mul_ps(_mm_set_ps(-m23, m33, -m03, m13), _mm_set1_ps(c3))));

        // Row 1: adj = [-(m10*c5-m12*c2+m13*c1), m00*c5-m02*c2+m03*c1, -(m30*s5-m32*s2+m33*s1), m20*s5-m22*s2+m23*s1]
        out.xmm_row[1] = _mm_mul_ps(vDet, _mm_add_ps(_mm_sub_ps(
            _mm_mul_ps(_mm_set_ps(m20, -m30, m00, -m10), _mm_set1_ps(c5)),
            _mm_mul_ps(_mm_set_ps(m22, -m32, m02, -m12), _mm_set1_ps(c2))),
            _mm_mul_ps(_mm_set_ps(m23, -m33, m03, -m13), _mm_set1_ps(c1))));

        // Row 2: adj = [m10*c4-m11*c2+m13*c0, -(m00*c4-m01*c2+m03*c0), m30*s4-m31*s2+m33*s0, -(m20*s4-m21*s2+m23*s0)]
        out.xmm_row[2] = _mm_mul_ps(vDet, _mm_add_ps(_mm_sub_ps(
            _mm_mul_ps(_mm_set_ps(-m20, m30, -m00, m10), _mm_set1_ps(c4)),
            _mm_mul_ps(_mm_set_ps(-m21, m31, -m01, m11), _mm_set1_ps(c2))),
            _mm_mul_ps(_mm_set_ps(-m23, m33, -m03, m13), _mm_set1_ps(c0))));

        // Row 3: adj = [-(m10*c3-m11*c1+m12*c0), m00*c3-m01*c1+m02*c0, -(m30*s3-m31*s1+m32*s0), m20*s3-m21*s1+m22*s0]
        out.xmm_row[3] = _mm_mul_ps(vDet, _mm_add_ps(_mm_sub_ps(
            _mm_mul_ps(_mm_set_ps(m20, -m30, m00, -m10), _mm_set1_ps(c3)),
            _mm_mul_ps(_mm_set_ps(m21, -m31, m01, -m11), _mm_set1_ps(c1))),
            _mm_mul_ps(_mm_set_ps(m22, -m32, m02, -m12), _mm_set1_ps(c0))));
    }

    inline XMATRIX4 __vectorcall xmat4_inverse(const XMATRIX4& m) noexcept
    {
        XMATRIX4 r;
        xmat4_inverse(m, r);
        return r;
    }

    inline void __vectorcall xmat4_mul(const XMATRIX4& m0, const XMATRIX4& m1, XMATRIX4& out) noexcept
    {
        // m0 ??transpose
        // 각 row에 대해
        for (int i = 0; i < 4; ++i) 
        {
            __m128 row = m0.xmm_row[i];

        // row의 각 성분을 브로드캐스트
            __m128 row0 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(0, 0, 0, 0));
            __m128 row1 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(1, 1, 1, 1));
            __m128 row2 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(2, 2, 2, 2));
            __m128 row3 = _mm_shuffle_ps(row, row, _MM_SHUFFLE(3, 3, 3, 3));

        // 4-way dot-product
            __m128 sum01 = _mm_add_ps(
                _mm_mul_ps(row0, m1.xmm_row[0]),
                _mm_mul_ps(row1, m1.xmm_row[1])
            );
            __m128 sum23 = _mm_add_ps(
                _mm_mul_ps(row2, m1.xmm_row[2]),
                _mm_mul_ps(row3, m1.xmm_row[3])
            );
            __m128 res = _mm_add_ps(sum01, sum23);

        // 결과 저장
            //_mm_store_ps(out->row[i], result);
            out.xmm_row[i] = res;
        }
    }

    inline XMATRIX4 __vectorcall xmat4_mul(const XMATRIX4& m0, const XMATRIX4& m1) noexcept
    {
        XMATRIX4 r;
        xmat4_mul(m0, m1, r);
        return r;
    }

    inline void __vectorcall xmat4_mul(const XMATRIX4& m, const XFLOAT3& v, XFLOAT3& out) noexcept
    {
        __m128 vec = _mm_set_ps(1.0f, v.z, v.y, v.x);

        __m128 row0 = m.xmm_row[0]; // 첫 번째 행
        __m128 row1 = m.xmm_row[1]; // 두 번째 행
        __m128 row2 = m.xmm_row[2]; // 세 번째 행
        __m128 row3 = m.xmm_row[3]; // 네 번째 행

        __m128 xxxx = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 yyyy = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 zzzz = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 wwww = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 res = _mm_mul_ps(row0, xxxx);
        res = _mm_add_ps(res, _mm_mul_ps(row1, yyyy));
        res = _mm_add_ps(res, _mm_mul_ps(row2, zzzz));
        res = _mm_add_ps(res, _mm_mul_ps(row3, wwww));

        out.xmm = res;
    }

    inline void __vectorcall xmat4_mul(const XMATRIX4& m, const XFLOAT4& v, XFLOAT4& out) noexcept
    {
        __m128 vVec = v.xmm;

        __m128 xxxx = _mm_shuffle_ps(vVec, vVec, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 yyyy = _mm_shuffle_ps(vVec, vVec, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 zzzz = _mm_shuffle_ps(vVec, vVec, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 wwww = _mm_shuffle_ps(vVec, vVec, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 res = _mm_mul_ps(m.xmm_row[0], xxxx);
        res = _mm_add_ps(res, _mm_mul_ps(m.xmm_row[1], yyyy));
        res = _mm_add_ps(res, _mm_mul_ps(m.xmm_row[2], zzzz));
        res = _mm_add_ps(res, _mm_mul_ps(m.xmm_row[3], wwww));

        out.xmm = res;
    }

    inline XFLOAT3 __vectorcall xmat4_mul(const XMATRIX4& m, const XFLOAT3& v) noexcept
    {
        XFLOAT3 r;
        xmat4_mul(m, v, r);
        return r;
    }

    inline XFLOAT4 __vectorcall xmat4_mul(const XMATRIX4& m, const XFLOAT4& v) noexcept
    {
        XFLOAT4 r;
        xmat4_mul(m, v, r);
        return r;
    }

    inline void __vectorcall xmat4_scale(float sx, float sy, float sz, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);

        // Set scale factors on diagonal
		out.m00 = sx;
		out.m11 = sy;
		out.m22 = sz;
    }

    inline XMATRIX4 __vectorcall xmat4_scale(float sx, float sy, float sz) noexcept
    {
        XMATRIX4 r;
        xmat4_scale(sx, sy, sz, r);
        return r;
    }

    inline void __vectorcall xmat4_scale(XFLOAT3 v, XMATRIX4& out) noexcept
    {
        xmat4_scale(v.x, v.y, v.z, out);
    }

    inline XMATRIX4 __vectorcall xmat4_scale(XFLOAT3 v) noexcept
    {
        XMATRIX4 r;
        xmat4_scale(v, r);
        return r;
    }

    inline void __vectorcall xmat4_rotation_x(float angle, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);

        // Calculate X-axis rotation matrix
        float c = cosf(angle);
        float s = sinf(angle);

        out.m11 = c;
        out.m12 = s;
        out.m21 = -s;
        out.m22 = c;
    }

    inline XMATRIX4 __vectorcall xmat4_rotation_x(float angle) noexcept
    {
        XMATRIX4 r;
        xmat4_rotation_x(angle, r);
        return r;
    }
    
    inline void __vectorcall xmat4_rotation_y(float angle, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);

        // Calculate Y-axis rotation matrix
        float c = cosf(angle);
        float s = sinf(angle);

        out.m00 = c;
        out.m02 = -s;
        out.m20 = s;
        out.m22 = c;
    }

    inline XMATRIX4 __vectorcall xmat4_rotation_y(float angle) noexcept
    {
        XMATRIX4 r;
        xmat4_rotation_y(angle, r);
        return r;
    }

    inline void __vectorcall xmat4_rotation_z(float angle, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);

        // Calculate Z-axis rotation matrix
        float c = cosf(angle);
        float s = sinf(angle);

        out.m00 = c;
        out.m01 = s;
        out.m10 = -s;
        out.m11 = c;
    }

    inline XMATRIX4 __vectorcall xmat4_rotation_z(float angle) noexcept
    {
        XMATRIX4 r;
        xmat4_rotation_z(angle, r);
        return r;
    }

    inline void __vectorcall xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, XMATRIX4& out) noexcept
    {
        // Convert to quaternion then to matrix (more stable method)
        XQUAT q = xquat_rotation_yaw_pitch_roll(yaw, pitch, roll);
        xquat_to_matrix(q, out);
    }

    inline XMATRIX4 __vectorcall xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll) noexcept
    {
        XMATRIX4 r;
        xmat4_rotation_yaw_pitch_roll(yaw, pitch, roll, r);
        return r;
    }

    inline void __vectorcall xmat4_rotation_axis(XFLOAT3 axis, float angle, XMATRIX4& out) noexcept
    {
        // Convert to quaternion then to matrix (more stable method)
        XQUAT q = xquat_rotation_axis(angle, axis);
        xquat_to_matrix(q, out);
    }

    inline XMATRIX4 __vectorcall xmat4_rotation_axis(XFLOAT3 axis, float angle) noexcept
    {
        XMATRIX4 r;
        xmat4_rotation_axis(axis, angle, r);
        return r;
    }

    inline void __vectorcall xmat4_lookto_lh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up, XMATRIX4& out) noexcept
    {
        // SSE optimized lookto matrix calculation (left-handed coordinate system)
        // Create SSE vectors
        __m128 eyeVec = _mm_set_ps(0.0f, eye.z, eye.y, eye.x);
        __m128 toVec = _mm_set_ps(0.0f, to.z, to.y, to.x);
        __m128 upVec = _mm_set_ps(0.0f, up.z, up.y, up.x);

        // zaxis = normalize(to - eye)
        __m128 zaxis = _mm_sub_ps(toVec, eyeVec);

        // Normalize zaxis
        __m128 zaxis_dot = _mm_dp_ps(zaxis, zaxis, 0x7F); // 0x7F = 0111 1111, dot product of lowest 3 elements
        __m128 zaxis_length = _mm_sqrt_ps(zaxis_dot);

        // Prevent division by zero
        __m128 zaxis_is_zero = _mm_cmplt_ps(zaxis_dot, _mm_set1_ps(M_TINY));
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
        __m128 xaxis_is_zero = _mm_cmplt_ps(xaxis_dot, _mm_set1_ps(M_TINY));
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
        out.m00 = _mm_cvtss_f32(xaxis);
        out.m01 = _mm_cvtss_f32(yaxis);
        out.m02 = _mm_cvtss_f32(zaxis);
        out.m03 = 0.0f;

        out.m10 = _mm_cvtss_f32(_mm_shuffle_ps(xaxis, xaxis, _MM_SHUFFLE(0, 0, 0, 1)));
        out.m11 = _mm_cvtss_f32(_mm_shuffle_ps(yaxis, yaxis, _MM_SHUFFLE(0, 0, 0, 1)));
        out.m12 = _mm_cvtss_f32(_mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(0, 0, 0, 1)));
        out.m13 = 0.0f;

        out.m20 = _mm_cvtss_f32(_mm_shuffle_ps(xaxis, xaxis, _MM_SHUFFLE(0, 0, 0, 2)));
        out.m21 = _mm_cvtss_f32(_mm_shuffle_ps(yaxis, yaxis, _MM_SHUFFLE(0, 0, 0, 2)));
        out.m22 = _mm_cvtss_f32(_mm_shuffle_ps(zaxis, zaxis, _MM_SHUFFLE(0, 0, 0, 2)));
        out.m23 = 0.0f;

        out.m30 = _mm_cvtss_f32(neg_eye_dot_xaxis);
        out.m31 = _mm_cvtss_f32(neg_eye_dot_yaxis);
        out.m32 = _mm_cvtss_f32(neg_eye_dot_zaxis);
        out.m33 = 1.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_lookto_lh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up) noexcept
    {
        XMATRIX4 r;
        xmat4_lookto_lh(eye, to, up, r);
        return r;
    }

    inline void __vectorcall xmat4_lookto_rh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up, XMATRIX4& out) noexcept
    {
        // SSE optimized lookto matrix calculation (right-handed coordinate system)
        // Only direction is flipped (zaxis = eye - to)
        XFLOAT3 negTo;
        negTo.x = -to.x;
        negTo.y = -to.y;
        negTo.z = -to.z;

        // Reuse left-handed function, just invert direction
        xmat4_lookto_lh(eye, negTo, up, out);

        // Flip Z axis direction (right-handed coordinate system)
        out.m02 = -out.m02;
        out.m12 = -out.m12;
        out.m22 = -out.m22;
        out.m32 = -out.m32;
    }

    inline XMATRIX4 __vectorcall xmat4_lookto_rh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up) noexcept
    {
        XMATRIX4 r;
        xmat4_lookto_rh(eye, to, up, r);
        return r;
    }

    inline void __vectorcall xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        float tanHalfFov = tanf(fov * 0.5f);
        float yScale = 1.0f / tanHalfFov;
        float xScale = yScale / aspect;

        out.m00 = xScale;
        out.m01 = 0.0f;
        out.m02 = 0.0f;
        out.m03 = 0.0f;

        out.m10 = 0.0f;
        out.m11 = yScale;
        out.m12 = 0.0f;
        out.m13 = 0.0f;

        out.m20 = 0.0f;
        out.m21 = 0.0f;
        out.m22 = farZ / (farZ - nearZ);
        out.m23 = 1.0f;

        out.m30 = 0.0f;
        out.m31 = 0.0f;
        out.m32 = -nearZ * farZ / (farZ - nearZ);
        out.m33 = 0.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_perspective_lh(fov, aspect, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        // Calculate values for projection matrix
        float tanHalfFov = tanf(fov * 0.5f);
        float yScale = 1.0f / tanHalfFov;
        float xScale = yScale / aspect;

        // Calculate near and far plane values
        float rangeInv = 1.0f / (nearZ - farZ);

        // Explicitly set matrix elements
        // Right-handed perspective projection matrix
        out.m00 = xScale;
        out.m01 = 0.0f;
        out.m02 = 0.0f;
        out.m03 = 0.0f;

        out.m10 = 0.0f;
        out.m11 = yScale;
        out.m12 = 0.0f;
        out.m13 = 0.0f;

        out.m20 = 0.0f;
        out.m21 = 0.0f;
        out.m22 = farZ * rangeInv;  // Negative value in right-handed system
        out.m23 = -1.0f;            // w component is -1 in right-handed system

        out.m30 = 0.0f;
        out.m31 = 0.0f;
        out.m32 = nearZ * farZ * rangeInv;
        out.m33 = 0.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_perspective_rh(fov, aspect, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.m00 = 2.0f * nearZ / (right - left);
        out.m11 = -2.0f * nearZ / (bottom - top);
        out.m20 = -1.0f - 2.0f * left / (right - left);
        out.m21 = 1.0f + 2.0f * top / (bottom - top);
        out.m22 = -farZ / (nearZ - farZ);
        out.m32 = (nearZ * farZ) / (nearZ - farZ);
        out.m23 = 1.0f;
        out.m33 = 0.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_perspective_lh(left, right, top, bottom, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.m00 = 2.0f * nearZ / (right - left);
        out.m11 = -2.0f * nearZ / (bottom - top);
        out.m20 = 1.0f + 2.0f * left / (right - left);
        out.m21 = -1.0f - 2.0f * top / (bottom - top);
        out.m22 = farZ / (nearZ - farZ);
        out.m32 = (nearZ * farZ) / (nearZ - farZ);
        out.m23 = -1.0f;
        out.m33 = 0.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_perspective_rh(left, right, top, bottom, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_ortho_lh(float width, float height, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.m00 = 2.0f / width;
        out.m11 = 2.0f / height;
        out.m22 = 1.0f / (farZ - nearZ);
        out.m32 = nearZ / (nearZ - farZ);
    }

    inline XMATRIX4 __vectorcall xmat4_ortho_lh(float width, float height, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_ortho_lh(width, height, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_ortho_rh(float width, float height, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.m00 = 2.0f / width;
        out.m11 = 2.0f / height;
        out.m22 = 1.0f / (nearZ - farZ);
        out.m32 = nearZ / (nearZ - farZ);
    }

    inline XMATRIX4 __vectorcall xmat4_ortho_rh(float width, float height, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_ortho_rh(width, height, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.m00 = 2.0f / (right - left);
        out.m11 = 2.0f / (top - bottom);
        out.m22 = 1.0f / (farZ - nearZ);
        out.m30 = -1.0f - 2.0f * left / (right - left);
        out.m31 = 1.0f + 2.0f * top / (bottom - top);
        out.m32 = nearZ / (nearZ - farZ);
    }

    inline XMATRIX4 __vectorcall xmat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_ortho_lh(left, right, top, bottom, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        xmat4_ident(out);
        out.m00 = 2.0f / (right - left);
        out.m11 = 2.0f / (top - bottom);
        out.m22 = 1.0f / (nearZ - farZ);
        out.m30 = -1.0f - 2.0f * left / (right - left);
        out.m31 = 1.0f + 2.0f * top / (bottom - top);
        out.m32 = nearZ / (nearZ - farZ);
    }

    inline XMATRIX4 __vectorcall xmat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_ortho_rh(left, right, top, bottom, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        float width = right - left;
        float height = top - bottom;
        float depth = farZ - nearZ;

        out.m00 = (2.0f * nearZ) / width;
        out.m01 = 0.0f;
        out.m02 = 0.0f;
        out.m03 = 0.0f;

        out.m10 = 0.0f;
        out.m11 = (2.0f * nearZ) / height;
        out.m12 = 0.0f;
        out.m13 = 0.0f;

        out.m20 = (left + right) / width;
        out.m21 = (top + bottom) / height;
        out.m22 = farZ / depth;
        out.m23 = 1.0f;

        out.m30 = 0.0f;
        out.m31 = 0.0f;
        out.m32 = -(farZ * nearZ) / depth;
        out.m33 = 0.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_frustum_lh(left, right, bottom, top, nearZ, farZ, r);
        return r;
    }

    inline void __vectorcall xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4& out) noexcept
    {
        float width = right - left;
        float height = top - bottom;
        float depth = farZ - nearZ;

        out.m00 = (2.0f * nearZ) / width;
        out.m01 = 0.0f;
        out.m02 = 0.0f;
        out.m03 = 0.0f;

        out.m10 = 0.0f;
        out.m11 = (2.0f * nearZ) / height;
        out.m12 = 0.0f;
        out.m13 = 0.0f;

        out.m20 = (left + right) / width;
        out.m21 = (top + bottom) / height;
        out.m22 = -(farZ + nearZ) / depth;
        out.m23 = -1.0f;

        out.m30 = 0.0f;
        out.m31 = 0.0f;
        out.m32 = -(2.0f * farZ * nearZ) / depth;
        out.m33 = 0.0f;
    }

    inline XMATRIX4 __vectorcall xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
    {
        XMATRIX4 r;
        xmat4_frustum_rh(left, right, bottom, top, nearZ, farZ, r);
        return r;
    }
}


#endif // #if defined(__SSE__)
