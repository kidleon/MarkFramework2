#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include <math.h>


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
inline void int2_zero(INT2* v);
inline INT2 int2_set(int x, int y);
inline void int2_set(int x, int y, INT2* out);
inline INT2 int2_add(INT2* v0, INT2* v1);
inline void int2_add(INT2* v0, INT2* v1, INT2* out);
inline INT2 int2_add(INT2* v0, INT i);
inline void int2_add(INT2* v0, INT i, INT2* out);
inline INT2 int2_sub(INT2* v0, INT2* v1);
inline void int2_sub(INT2* v0, INT2* v1, INT2* out);
inline INT2 int2_sub(INT2* v0, INT i);
inline void int2_sub(INT2* v0, INT i, INT2* out);
inline INT2 int2_mul(INT2* v0, INT2* v1);
inline void int2_mul(INT2* v0, INT2* v1, INT2* out);
inline INT2 int2_mul(INT2* v, int i);
inline void int2_mul(INT2* v, int i, INT2* out);
inline INT2 int2_div(INT2* v0, INT2* v1);
inline void int2_div(INT2* v0, INT2* v1, INT2* out);
inline INT2 int2_div(INT2* v, int i);
inline void int2_div(INT2* v, int i, INT2* out);

inline FLOAT2 vec2_zero();
inline void vec2_zero(FLOAT2* v);
inline FLOAT2 vec2_unit_x();
inline FLOAT2 vec2_unit_y();
inline FLOAT2 vec2_set(float x, float y);
inline void vec2_set(float x, float y, FLOAT2* out);
inline FLOAT2 vec2_add(FLOAT2* v0, FLOAT2* v1);
inline void vec2_add(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_sub(FLOAT2* v0, FLOAT2* v1);
inline void vec2_sub(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_mul(FLOAT2* v0, FLOAT2* v1);
inline void vec2_mul(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_mul(FLOAT2* v, float f);
inline void vec2_mul(FLOAT2* v, float f, FLOAT2* out);
inline FLOAT2 vec2_div(FLOAT2* v0, FLOAT2* v1);
inline void vec2_div(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_div(FLOAT2* v, float f);
inline void vec2_div(FLOAT2* v, float f, FLOAT2* out);
inline FLOAT2 vec2_dot(FLOAT2* v0, FLOAT2* v1);
inline void vec2_dot(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_cross(FLOAT2* v0, FLOAT2* v1);
inline void vec2_cross(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_lengthsq(FLOAT2* v);
inline void vec2_lengthsq(FLOAT2* v, FLOAT2* out);
inline FLOAT2 vec2_length(FLOAT2* v);
inline void vec2_length(FLOAT2* v, FLOAT2* out);
inline void vec2_normalize(FLOAT2* v);
inline FLOAT2 vec2_normalized(FLOAT2* v);
inline void vec2_normalized(FLOAT2* v, FLOAT2* out);
inline FLOAT2 vec2_lerp(FLOAT2* v0, FLOAT2* v1, float t);
inline void vec2_lerp(FLOAT2* v0, FLOAT2* v1, float t, FLOAT2* out);
inline FLOAT2 vec2_clamp(FLOAT2* v, FLOAT2* vmin, FLOAT2* vmax);
inline void vec2_clamp(FLOAT2* v, FLOAT2* vmin, FLOAT2* vmax, FLOAT2* out);
inline FLOAT2 vec2_saturate(FLOAT2* v);
inline void vec2_saturate(FLOAT2* v, FLOAT2* out);
inline FLOAT2 vec2_min(FLOAT2* v0, FLOAT2* v1);
inline void vec2_min(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_max(FLOAT2* v0, FLOAT2* v1);
inline void vec2_max(FLOAT2* v0, FLOAT2* v1, FLOAT2* out);
inline FLOAT2 vec2_ceil(FLOAT2* v); // 올림
inline void vec2_ceil(FLOAT2* v, FLOAT2* out);
inline FLOAT2 vec2_floor(FLOAT2* v); // 내림
inline void vec2_floor(FLOAT2* v, FLOAT2* out);
inline FLOAT2 vec2_round(FLOAT2* v); // 반올림
inline void vec2_round(FLOAT2* v, FLOAT2* out);
inline FLOAT2 vec2_trunc(FLOAT2* v); // 버림
inline void vec2_trunc(FLOAT2* v, FLOAT2* out);


inline FLOAT3 vec3_zero();
inline void vec3_zero(FLOAT3* v);
inline FLOAT3 vec3_unit_x();
inline FLOAT3 vec3_unit_y();
inline FLOAT3 vec3_unit_z();
inline FLOAT3 vec3_set(float x, float y);
inline FLOAT3 vec3_set(float x, float y, float z);
inline void vec3_set(float x, float y, FLOAT3* out);
inline void vec3_set(float x, float y, float z, FLOAT3* out);
inline FLOAT3 vec3_add(FLOAT3* v0, FLOAT3* v1);
inline void vec3_add(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_sub(FLOAT3* v0, FLOAT3* v1);
inline void vec3_sub(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_mul(FLOAT3* v0, FLOAT3* v1);
inline void vec3_mul(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_mul(FLOAT3* v, float f);
inline void vec3_mul(FLOAT3* v, float f, FLOAT3* out);
inline FLOAT3 vec3_div(FLOAT3* v0, FLOAT3* v1);
inline void vec3_div(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_div(FLOAT3* v, float f);
inline void vec3_div(FLOAT3* v, float f, FLOAT3* out);
inline float vec3_dot_f(FLOAT3* v0, FLOAT3* v1);
inline FLOAT3 vec3_dot(FLOAT3* v0, FLOAT3* v1);
inline void vec3_dot(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_cross(FLOAT3* v0, FLOAT3* v1);
inline void vec3_cross(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_lengthsq(FLOAT3* v);
inline void vec3_lengthsq(FLOAT3* v, FLOAT3* out);
inline FLOAT3 vec3_length(FLOAT3* v);
inline void vec3_length(FLOAT3* v, FLOAT3* out);
inline void vec3_normalize(FLOAT3* v);
inline FLOAT3 vec3_normalized(FLOAT3* v);
inline void vec3_normalized(FLOAT3* v, FLOAT3* out);
inline FLOAT3 vec3_lerp(FLOAT3* v0, FLOAT3* v1, float t);
inline void vec3_lerp(FLOAT3* v0, FLOAT3* v1, float t, FLOAT3* out);
inline FLOAT3 vec3_clamp(FLOAT3* v, FLOAT3* vmin, FLOAT3* vmax);
inline void vec3_clamp(FLOAT3* v, FLOAT3* vmin, FLOAT3* vmax, FLOAT3* out);
inline FLOAT3 vec3_saturate(FLOAT3* v);
inline void vec3_saturate(FLOAT3* v, FLOAT3* out);
inline FLOAT3 vec3_min(FLOAT3* v0, FLOAT3* v1);
inline void vec3_min(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_max(FLOAT3* v0, FLOAT3* v1);
inline void vec3_max(FLOAT3* v0, FLOAT3* v1, FLOAT3* out);
inline FLOAT3 vec3_ceil(FLOAT3* v); // 올림
inline void vec3_ceil(FLOAT3* v, FLOAT3* out);
inline FLOAT3 vec3_floor(FLOAT3* v); // 내림
inline void vec3_floor(FLOAT3* v, FLOAT3* out);
inline FLOAT3 vec3_round(FLOAT3* v); // 반올림
inline void vec3_round(FLOAT3* v, FLOAT3* out);
inline FLOAT3 vec3_trunc(FLOAT3* v); // 버림
inline void vec3_trunc(FLOAT3* v, FLOAT3* out);

inline FLOAT4 vec4_zero();
inline void vec4_zero(FLOAT4* v);
inline FLOAT4 vec4_unit_x();
inline FLOAT4 vec4_unit_y();
inline FLOAT4 vec4_unit_z();
inline FLOAT4 vec4_unit_w();
inline FLOAT4 vec4_set(float x, float y, float z, float w);
inline FLOAT4 vec4_set(float x, float y, float z);
inline void vec4_set(float x, float y, float z, float w, FLOAT4* out);
inline void vec4_set(float x, float y, float z, FLOAT4* out);
inline FLOAT4 vec4_add(FLOAT4* v0, FLOAT4* v1);
inline void vec4_add(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_sub(FLOAT4* v0, FLOAT4* v1);
inline void vec4_sub(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_mul(FLOAT4* v0, FLOAT4* v1);
inline void vec4_mul(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_mul(FLOAT4* v, float f);
inline void vec4_mul(FLOAT4* v, float f, FLOAT4* out);
inline FLOAT4 vec4_div(FLOAT4* v0, FLOAT4* v1);
inline void vec4_div(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_div(FLOAT4* v, float f);
inline void vec4_div(FLOAT4* v, float f, FLOAT4* out);
inline FLOAT4 vec4_dot(FLOAT4* v0, FLOAT4* v1);
inline void vec4_dot(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_cross(FLOAT4* v0, FLOAT4* v1);
inline void vec4_cross(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_lengthsq(FLOAT4* v);
inline void vec4_lengthsq(FLOAT4* v, FLOAT4* out);
inline FLOAT4 vec4_length(FLOAT4* v);
inline void vec4_length(FLOAT4* v, FLOAT4* out);
inline void vec4_normalize(FLOAT4* v);
inline FLOAT4 vec4_normalized(FLOAT4* v);
inline void vec4_normalized(FLOAT4* v, FLOAT4* out);
inline FLOAT4 vec4_lerp(FLOAT4* v0, FLOAT4* v1, float t);
inline void vec4_lerp(FLOAT4* v0, FLOAT4* v1, float t, FLOAT4* out);
inline FLOAT4 vec4_clamp(FLOAT4* v, FLOAT4* vmin, FLOAT4* vmax);
inline void vec4_clamp(FLOAT4* v, FLOAT4* vmin, FLOAT4* vmax, FLOAT4* out);
inline FLOAT4 vec4_saturate(FLOAT4* v);
inline void vec4_saturate(FLOAT4* v, FLOAT4* out);
inline FLOAT4 vec4_min(FLOAT4* v0, FLOAT4* v1);
inline void vec4_min(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_max(FLOAT4* v0, FLOAT4* v1);
inline void vec4_max(FLOAT4* v0, FLOAT4* v1, FLOAT4* out);
inline FLOAT4 vec4_ceil(FLOAT4* v); // 올림
inline void vec4_ceil(FLOAT4* v, FLOAT4* out);
inline FLOAT4 vec4_floor(FLOAT4* v); // 내림
inline void vec4_floor(FLOAT4* v, FLOAT4* out);
inline FLOAT4 vec4_round(FLOAT4* v); // 반올림
inline void vec4_round(FLOAT4* v, FLOAT4* out);
inline FLOAT4 vec4_trunc(FLOAT4* v); // 버림
inline void vec4_trunc(FLOAT4* v, FLOAT4* out);

inline QUAT quat_zero();
inline void quat_zero(QUAT* q);
inline QUAT quat_ident();
inline void quat_ident(QUAT* q);
inline QUAT quat_set(float x, float y, float z, float w);
inline void quat_set(float x, float y, float z, float w, QUAT* out);
inline QUAT quat_mul(QUAT* q0, QUAT* q1);
inline void quat_mul(QUAT* q0, QUAT* q1, QUAT* out);
inline QUAT quat_mul(QUAT* q, float f);
inline void quat_mul(QUAT* q, float f, QUAT* out);
inline FLOAT3 quat_mul(QUAT* q, FLOAT3* v);
inline void quat_mul(QUAT* q, FLOAT3* v, FLOAT3* out);
inline QUAT quat_inverse(QUAT* q);
inline void quat_inverse(QUAT* q, QUAT* out);
inline QUAT quat_conjugate(QUAT* q);
inline void quat_conjugate(QUAT* q, QUAT* out);
inline QUAT quat_slerp(QUAT* q0, QUAT* q1, float t);
inline void quat_slerp(QUAT* q0, QUAT* q1, float t, QUAT* out);
inline QUAT quat_rotation_roll_pitch_yaw(float roll, float pitch, float yaw);
inline void quat_rotation_roll_pitch_yaw(float roll, float pitch, float yaw, QUAT* out);
inline QUAT quat_rotation_roll_pitch_yaw(FLOAT3* v);
inline void quat_rotation_roll_pitch_yaw(FLOAT3* v, QUAT* out);
inline QUAT quat_rotation_axis(float angle, FLOAT3* v);
inline void quat_rotation_axis(float angle, FLOAT3* v, QUAT* out);
inline QUAT quat_rotation_axis(float angle, float x, float y, float z);
inline void quat_rotation_axis(float angle, float x, float y, float z, QUAT* out);
inline QUAT quat_rotation_matrix(MATRIX4* m);
inline void quat_rotation_matrix(MATRIX4* m, QUAT* out);
inline QUAT quat_from_lookat(FLOAT3* forward);
inline void quat_from_lookat(FLOAT3* forward, QUAT* out);
inline QUAT quat_from_lookat(FLOAT3* forward, FLOAT3* up);
inline void quat_from_lookat(FLOAT3* forward, FLOAT3* up, QUAT* out);
inline FLOAT3 quat_to_roll_pitch_yaw(QUAT* q);
inline void quat_to_roll_pitch_yaw(QUAT* q, FLOAT3* out);
inline FLOAT4 quat_to_axis_angle(QUAT* q);
inline void quat_to_axis_angle(QUAT* q, FLOAT3* axis, float* angle);
inline MATRIX4 quat_to_matrix(QUAT* q);
inline void quat_to_matrix(QUAT* q, MATRIX4* out);

inline MATRIX4 mat4_zero();
inline void mat4_zero(MATRIX4* m);
inline MATRIX4 mat4_ident();
inline void mat4_ident(MATRIX4* m);
inline bool mat4_is_ident(MATRIX4* m);
inline bool mat4_is_nan(MATRIX4* m);
inline bool mat4_is_infinite(MATRIX4* m);
inline MATRIX4 mat4_set(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33);
inline void mat4_set(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33,
	MATRIX4* out);
inline MATRIX4 mat4_trans(FLOAT3* pos);
inline void mat4_trans(FLOAT3* pos, MATRIX4* out);
inline MATRIX4 mat4_trans(float x, float y, float z);
inline void mat4_trans(float x, float y, float z, MATRIX4* out);
inline FLOAT3 mat4_get_trans(MATRIX4* m);
inline void mat4_get_trans(MATRIX4* m, FLOAT3* out);
inline MATRIX4 mat4_transpose(MATRIX4* m);
inline void mat4_transpose(MATRIX4* m, MATRIX4* out);
inline MATRIX4 mat4_inverse(MATRIX4* m);
inline void mat4_inverse(MATRIX4* m, MATRIX4* out);
inline MATRIX4 mat4_mul(MATRIX4* m0, MATRIX4* m1);
inline void mat4_mul(MATRIX4* m0, MATRIX4* m1, MATRIX4* out);
inline FLOAT3 mat4_mul(MATRIX4* m, FLOAT3* v);
inline void mat4_mul(MATRIX4* m, FLOAT3* v, FLOAT3* out);
inline FLOAT4 mat4_mul(MATRIX4* m, FLOAT4* v);
inline void mat4_mul(MATRIX4* m, FLOAT4* v, FLOAT4* out);
inline MATRIX4 mat4_scale(float sx, float sy, float sz);
inline void mat4_scale(float sx, float sy, float sz, MATRIX4* out);
inline MATRIX4 mat4_scale(FLOAT3* v);
inline void mat4_scale(FLOAT3* v, MATRIX4* out);
inline MATRIX4 mat4_rotation_x(float angle);
inline void mat4_rotation_x(float angle, MATRIX4* out);
inline MATRIX4 mat4_rotation_y(float angle);
inline void mat4_rotation_y(float angle, MATRIX4* out);
inline MATRIX4 mat4_rotation_z(float angle);
inline void mat4_rotation_z(float angle, MATRIX4* out);
inline MATRIX4 mat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll);
inline void mat4_rotation_yaw_pitch_roll(float yaw, float pitch, float roll, MATRIX4* out);
inline MATRIX4 mat4_rotation_axis(FLOAT3 axis, float angle);
inline void mat4_rotation_axis(FLOAT3 axis, float angle, MATRIX4* out);
inline MATRIX4 mat4_rotation_quat(QUAT* q);
inline void mat4_rotation_quat(QUAT* q, MATRIX4* out);
inline MATRIX4 mat4_lookto_lh(FLOAT3* eye, FLOAT3* to, FLOAT3* up);
inline void mat4_lookto_lh(FLOAT3* eye, FLOAT3* to, FLOAT3* up, MATRIX4* out);
inline MATRIX4 mat4_lookto_rh(FLOAT3* eye, FLOAT3* to, FLOAT3* up);
inline void mat4_lookto_rh(FLOAT3* eye, FLOAT3* to, FLOAT3* up, MATRIX4* out);
inline MATRIX4 mat4_perspective_lh(float fov, float aspect, float nearZ, float farZ);
inline void mat4_perspective_lh(float fov, float aspect, float nearZ, float farZ, MATRIX4* out);
inline MATRIX4 mat4_perspective_rh(float fov, float aspect, float nearZ, float farZ);
inline void mat4_perspective_rh(float fov, float aspect, float nearZ, float farZ, MATRIX4* out);
inline MATRIX4 mat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void mat4_perspective_lh(float left, float right, float top, float bottom, float nearZ, float farZ, MATRIX4* out);
inline MATRIX4 mat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void mat4_perspective_rh(float left, float right, float top, float bottom, float nearZ, float farZ, MATRIX4* out);

inline MATRIX4 mat4_ortho_lh(float width, float height, float nearZ, float farZ);
inline void mat4_ortho_lh(float width, float height, float nearZ, float farZ, MATRIX4* out);
inline MATRIX4 mat4_ortho_rh(float width, float height, float nearZ, float farZ);
inline void mat4_ortho_rh(float width, float height, float nearZ, float farZ, MATRIX4* out);

inline MATRIX4 mat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void mat4_ortho_lh(float left, float right, float top, float bottom, float nearZ, float farZ, MATRIX4* out);
inline MATRIX4 mat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ);
inline void mat4_ortho_rh(float left, float right, float top, float bottom, float nearZ, float farZ, MATRIX4* out);

inline MATRIX4 mat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ);
inline void mat4_frustum_lh(float left, float right, float bottom, float top, float nearZ, float farZ, MATRIX4* out);
inline MATRIX4 mat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ);
inline void mat4_frustum_rh(float left, float right, float bottom, float top, float nearZ, float farZ, MATRIX4* out);



#include "mathlib.inl"

#endif // __MATHLIB_H__
