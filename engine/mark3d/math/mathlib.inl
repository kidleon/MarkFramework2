inline INT2 int2_zero()
{
	INT2 r;
	r.x = r.y = 0;
	return r;
}

inline void int2_zero(INT2& v) noexcept
{
	v.x = v.y = 0;
}

inline INT2 int2_set(int x, int y)
{
	INT2 r;
	r.x = x;
	r.y = y;
	return r;
}

inline void int2_set(int x, int y, INT2& out) noexcept
{
	out.x = x;
	out.y = y;
}

inline INT2 int2_add(const INT2& v0, const INT2& v1) noexcept
{
	INT2 r;
	r.x = v0.x + v1.x;
	r.y = v0.y + v1.y;
	return r;
}

inline void int2_add(const INT2& v0, const INT2& v1, INT2& out) noexcept
{
	out.x = v0.x + v1.x;
	out.y = v0.y + v1.y;
}

inline INT2 int2_add(const INT2& v0, INT i) noexcept
{
	INT2 r;
	r.x = v0.x + i;
	r.y = v0.y + i;
	return r;
}

inline void int2_add(const INT2& v0, INT i, INT2& out) noexcept
{
	out.x = v0.x + i;
	out.y = v0.y + i;
}

inline INT2 int2_sub(const INT2& v0, const INT2& v1) noexcept
{
	INT2 r;
	r.x = v0.x - v1.x;
	r.y = v0.y - v1.y;
	return r;
}

inline void int2_sub(const INT2& v0, const INT2& v1, INT2& out) noexcept
{
	out.x = v0.x - v1.x;
	out.y = v0.y - v1.y;
}

inline INT2 int2_sub(const INT2& v0, INT i) noexcept
{
	INT2 r;
	r.x = v0.x - i;
	r.y = v0.y - i;
	return r;
}

inline void int2_sub(const INT2& v0, INT i, INT2& out) noexcept
{
	out.x = v0.x - i;
	out.y = v0.y - i;
}

inline INT2 int2_mul(const INT2& v0, const INT2& v1) noexcept
{
	INT2 r;
	r.x = v0.x * v1.x;
	r.y = v0.y * v1.y;
	return r;
}

inline void int2_mul(const INT2& v0, const INT2& v1, INT2& out) noexcept
{
	out.x = v0.x * v1.x;
	out.y = v0.y * v1.y;
}

inline INT2 int2_mul(const INT2& v, int i) noexcept
{
	INT2 r;
	r.x = v.x * i;
	r.y = v.y * i;
	return r;
}

inline void int2_mul(const INT2& v, int i, INT2& out) noexcept
{
	out.x = v.x * i;
	out.y = v.y * i;
}

inline INT2 int2_div(const INT2& v0, const INT2& v1) noexcept
{
	__ASSERT(v1.x != 0 && v1.y != 0, "Division by zero in int2_div.");

	INT2 r;
	r.x = v0.x / v1.x;
	r.y = v0.y / v1.y;
	return r;
}

inline void int2_div(const INT2& v0, const INT2& v1, INT2& out) noexcept
{
	__ASSERT(v1.x != 0 && v1.y != 0, "Division by zero in int2_div.");

	out.x = v0.x / v1.x;
	out.y = v0.y / v1.y;
}

inline INT2 int2_div(const INT2& v, int i) noexcept
{
	__ASSERT(i != 0, "Division by zero in int2_div.");

	INT2 r;
	r.x = v.x / i;
	r.y = v.y / i;
	return r;
}

inline void int2_div(const INT2& v, int i, INT2& out) noexcept
{
	__ASSERT(i != 0, "Division by zero in int2_div.");

	out.x = v.x / i;
	out.y = v.y / i;
}

//----------------------------------------------------------------------------------
// FLOAT2 관련 함수들.
inline FLOAT2 vec2_zero() noexcept
{
	FLOAT2 r;
	r.x = r.y = 0.0f;
	return r;
}

inline void vec2_zero(FLOAT2& v) noexcept
{
	v.x = v.y = 0.0f;
}

inline FLOAT2 vec2_unit_x() noexcept
{
	FLOAT2 r;
	r.x = 1.0f;
	r.y = 0.0f;
	return r;
}

inline FLOAT2 vec2_unit_y() noexcept
{
	FLOAT2 r;
	r.x = 0.0f;
	r.y = 1.0f;
	return r;
}

inline FLOAT2 vec2_set(FLOAT x, FLOAT y) noexcept
{
	FLOAT2 r;
	r.x = x;
	r.y = y;

	return r;
}

inline void vec2_set(FLOAT x, FLOAT y, FLOAT2& out) noexcept
{
	out.x = x;
	out.y = y;
}

inline FLOAT2 vec2_add(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	FLOAT2 r;
	r.x = v0.x + v1.x;
	r.y = v0.y + v1.y;
	return r;
}

inline void vec2_add(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	out.x = v0.x + v1.x;
	out.y = v0.y + v1.y;
}

inline FLOAT2 vec2_sub(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	FLOAT2 r;
	r.x = v0.x - v1.x;
	r.y = v0.y - v1.y;

	return r;
}

inline void vec2_sub(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	out.x = v0.x - v1.x;
	out.y = v0.y - v1.y;
}

inline FLOAT2 vec2_mul(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	FLOAT2 r;
	r.x = v0.x * v1.x;
	r.y = v0.y * v1.y;
	return r;
}

inline void vec2_mul(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	out.x = v0.x * v1.x;
	out.y = v0.y * v1.y;
}

inline FLOAT2 vec2_mul(const FLOAT2& v, FLOAT f) noexcept
{
	FLOAT2 r;
	r.x = v.x * f;
	r.y = v.y * f;
	return r;
}

inline void vec2_mul(const FLOAT2& v, FLOAT f, FLOAT2& out) noexcept
{
	out.x = v.x * f;
	out.y = v.y * f;
}

inline FLOAT2 vec2_div(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	__ASSERT(v1.x != 0.0f && v1.y != 0.0f, "Division by zero in vec2_div.");

	FLOAT2 r;
	r.x = v0.x / v1.x;
	r.y = v0.y / v1.y;
}

inline void vec2_div(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	__ASSERT(v1.x != 0.0f && v1.y != 0.0f, "Division by zero in vec2_div.");
	out.x = v0.x / v1.x;
	out.y = v0.y / v1.y;
}

inline FLOAT2 vec2_div(const FLOAT2& v, FLOAT f) noexcept
{
	__ASSERT(f != 0.0f, "Division by zero in vec2_div.");
	FLOAT2 r;
	r.x = v.x / f;
	r.y = v.y / f;
	return r;
}

inline void vec2_div(const FLOAT2& v, FLOAT f, FLOAT2& out) noexcept
{
	__ASSERT(f != 0.0f, "Division by zero in vec2_div.");
	out.x = v.x / f;
	out.y = v.y / f;
}

inline FLOAT2 vec2_dot(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y);
	
	FLOAT2 r;
	r.x = dot;
	r.y = dot;

	return r;
}

inline void vec2_dot(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y);
	out.x = dot;
	out.y = dot;
}

inline FLOAT2 vec2_cross(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	// [ pV0->x*pV1->y - pV0->y*pV1->x, pV0->x*pV1->y - pV0->y*pV1->x ]
	FLOAT2 r;
	r.x = v0.x * v1.y - v0.y * v1.x;
	r.y = v0.x * v1.y - v0.y * v1.x;
	return r;
}

inline void vec2_cross(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	out.x = v0.x * v1.y - v0.y * v1.x;
	out.y = v0.x * v1.y - v0.y * v1.x;
}

inline FLOAT2 vec2_lengthsq(const FLOAT2& v) noexcept
{
	return vec2_dot(v, v);
}

inline void vec2_lengthsq(const FLOAT2& v, FLOAT2& out) noexcept
{
	out = vec2_dot(v, v);
}

inline FLOAT2 vec2_length(const FLOAT2& v) noexcept
{
	FLOAT2 t;
	vec2_lengthsq(v, t);

	float len = sqrtf(t.x);
	t.x = len;
	t.y = len;

	return t;
}

inline void vec2_length(const FLOAT2& v, FLOAT2& out) noexcept
{
	FLOAT2 t;
	vec2_lengthsq(v, t);

	float len = sqrtf(t.x);
	out.x = len;
	out.y = len;
}

inline void vec2_normalize(FLOAT2& v) noexcept
{
	FLOAT2 t;
	vec2_length(v, t);

	float inv_len = 1.0f / t.x;
	vec2_mul(v, inv_len, v);
}

inline FLOAT2 vec2_normalized(const FLOAT2& v) noexcept
{
	FLOAT2 t;
	vec2_length(v, t);
	float inv_len = 1.0f / t.x;
	vec2_mul(v, inv_len, t);

	return t;
}

inline void vec2_normalized(const FLOAT2& v, FLOAT2& out) noexcept
{
	FLOAT2 t;
	vec2_length(v, t);

	float inv_len = 1.0f / t.x;
	vec2_mul(v, inv_len, out);
}

inline FLOAT2 vec2_lerp(const FLOAT2& v0, const FLOAT2& v1, FLOAT t) noexcept
{
	// V0 + ((V1 - V0) * t)
	FLOAT2 r;
	vec2_sub(v1, v0, r);
	vec2_mul(r, t, r);
	vec2_add(v0, r, r);
	return r;
}

inline void vec2_lerp(const FLOAT2& v0, const FLOAT2& v1, FLOAT t, FLOAT2& out) noexcept
{
	// V0 + ((V1 - V0) * t)
	FLOAT2 r;
	vec2_sub(v1, v0, r);
	vec2_mul(r, t, r);
	vec2_add(v0, r, out);
}

inline FLOAT2 vec2_clamp(const FLOAT2& v, const FLOAT2& vmin, const FLOAT2& vmax) noexcept
{
	FLOAT2 r;
	r.x = TMAX(vmin.x, v.x);
	r.y = TMAX(vmin.y, v.y);
	r.x = TMIN(vmax.x, r.x);
	r.y = TMIN(vmax.y, r.y);
	return r;
}

inline void vec2_clamp(const FLOAT2& v, const FLOAT2& vmin, const FLOAT2& vmax, FLOAT2& out) noexcept
{
	out.x = TMAX(vmin.x, v.x);
	out.y = TMAX(vmin.y, v.y);
	out.x = TMIN(vmax.x, out.x);
	out.y = TMIN(vmax.y, out.y);
}

inline FLOAT2 vec2_saturate(const FLOAT2& v) noexcept
{
	FLOAT2 r;

	// Set <0 to 0
	r.x = TMAX(v.x, 0.0f);
	r.y = TMAX(v.y, 0.0f);

	// Set>1 to 1
	r.x = TMIN(r.x, 1.0f);
	r.y = TMIN(r.y, 1.0f);

	return r;
}

inline void vec2_saturate(const FLOAT2& v, FLOAT2& out) noexcept
{
	// Set <0 to 0
	out.x = TMAX(v.x, 0.0f);
	out.y = TMAX(v.y, 0.0f);
	// Set>1 to 1
	out.x = TMIN(out.x, 1.0f);
	out.y = TMIN(out.y, 1.0f);
}

inline FLOAT2 vec2_min(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	FLOAT2 r;
	r.x = TMIN(v0.x, v1.x);
	r.y = TMIN(v0.y, v1.y);
	return r;
}

inline void vec2_min(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	out.x = TMIN(v0.x, v1.x);
	out.y = TMIN(v0.y, v1.y);
}

inline FLOAT2 vec2_max(const FLOAT2& v0, const FLOAT2& v1) noexcept
{
	FLOAT2 r;
	r.x = TMAX(v0.x, v1.x);
	r.y = TMAX(v0.y, v1.y);
	return r;
}

inline void vec2_max(const FLOAT2& v0, const FLOAT2& v1, FLOAT2& out) noexcept
{
	out.x = TMAX(v0.x, v1.x);
	out.y = TMAX(v0.y, v1.y);
}

inline FLOAT2 vec2_ceil(const FLOAT2& v) noexcept
{
	FLOAT2 r;
	r.x = ceilf(v.x);
	r.y = ceilf(v.y);
	return r;
}

inline void vec2_ceil(const FLOAT2& v, FLOAT2& out) noexcept
{
	out.x = ceilf(v.x);
	out.y = ceilf(v.y);
}

inline FLOAT2 vec2_floor(const FLOAT2& v) noexcept
{
	FLOAT2 r;
	r.x = floorf(v.x);
	r.y = floorf(v.y);
	return r;
}

inline void vec2_floor(const FLOAT2& v, FLOAT2& out) noexcept
{
	out.x = floorf(v.x);
	out.y = floorf(v.y);
}

inline FLOAT2 vec2_round(const FLOAT2& v) noexcept
{
	FLOAT2 r;
	r.x = roundf(v.x);
	r.y = roundf(v.y);
	return r;
}

inline void vec2_round(const FLOAT2& v, FLOAT2& out) noexcept
{
	out.x = roundf(v.x);
	out.y = roundf(v.y);
}

inline FLOAT2 vec2_trunc(const FLOAT2& v) noexcept
{
	FLOAT2 r;
	r.x = truncf(v.x);
	r.y = truncf(v.y);
	return r;
}

inline void vec2_trunc(const FLOAT2& v, FLOAT2& out) noexcept
{
	out.x = truncf(v.x);
	out.y = truncf(v.y);
}

//----------------------------------------------------------------------------------
// FLOAT3 관련 함수들.
inline FLOAT3 vec3_zero() noexcept
{
	FLOAT3 r;
	r.x = r.y = r.z = 0.0f;
	return r;
}

inline void vec3_zero(FLOAT3& v) noexcept
{
	v.x = v.y = v.z = 0.0f;
}

inline FLOAT3 vec3_unit_x() noexcept
{
	FLOAT3 r;
	r.x = 1.0f;
	r.y = 0.0f;
	r.z = 0.0f;
	return r;
}

inline FLOAT3 vec3_unit_y() noexcept
{
	FLOAT3 r;
	r.x = 0.0f;
	r.y = 1.0f;
	r.z = 0.0f;
	return r;
}

inline FLOAT3 vec3_unit_z() noexcept
{
	FLOAT3 r;
	r.x = 0.0f;
	r.y = 0.0f;
	r.z = 1.0f;
	return r;
}

inline FLOAT3 vec3_set(FLOAT x, FLOAT y) noexcept
{
	FLOAT3 r;
	r.x = x;
	r.y = y;
	r.z = 0.0f;
	return r;
}

inline FLOAT3 vec3_set(FLOAT x, FLOAT y, FLOAT z) noexcept
{
	FLOAT3 r;
	r.x = x;
	r.y = y;
	r.z = z;
	return r;
}

inline void vec3_set(FLOAT x, FLOAT y, FLOAT3& out) noexcept
{
	out.x = x;
	out.y = y;
	out.z = 0.0f;
}

inline void vec3_set(FLOAT x, FLOAT y, FLOAT z, FLOAT3& out) noexcept
{
	out.x = x;
	out.y = y;
	out.z = z;
}

inline FLOAT3 vec3_add(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = v0.x + v1.x;
	r.y = v0.y + v1.y;
	r.z = v0.z + v1.z;
	return r;
}

inline void vec3_add(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = v0.x + v1.x;
	out.y = v0.y + v1.y;
	out.z = v0.z + v1.z;
}

inline FLOAT3 vec3_sub(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = v0.x - v1.x;
	r.y = v0.y - v1.y;
	r.z = v0.z - v1.z;
	return r;
}

inline void vec3_sub(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = v0.x - v1.x;
	out.y = v0.y - v1.y;
	out.z = v0.z - v1.z;
}

inline FLOAT3 vec3_mul(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = v0.x * v1.x;
	r.y = v0.y * v1.y;
	r.z = v0.z * v1.z;
	return r;
}

inline void vec3_mul(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = v0.x * v1.x;
	out.y = v0.y * v1.y;
	out.z = v0.z * v1.z;
}

inline FLOAT3 vec3_mul(const FLOAT3& v, FLOAT f) noexcept
{
	FLOAT3 r;
	r.x = v.x * f;
	r.y = v.y * f;
	r.z = v.z * f;
	return r;
}

inline void vec3_mul(const FLOAT3& v, FLOAT f, FLOAT3& out) noexcept
{
	out.x = v.x * f;
	out.y = v.y * f;
	out.z = v.z * f;
}

inline FLOAT3 vec3_div(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = v0.x / v1.x;
	r.y = v0.y / v1.y;
	r.z = v0.z / v1.z;
	return r;
}

inline void vec3_div(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = v0.x / v1.x;
	out.y = v0.y / v1.y;
	out.z = v0.z / v1.z;
}

inline FLOAT3 vec3_div(const FLOAT3& v, FLOAT f) noexcept
{
	FLOAT3 r;
	r.x = v.x / f;
	r.y = v.y / f;
	r.z = v.z / f;
	return r;
}

inline void vec3_div(const FLOAT3& v, FLOAT f, FLOAT3& out) noexcept
{
	out.x = v.x / f;
	out.y = v.y / f;
	out.z = v.z / f;
}

inline float vec3_dot_f(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
	return dot;
}

inline FLOAT3 vec3_dot(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);

	FLOAT3 r;
	r.x = dot;
	r.y = dot;
	r.z = dot;
	return r;
}

inline void vec3_dot(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
	out.x = dot;
	out.y = dot;
	out.z = dot;
}

inline FLOAT3 vec3_cross(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = (v0.y * v1.z) - (v0.z * v1.y);
	r.y = (v0.z * v1.x) - (v0.x * v1.z);
	r.z = (v0.x * v1.y) - (v0.y * v1.x);
	return r;
}

inline void vec3_cross(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = (v0.y * v1.z) - (v0.z * v1.y);
	out.y = (v0.z * v1.x) - (v0.x * v1.z);
	out.z = (v0.x * v1.y) - (v0.y * v1.x);
}

inline FLOAT3 vec3_lengthsq(const FLOAT3& v) noexcept
{
	return vec3_dot(v, v);
}

inline void vec3_lengthsq(const FLOAT3& v, FLOAT3& out) noexcept
{
	return vec3_dot(v, v, out);
}

inline FLOAT3 vec3_length(const FLOAT3& v) noexcept
{
	FLOAT3 t;
	vec3_lengthsq(v, t);
	float len = sqrtf(t.x);
	t.x = len;
	t.y = len;
	t.z = len;
	return t;
}

inline void vec3_length(const FLOAT3& v, FLOAT3& out) noexcept
{
	FLOAT3 t;
	vec3_lengthsq(v, t);
	float len = sqrtf(t.x);
	out.x = len;
	out.y = len;
	out.z = len;
}

inline void vec3_normalize(FLOAT3& v) noexcept
{
	FLOAT3 t;
	vec3_length(v, t);
	float inv_len = 1.0f / t.x;
	vec3_mul(v, inv_len, v);
}
inline FLOAT3 vec3_normalized(const FLOAT3& v) noexcept
{
	FLOAT3 t;
	vec3_length(v, t);
	float inv_len = 1.0f / t.x;
	vec3_mul(v, inv_len, t);
	return t;
}

inline void vec3_normalized(const FLOAT3& v, FLOAT3& out) noexcept
{
	FLOAT3 t;
	vec3_length(v, t);
	float inv_len = 1.0f / t.x;
	vec3_mul(v, inv_len, out);
}

inline FLOAT3 vec3_lerp(const FLOAT3& v0, const FLOAT3& v1, FLOAT t) noexcept
{
	// V0 + ((V1 - V0) * t)
	FLOAT3 r;
	vec3_sub(v0, v1, r);
	vec3_mul(r, t, r);
	vec3_add(v0, r, r);
	return r;
}

inline void vec3_lerp(const FLOAT3& v0, const FLOAT3& v1, FLOAT t, FLOAT3& out) noexcept
{
	// V0 + ((V1 - V0) * t)
	FLOAT3 r;
	vec3_sub(v0, v1, r);
	vec3_mul(r, t, r);
	vec3_add(v0, r, out);
}

inline FLOAT3 vec3_clamp(const FLOAT3& v, const FLOAT3& vmin, const FLOAT3& vmax) noexcept
{
	FLOAT3 r;
	r.x = TMAX(vmin.x, v.x);
	r.y = TMAX(vmin.y, v.y);
	r.z = TMAX(vmin.z, v.z);
	r.x = TMIN(vmax.x, r.x);
	r.y = TMIN(vmax.y, r.y);
	r.z = TMIN(vmax.z, r.z);
	return r;
}
inline void vec3_clamp(const FLOAT3& v, const FLOAT3& vmin, const FLOAT3& vmax, FLOAT3& out) noexcept
{
	out.x = TMAX(vmin.x, v.x);
	out.y = TMAX(vmin.y, v.y);
	out.z = TMAX(vmin.z, v.z);
	out.x = TMIN(vmax.x, out.x);
	out.y = TMIN(vmax.y, out.y);
	out.z = TMIN(vmax.z, out.z);
}

inline FLOAT3 vec3_saturate(const FLOAT3& v) noexcept
{
	FLOAT3 r;
	// Set <0 to 0
	r.x = TMAX(v.x, 0.0f);
	r.y = TMAX(v.y, 0.0f);
	r.z = TMAX(v.z, 0.0f);

	// Set>1 to 1
	r.x = TMIN(r.x, 1.0f);
	r.y = TMIN(r.y, 1.0f);
	r.z = TMIN(r.z, 1.0f);
	return r;
}

inline void vec3_saturate(const FLOAT3& v, FLOAT3& out) noexcept
{
	// Set <0 to 0
	out.x = TMAX(v.x, 0.0f);
	out.y = TMAX(v.y, 0.0f);
	out.z = TMAX(v.z, 0.0f);
	// Set>1 to 1
	out.x = TMIN(out.x, 1.0f);
	out.y = TMIN(out.y, 1.0f);
	out.z = TMIN(out.z, 1.0f);
}

inline FLOAT3 vec3_min(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = TMIN(v0.x, v1.x);
	r.y = TMIN(v0.y, v1.y);
	r.z = TMIN(v0.z, v1.z);
	return r;
}

inline void vec3_min(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = TMIN(v0.x, v1.x);
	out.y = TMIN(v0.y, v1.y);
	out.z = TMIN(v0.z, v1.z);
}

inline FLOAT3 vec3_max(const FLOAT3& v0, const FLOAT3& v1) noexcept
{
	FLOAT3 r;
	r.x = TMAX(v0.x, v1.x);
	r.y = TMAX(v0.y, v1.y);
	r.z = TMAX(v0.z, v1.z);
	return r;
}

inline void vec3_max(const FLOAT3& v0, const FLOAT3& v1, FLOAT3& out) noexcept
{
	out.x = TMAX(v0.x, v1.x);
	out.y = TMAX(v0.y, v1.y);
	out.z = TMAX(v0.z, v1.z);
}

inline FLOAT3 vec3_ceil(const FLOAT3& v) noexcept
{
	FLOAT3 r;
	r.x = ceilf(v.x);
	r.y = ceilf(v.y);
	r.z = ceilf(v.z);
	return r;
}

inline void vec3_ceil(const FLOAT3& v, FLOAT3& out) noexcept
{
	out.x = ceilf(v.x);
	out.y = ceilf(v.y);
	out.z = ceilf(v.z);
}
inline FLOAT3 vec3_floor(const FLOAT3& v) noexcept
{
	FLOAT3 r;
	r.x = floorf(v.x);
	r.y = floorf(v.y);
	r.z = floorf(v.z);
	return r;
}

inline void vec3_floor(const FLOAT3& v, FLOAT3& out) noexcept
{
	out.x = floorf(v.x);
	out.y = floorf(v.y);
	out.z = floorf(v.z);
}

inline FLOAT3 vec3_round(const FLOAT3& v) noexcept
{
	FLOAT3 r;
	r.x = roundf(v.x);
	r.y = roundf(v.y);
	r.z = roundf(v.z);
	return r;
}

inline void vec3_round(const FLOAT3& v, FLOAT3& out) noexcept
{
	out.x = roundf(v.x);
	out.y = roundf(v.y);
	out.z = roundf(v.z);
}

inline FLOAT3 vec3_trunc(const FLOAT3& v) noexcept
{
	FLOAT3 r;
	r.x = truncf(v.x);
	r.y = truncf(v.y);
	r.z = truncf(v.z);
	return r;
}

inline void vec3_trunc(const FLOAT3& v, FLOAT3& out) noexcept
{
	out.x = truncf(v.x);
	out.y = truncf(v.y);
	out.z = truncf(v.z);
}


inline FLOAT4 vec4_zero() noexcept
{
	FLOAT4 r;
	r.x = r.y = r.z = r.w = 0.0f;
	return r;
}

inline void vec4_zero(FLOAT4& v) noexcept
{
	v.x = v.y = v.z = v.w = 0.0f;
}

inline FLOAT4 vec4_unit_x() noexcept
{
	FLOAT4 r;
	r.x = 1.0f;
	r.y = 0.0f;
	r.z = 0.0f;
	r.w = 0.0f;
	return r;
}

inline FLOAT4 vec4_unit_y() noexcept
{
	FLOAT4 r;
	r.x = 0.0f;
	r.y = 1.0f;
	r.z = 0.0f;
	r.w = 0.0f;
	return r;
}

inline FLOAT4 vec4_unit_z() noexcept
{
	FLOAT4 r;
	r.x = 0.0f;
	r.y = 0.0f;
	r.z = 1.0f;
	r.w = 0.0f;
	return r;
}

inline FLOAT4 vec4_unit_w() noexcept	
{
	FLOAT4 r;
	r.x = 0.0f;
	r.y = 0.0f;
	r.z = 1.0f;
	r.w = 0.0f;
	return r;
}

inline FLOAT4 vec4_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w) noexcept
{
	FLOAT4 r;
	r.x = x;
	r.y = y;
	r.z = z;
	r.w = w;
	return r;
}

inline FLOAT4 vec4_set(FLOAT x, FLOAT y, FLOAT z) noexcept
{
	FLOAT4 r;
	r.x = x;
	r.y = y;
	r.z = z;
	r.w = 0.0f;
	return r;
}

inline void vec4_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w, FLOAT4& out) noexcept
{
	out.x = x;
	out.y = y;
	out.z = z;
	out.w = w;
}

inline FLOAT4 vec4_add(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	FLOAT4 r;
	r.x = v0.x + v1.x;
	r.y = v0.y + v1.y;
	r.z = v0.z + v1.z;
	r.w = v0.w + v1.w;
	return r;
}

inline void vec4_add(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	out.x = v0.x + v1.x;
	out.y = v0.y + v1.y;
	out.z = v0.z + v1.z;
	out.w = v0.w + v1.w;
}

inline FLOAT4 vec4_sub(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	FLOAT4 r;
	r.x = v0.x - v1.x;
	r.y = v0.y - v1.y;
	r.z = v0.z - v1.z;
	r.w = v0.w - v1.w;
	return r;
}

inline void vec4_sub(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	out.x = v0.x - v1.x;
	out.y = v0.y - v1.y;
	out.z = v0.z - v1.z;
	out.w = v0.w - v1.w;
}

inline FLOAT4 vec4_mul(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	FLOAT4 r;
	r.x = v0.x * v1.x;
	r.y = v0.y * v1.y;
	r.z = v0.z * v1.z;
	r.w = v0.w * v1.w;
	return r;
}

inline void vec4_mul(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	out.x = v0.x * v1.x;
	out.y = v0.y * v1.y;
	out.z = v0.z * v1.z;
	out.w = v0.w * v1.w;
}

inline FLOAT4 vec4_mul(const FLOAT4& v, FLOAT f) noexcept
{
	FLOAT4 r;
	r.x = v.x * f;
	r.y = v.y * f;
	r.z = v.z * f;
	r.w = v.w * f;
	return r;
}

inline void vec4_mul(const FLOAT4& v, FLOAT f, FLOAT4& out) noexcept
{
	out.x = v.x * f;
	out.y = v.y * f;
	out.z = v.z * f;
	out.w = v.w * f;
}

inline FLOAT4 vec4_div(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	__ASSERT(v1.x != 0.0f && v1.y != 0.0f && v1.z != 0.0f && v1.w != 0.0f, "Division by zero in vec4_div.");
	FLOAT4 r;
	r.x = v0.x / v1.x;
	r.y = v0.y / v1.y;
	r.z = v0.z / v1.z;
	r.w = v0.w / v1.w;
	return r;
}

inline void vec4_div(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	__ASSERT(v1.x != 0.0f && v1.y != 0.0f && v1.z != 0.0f && v1.w != 0.0f, "Division by zero in vec4_div.");
	out.x = v0.x / v1.x;
	out.y = v0.y / v1.y;
	out.z = v0.z / v1.z;
	out.w = v0.w / v1.w;
}

inline FLOAT4 vec4_div(const FLOAT4& v, FLOAT f) noexcept
{
	__ASSERT(f != 0.0f, "Division by zero in vec4_div.");
	FLOAT4 r;
	r.x = v.x / f;
	r.y = v.y / f;
	r.z = v.z / f;
	r.w = v.w / f;
	return r;
}

inline void vec4_div(const FLOAT4& v, FLOAT f, FLOAT4& out) noexcept
{
	__ASSERT(f != 0.0f, "Division by zero in vec4_div.");
	out.x = v.x / f;
	out.y = v.y / f;
	out.z = v.z / f;
	out.w = v.w / f;
}

inline FLOAT4 vec4_dot(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z) + (v0.w * v1.w);
	FLOAT4 r;
	r.x = dot;
	r.y = dot;
	r.z = dot;
	r.w = dot;
	return r;
}

inline void vec4_dot(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	float dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z) + (v0.w * v1.w);
	out.x = dot;
	out.y = dot;
	out.z = dot;
	out.w = dot;
}

inline FLOAT4 vec4_cross(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	// [ pV0->y*pV1->z - pV0->z*pV1->y, pV0->z*pV1->x - pV0->x*pV1->z, pV0->x*pV1->y - pV0->y*pV1->x, 0.0f ]
	FLOAT4 r;
	r.x = (v0.y * v1.z) - (v0.z * v1.y);
	r.y = (v0.z * v1.x) - (v0.x * v1.z);
	r.z = (v0.x * v1.y) - (v0.y * v1.x);
	r.w = 0.0f;
	return r;
}

inline void vec4_cross(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	out.x = (v0.y * v1.z) - (v0.z * v1.y);
	out.y = (v0.z * v1.x) - (v0.x * v1.z);
	out.z = (v0.x * v1.y) - (v0.y * v1.x);
	out.w = 0.0f;
}

inline FLOAT4 vec4_lengthsq(const FLOAT4& v) noexcept
{
	return vec4_dot(v, v);
}

inline void vec4_lengthsq(const FLOAT4& v, FLOAT4& out) noexcept
{
	out = vec4_dot(v, v);
}

inline FLOAT4 vec4_length(const FLOAT4& v) noexcept
{
	FLOAT4 t;
	vec4_lengthsq(v, t);
	float len = sqrtf(t.x);
	t.x = len;
	t.y = len;
	t.z = len;
	t.w = len;
	return t;
}

inline void vec4_length(const FLOAT4& v, FLOAT4& out) noexcept
{
	FLOAT4 t;
	vec4_lengthsq(v, t);
	float len = sqrtf(t.x);
	out.x = len;
	out.y = len;
	out.z = len;
	out.w = len;
}

inline void vec4_normalize(FLOAT4& v) noexcept
{
	FLOAT4 t;
	vec4_length(v, t);
	float inv_len = 1.0f / t.x;
	vec4_mul(v, inv_len, v);
}

inline FLOAT4 vec4_normalized(const FLOAT4& v) noexcept
{
	FLOAT4 t;
	vec4_length(v, t);
	float inv_len = 1.0f / t.x;
	vec4_mul(v, inv_len, t);
	return t;
}

inline void vec4_normalized(const FLOAT4& v, FLOAT4& out) noexcept
{
	FLOAT4 t;
	vec4_length(v, t);
	float inv_len = 1.0f / t.x;
	vec4_mul(v, inv_len, out);
}

inline FLOAT4 vec4_lerp(const FLOAT4& v0, const FLOAT4& v1, FLOAT t) noexcept
{
	// V0 + ((V1 - V0) * t)
	FLOAT4 r;
	vec4_sub(v0, v1, r);
	vec4_mul(r, t, r);
	vec4_add(v0, r, r);
	return r;
}

inline void vec4_lerp(const FLOAT4& v0, const FLOAT4& v1, FLOAT t, FLOAT4& out) noexcept
{
	// V0 + ((V1 - V0) * t)
	FLOAT4 r;
	vec4_sub(v0, v1, r);
	vec4_mul(r, t, r);
	vec4_add(v0, r, out);
}

inline FLOAT4 vec4_clamp(const FLOAT4& v, const FLOAT4& vmin, const FLOAT4& vmax) noexcept
{
	FLOAT4 r;
	r.x = TMAX(vmin.x, v.x);
	r.y = TMAX(vmin.y, v.y);
	r.z = TMAX(vmin.z, v.z);
	r.w = TMAX(vmin.w, v.w);
	r.x = TMIN(vmax.x, r.x);
	r.y = TMIN(vmax.y, r.y);
	r.z = TMIN(vmax.z, r.z);
	r.w = TMIN(vmax.w, r.w);
	return r;
}

inline void vec4_clamp(const FLOAT4& v, const FLOAT4& vmin, const FLOAT4& vmax, FLOAT4& out) noexcept
{
	out.x = TMAX(vmin.x, v.x);
	out.y = TMAX(vmin.y, v.y);
	out.z = TMAX(vmin.z, v.z);
	out.w = TMAX(vmin.w, v.w);
	out.x = TMIN(vmax.x, out.x);
	out.y = TMIN(vmax.y, out.y);
	out.z = TMIN(vmax.z, out.z);
	out.w = TMIN(vmax.w, out.w);
}

inline FLOAT4 vec4_saturate(const FLOAT4& v) noexcept
{
	FLOAT4 r;
	// Set <0 to 0
	r.x = TMAX(v.x, 0.0f);
	r.y = TMAX(v.y, 0.0f);
	r.z = TMAX(v.z, 0.0f);
	r.w = TMAX(v.w, 0.0f);
	// Set>1 to 1
	r.x = TMIN(r.x, 1.0f);
	r.y = TMIN(r.y, 1.0f);
	r.z = TMIN(r.z, 1.0f);
	r.w = TMIN(r.w, 1.0f);
	return r;
}

inline void vec4_saturate(const FLOAT4& v, FLOAT4& out) noexcept
{
	// Set <0 to 0
	out.x = TMAX(v.x, 0.0f);
	out.y = TMAX(v.y, 0.0f);
	out.z = TMAX(v.z, 0.0f);
	out.w = TMAX(v.w, 0.0f);
	// Set>1 to 1
	out.x = TMIN(out.x, 1.0f);
	out.y = TMIN(out.y, 1.0f);
	out.z = TMIN(out.z, 1.0f);
	out.w = TMIN(out.w, 1.0f);
}

inline FLOAT4 vec4_min(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	FLOAT4 r;
	r.x = TMIN(v0.x, v1.x);
	r.y = TMIN(v0.y, v1.y);
	r.z = TMIN(v0.z, v1.z);
	r.w = TMIN(v0.w, v1.w);
	return r;
}

inline void vec4_min(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	out.x = TMIN(v0.x, v1.x);
	out.y = TMIN(v0.y, v1.y);
	out.z = TMIN(v0.z, v1.z);
	out.w = TMIN(v0.w, v1.w);
}

inline FLOAT4 vec4_max(const FLOAT4& v0, const FLOAT4& v1) noexcept
{
	FLOAT4 r;
	r.x = TMAX(v0.x, v1.x);
	r.y = TMAX(v0.y, v1.y);
	r.z = TMAX(v0.z, v1.z);
	r.w = TMAX(v0.w, v1.w);
	return r;
}

inline void vec4_max(const FLOAT4& v0, const FLOAT4& v1, FLOAT4& out) noexcept
{
	out.x = TMAX(v0.x, v1.x);
	out.y = TMAX(v0.y, v1.y);
	out.z = TMAX(v0.z, v1.z);
	out.w = TMAX(v0.w, v1.w);
}

inline FLOAT4 vec4_ceil(const FLOAT4& v) noexcept // 올림
{
	FLOAT4 r;
	r.x = ceilf(v.x);
	r.y = ceilf(v.y);
	r.z = ceilf(v.z);
	r.w = ceilf(v.w);
	return r;
}

inline void vec4_ceil(const FLOAT4& v, FLOAT4& out) noexcept
{
	out.x = ceilf(v.x);
	out.y = ceilf(v.y);
	out.z = ceilf(v.z);
	out.w = ceilf(v.w);
}

inline FLOAT4 vec4_floor(const FLOAT4& v) noexcept // 내림
{
	FLOAT4 r;
	r.x = floorf(v.x);
	r.y = floorf(v.y);
	r.z = floorf(v.z);
	r.w = floorf(v.w);
	return r;
}

inline void vec4_floor(const FLOAT4& v, FLOAT4& out) noexcept
{
	out.x = floorf(v.x);
	out.y = floorf(v.y);
	out.z = floorf(v.z);
	out.w = floorf(v.w);
}

inline FLOAT4 vec4_round(const FLOAT4& v) noexcept // 반올림
{
	FLOAT4 r;
	r.x = roundf(v.x);
	r.y = roundf(v.y);
	r.z = roundf(v.z);
	r.w = roundf(v.w);
	return r;
}

inline void vec4_round(const FLOAT4& v, FLOAT4& out) noexcept
{
	out.x = roundf(v.x);
	out.y = roundf(v.y);
	out.z = roundf(v.z);
	out.w = roundf(v.w);
}

inline FLOAT4 vec4_trunc(const FLOAT4& v) noexcept // 버림
{
	FLOAT4 r;
	r.x = truncf(v.x);
	r.y = truncf(v.y);
	r.z = truncf(v.z);
	r.w = truncf(v.w);
	return r;
}

inline void vec4_trunc(const FLOAT4& v, FLOAT4& out) noexcept
{
	out.x = truncf(v.x);
	out.y = truncf(v.y);
	out.z = truncf(v.z);
	out.w = truncf(v.w);
}


inline QUAT quat_zero() noexcept
{
	QUAT r;
	r.x = r.y = r.z = r.w = 0.0f;
	return r;
}

inline void quat_zero(QUAT& q) noexcept
{
	q.x = q.y = q.z = q.w = 0.0f;
}

inline QUAT quat_ident() noexcept
{
	QUAT r;
	r.x = 0.0f;
	r.y = 0.0f;
	r.z = 0.0f;
	r.w = 1.0f;
	return r;
}

inline void quat_ident(QUAT& q) noexcept
{
	q.x = 0.0f;
	q.y = 0.0f;
	q.z = 0.0f;
	q.w = 1.0f;
}

inline QUAT quat_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w) noexcept
{
	QUAT r;
	r.x = x;
	r.y = y;
	r.z = z;
	r.w = w;
	return r;
}

inline void quat_set(FLOAT x, FLOAT y, FLOAT z, FLOAT w, QUAT& out) noexcept
{
	out.x = x;
	out.y = y;
	out.z = z;
	out.w = w;
}

inline QUAT quat_mul(const QUAT& q0, const QUAT& q1) noexcept
{
	QUAT r;
	r.x = (q0.w * q1.x) + (q0.x * q1.w) + (q0.y * q1.z) - (q0.z * q1.y);
	r.y = (q0.w * q1.y) - (q0.x * q1.z) + (q0.y * q1.w) + (q0.z * q1.x);
	r.z = (q0.w * q1.z) + (q0.x * q1.y) - (q0.y * q1.x) + (q0.z * q1.w);
	r.w = (q0.w * q1.w) - (q0.x * q1.x) - (q0.y * q1.y) - (q0.z * q1.z);
	return r;
}

inline void quat_mul(const QUAT& q0, const QUAT& q1, QUAT& out) noexcept
{
	out.x = (q0.w * q1.x) + (q0.x * q1.w) + (q0.y * q1.z) - (q0.z * q1.y);
	out.y = (q0.w * q1.y) - (q0.x * q1.z) + (q0.y * q1.w) + (q0.z * q1.x);
	out.z = (q0.w * q1.z) + (q0.x * q1.y) - (q0.y * q1.x) + (q0.z * q1.w);
	out.w = (q0.w * q1.w) - (q0.x * q1.x) - (q0.y * q1.y) - (q0.z * q1.z);
}

inline QUAT quat_mul(const QUAT& q, FLOAT f) noexcept
{
	QUAT r;
	r.x = q.x * f;
	r.y = q.y * f;
	r.z = q.z * f;
	r.w = q.w * f;
	return r;
}

inline void quat_mul(const QUAT& q, FLOAT f, QUAT& out) noexcept
{
	out.x = q.x * f;
	out.y = q.y * f;
	out.z = q.z * f;
	out.w = q.w * f;
}

inline FLOAT3 quat_mul(const QUAT& q, const FLOAT3& v) noexcept
{
	// nVidia SDK implementation
	// FVector3 uv, uuv;
	// FVector3 qvec(x, y, z);
	// uv = qvec.Cross(v);
	// uuv = qvec.Cross(uv);
	// uv *= (2.0f * w);
	// uuv *= 2.0f;
	// return v + uv + uuv;

	FLOAT3 uv, uuv;
	FLOAT3 qvec = { q.x, q.y, q.z };

	vec3_cross(qvec, v, uv);
	vec3_cross(qvec, uv, uuv);

	uv.x *= (2.0f * q.w);
	uv.y *= (2.0f * q.w);
	uv.z *= (2.0f * q.w);
	uuv.x *= 2.0f;
	uuv.y *= 2.0f;
	uuv.z *= 2.0f;

	FLOAT3 r;
	r.x = v.x + uv.x + uuv.x;
	r.y = v.y + uv.y + uuv.y;
	r.z = v.z + uv.z + uuv.z;

	return r;
}

inline void quat_mul(const QUAT& q, const FLOAT3& v, FLOAT3& out) noexcept
{
	// nVidia SDK implementation
	// FVector3 uv, uuv;
	// FVector3 qvec(x, y, z);
	// uv = qvec.Cross(v);
	// uuv = qvec.Cross(uv);
	// uv *= (2.0f * w);
	// uuv *= 2.0f;
	// return v + uv + uuv;

	FLOAT3 uv, uuv;
	FLOAT3 qvec = { q.x, q.y, q.z };

	vec3_cross(qvec, v, uv);
	vec3_cross(qvec, uv, uuv);

	uv.x *= (2.0f * q.w);
	uv.y *= (2.0f * q.w);
	uv.z *= (2.0f * q.w);
	uuv.x *= 2.0f;
	uuv.y *= 2.0f;
	uuv.z *= 2.0f;

	out.x = v.x + uv.x + uuv.x;
	out.y = v.y + uv.y + uuv.y;
	out.z = v.z + uv.z + uuv.z;
}

inline QUAT quat_inverse(const QUAT& q) noexcept
{
	QUAT r;
	quat_inverse(q, r);
	return r;
}

inline void quat_inverse(const QUAT& q, QUAT& out) noexcept
{
	// Conjugate
	out.x = -q.x;
	out.y = -q.y;
	out.z = -q.z;
	out.w = q.w;

	// Normalize
	float len_sq = (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
	float inv_len_sq = 1.0f / len_sq;
	out.x *= inv_len_sq;
	out.y *= inv_len_sq;
	out.z *= inv_len_sq;
	out.w *= inv_len_sq;
}

inline QUAT quat_conjugate(const QUAT& q) noexcept
{
	QUAT r;
	quat_conjugate(q, r);
	return r;
}

inline void quat_conjugate(const QUAT& q, QUAT& out) noexcept
{
	out.x = -q.x;
	out.y = -q.y;
	out.z = -q.z;
	out.w = q.w;
}

inline void quat_slerp(const QUAT& q0, const QUAT& q1, FLOAT t, QUAT& out) noexcept
{
	// Compute the cosine of the angle between the two vectors.
	float cos_theta = (q0.x * q1.x) + (q0.y * q1.y) + (q0.z * q1.z) + (q0.w * q1.w);

	// If the angle is small, use linear interpolation
	if (cos_theta > 0.9995f)
	{
		out.x = q0.x + t * (q1.x - q0.x);
		out.y = q0.y + t * (q1.y - q0.y);
		out.z = q0.z + t * (q1.z - q0.z);
		out.w = q0.w + t * (q1.w - q0.w);
		vec4_normalize((FLOAT4&)out);
		return;
	}

	// Calculate the angle between the two quaternions
	float theta = acosf(cos_theta);
	float sin_theta = sinf(theta);

	// Calculate the weights for each quaternion
	float w0 = sinf((1.0f - t) * theta) / sin_theta;
	float w1 = sinf(t * theta) / sin_theta;

	out.x = (w0 * q0.x) + (w1 * q1.x);
	out.y = (w0 * q0.y) + (w1 * q1.y);
	out.z = (w0 * q0.z) + (w1 * q1.z);
	out.w = (w0 * q0.w) + (w1 * q1.w);
}

inline QUAT quat_slerp(const QUAT& q0, const QUAT& q1, FLOAT t) noexcept
{
	QUAT r;
	quat_slerp(q0, q1, t, r);
	return r;
}

inline QUAT quat_rotation_roll_pitch_yaw(FLOAT roll, FLOAT pitch, FLOAT yaw) noexcept
{
	QUAT r;
	quat_rotation_roll_pitch_yaw(roll, pitch, yaw, r);
	return r;
}

inline void quat_rotation_roll_pitch_yaw(FLOAT roll, FLOAT pitch, FLOAT yaw, QUAT& out) noexcept
{
	FLOAT c1 = cosf(yaw);
	FLOAT s1 = cosf(yaw);
	FLOAT c2 = cosf(pitch);
	FLOAT s2 = sinf(pitch);
	FLOAT c3 = cosf(roll);
	FLOAT s3 = sinf(roll);
	out.w = sqrtf(1.0f + c1 * c2 + c1 * c3 - s1 * s2 * s3 + c2 * c3) * 0.5f;
	FLOAT w4 = (4.0f * out.w);
	out.x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4;
	out.y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4;
	out.z = (-s1 * s3 + c1 * s2 * c3 + s2) / w4;
}

inline QUAT quat_rotation_roll_pitch_yaw(FLOAT3& v) noexcept
{
	return quat_rotation_roll_pitch_yaw(v.z, v.x, v.y);
}

inline void quat_rotation_roll_pitch_yaw(FLOAT3& v, QUAT& out) noexcept
{
	quat_rotation_roll_pitch_yaw(v.z, v.x, v.y, out);
}

inline QUAT quat_rotation_axis(FLOAT angle, FLOAT3& v) noexcept
{
	QUAT r;
	quat_rotation_axis(angle, v, r);
	return r;
}

inline void quat_rotation_axis(FLOAT angle, FLOAT3& v, QUAT& out) noexcept
{
	quat_rotation_axis(angle, v.x, v.y, v.z, out);
}

inline QUAT quat_rotation_axis(FLOAT angle, FLOAT x, FLOAT y, FLOAT z) noexcept
{
	QUAT r;
	quat_rotation_axis(angle, x, y, z, r);
	return r;
}

inline void quat_rotation_axis(FLOAT angle, FLOAT x, FLOAT y, FLOAT z, QUAT& out) noexcept
{
	// angle = rotation around axis
	float half_angle = angle * 0.5f;
	float sin_half_angle = sinf(half_angle);
	float cos_half_angle = cosf(half_angle);
	out.x = x * sin_half_angle;
	out.y = y * sin_half_angle;
	out.z = z * sin_half_angle;
	out.w = cos_half_angle;
}

inline QUAT quat_rotation_matrix(const MATRIX4& m) noexcept
{
	QUAT r;
	quat_rotation_matrix(m, r);
	return r;
}

inline void quat_rotation_matrix(const MATRIX4& m, QUAT& out) noexcept
{
	// [ 1 - 2*(y^2 + z^2), 2*(x*y - z*w), 2*(x*z + y*w), 0.0f ]
	// [ 2*(x*y + z*w), 1 - 2*(x^2 + z^2), 2*(y*z - x*w), 0.0f ]
	// [ 2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x^2 + y^2), 0.0f ]
	// [ 0.0f, 0.0f, 0.0f, 1.0f ]
	FLOAT4 q;
	FLOAT r22 = m.m22;
	if (r22 <= 0.f)  // x^2 + y^2 >= z^2 + w^2
	{
		FLOAT dif10 = m.m11 - m.m00;
		FLOAT omr22 = 1.f - r22;
		if (dif10 <= 0.f)  // x^2 >= y^2
		{
			FLOAT fourXSqr = omr22 - dif10;
			FLOAT inv4x = 0.5f / sqrtf(fourXSqr);
			q.v[0] = fourXSqr * inv4x;
			q.v[1] = (m.m01 + m.m10) * inv4x;
			q.v[2] = (m.m02 + m.m20) * inv4x;
			q.v[3] = (m.m12 - m.m21) * inv4x;
		}
		else  // y^2 >= x^2
		{
			FLOAT fourYSqr = omr22 + dif10;
			FLOAT inv4y = 0.5f / sqrtf(fourYSqr);
			q.v[0] = (m.m01 + m.m10) * inv4y;
			q.v[1] = fourYSqr * inv4y;
			q.v[2] = (m.m12 + m.m21) * inv4y;
			q.v[3] = (m.m20 - m.m02) * inv4y;
		}
	}
	else  // z^2 + w^2 >= x^2 + y^2
	{
		FLOAT suRight = m.m11 + m.m00;
		FLOAT opr22 = 1.f + r22;
		if (suRight <= 0.f)  // z^2 >= w^2
		{
			FLOAT fourZSqr = opr22 - suRight;
			FLOAT inv4z = 0.5f / sqrtf(fourZSqr);
			q.v[0] = (m.m02 + m.m20) * inv4z;
			q.v[1] = (m.m12 + m.m21) * inv4z;
			q.v[2] = fourZSqr * inv4z;
			q.v[3] = (m.m01 - m.m10) * inv4z;
		}
		else  // w^2 >= z^2
		{
			FLOAT fourWSqr = opr22 + suRight;
			FLOAT inv4w = 0.5f / sqrtf(fourWSqr);
			q.v[0] = (m.m12 - m.m21) * inv4w;
			q.v[1] = (m.m20 - m.m02) * inv4w;
			q.v[2] = (m.m01 - m.m10) * inv4w;
			q.v[3] = fourWSqr * inv4w;
		}
	}

	out.x = q.x;
	out.y = q.y;
	out.z = q.z;
	out.w = q.w;
}

inline QUAT quat_from_lookat(const FLOAT3& forward) noexcept
{
	QUAT r;
	quat_from_lookat(forward, r);
	return r;
}

inline void quat_from_lookat(const FLOAT3& forward, QUAT& out) noexcept
{
	// [ 0.0f, 0.0f, 1.0f ] -> [ forward.x, forward.y, forward.z ]
	// [ 1.0f, 0.0f, 0.0f ] -> [ right.x, right.y, right.z ]
	// [ 0.0f, 1.0f, 0.0f ] -> [ up.x, up.y, up.z ]
	FLOAT3 right = { 1.0f, 0.0f, 0.0f };
	FLOAT3 up = { 0.0f, 1.0f, 0.0f };
	vec3_cross(forward, up, right);
	vec3_cross(right, forward, up);
	quat_rotation_axis(acosf(vec3_dot(forward, up).x), right, out);
}

inline QUAT quat_from_lookat(const FLOAT3& forward, const FLOAT3& up) noexcept
{
	QUAT r;
	quat_from_lookat(forward, up, r);
	return r;
}

inline void quat_from_lookat(const FLOAT3& forward, const FLOAT3& up, QUAT& out) noexcept
{
	// [ 0.0f, 0.0f, 1.0f ] -> [ forward.x, forward.y, forward.z ]
	// [ 1.0f, 0.0f, 0.0f ] -> [ right.x, right.y, right.z ]
	// [ 0.0f, 1.0f, 0.0f ] -> [ up.x, up.y, up.z ]
	FLOAT3 right;
	vec3_cross(forward, up, right);
	quat_rotation_axis(acosf(vec3_dot(forward, up).x), right, out);
}

inline FLOAT3 quat_to_roll_pitch_yaw(const QUAT& q) noexcept
{
	FLOAT3 r;
	quat_to_roll_pitch_yaw(q, r);
	return r;
}

inline void quat_to_roll_pitch_yaw(const QUAT& q, FLOAT3& out) noexcept
{
	// roll = rotation around x-axis
	// pitch = rotation around y-axis
	// yaw = rotation around z-axis
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	out.x = atan2f(sinr_cosp, cosr_cosp); // roll
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (fabsf(sinp) >= 1.0f)
		out.y = copysignf(M_PI / 2.0f, sinp); // pitch
	else
		out.y = asinf(sinp); // pitch
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	out.z = atan2f(siny_cosp, cosy_cosp); // yaw
}

inline FLOAT4 quat_to_axis_angle(const QUAT& q) noexcept
{
	FLOAT3 axis;
	float angle;
	quat_to_axis_angle(q, axis, angle);

	FLOAT4 r;
	r.x = axis.x;
	r.y = axis.y;
	r.z = axis.z;
	r.w = angle;
	return r;
}

inline void quat_to_axis_angle(const QUAT& q, FLOAT3& axis, FLOAT& angle) noexcept
{
	// angle = rotation around axis
	// axis = rotation axis
	float sin_half_angle = sqrtf(1.0f - (q.w * q.w));
	if (sin_half_angle < 0.001f)
	{
		axis.x = q.x;
		axis.y = q.y;
		axis.z = q.z;
	}
	else
	{
		axis.x = q.x / sin_half_angle;
		axis.y = q.y / sin_half_angle;
		axis.z = q.z / sin_half_angle;
	}
	angle = 2.0f * acosf(q.w);
}

inline MATRIX4 quat_to_matrix(const QUAT& q) noexcept
{
	MATRIX4 r;
	quat_to_matrix(q, r);
	return r;
}

inline void quat_to_matrix(const QUAT& q, MATRIX4& out) noexcept
{
	// [ 1 - 2*(y^2 + z^2), 2*(x*y - z*w), 2*(x*z + y*w), 0.0f ]
	// [ 2*(x*y + z*w), 1 - 2*(x^2 + z^2), 2*(y*z - x*w), 0.0f ]
	// [ 2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x^2 + y^2), 0.0f ]
	// [ 0.0f, 0.0f, 0.0f, 1.0f ]
	out.m00 = (1.0f - (2.0f * (q.y * q.y))) - (2.0f * (q.z * q.z));
	out.m10 = (2.0f * (q.x * q.y)) - (2.0f * (q.z * q.w));
	out.m20 = (2.0f * (q.x * q.z)) + (2.0f * (q.y * q.w));
	out.m30 = 0.0f;
	out.m01 = (2.0f * (q.x * q.y)) + (2.0f * (q.z * q.w));
	out.m11 = (1.0f - (2.0f * (q.x * q.x))) - (2.0f * (q.z * q.z));
	out.m21 = (2.0f * (q.y * q.z)) - (2.0f * (q.x * q.w));
	out.m31 = 0.0f;
	out.m02 = (2.0f * (q.x * q.z)) - (2.0f * (q.y * q.w));
	out.m12 = (2.0f * (q.y * q.z)) + (2.0f * (q.x * q.w));
	out.m22 = (1.0f - (2.0f * (q.x * q.x))) - (2.0f * (q.y * q.y));
	out.m32 = 0.0f;
	out.m03 = 0.0f;
	out.m13 = 0.0f;
	out.m23 = 0.0f;
	out.m33 = 1.0f;
}


inline MATRIX4 mat4_zero() noexcept
{
	MATRIX4 r;
	mat4_zero(r);
	return r;
}

inline void mat4_zero(MATRIX4& m) noexcept
{
	m.m00 = 0.0f;
	m.m01 = 0.0f;
	m.m02 = 0.0f;
	m.m03 = 0.0f;
	m.m10 = 0.0f;
	m.m11 = 0.0f;
	m.m12 = 0.0f;
	m.m13 = 0.0f;
	m.m20 = 0.0f;
	m.m21 = 0.0f;
	m.m22 = 0.0f;
	m.m23 = 0.0f;
	m.m30 = 0.0f;
	m.m31 = 0.0f;
	m.m32 = 0.0f;
	m.m33 = 0.0f;
}

inline MATRIX4 mat4_ident() noexcept
{
	MATRIX4 r;
	mat4_ident(r);
	return r;
}

inline void mat4_ident(MATRIX4& m) noexcept
{
	m.m00 = 1.0f;
	m.m01 = 0.0f;
	m.m02 = 0.0f;
	m.m03 = 0.0f;
	m.m10 = 0.0f;
	m.m11 = 1.0f;
	m.m12 = 0.0f;
	m.m13 = 0.0f;
	m.m20 = 0.0f;
	m.m21 = 0.0f;
	m.m22 = 1.0f;
	m.m23 = 0.0f;
	m.m30 = 0.0f;
	m.m31 = 0.0f;
	m.m32 = 0.0f;
	m.m33 = 1.0f;
}

inline bool mat4_is_ident(const MATRIX4& m) noexcept
{
	// Use the integer pipeline to reduce branching to a minimum
	auto pWork = reinterpret_cast<const uint32_t*>(&m.m00);
	// Convert 1.0f to zero and or them together
	uint32_t uOne = pWork[0] ^ 0x3F800000U;
	// Or all the 0.0f entries together
	uint32_t uZero = pWork[1];
	uZero |= pWork[2];
	uZero |= pWork[3];
	// 2nd row
	uZero |= pWork[4];
	uOne |= pWork[5] ^ 0x3F800000U;
	uZero |= pWork[6];
	uZero |= pWork[7];
	// 3rd row
	uZero |= pWork[8];
	uZero |= pWork[9];
	uOne |= pWork[10] ^ 0x3F800000U;
	uZero |= pWork[11];
	// 4th row
	uZero |= pWork[12];
	uZero |= pWork[13];
	uZero |= pWork[14];
	uOne |= pWork[15] ^ 0x3F800000U;
	// If all zero entries are zero, the uZero==0
	uZero &= 0x7FFFFFFF;    // Allow -0.0f
	// If all 1.0f entries are 1.0f, then uOne==0
	uOne |= uZero;
	return (uOne == 0);
}

inline bool mat4_is_nan(const MATRIX4& m) noexcept
{
	size_t i = 16;
	auto work = reinterpret_cast<const uint32_t*>(&m.m00);
	do 
	{
		// Fetch value into integer unit
		uint32_t uTest = work[0];

		// Remove sign
		uTest &= 0x7FFFFFFFU;

		// NaN is 0x7F800001 through 0x7FFFFFFF inclusive
		uTest -= 0x7F800001U;
		if (uTest < 0x007FFFFFU)
		{
			break;      // NaN found
		}
		++work;        // Next entry
	} while (--i);
	return (i != 0);      // i == 0 if nothing matched
}

inline bool mat4_is_infinite(const MATRIX4& m) noexcept
{
	size_t i = 16;
	auto work = reinterpret_cast<const uint32_t*>(&m.m00);
	do 
	{
		// Fetch value into integer unit
		uint32_t uTest = work[0];
		// Remove sign
		uTest &= 0x7FFFFFFFU;
		// INF is 0x7F800000
		if (uTest == 0x7F800000U)
		{
			break;      // INF found
		}
		++work;        // Next entry
	} while (--i);
	return (i != 0);      // i == 0 if nothing matched
}

inline MATRIX4 mat4_set(
	FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
	FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
	FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
	FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33
) noexcept
{
	MATRIX4 r;
	r.m00 = m00;
	r.m01 = m01;
	r.m02 = m02;
	r.m03 = m03;
	r.m10 = m10;
	r.m11 = m11;
	r.m12 = m12;
	r.m13 = m13;
	r.m20 = m20;
	r.m21 = m21;
	r.m22 = m22;
	r.m23 = m23;
	r.m30 = m30;
	r.m31 = m31;
	r.m32 = m32;
	r.m33 = m33;
	return r;
}

inline void mat4_set(
	FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
	FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
	FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
	FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33,
	MATRIX4& out) noexcept
{
	out.m00 = m00;
	out.m01 = m01;
	out.m02 = m02;
	out.m03 = m03;
	out.m10 = m10;
	out.m11 = m11;
	out.m12 = m12;
	out.m13 = m13;
	out.m20 = m20;
	out.m21 = m21;
	out.m22 = m22;
	out.m23 = m23;
	out.m30 = m30;
	out.m31 = m31;
	out.m32 = m32;
	out.m33 = m33;
}

inline MATRIX4 mat4_trans(const FLOAT3& pos) noexcept
{
	MATRIX4 r;
	mat4_ident(r);
	r.m30 = pos.x;
	r.m31 = pos.y;
	r.m32 = pos.z;
	return r;
}

inline void mat4_trans(const FLOAT3& pos, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m30 = pos.x;
	out.m31 = pos.y;
	out.m32 = pos.z;
}

inline FLOAT3 mat4_get_trans(const MATRIX4& m) noexcept
{
	FLOAT3 r;
	r.x = m.m30;
	r.y = m.m31;
	r.z = m.m32;
	return r;
}

inline void mat4_get_trans(const MATRIX4& m, FLOAT3& out) noexcept
{
	out.x = m.m30;
	out.y = m.m31;
	out.z = m.m32;
}

inline MATRIX4 mat4_trans(FLOAT x, FLOAT y, FLOAT z) noexcept
{
	MATRIX4 r;
	mat4_trans(x, y, z, r);
	return r;
}

inline void mat4_trans(FLOAT x, FLOAT y, FLOAT z, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m30 = x;
	out.m31 = y;
	out.m32 = z;
}

inline MATRIX4 mat4_transpose(const MATRIX4& m) noexcept
{
	MATRIX4 r;
	mat4_transpose(m, r);
	return r;
}

inline void mat4_transpose(const MATRIX4& m, MATRIX4& out) noexcept
{
	out.m00 = m.m00;
	out.m10 = m.m01;
	out.m20 = m.m02;
	out.m30 = m.m03;
	out.m01 = m.m10;
	out.m11 = m.m11;
	out.m21 = m.m12;
	out.m31 = m.m13;
	out.m02 = m.m20;
	out.m12 = m.m21;
	out.m22 = m.m22;
	out.m32 = m.m23;
	out.m03 = m.m30;
	out.m13 = m.m31;
	out.m23 = m.m32;
	out.m33 = m.m33;
}

inline MATRIX4 mat4_inverse(const MATRIX4& m) noexcept
{
	MATRIX4 r;
	mat4_inverse(m, r);
	return r;
}

inline void mat4_inverse(const MATRIX4& m, MATRIX4& out) noexcept
{
	// Calculate the determinant of the matrix
	float det =
		m.m00 * (m.m11 * (m.m22 * m.m33 - m.m32 * m.m23) -
			m.m12 * (m.m21 * m.m33 - m.m31 * m.m23) +
			m.m13 * (m.m21 * m.m32 - m.m31 * m.m22)) -
		m.m01 * (m.m10 * (m.m22 * m.m33 - m.m32 * m.m23) -
			m.m12 * (m.m20 * m.m33 - m.m30 * m.m23) +
			m.m13 * (m.m20 * m.m32 - m.m30 * m.m22)) +
		m.m02 * (m.m10 * (m.m21 * m.m33 - m.m31 * m.m23) -
			m.m11 * (m.m20 * m.m33 - m.m30 * m.m23) +
			m.m13 * (m.m20 * m.m31 - m.m30 * m.m21)) -
		m.m03 * (m.m10 * (m.m21 * m.m32 - m.m31 * m.m22) -
			m.m11 * (m.m20 * m.m32 - m.m30 * m.m22) +
			m.m12 * (m.m20 * m.m31 - m.m30 * m.m21));

	// If determinant is close to zero, return identity matrix
	if (fabsf(det) < 1e-6f)
	{
		mat4_ident(out);
		return;
	}

	float inv_det = 1.0f / det;

	// Calculate the adjugate matrix and multiply by 1/det
	out.m00 = inv_det * (m.m11 * (m.m22 * m.m33 - m.m32 * m.m23) -
		m.m12 * (m.m21 * m.m33 - m.m31 * m.m23) +
		m.m13 * (m.m21 * m.m32 - m.m31 * m.m22));
	out.m01 = -inv_det * (m.m01 * (m.m22 * m.m33 - m.m32 * m.m23) -
		m.m02 * (m.m21 * m.m33 - m.m31 * m.m23) +
		m.m03 * (m.m21 * m.m32 - m.m31 * m.m22));
	out.m02 = inv_det * (m.m01 * (m.m12 * m.m33 - m.m32 * m.m13) -
		m.m02 * (m.m11 * m.m33 - m.m31 * m.m13) +
		m.m03 * (m.m11 * m.m32 - m.m31 * m.m12));
	out.m03 = -inv_det * (m.m01 * (m.m12 * m.m23 - m.m22 * m.m13) -
		m.m02 * (m.m11 * m.m23 - m.m21 * m.m13) +
		m.m03 * (m.m11 * m.m22 - m.m21 * m.m12));

	// Repeat for the remaining rows
	// Row 1
	out.m10 = -inv_det * (m.m10 * (m.m22 * m.m33 - m.m32 * m.m23) -
		m.m12 * (m.m20 * m.m33 - m.m30 * m.m23) +
		m.m13 * (m.m20 * m.m32 - m.m30 * m.m22));
	out.m11 = inv_det * (m.m00 * (m.m22 * m.m33 - m.m32 * m.m23) -
		m.m02 * (m.m20 * m.m33 - m.m30 * m.m23) +
		m.m03 * (m.m20 * m.m32 - m.m30 * m.m22));
	out.m12 = -inv_det * (m.m00 * (m.m12 * m.m33 - m.m32 * m.m13) -
		m.m02 * (m.m10 * m.m33 - m.m30 * m.m13) +
		m.m03 * (m.m10 * m.m32 - m.m30 * m.m12));
	out.m13 = inv_det * (m.m00 * (m.m12 * m.m23 - m.m22 * m.m13) -
		m.m02 * (m.m10 * m.m23 - m.m20 * m.m13) +
		m.m03 * (m.m10 * m.m22 - m.m20 * m.m12));

	// Row 2
	out.m20 = inv_det * (m.m10 * (m.m21 * m.m33 - m.m31 * m.m23) -
		m.m11 * (m.m20 * m.m33 - m.m30 * m.m23) +
		m.m13 * (m.m20 * m.m31 - m.m30 * m.m21));
	out.m21 = -inv_det * (m.m00 * (m.m21 * m.m33 - m.m31 * m.m23) -
		m.m01 * (m.m20 * m.m33 - m.m30 * m.m23) +
		m.m03 * (m.m20 * m.m31 - m.m30 * m.m21));
	out.m22 = inv_det * (m.m00 * (m.m11 * m.m33 - m.m31 * m.m13) -
		m.m01 * (m.m10 * m.m33 - m.m30 * m.m13) +
		m.m03 * (m.m10 * m.m31 - m.m30 * m.m11));
	out.m23 = -inv_det * (m.m00 * (m.m11 * m.m23 - m.m21 * m.m13) -
		m.m01 * (m.m10 * m.m23 - m.m20 * m.m13) +
		m.m03 * (m.m10 * m.m21 - m.m20 * m.m11));

	// Row 3
	out.m30 = -inv_det * (m.m10 * (m.m21 * m.m32 - m.m31 * m.m22) -
		m.m11 * (m.m20 * m.m32 - m.m30 * m.m22) +
		m.m12 * (m.m20 * m.m31 - m.m30 * m.m21));
	out.m31 = inv_det * (m.m00 * (m.m21 * m.m32 - m.m31 * m.m22) -
		m.m01 * (m.m20 * m.m32 - m.m30 * m.m22) +
		m.m02 * (m.m20 * m.m31 - m.m30 * m.m21));
	out.m32 = -inv_det * (m.m00 * (m.m11 * m.m32 - m.m31 * m.m12) -
		m.m01 * (m.m10 * m.m32 - m.m30 * m.m12) +
		m.m02 * (m.m10 * m.m31 - m.m30 * m.m11));
	out.m33 = inv_det * (m.m00 * (m.m11 * m.m22 - m.m21 * m.m12) -
		m.m01 * (m.m10 * m.m22 - m.m20 * m.m12) +
		m.m02 * (m.m10 * m.m21 - m.m20 * m.m11));
}

inline MATRIX4 mat4_mul(const MATRIX4& m0, const MATRIX4& m1) noexcept
{
	MATRIX4 r;
	mat4_mul(m0, m1, r);
	return r;
}

inline void mat4_mul(const MATRIX4& m0, const MATRIX4& m1, MATRIX4& out) noexcept
{
	/*
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			out->row[i][j] = m0->row[i][0] * m1->row[0][j] + m0->row[i][1] * m1->row[1][j] + m0->row[i][2] * m1->row[2][j] + m0->row[i][3] * m1->row[3][j];
		}
	}
	*/
	// 결과 행렬을 저장할 임시 행렬
	MATRIX4 temp;

	// 행렬 곱셈 수행
	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++) 
		{
			// m0의 i번째 행과 m1의 j번째 열의 내적 계산
			temp.row[i][j] = 
				m0.row[i][0] * m1.row[0][j] +
				m0.row[i][1] * m1.row[1][j] +
				m0.row[i][2] * m1.row[2][j] +
				m0.row[i][3] * m1.row[3][j];
		}
	}

	// 결과를 출력 행렬에 복사
	// (입력과 출력이 같은 행렬일 수 있으므로 임시 행렬 사용)
	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++) 
		{
			out.row[i][j] = temp.row[i][j];
		}
	}
}

inline FLOAT3 mat4_mul(const MATRIX4& m, const FLOAT3& v) noexcept
{
	FLOAT3 r;
	mat4_mul(m, v, r);
	return r;
}

inline void mat4_mul(const MATRIX4& m, const FLOAT3& v, FLOAT3& out) noexcept
{
	out.x = (m.m00 * v.x) + (m.m10 * v.y) + (m.m20 * v.z) + m.m30;
	out.y = (m.m01 * v.x) + (m.m11 * v.y) + (m.m21 * v.z) + m.m31;
	out.z = (m.m02 * v.x) + (m.m12 * v.y) + (m.m22 * v.z) + m.m32;
}

inline FLOAT4 mat4_mul(const MATRIX4& m, const FLOAT4& v) noexcept
{
	FLOAT4 r;
	mat4_mul(m, v, r);
	return r;
}

inline void mat4_mul(const MATRIX4& m, const FLOAT4& v, FLOAT4& out) noexcept
{
	out.x = (m.m00 * v.x) + (m.m10 * v.y) + (m.m20 * v.z) + m.m30 * v.w;
	out.y = (m.m01 * v.x) + (m.m11 * v.y) + (m.m21 * v.z) + m.m31 * v.w;
	out.z = (m.m02 * v.x) + (m.m12 * v.y) + (m.m22 * v.z) + m.m32 * v.w;
	out.w = (m.m03 * v.x) + (m.m13 * v.y) + (m.m23 * v.z) + m.m33 * v.w;
}

inline MATRIX4 mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz) noexcept
{
	MATRIX4 r;
	mat4_scale(sx, sy, sz, r);
	return r;
}

inline void mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = sx;
	out.m11 = sy;
	out.m22 = sz;
}

inline MATRIX4 mat4_scale(const FLOAT3& v) noexcept
{
	MATRIX4 r;
	mat4_scale(v.x, v.y, v.z, r);
	return r;
}

inline void mat4_scale(const FLOAT3& v, MATRIX4& out) noexcept
{
	mat4_scale(v.x, v.y, v.z, out);
}

inline MATRIX4 mat4_rotation_x(FLOAT angle) noexcept
{
	MATRIX4 r;
	mat4_rotation_x(angle, r);
	return r;
}

inline void mat4_rotation_x(FLOAT angle, MATRIX4& out) noexcept
{
	mat4_ident(out);
	float c = cosf(angle);
	float s = sinf(angle);
	out.m11 = c;
	out.m12 = s;
	out.m21 = -s;
	out.m22 = c;
}

inline MATRIX4 mat4_rotation_y(FLOAT angle) noexcept
{
	MATRIX4 r;
	mat4_rotation_y(angle, r);
	return r;
}

inline void mat4_rotation_y(FLOAT angle, MATRIX4& out) noexcept
{
	mat4_ident(out);
	float c = cosf(angle);
	float s = sinf(angle);
	out.m00 = c;
	out.m02 = -s;
	out.m20 = s;
	out.m22 = c;
}

inline MATRIX4 mat4_rotation_z(FLOAT angle) noexcept
{
	MATRIX4 r;
	mat4_rotation_z(angle, r);
	return r;
}

inline void mat4_rotation_z(FLOAT angle, MATRIX4& out) noexcept
{
	mat4_ident(out);
	float c = cosf(angle);
	float s = sinf(angle);
	out.m00 = c;
	out.m01 = s;
	out.m10 = -s;
	out.m11 = c;
}
inline MATRIX4 mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll) noexcept
{
	MATRIX4 r;
	mat4_rotation_yaw_pitch_roll(yaw, pitch, roll, r);
	return r;
}

inline void mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll, MATRIX4& out) noexcept
{
	mat4_ident(out);
	float sy = sinf(yaw);
	float cy = cosf(yaw);
	float sp = sinf(pitch);
	float cp = cosf(pitch);
	float sr = sinf(roll);
	float cr = cosf(roll);

	// Yaw(Y축), Pitch(X축), Roll(Z축) 순서로 회전 적용
	// 최종 회전 행렬 계산
	out.m00 = cy * cr + sy * sp * sr;
	out.m01 = -cy * sr + sy * sp * cr;
	out.m02 = sy * cp;
	out.m03 = 0.0f;

	out.m10 = sr * cp;
	out.m11 = cr * cp;
	out.m12 = -sp;
	out.m13 = 0.0f;

	out.m20 = -sy * cr + cy * sp * sr;
	out.m21 = sy * sr + cy * sp * cr;
	out.m22 = cy * cp;
	out.m23 = 0.0f;

	out.m30 = 0.0f;
	out.m31 = 0.0f;
	out.m32 = 0.0f;
	out.m33 = 1.0f;
}

inline MATRIX4 mat4_rotation_axis(const FLOAT3& axis, FLOAT angle) noexcept
{
	MATRIX4 r;
	mat4_rotation_axis(axis, angle, r);
	return r;
}

inline void mat4_rotation_axis(const FLOAT3& axis, FLOAT angle, MATRIX4& out) noexcept
{
	// 축 벡터 정규화
	float length = sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);

	// 0으로 나누기 방지
	if (length < 0.000001f) 
	{
		// 단위 행렬 반환
		out.m00 = 1.0f; out.m01 = 0.0f; out.m02 = 0.0f; out.m03 = 0.0f;
		out.m10 = 0.0f; out.m11 = 1.0f; out.m12 = 0.0f; out.m13 = 0.0f;
		out.m20 = 0.0f; out.m21 = 0.0f; out.m22 = 1.0f; out.m23 = 0.0f;
		out.m30 = 0.0f; out.m31 = 0.0f; out.m32 = 0.0f; out.m33 = 1.0f;
		return;
	}

	// 정규화된 축 벡터
	float x = axis.x / length;
	float y = axis.y / length;
	float z = axis.z / length;

	// 사인과 코사인 값 계산
	float s = sinf(angle);
	float c = cosf(angle);
	float t = 1.0f - c;  // 1-코사인

	// 로드리게스 회전 공식을 사용하여 행렬 계산
	out.m00 = t * x * x + c;
	out.m01 = t * x * y - s * z;
	out.m02 = t * x * z + s * y;
	out.m03 = 0.0f;

	out.m10 = t * x * y + s * z;
	out.m11 = t * y * y + c;
	out.m12 = t * y * z - s * x;
	out.m13 = 0.0f;

	out.m20 = t * x * z - s * y;
	out.m21 = t * y * z + s * x;
	out.m22 = t * z * z + c;
	out.m23 = 0.0f;

	out.m30 = 0.0f;
	out.m31 = 0.0f;
	out.m32 = 0.0f;
	out.m33 = 1.0f;
}

inline MATRIX4 mat4_rotation_quat(const QUAT& q) noexcept
{
	MATRIX4 r;
	mat4_rotation_quat(q, r);
	return r;
}

inline void mat4_rotation_quat(const QUAT& q, MATRIX4& out) noexcept
{
	// 쿼터니언 성분
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;

	// 쿼터니언의 노말(norm)을 계산
	float norm = w * w + x * x + y * y + z * z;

	// 처리하기 전에 노말이 0에 가까운지 확인 (0으로 나누기 방지)
	if (norm < 0.000001f) 
	{
		// 단위 행렬 반환
		out.m00 = 1.0f; out.m01 = 0.0f; out.m02 = 0.0f; out.m03 = 0.0f;
		out.m10 = 0.0f; out.m11 = 1.0f; out.m12 = 0.0f; out.m13 = 0.0f;
		out.m20 = 0.0f; out.m21 = 0.0f; out.m22 = 1.0f; out.m23 = 0.0f;
		out.m30 = 0.0f; out.m31 = 0.0f; out.m32 = 0.0f; out.m33 = 1.0f;
		return;
	}

	// 단위 쿼터니언으로 정규화하기 위한 요소
	float s = 2.0f / norm;

	// 행렬 계산에 사용할 중간 값
	float xx = x * x * s;
	float yy = y * y * s;
	float zz = z * z * s;
	float xy = x * y * s;
	float xz = x * z * s;
	float yz = y * z * s;
	float wx = w * x * s;
	float wy = w * y * s;
	float wz = w * z * s;

	// 쿼터니언에서 회전 행렬 계산
	out.m00 = 1.0f - (yy + zz);
	out.m01 = xy - wz;
	out.m02 = xz + wy;
	out.m03 = 0.0f;

	out.m10 = xy + wz;
	out.m11 = 1.0f - (xx + zz);
	out.m12 = yz - wx;
	out.m13 = 0.0f;

	out.m20 = xz - wy;
	out.m21 = yz + wx;
	out.m22 = 1.0f - (xx + yy);
	out.m23 = 0.0f;

	out.m30 = 0.0f;
	out.m31 = 0.0f;
	out.m32 = 0.0f;
	out.m33 = 1.0f;
}

inline MATRIX4 mat4_lookat_lh(const FLOAT3& eye, const FLOAT3& dir, const FLOAT3& up) noexcept
{
	MATRIX4 r;
	mat4_lookat_lh(eye, dir, up, r);
	return r;
}

inline void mat4_lookat_lh(const FLOAT3& eye, const FLOAT3& dir, const FLOAT3& up, MATRIX4& out) noexcept
{
	FLOAT3 ndir = dir;
	vec3_normalize(ndir);

	FLOAT3 nup = up;
	vec3_normalize(nup);

	// right = up × dir
	FLOAT3 right;
	vec3_cross(nup, ndir, right);

	// dir과 up이 거의 평행한지 확인
	FLOAT3 rightLen = vec3_length(right);
	if (rightLen.x < 1e-6f) 
	{
		// 폴백: 임의의 직교 벡터 선택
		if (fabsf(ndir.y) < 0.999f) 
		{
			FLOAT3 worldUp = { 0.0f, 1.0f, 0.0f };
			vec3_cross(worldUp, ndir, right);
		}
		else 
		{
			FLOAT3 worldRight = { 1.0f, 0.0f, 0.0f };
			vec3_cross(ndir, worldRight, right);
		}
	}

	vec3_normalize(right);

	// up = dir × right (재직교화)
	vec3_cross(ndir, right, nup);
	vec3_normalize(nup);

	// 행렬 설정 (기존 코드와 동일)
	out.m00 = right.x;
	out.m10 = right.y;
	out.m20 = right.z;
	out.m30 = -vec3_dot_f(right, eye);

	out.m01 = nup.x;
	out.m11 = nup.y;
	out.m21 = nup.z;
	out.m31 = -vec3_dot_f(nup, eye);

	out.m02 = ndir.x;
	out.m12 = ndir.y;
	out.m22 = ndir.z;
	out.m32 = -vec3_dot_f(ndir, eye);

	out.m03 = 0.0f;
	out.m13 = 0.0f;
	out.m23 = 0.0f;
	out.m33 = 1.0f;
}

inline MATRIX4 mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, FLOAT3& up) noexcept
{
	MATRIX4 r;
	mat4_lookto_lh(eye, to, up, r);
	return r;
}

inline void mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept
{
	FLOAT3 right, upn, dir;

	vec3_sub(to, eye, dir);
	vec3_normalize(dir);
	
	vec3_cross(up, dir, right);
	vec3_normalize(right);

	vec3_cross(dir, right, upn);
	vec3_normalize(upn);
	out.m00 = right.x;
	out.m10 = right.y;
	out.m20 = right.z;
	out.m30 = -vec3_dot_f(right, eye);
	out.m01 = upn.x;
	out.m11 = upn.y;
	out.m21 = upn.z;
	out.m31 = -vec3_dot_f(upn, eye);
	out.m02 = dir.x;
	out.m12 = dir.y;
	out.m22 = dir.z;
	out.m32 = -vec3_dot_f(dir, eye);
	out.m03 = 0.0f;
	out.m13 = 0.0f;
	out.m23 = 0.0f;
	out.m33 = 1.0f;
}

inline MATRIX4 mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up) noexcept
{
	MATRIX4 r;
	mat4_lookto_rh(eye, to, up, r);
	return r;
}

inline void mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept
{
	FLOAT3 right, upn, dir;

	vec3_sub(to, eye, dir);
	vec3_normalize(dir);

	vec3_cross(up, dir, right);
	vec3_normalize(right);
	vec3_cross(dir, right, upn);
	vec3_normalize(upn);

	out.m00 = right.x;
	out.m10 = right.y;
	out.m20 = right.z;
	out.m30 = -vec3_dot_f(right, eye);
	out.m01 = upn.x;
	out.m11 = upn.y;
	out.m21 = upn.z;
	out.m31 = -vec3_dot_f(upn, eye);
	out.m02 = -dir.x;
	out.m12 = -dir.y;
	out.m22 = -dir.z;
	out.m32 = vec3_dot_f(dir, eye);
	out.m03 = 0.0f;
	out.m13 = 0.0f;
	out.m23 = 0.0f;
	out.m33 = 1.0f;
}

inline MATRIX4 mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_perspective_lh(fov, aspect, nearZ, farZ, r);
	return r;
}

inline void mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);

	float yScale = 1.0f / tanf(fov * 0.5f);
	float xScale = yScale / aspect;
	float zRange = farZ / (farZ - nearZ);

	out.m[0] = xScale;  // [0,0]
	out.m[5] = yScale;  // [1,1]
	out.m[10] = zRange;  // [2,2]
	out.m[11] = 1.0f;    // [2,3]
	out.m[14] = -nearZ * zRange;  // [3,2]

	/*
	out->m00 = 1.0f / (aspect * tanf(fov * 0.5f));
	out->m11 = 1.0f / tanf(fov * 0.5f);
	out->m22 = farZ / (farZ - nearZ);
	out->m23 = 1.0f;
	out->m32 = (farZ * nearZ) / (nearZ - farZ);
	out->m33 = 0.0f;
	*/
}

inline MATRIX4 mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_perspective_rh(fov, aspect, nearZ, farZ, r);
	return r;
}

inline void mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 1.0f / (aspect * tanf(fov * 0.5f));
	out.m11 = 1.0f / tanf(fov * 0.5f);
	out.m22 = farZ / (nearZ - farZ);
	out.m23 = -1.0f;
	out.m32 = (farZ * nearZ) / (nearZ - farZ);
	out.m33 = 0.0f;
}

inline MATRIX4 mat4_perspective_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_perspective_lh(left, right, top, bottom, nearZ, farZ, r);
	return r;
}

inline void mat4_perspective_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 2.0f * nearZ / (right - left);
	out.m11 = -2.0f * nearZ / (bottom - top);
	out.m20 = -1.0f - 2.0f * left / (right - left);
	out.m21 = 1.0f + 2.0f * top / (bottom - top);
	out.m22 = -farZ / (nearZ - farZ);
	out.m32 = (nearZ * farZ) / (nearZ - farZ);
	out.m23 = 1.0f;
	out.m33 = 0.0f;
}

inline MATRIX4 mat4_perspective_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_perspective_rh(left, right, top, bottom, nearZ, farZ, r);
	return r;
}

inline void mat4_perspective_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 2.0f * nearZ / (right - left);
	out.m11 = -2.0f * nearZ / (bottom - top);
	out.m20 = 1.0f + 2.0f * left / (right - left);
	out.m21 = -1.0f - 2.0f * top / (bottom - top);
	out.m22 = farZ / (nearZ - farZ);
	out.m32 = (nearZ * farZ) / (nearZ - farZ);
	out.m23 = -1.0f;
	out.m33 = 0.0f;
}

inline MATRIX4 mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_ortho_lh(width, height, nearZ, farZ, r);
	return r;
}

inline void mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 2.0f / width;
	out.m11 = 2.0f / height;
	out.m22 = 1.0f / (farZ - nearZ);
	out.m32 = nearZ / (nearZ - farZ);
}

inline MATRIX4 mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_ortho_rh(width, height, nearZ, farZ, r);
	return r;
}

inline void mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 2.0f / width;
	out.m11 = 2.0f / height;
	out.m22 = 1.0f / (nearZ - farZ);
	out.m32 = nearZ / (nearZ - farZ);
}

inline MATRIX4 mat4_ortho_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_ortho_lh(left, right, top, bottom, nearZ, farZ, r);
	return r;
}

inline void mat4_ortho_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 2.0f / (right - left);
	out.m11 = 2.0f / (top - bottom);
	out.m22 = 1.0f / (farZ - nearZ);
	out.m30 = -1.0f - 2.0f * left / (right - left);
	out.m31 = 1.0f + 2.0f * top / (bottom - top);
	out.m32 = nearZ / (nearZ - farZ);
}

inline MATRIX4 mat4_ortho_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_ortho_rh(left, right, top, bottom, nearZ, farZ, r);
	return r;
}

inline void mat4_ortho_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	mat4_ident(out);
	out.m00 = 2.0f / (right - left);
	out.m11 = 2.0f / (top - bottom);
	out.m22 = 1.0f / (nearZ - farZ);
	out.m30 = -1.0f - 2.0f * left / (right - left);
	out.m31 = 1.0f + 2.0f * top / (bottom - top);
	out.m32 = nearZ / (nearZ - farZ);
}

inline MATRIX4 mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_frustum_lh(left, right, bottom, top, nearZ, farZ, r);
	return r;
}

inline void mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	/*
	// 나눗셈에서 0으로 나누기 방지 검사
	if (right == left || top == bottom || nearZ == farZ || nearZ <= 0 || farZ <= 0) 
	{
		// 오류 상황에서는 단위 행렬 반환
		out->m00 = 1.0f; out->m01 = 0.0f; out->m02 = 0.0f; out->m03 = 0.0f;
		out->m10 = 0.0f; out->m11 = 1.0f; out->m12 = 0.0f; out->m13 = 0.0f;
		out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = 1.0f; out->m23 = 0.0f;
		out->m30 = 0.0f; out->m31 = 0.0f; out->m32 = 0.0f; out->m33 = 1.0f;
		return;
	}
	*/

	// 투영 행렬 계산에 필요한 값들
	float width = right - left;
	float height = top - bottom;
	float depth = farZ - nearZ;

	// 왼손 좌표계 절두체 투영 행렬 계산
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

inline MATRIX4 mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept
{
	MATRIX4 r;
	mat4_frustum_rh(left, right, bottom, top, nearZ, farZ, r);
	return r;
}

inline void mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
{
	/*
	// 나눗셈에서 0으로 나누기 방지 검사
	if (right == left || top == bottom || nearZ == farZ || nearZ <= 0 || farZ <= 0) 
	{
		// 오류 상황에서는 단위 행렬 반환
		out->m00 = 1.0f; out->m01 = 0.0f; out->m02 = 0.0f; out->m03 = 0.0f;
		out->m10 = 0.0f; out->m11 = 1.0f; out->m12 = 0.0f; out->m13 = 0.0f;
		out->m20 = 0.0f; out->m21 = 0.0f; out->m22 = 1.0f; out->m23 = 0.0f;
		out->m30 = 0.0f; out->m31 = 0.0f; out->m32 = 0.0f; out->m33 = 1.0f;
		return;
	}
	*/

	// 투영 행렬 계산에 필요한 값들
	float width = right - left;
	float height = top - bottom;
	float depth = farZ - nearZ;

	// 오른손 좌표계 절두체 투영 행렬 계산
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

inline void compute_transform(LOCAL_TRANSFORM& tf)
{
	MATRIX4 s, r, t, sr;
	mat4_scale((FLOAT3&)tf.Scale, s);
	mat4_rotation_quat(tf.Rotation, r);
	mat4_mul(s, r, sr);
	mat4_trans((FLOAT3&)tf.Position, t);
	mat4_mul(sr, t, tf.TM);
}

