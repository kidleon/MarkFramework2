#pragma once
#if defined(__SSE__)
#	include <immintrin.h>
#endif // #if defined(__SSE__)
#include <format>


namespace mark
{
	struct SINT2
	{
		union
		{
			struct { int32_t x, y; };
			int32_t v[2];
		};
	};

	struct SINT3
	{
		union
		{
			struct { int32_t x, y, z; };
			int32_t v[3];
		};
	};

	struct SINT4
	{
		union
		{
			struct { int32_t x, y, z, w; };
			int32_t v[4];
		};
	};

	struct UINT2
	{
		union
		{
			struct { uint32_t x, y; };
			uint32_t v[2];
		};
	};

	struct UINT3
	{
		union
		{
			struct { uint32_t x, y, z; };
			uint32_t v[3];
		};
	};

	struct UINT4
	{
		union
		{
			struct { uint32_t x, y, z, w; };
			uint32_t v[4];
		};
	};

	struct FLOAT2
	{
		union
		{
			struct { float x, y; };
			float v[2];
		};
	};

	struct FLOAT3
	{
		union
		{
			struct { float x, y, z; };
			float v[3];
		};
	};

	struct FLOAT4
	{
		union
		{
			struct { float x, y, z, w; };
			float v[4];
		};
	};

	struct QUAT
	{
		union
		{
			struct { float x, y, z, w; };
			float q[4];
		};
	};

	struct COLOR4F
	{
		union
		{
			struct { float r, g, b, a; };
			float v[4];
		};
	};

	struct COLOR4U
	{
		union
		{
			struct { uint8_t r, g, b, a; };
			uint8_t v[4];
		};
	};

	struct MATRIX4
	{
		union
		{
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};

			FLOAT4 r[4];  // 행 단위로 접근 가능
			float v[16];
			struct { float m[4][4]; };
		};
	};

#if defined(__SSE__)
	struct alignas(16) SIMD_VEC
	{
		union
		{
			struct { float x, y, z, w; };
			__m128 xmm;
		};
	};

	struct alignas(16) XFLOAT2
	{
		union
		{
			struct { float x, y; };
			float v[2];
			__m128 xmm;
		};

		XFLOAT2() : xmm(_mm_setzero_ps()) {}
		constexpr XFLOAT2(float _x, float _y) : x(_x), y(_y) {}
		explicit XFLOAT2(__m128 _xmm) : xmm(_xmm) {}
	};

	struct alignas(16) XFLOAT3
	{
		union
		{
			struct { float x, y, z; };
			float v[3];
			__m128 xmm;
		};

		XFLOAT3() : xmm(_mm_setzero_ps()) {}
		constexpr XFLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		explicit XFLOAT3(__m128 _xmm) : xmm(_xmm) {}
	};

	struct alignas(16) XFLOAT4
	{
		union
		{
			struct { float x, y, z, w; };
			float v[4];
			__m128 xmm;
		};

		XFLOAT4() : xmm(_mm_setzero_ps()) {}
		constexpr XFLOAT4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		explicit XFLOAT4(__m128 _xmm) : xmm(_xmm) {}
	};

	struct alignas(16) XMATRIX4
	{
		union
		{
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
			__m128 xmm_row[4];
			float m[4][4];
			float v[16];
		};
	};
#elif defined(__NEON__)
	struct alignas(16) XFLOAT2
	{
		union
		{
			struct { float x, y; };
			float32x4_t xmm;
			float v[2];
		};
	};

	struct alignas(16) XFLOAT3
	{
		union
		{
			struct { float x, y, z; };
			float32x4_t xmm;
			float v[3];
		};
	};

	struct alignas(16) XFLOAT4
	{
		union
		{
			struct { float x, y, z, w; };
			float32x4_t xmm;
			float v[4];
		};
	};

	struct alignas(16) XMATRIX4
	{
		union
		{
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
			float32x4_t xmm_row[4];
			float m[4][4];
			float v[16];
		};
	};
#endif // #if defined(__SSE__)
	using XQUAT = XFLOAT4; // 쿼터니언도 4개의 float로 표현되므로 XFLOAT4를 재사용


	//-----------------------------------------------------------------------------
	// MATH UTILITY
	template<typename T> inline T T_MIN(const T& a, const T& b) { return a < b ? a : b; }
	template<typename T> inline T T_MAX(const T& a, const T& b) { return a > b ? a : b; }
	template<typename T> inline T T_CLAMP(const T& value, const T& _min, const T& _max) { return T_MAX(_min, T_MIN(value, _max)); }
	template<typename T> inline T T_SATURATE(const T& value) { return T_CLAMP(value, static_cast<T>(0), static_cast<T>(1)); }
	template<typename T> inline T T_MIN3(const T& a, const T& b, const T& c) { return T_MIN(T_MIN(a, b), c); }
	template<typename T> inline T T_MAX3(const T& a, const T& b, const T& c) { return T_MAX(T_MAX(a, b), c); }
	template<typename T> inline T T_MIN4(const T& a, const T& b, const T& c, const T& d) { return T_MIN(T_MIN3(a, b, c), d); }
	template<typename T> inline T T_MAX4(const T& a, const T& b, const T& c, const T& d) { return T_MAX(T_MAX3(a, b, c), d); }
	template<typename T> inline T T_LERP(const T& a, const T& b, float t) { return a + (b - a) * t; }
	inline float F_SIGN(float value) { return (float)((0.0f < value) - (value < 0.0f)); }

	constexpr float MX_PI= 3.14159265358979323846f; // 파이값.
	constexpr float MX_2PI = 6.283185307f; // 2 * 파이값.
	constexpr float MX_1DIVPI = 0.318309886f; // 1 / 파이값.
	constexpr float MX_1DIV2PI = 0.159154943f; // 1 / (2 * 파이값).
	constexpr float MX_PIDIV2 = 1.570796327f; // 파이값 / 2.
	constexpr float MX_TINY = 0.00001f; // 작은 수. 주로 0으로 나누는 것을 방지하기 위해 사용.
	constexpr float MX_EPSILON = 0.000001f; // 부동 소수점 비교 시 허용되는 오차 범위. 두 값이 이 범위 내에 있으면 거의 같다고 간주.
	constexpr float MX_2EPSILON = 0.000002f; // 2 * EPSILON. 부동 소수점 비교 시 더 넓은 오차 범위를 허용할 때 사용.
	constexpr float DEG_TO_RAD = MX_PI / 180.0f; // 1 degree를 라디안으로 변환하는 상수
	constexpr float RAD_TO_DEG = 180.0f / MX_PI; // 1 radian을 degree로 변환하는 상수

	constexpr SINT2 SINT2_ZERO = { 0, 0 };
	constexpr SINT2 SINT2_ONE = { 1, 1 };
	constexpr SINT2 SINT2_UNIT_X = { 1, 0 };
	constexpr SINT2 SINT2_UNIT_Y = { 0, 1 };

	constexpr SINT3 SINT3_ZERO = { 0, 0, 0 };
	constexpr SINT3 SINT3_ONE = { 1, 1, 1 };
	constexpr SINT3 SINT3_UNIT_X = { 1, 0, 0 };
	constexpr SINT3 SINT3_UNIT_Y = { 0, 1, 0 };
	constexpr SINT3 SINT3_UNIT_Z = { 0, 0, 1 };

	constexpr SINT4 SINT4_ZERO = { 0, 0, 0, 0 };
	constexpr SINT4 SINT4_ONE = { 1, 1, 1, 1 };
	constexpr SINT4 SINT4_UNIT_X = { 1, 0, 0, 0 };
	constexpr SINT4 SINT4_UNIT_Y = { 0, 1, 0, 0 };
	constexpr SINT4 SINT4_UNIT_Z = { 0, 0, 1, 0 };
	constexpr SINT4 SINT4_UNIT_W = { 0, 0, 0, 1 };

	constexpr UINT2 UINT2_ZERO = { 0, 0 };
	constexpr UINT2 UINT2_ONE = { 1, 1 };
	constexpr UINT2 UINT2_UNIT_X = { 1, 0 };
	constexpr UINT2 UINT2_UNIT_Y = { 0, 1 };

	constexpr UINT3 UINT3_ZERO = { 0, 0, 0 };
	constexpr UINT3 UINT3_ONE = { 1, 1, 1 };
	constexpr UINT3 UINT3_UNIT_X = { 1, 0, 0 };
	constexpr UINT3 UINT3_UNIT_Y = { 0, 1, 0 };
	constexpr UINT3 UINT3_UNIT_Z = { 0, 0, 1 };

	constexpr UINT4 UINT4_ZERO = { 0, 0, 0, 0 };
	constexpr UINT4 UINT4_ONE = { 1, 1, 1, 1 };
	constexpr UINT4 UINT4_UNIT_X = { 1, 0, 0, 0 };
	constexpr UINT4 UINT4_UNIT_Y = { 0, 1, 0, 0 };
	constexpr UINT4 UINT4_UNIT_Z = { 0, 0, 1, 0 };
	constexpr UINT4 UINT4_UNIT_W = { 0, 0, 0, 1 };

	constexpr FLOAT2 FLOAT2_ZERO = { 0.0f, 0.0f };
	constexpr FLOAT2 FLOAT2_HALF = { 0.5f, 0.5f };
	constexpr FLOAT2 FLOAT2_ONE = { 1.0f, 1.0f };
	constexpr FLOAT2 FLOAT2_UNIT_X = { 1.0f, 0.0f };
	constexpr FLOAT2 FLOAT2_UNIT_Y = { 0.0f, 1.0f };

	constexpr FLOAT3 FLOAT3_ZERO = { 0.0f, 0.0f, 0.0f };
	constexpr FLOAT3 FLOAT3_HALF = { 0.5f, 0.5f, 0.5f };
	constexpr FLOAT3 FLOAT3_ONE = { 1.0f, 1.0f, 1.0f };
	constexpr FLOAT3 FLOAT3_UNIT_X = { 1.0f, 0.0f, 0.0f };
	constexpr FLOAT3 FLOAT3_UNIT_Y = { 0.0f, 1.0f, 0.0f };
	constexpr FLOAT3 FLOAT3_UNIT_Z = { 0.0f, 0.0f, 1.0f };

	constexpr FLOAT4 FLOAT4_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr FLOAT4 FLOAT4_HALF = { 0.5f, 0.5f, 0.5f, 0.5f };
	constexpr FLOAT4 FLOAT4_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr FLOAT4 FLOAT4_UNIT_X = { 1.0f, 0.0f, 0.0f, 0.0f };
	constexpr FLOAT4 FLOAT4_UNIT_Y = { 0.0f, 1.0f, 0.0f, 0.0f };
	constexpr FLOAT4 FLOAT4_UNIT_Z = { 0.0f, 0.0f, 1.0f, 0.0f };
	constexpr FLOAT4 FLOAT4_UNIT_W = { 0.0f, 0.0f, 0.0f, 1.0f };

	constexpr XFLOAT4 XFLOAT4_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr XFLOAT4 XFLOAT4_HALF = { 0.5f, 0.5f, 0.5f, 0.5f };
	constexpr XFLOAT4 XFLOAT4_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr XFLOAT4 XFLOAT4_UNIT_X = { 1.0f, 0.0f, 0.0f, 0.0f };
	constexpr XFLOAT4 XFLOAT4_UNIT_Y = { 0.0f, 1.0f, 0.0f, 0.0f };
	constexpr XFLOAT4 XFLOAT4_UNIT_Z = { 0.0f, 0.0f, 1.0f, 0.0f };
	constexpr XFLOAT4 XFLOAT4_UNIT_W = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr XFLOAT4 XFLOAT4_IDENTITY = { 0.0f, 0.0f, 0.0f, 1.0f }; // 단위 벡터 (회전 없음)
	constexpr XFLOAT4 XFLOAT4_NEGATIVE_IDENTITY = { 0.0f, 0.0f, 0.0f, -1.0f }; // 반전된 단위 벡터 (180도 회전)
	constexpr XFLOAT4 XFLOAT4_UNITY_XY = { 1.0f, 1.0f, 0.0f, 0.0f }; // XY 평면에서 단위 벡터 (Z와 W는 0)
	constexpr XFLOAT4 XFLOAT4_UNITY_XYZ = { 1.0f, 1.0f, 1.0f, 0.0f }; // XY 평면에서 단위 벡터 (Z와 W는 0)

	constexpr QUAT QUAT_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f }; // 모든 성분이 0인 쿼터니언
	constexpr QUAT QUAT_IDENTITY = { 0.0f, 0.0f, 0.0f, 1.0f }; // 단위 쿼터니언 (회전 없음)

	constexpr MATRIX4 MATRIX4_ZERO = 
	{
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	constexpr MATRIX4 MATRIX4_IDENT = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f 
	};

	constexpr COLOR4F COLOR4F_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };		// 흰색
	constexpr COLOR4F COLOR4F_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };		// 검은색
	constexpr COLOR4F COLOR4F_RED = { 1.0f, 0.0f, 0.0f, 1.0f };			// 빨간색
	constexpr COLOR4F COLOR4F_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };		// 초록색
	constexpr COLOR4F COLOR4F_BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };		// 파란색
	constexpr COLOR4F COLOR4F_YELLOW = { 1.0f, 1.0f, 0.0f, 1.0f };		// 노란색
	constexpr COLOR4F COLOR4F_CYAN = { 0.0f, 1.0f, 1.0f, 1.0f };		// 청록색
	constexpr COLOR4F COLOR4F_MAGENTA = { 1.0f, 0.0f, 1.0f, 1.0f };		// 밝은 보라색 (마젠타)

	constexpr COLOR4U COLOR4U_WHITE = { 255, 255, 255, 255 };			// 흰색
	constexpr COLOR4U COLOR4U_BLACK = { 0, 0, 0, 255 };					// 검은색
	constexpr COLOR4U COLOR4U_RED = { 255, 0, 0, 255 };					// 빨간색
	constexpr COLOR4U COLOR4U_GREEN = { 0, 255, 0, 255 };				// 초록색
	constexpr COLOR4U COLOR4U_BLUE = { 0, 0, 255, 255 };				// 파란색
	constexpr COLOR4U COLOR4U_YELLOW = { 255, 255, 0, 255 };			// 노란색
	constexpr COLOR4U COLOR4U_CYAN = { 0, 255, 255, 255 };				// 청록색
	constexpr COLOR4U COLOR4U_MAGENTA = { 255, 0, 255, 255 };			// 밝은 보라색 (마젠타)

	// ============================================================
	// Function Declarations
	// ============================================================

	// --- Vector Operations (MathOpts.inl) ---
	inline FLOAT2 math_set(float x, float y);
	inline FLOAT3 math_set(float x, float y, float z);
	inline FLOAT4 math_set(float x, float y, float z, float w);
	inline FLOAT2 math_add(FLOAT2 a, FLOAT2 b);
	inline FLOAT3 math_add(FLOAT3 a, FLOAT3 b);
	inline FLOAT4 math_add(FLOAT4 a, FLOAT4 b);
	inline FLOAT2 math_sub(FLOAT2 a, FLOAT2 b);
	inline FLOAT3 math_sub(FLOAT3 a, FLOAT3 b);
	inline FLOAT4 math_sub(FLOAT4 a, FLOAT4 b);
	inline FLOAT2 math_mul(FLOAT2 a, FLOAT2 b);
	inline FLOAT3 math_mul(FLOAT3 a, FLOAT3 b);
	inline FLOAT4 math_mul(FLOAT4 a, FLOAT4 b);
	inline FLOAT2 math_mul(FLOAT2 a, float scalar);
	inline FLOAT3 math_mul(FLOAT3 a, float scalar);
	inline FLOAT4 math_mul(FLOAT4 a, float scalar);
	inline FLOAT2 math_div(FLOAT2 a, FLOAT2 b);
	inline FLOAT3 math_div(FLOAT3 a, FLOAT3 b);
	inline FLOAT4 math_div(FLOAT4 a, FLOAT4 b);
	inline FLOAT2 math_div(FLOAT2 a, float scalar);
	inline FLOAT3 math_div(FLOAT3 a, float scalar);
	inline FLOAT4 math_div(FLOAT4 a, float scalar);
	inline float math_dot(FLOAT2 a, FLOAT2 b);
	inline float math_dot(FLOAT3 a, FLOAT3 b);
	inline float math_dot(FLOAT4 a, FLOAT4 b);
	inline void math_dot(FLOAT2 a, FLOAT2 b, float& out);
	inline void math_dot(FLOAT3 a, FLOAT3 b, float& out);
	inline void math_dot(FLOAT4 a, FLOAT4 b, float& out);
	inline float math_cross(FLOAT2 a, FLOAT2 b) noexcept;
	inline FLOAT3 math_cross(FLOAT3 a, FLOAT3 b) noexcept;
	inline FLOAT4 math_cross(FLOAT4 a, FLOAT4 b, FLOAT4 c) noexcept;
	inline void math_cross(FLOAT2 a, FLOAT2 b, float& out) noexcept;
	inline void math_cross(FLOAT3 a, FLOAT3 b, FLOAT3& out) noexcept;
	inline void math_cross(FLOAT4 a, FLOAT4 b, FLOAT4 c, FLOAT4& out) noexcept;
	inline float math_length(FLOAT2 v);
	inline float math_length(FLOAT3 v);
	inline float math_length(FLOAT4 v);
	inline float math_lengthsq(FLOAT2 v);
	inline float math_lengthsq(FLOAT3 v);
	inline float math_lengthsq(FLOAT4 v);
	inline FLOAT2 math_normalized(FLOAT2 v);
	inline FLOAT3 math_normalized(FLOAT3 v);
	inline FLOAT4 math_normalized(FLOAT4 v);
	inline void math_normalize(FLOAT2& v);
	inline void math_normalize(FLOAT3& v);
	inline void math_normalize(FLOAT4& v);
	inline FLOAT2 math_lerp(FLOAT2 a, FLOAT2 b, float t);
	inline FLOAT3 math_lerp(FLOAT3 a, FLOAT3 b, float t);
	inline FLOAT4 math_lerp(FLOAT4 a, FLOAT4 b, float t);
	inline FLOAT2 math_clamp(FLOAT2 v, FLOAT2 vmin, FLOAT2 vmax);
	inline FLOAT3 math_clamp(FLOAT3 v, FLOAT3 vmin, FLOAT3 vmax);
	inline FLOAT4 math_clamp(FLOAT4 v, FLOAT4 vmin, FLOAT4 vmax);
	inline FLOAT2 math_saturate(FLOAT2 v);
	inline FLOAT3 math_saturate(FLOAT3 v);
	inline FLOAT4 math_saturate(FLOAT4 v);
	inline FLOAT2 math_min(FLOAT2 a, FLOAT2 b);
	inline FLOAT3 math_min(FLOAT3 a, FLOAT3 b);
	inline FLOAT4 math_min(FLOAT4 a, FLOAT4 b);
	inline FLOAT2 math_max(FLOAT2 a, FLOAT2 b);
	inline FLOAT3 math_max(FLOAT3 a, FLOAT3 b);
	inline FLOAT4 math_max(FLOAT4 a, FLOAT4 b);
	inline FLOAT2 math_ceil(FLOAT2 v);
	inline FLOAT3 math_ceil(FLOAT3 v);
	inline FLOAT4 math_ceil(FLOAT4 v);
	inline FLOAT2 math_floor(FLOAT2 v);
	inline FLOAT3 math_floor(FLOAT3 v);
	inline FLOAT4 math_floor(FLOAT4 v);
	inline FLOAT2 math_round(FLOAT2 v);
	inline FLOAT3 math_round(FLOAT3 v);
	inline FLOAT4 math_round(FLOAT4 v);
	inline FLOAT2 math_trunc(FLOAT2 v);
	inline FLOAT3 math_trunc(FLOAT3 v);
	inline FLOAT4 math_trunc(FLOAT4 v);

	// --- Quaternion Operations (MathOpts.inl) ---
	inline QUAT quat_ident();
	inline QUAT quat_mul(QUAT q, float scalar);
	inline QUAT quat_mul(QUAT q1, QUAT q2);
	inline FLOAT3 quat_mul(QUAT q, FLOAT3 v);
	inline QUAT quat_inverse(QUAT q);
	inline QUAT quat_normalize(QUAT q);
	inline void quat_normalized(QUAT& q);
	inline QUAT quat_conjugate(QUAT q);
	inline QUAT quat_slerp(QUAT q0, QUAT q1, float t);
	inline QUAT quat_rotation_roll_pitch_yaw(float roll, float pitch, float yaw);
	inline QUAT quat_rotation_axis_angle(FLOAT3 axis, float angle);
	inline QUAT quat_rotation_lookat(FLOAT3 forward, FLOAT3 up);
	inline QUAT quat_rotation_between(FLOAT3 from, FLOAT3 to);
	inline QUAT quat_from_matrix(const MATRIX4& m) noexcept;
	inline FLOAT3 quat_to_roll_pitch_yaw(QUAT q);
	inline FLOAT4 quat_to_axis_angle(QUAT q);
	inline MATRIX4 quat_to_matrix(QUAT q);
	inline MATRIX4 quat_to_matrix2(QUAT q);
	inline MATRIX4 quat_to_matrix3(QUAT q);

	// --- Matrix Operations (MathOpts_Matrix.inl) ---
	inline bool mat4_is_ident(const MATRIX4& m) noexcept;
	inline bool mat4_is_nan(const MATRIX4& m) noexcept;
	inline bool mat4_is_infinite(const MATRIX4& m) noexcept;
	inline MATRIX4 mat4_trans(const FLOAT3& pos) noexcept;
	inline void mat4_trans(const FLOAT3& pos, MATRIX4& out) noexcept;
	inline FLOAT3 mat4_get_trans(const MATRIX4& m) noexcept;
	inline void mat4_get_trans(const MATRIX4& m, FLOAT3& out) noexcept;
	inline void mat4_trans(float x, float y, float z, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_trans(float x, float y, float z) noexcept;
	inline void mat4_transpose(const MATRIX4& m, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_transpose(const MATRIX4& m) noexcept;
	inline void mat4_inverse(const MATRIX4& m, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_inverse(const MATRIX4& m) noexcept;
	inline void mat4_mul(const MATRIX4& m0, const MATRIX4& m1, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_mul(const MATRIX4& m0, const MATRIX4& m1) noexcept;
	inline void mat4_mul(const MATRIX4& m, const FLOAT3& v, FLOAT3& out) noexcept;
	inline FLOAT3 mat4_mul(const MATRIX4& m, const FLOAT3& v) noexcept;
	inline void mat4_mul(const MATRIX4& m, const FLOAT4& v, FLOAT4& out) noexcept;
	inline FLOAT4 mat4_mul(const MATRIX4& m, const FLOAT4& v) noexcept;
	inline void mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz) noexcept;
	inline MATRIX4 mat4_scale(const FLOAT3& v) noexcept;
	inline void mat4_scale(const FLOAT3& v, MATRIX4& out) noexcept;
	inline void mat4_rotation_x(FLOAT angle, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_rotation_x(FLOAT angle) noexcept;
	inline void mat4_rotation_y(FLOAT angle, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_rotation_y(FLOAT angle) noexcept;
	inline void mat4_rotation_z(FLOAT angle, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_rotation_z(FLOAT angle) noexcept;
	inline void mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll) noexcept;
	inline void mat4_rotation_axis(const FLOAT3& axis, FLOAT angle, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_rotation_axis(const FLOAT3& axis, FLOAT angle) noexcept;
	inline void mat4_rotation_quat(const QUAT& q, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_rotation_quat(const QUAT& q) noexcept;
	inline void mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, FLOAT3& up) noexcept;
	inline void mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up) noexcept;
	inline void mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept;
	inline void mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept;
	inline void mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept;
	inline void mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept;
	inline void mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept;
	inline void mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
	inline MATRIX4 mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept;

	// --- Utility Functions (MathOpts_Utils.inl) ---
	inline float repeat(float t, float length) noexcept;
	inline FLOAT2 repeat(FLOAT2 t, FLOAT2 length) noexcept;
	inline FLOAT3 repeat(FLOAT3 t, FLOAT3 length) noexcept;
	inline float delta_angle(float current, float target) noexcept;

	inline FLOAT2 move_towards(FLOAT2 current, FLOAT2 target, float max_delta) noexcept;
	inline FLOAT3 move_towards(FLOAT3 current, FLOAT3 target, float max_delta) noexcept;
	inline FLOAT4 move_towards(FLOAT4 current, FLOAT4 target, float max_delta) noexcept;

	inline FLOAT2 rotate_towards(FLOAT2 current, FLOAT2 target, float max_angle) noexcept;
	inline FLOAT3 rotate_towards(FLOAT3 current, FLOAT3 target, float max_angle) noexcept;

	inline QUAT forward_to_quat(FLOAT2 forward) noexcept;
	inline QUAT forward_to_quat(FLOAT3 forward, FLOAT3 up = FLOAT3_UNIT_Y) noexcept;

	// --- SSE Quaternion Operations (MathOpts_SSE.inl) ---
	inline void __vectorcall xquat_mul(XQUAT q1, XQUAT q2, XQUAT& out) noexcept;
	inline XQUAT __vectorcall xquat_mul(XQUAT q1, XQUAT q2) noexcept;
	inline void __vectorcall xquat_mul(XQUAT q, XFLOAT3 v, XFLOAT3& out) noexcept;
	inline XFLOAT3 __vectorcall xquat_mul(XQUAT q, XFLOAT3 v) noexcept;
	inline XQUAT __vectorcall xquat_inverse(XQUAT q) noexcept;
	inline XQUAT __vectorcall xquat_normalize(XQUAT q) noexcept;
	inline XQUAT __vectorcall xquat_conjugate(XQUAT q) noexcept;
	inline XQUAT __vectorcall xquat_slerp(XQUAT q0, XQUAT q1, float t) noexcept;
	inline XQUAT __vectorcall xquat_rotation_roll_pitch_yaw(float roll, float pitch, float yaw) noexcept;
	inline XQUAT __vectorcall xquat_rotation_axis_angle(XFLOAT3 axis, float angle) noexcept;
	inline XQUAT __vectorcall xquat_rotation_matrix(const XMATRIX4& m) noexcept;
	inline void __vectorcall xquat_to_matrix(XQUAT q, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xquat_to_matrix(XQUAT q) noexcept;

	// --- SSE Matrix Operations (MathOpts_SSE.inl) ---
	inline void __vectorcall xmat4_zero(XMATRIX4& m) noexcept;
	inline void __vectorcall xmat4_ident(XMATRIX4& m) noexcept;
	inline void __vectorcall xmat4_trans(float x, float y, float z, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_trans(float x, float y, float z) noexcept;
	inline void __vectorcall xmat4_trans(XFLOAT3 v, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_trans(XFLOAT3 v) noexcept;
	inline XFLOAT3 __vectorcall xmat4_get_trans(const XMATRIX4& m) noexcept;
	inline void __vectorcall xmat4_transpose(const XMATRIX4& m, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_transpose(const XMATRIX4& m) noexcept;
	inline void __vectorcall xmat4_inverse(const XMATRIX4& m, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_inverse(const XMATRIX4& m) noexcept;
	inline void __vectorcall xmat4_mul(const XMATRIX4& m0, const XMATRIX4& m1, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_mul(const XMATRIX4& m0, const XMATRIX4& m1) noexcept;
	inline void __vectorcall xmat4_mul(const XMATRIX4& m, const XFLOAT3& v, XFLOAT3& out) noexcept;
	inline void __vectorcall xmat4_mul(const XMATRIX4& m, const XFLOAT4& v, XFLOAT4& out) noexcept;
	inline void __vectorcall xmat4_scale(float sx, float sy, float sz, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_scale(float sx, float sy, float sz) noexcept;
	inline void __vectorcall xmat4_scale(XFLOAT3 v, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_scale(XFLOAT3 v) noexcept;
	inline void __vectorcall xmat4_rotation_x(float angle, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_rotation_x(float angle) noexcept;
	inline void __vectorcall xmat4_rotation_y(float angle, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_rotation_y(float angle) noexcept;
	inline void __vectorcall xmat4_rotation_z(float angle, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_rotation_z(float angle) noexcept;
	inline void __vectorcall xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll) noexcept;
	inline void __vectorcall xmat4_rotation_axis(XFLOAT3 axis, float angle, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_rotation_axis(XFLOAT3 axis, float angle) noexcept;
	inline void __vectorcall xmat4_lookto_lh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_lookto_lh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up) noexcept;
	inline void __vectorcall xmat4_lookto_rh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_lookto_rh(XFLOAT3 eye, XFLOAT3 to, XFLOAT3 up) noexcept;
	inline void __vectorcall xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_perspective_lh(float fov, float aspect, float nearZ, float farZ) noexcept;
	inline void __vectorcall xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_perspective_rh(float fov, float aspect, float nearZ, float farZ) noexcept;
	inline void __vectorcall xmat4_ortho_lh(float width, float height, float nearZ, float farZ, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_ortho_lh(float width, float height, float nearZ, float farZ) noexcept;
	inline void __vectorcall xmat4_ortho_rh(float width, float height, float nearZ, float farZ, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_ortho_rh(float width, float height, float nearZ, float farZ) noexcept;
	inline void __vectorcall xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept;
	inline void __vectorcall xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ, XMATRIX4& out) noexcept;
	inline XMATRIX4 __vectorcall xmat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept;
	
}


#include "mathopts_format.inl"
#include "mathopts.inl"
#include "mathopts_matrix.inl"
#include "mathopts_sse.inl"
#include "mathopts_utils.inl"
