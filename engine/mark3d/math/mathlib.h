#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include <math.h>
#include <stdint.h>


#define TMIN(a, b) a < b ? a : b; // 
#define TMAX(a, b) a > b ? a : b;
#define TABS(a) a < 0 ? -a : a;
#define TMIN3(a, b, c) TMIN(TMIN(a, b), c)
#define TMAX3(a, b, c) TMAX(TMAX(a, b), c)
#define TMIN4(a, b, c, d) TMIN3(TMIN(a, b), c, d)
#define TMAX4(a, b, c, d) TMAX3(TMAX(a, b), c, d)
#define TCLAMP(a, min, max) TMAX(min, TMIN(a, max))
#define TSATURATE(a) TMAX(0.0f, TMIN(a, 1.0f))
#define TINY_NUMBER 0.0001f
#define TEPSILON 0.000001f

static constexpr float M_PI = 3.14159265358979323846f;
static constexpr float M_PI_2 = 1.57079632679489661923f;


struct INT2
{
	union
	{
		struct { int x, y; };
		int v[2];
	};
};

struct UINT4
{
	union
	{
		struct { unsigned int x, y, z, w; };
		unsigned int v[4];
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

struct COLOR_VAULE
{
	union
	{
		struct { float r, g, b, a; };
		float c[4];
		unsigned int rgba;
	};
};

struct MATRIX4
{
	union
	{
		float row[4][4];

		struct
		{
			float row0[4];
			float row1[4];
			float row2[4];
			float row3[4];
		};

		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};

		float m[16];
	};
};

struct LOCAL_TRANSFORM
{
	FLOAT4 Position;
	QUAT Rotation;
	FLOAT4 Scale;

	MATRIX4 TM;
};

typedef FLOAT4 COLOR4;
typedef FLOAT4 QUATERNION;

constexpr FLOAT M_2PI = 6.283185307f;
constexpr FLOAT M_1DIVPI = 0.318309886f;
constexpr FLOAT M_1DIV2PI = 0.159154943f;
constexpr FLOAT M_PIDIV2 = 1.570796327f;
constexpr FLOAT M_PIDIV4 = 0.785398163f;

constexpr FLOAT4 COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
constexpr FLOAT4 COLOR_BLACK = { 1.0f, 1.0f, 1.0f, 1.0f };
constexpr FLOAT4 COLOR_RED = { 1.0f, 0.0f, 0.0f, 1.0f };
constexpr FLOAT4 COLOR_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
constexpr FLOAT4 COLOR_BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };

constexpr FLOAT2 FLOAT2_ZERO = { 0.0f, 0.0f };
constexpr FLOAT3 FLOAT3_ZERO = { 0.0f, 0.0f, 0.0f };
constexpr FLOAT4 FLOAT4_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
constexpr FLOAT2 FLOAT2_ONE = { 1.0f, 1.0f };
constexpr FLOAT3 FLOAT3_ONE = { 1.0f, 1.0f, 1.0f };
constexpr FLOAT4 FLOAT4_ONE = { 1.0f, 1.0f, 1.0f, 1.0f };

constexpr FLOAT2 FLOAT2_UNIT_X = { 1.0f, 0.0f };
constexpr FLOAT2 FLOAT2_UNIT_Y = { 0.0f, 1.0f };
constexpr FLOAT3 FLOAT3_UNIT_X = { 1.0f, 0.0f, 0.0f };
constexpr FLOAT3 FLOAT3_UNIT_Y = { 0.0f, 1.0f, 0.0f };
constexpr FLOAT3 FLOAT3_UNIT_Z = { 0.0f, 0.0f, 1.0f };
constexpr FLOAT4 FLOAT4_UNIT_X = { 1.0f, 0.0f, 0.0f, 0.0f };
constexpr FLOAT4 FLOAT4_UNIT_Y = { 0.0f, 1.0f, 0.0f, 0.0f };
constexpr FLOAT4 FLOAT4_UNIT_Z = { 0.0f, 0.0f, 1.0f, 0.0f };
constexpr FLOAT4 FLOAT4_UNIT_W = { 0.0f, 0.0f, 0.0f, 1.0f };
constexpr QUATERNION QUAT_IDENT = { 0.0f, 0.0f, 0.0f, 1.0f };
constexpr MATRIX4 MATRIX4_IDENT = { 1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f };

inline INT2 int2_zero();
inline void int2_zero(INT2& v) noexcept;
inline INT2 int2_set(int x, int y);
inline void int2_set(int x, int y, INT2& out) noexcept;
inline INT2 int2_add(const INT2& v0, const INT2& v1) noexcept;
inline void int2_add(const INT2& v0, const INT2& v1, INT2& out) noexcept;
inline INT2 int2_add(const INT2& v0, INT i) noexcept;
inline void int2_add(const INT2& v0, INT i, INT2& out) noexcept;
inline INT2 int2_sub(const INT2& v0, const INT2& v1) noexcept;
inline void int2_sub(const INT2& v0, const INT2& v1, INT2& out) noexcept;
inline INT2 int2_sub(const INT2& v0, INT i) noexcept;
inline void int2_sub(const INT2& v0, INT i, INT2& out) noexcept;
inline INT2 int2_mul(const INT2& v0, const INT2& v1) noexcept;
inline void int2_mul(const INT2& v0, const INT2& v1, INT2& out) noexcept;
inline INT2 int2_mul(const INT2& v, int i) noexcept;
inline void int2_mul(const INT2& v, int i, INT2& out) noexcept;
inline INT2 int2_div(const INT2& v0, const INT2& v1) noexcept;
inline void int2_div(const INT2& v0, const INT2& v1, INT2& out) noexcept;
inline INT2 int2_div(const INT2& v, int i) noexcept;
inline void int2_div(const INT2& v, int i, INT2& out) noexcept;

inline FLOAT2 vec2_zero() noexcept;
inline void vec2_zero(FLOAT2& v) noexcept;
inline FLOAT2 vec2_unit_x() noexcept;
inline FLOAT2 vec2_unit_y() noexcept;
inline FLOAT2 vec2_set(FLOAT x, FLOAT y) noexcept;
inline void vec2_set(FLOAT x, FLOAT y, FLOAT2& out) noexcept;
inline FLOAT2 vec2_add(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_add(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_sub(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_sub(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_mul(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_mul(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_mul(const FLOAT2& v, FLOAT f) noexcept;
inline void vec2_mul(const FLOAT2& v, FLOAT f, FLOAT2& out) noexcept;
inline FLOAT2 vec2_div(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_div(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_div(const FLOAT2& v, FLOAT f) noexcept;
inline void vec2_div(const FLOAT2& v, FLOAT f, FLOAT2& out) noexcept;
inline FLOAT2 vec2_dot(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_dot(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_cross(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_cross(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_lengthsq(const FLOAT2& v) noexcept;
inline void vec2_lengthsq(const FLOAT2& v, FLOAT2& out) noexcept;
inline FLOAT2 vec2_length(const FLOAT2& v) noexcept;
inline void vec2_length(const FLOAT2& v, FLOAT2& out) noexcept;
inline void vec2_normalize(FLOAT2& v) noexcept;
inline FLOAT2 vec2_normalized(const FLOAT2& v) noexcept;
inline void vec2_normalized(const FLOAT2& v, FLOAT2& out) noexcept;
inline FLOAT2 vec2_lerp(const FLOAT2& v0, const FLOAT2& v1, FLOAT t) noexcept;
inline void vec2_lerp(const FLOAT2& v0, const FLOAT2& v1, FLOAT t, FLOAT2& out) noexcept;
inline FLOAT2 vec2_clamp(const FLOAT2& v, const FLOAT2& vmin, const FLOAT2& vmax) noexcept;
inline void vec2_clamp(const FLOAT2& v, const FLOAT2& vmin, const FLOAT2& vmax, FLOAT2& out) noexcept;
inline FLOAT2 vec2_saturate(const FLOAT2& v) noexcept;
inline void vec2_saturate(const FLOAT2& v, FLOAT2& out) noexcept;
inline FLOAT2 vec2_min(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_min(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_max(const FLOAT2& v0, const FLOAT2& v1) noexcept;
inline void vec2_max(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept;
inline FLOAT2 vec2_ceil(const FLOAT2& v) noexcept; // 올림
inline void vec2_ceil(const FLOAT2& v, FLOAT2& out) noexcept;
inline FLOAT2 vec2_floor(const FLOAT2& v) noexcept; // 내림
inline void vec2_floor(const FLOAT2& v, FLOAT2& out) noexcept;
inline FLOAT2 vec2_round(const FLOAT2& v) noexcept; // 반올림
inline void vec2_round(const FLOAT2& v, FLOAT2& out) noexcept;
inline FLOAT2 vec2_trunc(const FLOAT2& v) noexcept; // 버림
inline void vec2_trunc(const FLOAT2& v, FLOAT2& out) noexcept;


inline FLOAT3 vec3_zero() noexcept;
inline void vec3_zero(FLOAT3& v) noexcept;
inline FLOAT3 vec3_unit_x() noexcept;
inline FLOAT3 vec3_unit_y() noexcept;
inline FLOAT3 vec3_unit_z() noexcept;
inline FLOAT3 vec3_set(FLOAT x, FLOAT y) noexcept;
inline FLOAT3 vec3_set(FLOAT x, FLOAT y, FLOAT z) noexcept;
inline void vec3_set(FLOAT x, FLOAT y, FLOAT3& out) noexcept;
inline void vec3_set(FLOAT x, FLOAT y, FLOAT z, FLOAT3& out) noexcept;
inline FLOAT3 vec3_add(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_add(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_sub(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_sub(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_mul(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_mul(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_mul(const FLOAT3& v, FLOAT f) noexcept;
inline void vec3_mul(const FLOAT3& v, FLOAT f, FLOAT3& out) noexcept;
inline FLOAT3 vec3_div(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_div(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_div(const FLOAT3& v, FLOAT f) noexcept;
inline void vec3_div(const FLOAT3& v, FLOAT f, FLOAT3& out) noexcept;
inline float vec3_dot_f(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline FLOAT3 vec3_dot(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_dot(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_cross(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_cross(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_lengthsq(const FLOAT3& v) noexcept;
inline void vec3_lengthsq(const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT3 vec3_length(const FLOAT3& v) noexcept;
inline void vec3_length(const FLOAT3& v, FLOAT3& out) noexcept;
inline void vec3_normalize(FLOAT3& v) noexcept;
inline FLOAT3 vec3_normalized(const FLOAT3& v) noexcept;
inline void vec3_normalized(const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT3 vec3_lerp(const FLOAT3& v0, const FLOAT3& v1, FLOAT t) noexcept;
inline void vec3_lerp(const FLOAT3& v0, const FLOAT3& v1, FLOAT t, FLOAT3& out) noexcept;
inline FLOAT3 vec3_clamp(const FLOAT3& v, const FLOAT3& vmin, const FLOAT3& vmax) noexcept;
inline void vec3_clamp(const FLOAT3& v, const FLOAT3& vmin, const FLOAT3& vmax, FLOAT3& out) noexcept;
inline FLOAT3 vec3_saturate(const FLOAT3& v) noexcept;
inline void vec3_saturate(const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT3 vec3_min(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_min(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_max(const FLOAT3& v0, const FLOAT3& v1) noexcept;
inline void vec3_max(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept;
inline FLOAT3 vec3_ceil(const FLOAT3& v) noexcept; // 올림
inline void vec3_ceil(const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT3 vec3_floor(const FLOAT3& v) noexcept; // 내림
inline void vec3_floor(const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT3 vec3_round(const FLOAT3& v) noexcept; // 반올림
inline void vec3_round(const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT3 vec3_trunc(const FLOAT3& v) noexcept; // 버림
inline void vec3_trunc(const FLOAT3& v, FLOAT3& out) noexcept;

inline FLOAT4 vec4_zero() noexcept;
inline void vec4_zero(FLOAT4& v) noexcept;
inline FLOAT4 vec4_unit_x() noexcept;
inline FLOAT4 vec4_unit_y() noexcept;
inline FLOAT4 vec4_unit_z() noexcept;
inline FLOAT4 vec4_unit_w() noexcept;
inline FLOAT4 vec4_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w) noexcept;
inline FLOAT4 vec4_set(FLOAT x, FLOAT y, FLOAT z) noexcept;
inline void vec4_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w, FLOAT4& out) noexcept;
inline FLOAT4 vec4_add(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_add(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_sub(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_sub(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_mul(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_mul(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_mul(const FLOAT4& v, FLOAT f) noexcept;
inline void vec4_mul(const FLOAT4& v, FLOAT f, FLOAT4& out) noexcept;
inline FLOAT4 vec4_div(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_div(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_div(const FLOAT4& v, FLOAT f) noexcept;
inline void vec4_div(const FLOAT4& v, FLOAT f, FLOAT4& out) noexcept;
inline FLOAT4 vec4_dot(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_dot(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_cross(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_cross(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_lengthsq(const FLOAT4& v) noexcept;
inline void vec4_lengthsq(const FLOAT4& v, FLOAT4& out) noexcept;
inline FLOAT4 vec4_length(const FLOAT4& v) noexcept;
inline void vec4_length(const FLOAT4& v, FLOAT4& out) noexcept;
inline void vec4_normalize(FLOAT4& v) noexcept;
inline FLOAT4 vec4_normalized(const FLOAT4& v) noexcept;
inline void vec4_normalized(const FLOAT4& v, FLOAT4& out) noexcept;
inline FLOAT4 vec4_lerp(const FLOAT4& v0, const FLOAT4& v1, float t) noexcept;
inline void vec4_lerp(const FLOAT4& v0, const FLOAT4& v1, float t, FLOAT4& out) noexcept;
inline FLOAT4 vec4_clamp(const FLOAT4& v, const FLOAT4& vmin, const FLOAT4& vmax) noexcept;
inline void vec4_clamp(const FLOAT4& v, const FLOAT4& vmin, const FLOAT4& vmax, FLOAT4& out) noexcept;
inline FLOAT4 vec4_saturate(const FLOAT4& v) noexcept;
inline void vec4_saturate(const FLOAT4& v, FLOAT4& out) noexcept;
inline FLOAT4 vec4_min(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_min(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_max(const FLOAT4& v0, const FLOAT4& v1) noexcept;
inline void vec4_max(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept;
inline FLOAT4 vec4_ceil(const FLOAT4& v) noexcept; // 올림
inline void vec4_ceil(const FLOAT4& v, FLOAT4& out) noexcept;
inline FLOAT4 vec4_floor(const FLOAT4& v) noexcept; // 내림
inline void vec4_floor(const FLOAT4& v, FLOAT4& out) noexcept;
inline FLOAT4 vec4_round(const FLOAT4& v) noexcept; // 반올림
inline void vec4_round(const FLOAT4& v, FLOAT4& out) noexcept;
inline FLOAT4 vec4_trunc(const FLOAT4& v) noexcept; // 버림
inline void vec4_trunc(const FLOAT4& v, FLOAT4& out) noexcept;


inline QUAT quat_zero() noexcept;
inline void quat_zero(QUAT& q) noexcept;
inline QUAT quat_ident() noexcept;
inline void quat_ident(QUAT& q) noexcept;
inline QUAT quat_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w) noexcept;
inline void quat_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w, QUAT& out) noexcept;
inline QUAT quat_mul(const QUAT& q0, const QUAT& q1) noexcept;
inline void quat_mul(const QUAT& q0, const QUAT& q1, QUAT& out) noexcept;
inline QUAT quat_mul(const QUAT& q, FLOAT f) noexcept;
inline void quat_mul(const QUAT& q, FLOAT f, QUAT& out) noexcept;
inline FLOAT3 quat_mul(const QUAT& q, const FLOAT3& v) noexcept;
inline void quat_mul(const QUAT& q, const FLOAT3& v, FLOAT3& out) noexcept;
inline QUAT quat_inverse(const QUAT& q) noexcept;
inline void quat_inverse(const QUAT& q, QUAT& out) noexcept;
inline QUAT quat_conjugate(const QUAT& q) noexcept;
inline void quat_conjugate(const QUAT& q, QUAT& out) noexcept;
inline QUAT quat_slerp(const QUAT& q0, const QUAT& q1, FLOAT t) noexcept;
inline void quat_slerp(const QUAT& q0, const QUAT& q1, FLOAT t, QUAT& out) noexcept;
inline QUAT quat_rotation_roll_pitch_yaw(FLOAT roll, FLOAT pitch, FLOAT yaw) noexcept;
inline void quat_rotation_roll_pitch_yaw(FLOAT roll, FLOAT pitch, FLOAT yaw, QUAT& out) noexcept;
inline QUAT quat_rotation_roll_pitch_yaw(const FLOAT3& v) noexcept;
inline void quat_rotation_roll_pitch_yaw(const FLOAT3& v, QUAT& out) noexcept;
inline QUAT quat_rotation_axis(FLOAT angle, const FLOAT3& v) noexcept;
inline void quat_rotation_axis(FLOAT angle, const FLOAT3& v, QUAT& out) noexcept;
inline QUAT quat_rotation_axis(FLOAT angle, FLOAT x, FLOAT y, FLOAT z) noexcept;
inline void quat_rotation_axis(FLOAT angle, FLOAT x, FLOAT y, FLOAT z, QUAT& out) noexcept;
inline QUAT quat_rotation_matrix(const MATRIX4& m) noexcept;
inline void quat_rotation_matrix(const MATRIX4& m, QUAT& out) noexcept;
inline QUAT quat_from_lookat(const FLOAT3& forward) noexcept;
inline void quat_from_lookat(const FLOAT3& forward, QUAT& out) noexcept;
inline QUAT quat_from_lookat(const FLOAT3& forward, const FLOAT3& up) noexcept;
inline void quat_from_lookat(const FLOAT3& forward, const FLOAT3& up, QUAT& out) noexcept;
inline FLOAT3 quat_to_roll_pitch_yaw(const QUAT& q) noexcept;
inline void quat_to_roll_pitch_yaw(const QUAT& q, FLOAT3& out) noexcept;
inline FLOAT4 quat_to_axis_angle(const QUAT& q) noexcept;
inline void quat_to_axis_angle(const QUAT& q, FLOAT3& axis, FLOAT& angle) noexcept;
inline MATRIX4 quat_to_matrix(const QUAT& q) noexcept;
inline void quat_to_matrix(const QUAT& q, MATRIX4& out) noexcept;

inline MATRIX4 mat4_zero() noexcept;
inline void mat4_zero(MATRIX4& m) noexcept;
inline MATRIX4 mat4_ident() noexcept;
inline void mat4_ident(MATRIX4& m) noexcept;
inline bool mat4_is_ident(const MATRIX4& m) noexcept;
inline bool mat4_is_nan(const MATRIX4& m) noexcept;
inline bool mat4_is_infinite(const MATRIX4& m) noexcept;
inline MATRIX4 mat4_set(
	FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
	FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
	FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
	FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33) noexcept;
inline void mat4_set(
	FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
	FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
	FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
	FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33,
	MATRIX4& out) noexcept;
inline MATRIX4 mat4_trans(const FLOAT3& pos) noexcept;
inline void mat4_trans(const FLOAT3& pos, MATRIX4& out) noexcept;
inline MATRIX4 mat4_trans(FLOAT x, FLOAT y, FLOAT z) noexcept;
inline void mat4_trans(FLOAT x, FLOAT y, FLOAT z, MATRIX4& out) noexcept;
inline FLOAT3 mat4_get_trans(const MATRIX4& m) noexcept;
inline void mat4_get_trans(const MATRIX4& m, FLOAT3& out) noexcept;
inline MATRIX4 mat4_transpose(MATRIX4& m) noexcept;
inline MATRIX4 mat4_transpose(const MATRIX4& m) noexcept;
inline void mat4_transpose(const MATRIX4& m, MATRIX4& out) noexcept;
inline MATRIX4 mat4_inverse(const MATRIX4& m) noexcept;
inline void mat4_inverse(const MATRIX4& m, MATRIX4& out) noexcept;
inline MATRIX4 mat4_mul(const MATRIX4& m0, const MATRIX4& m1) noexcept;
inline void mat4_mul(const MATRIX4& m0, const MATRIX4& m1, MATRIX4& out) noexcept;
inline FLOAT3 mat4_mul(const MATRIX4& m, const FLOAT3& v) noexcept;
inline void mat4_mul(const MATRIX4& m, const FLOAT3& v, FLOAT3& out) noexcept;
inline FLOAT4 mat4_mul(const MATRIX4& m, const FLOAT4& v) noexcept;
inline void mat4_mul(const MATRIX4& m, const FLOAT4& v, FLOAT4& out) noexcept;
inline MATRIX4 mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz) noexcept;
inline void mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz, MATRIX4& out) noexcept;
inline MATRIX4 mat4_scale(const FLOAT3& v) noexcept;
inline void mat4_scale(const FLOAT3& v, MATRIX4& out) noexcept;
inline MATRIX4 mat4_rotation_x(FLOAT angle) noexcept;
inline void mat4_rotation_x(FLOAT angle, MATRIX4& out) noexcept;
inline MATRIX4 mat4_rotation_y(FLOAT angle) noexcept;
inline void mat4_rotation_y(FLOAT angle, MATRIX4& out) noexcept;
inline MATRIX4 mat4_rotation_z(FLOAT angle) noexcept;
inline void mat4_rotation_z(FLOAT angle, MATRIX4& out) noexcept;
inline MATRIX4 mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll) noexcept;
inline void mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll, MATRIX4& out) noexcept;
inline MATRIX4 mat4_rotation_axis(const FLOAT3& axis, FLOAT angle) noexcept;
inline void mat4_rotation_axis(const FLOAT3& axis, FLOAT angle, MATRIX4& out) noexcept;
inline MATRIX4 mat4_rotation_quat(const QUAT& q) noexcept;
inline void mat4_rotation_quat(const QUAT& q, MATRIX4& out) noexcept;
inline MATRIX4 mat4_lookat_lh(const FLOAT3& eye, const FLOAT3& dir, const FLOAT3& up) noexcept;
inline void mat4_lookat_lh(const FLOAT3& eye, const FLOAT3& dir, const FLOAT3& up, MATRIX4& out) noexcept;
inline MATRIX4 mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up) noexcept;
inline void mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept;
inline MATRIX4 mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up) noexcept;
inline void mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept;
inline MATRIX4 mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
inline MATRIX4 mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
inline MATRIX4 mat4_perspective_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_perspective_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
inline MATRIX4 mat4_perspective_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_perspective_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;

inline MATRIX4 mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
inline MATRIX4 mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;

inline MATRIX4 mat4_ortho_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_ortho_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
inline MATRIX4 mat4_ortho_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_ortho_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;

inline MATRIX4 mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;
inline MATRIX4 mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept;
inline void mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept;

inline void compute_transform(LOCAL_TRANSFORM& tf);


#include "mathlib.inl"

#endif // __MATHLIB_H__
