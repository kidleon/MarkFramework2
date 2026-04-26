namespace mark
{
	/**
	* @brief 벡터를 생성합니다. 각 요소에 지정된 값을 대입한 결과입니다.
	*/
	inline FLOAT2 math_set(float x, float y)
	{
		return FLOAT2{ x, y };
	}

	inline FLOAT3 math_set(float x, float y, float z)
	{
		return FLOAT3{ x, y, z };
	}

	inline FLOAT4 math_set(float x, float y, float z, float w)
	{
		return FLOAT4{ x, y, z, w };
	}


	/**
	* @brief 벡터를 요소별로 더합니다.
	*/
	inline FLOAT2 math_add(FLOAT2 a, FLOAT2 b)
	{
		return FLOAT2{ a.x + b.x, a.y + b.y };
	}

	inline FLOAT3 math_add(FLOAT3 a, FLOAT3 b)
	{
		return FLOAT3{ a.x + b.x, a.y + b.y, a.z + b.z };
	}

	inline FLOAT4 math_add(FLOAT4 a, FLOAT4 b)
	{
		return FLOAT4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	}

	/**
	* @brief 벡터를 요소별로 뺍니다.
	*/
	inline FLOAT2 math_sub(FLOAT2 a, FLOAT2 b)
	{
		return FLOAT2{ a.x - b.x, a.y - b.y };
	}

	inline FLOAT3 math_sub(FLOAT3 a, FLOAT3 b)
	{
		return FLOAT3{ a.x - b.x, a.y - b.y, a.z - b.z };
	}

	inline FLOAT4 math_sub(FLOAT4 a, FLOAT4 b)
	{
		return FLOAT4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	}

	/**
	* @brief 벡터를 요소별로 곱합니다.
	*/
	inline FLOAT2 math_mul(FLOAT2 a, FLOAT2 b)
	{
		return FLOAT2{ a.x * b.x, a.y * b.y };
	}

	inline FLOAT3 math_mul(FLOAT3 a, FLOAT3 b)
	{
		return FLOAT3{ a.x * b.x, a.y * b.y, a.z * b.z };
	}

	inline FLOAT4 math_mul(FLOAT4 a, FLOAT4 b)
	{
		return FLOAT4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
	}

	/**
	* @brief 벡터를 스칼라 값으로 곱합니다. 각 요소를 스칼라 값으로 곱한 결과입니다.
	*/
	inline FLOAT2 math_mul(FLOAT2 a, float scalar)
	{
		return FLOAT2{ a.x * scalar, a.y * scalar };
	}

	inline FLOAT3 math_mul(FLOAT3 a, float scalar)
	{
		return FLOAT3{ a.x * scalar, a.y * scalar, a.z * scalar };
	}

	inline FLOAT4 math_mul(FLOAT4 a, float scalar)
	{
		return FLOAT4{ a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar };
	}

	/**
	* @brief 벡터를 요소별로 나눕니다.
	*/
	inline FLOAT2 math_div(FLOAT2 a, FLOAT2 b)
	{
		return FLOAT2{ a.x / b.x, a.y / b.y };
	}

	inline FLOAT3 math_div(FLOAT3 a, FLOAT3 b)
	{
		return FLOAT3{ a.x / b.x, a.y / b.y, a.z / b.z };
	}

	inline FLOAT4 math_div(FLOAT4 a, FLOAT4 b)
	{
		return FLOAT4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
	}

	/**
	* @brief 벡터를 스칼라 값으로 나눕니다. 각 요소를 스칼라 값으로 나눈 결과입니다.
	*/
	inline FLOAT2 math_div(FLOAT2 a, float scalar)
	{
		return FLOAT2{ a.x / scalar, a.y / scalar };
	}

	inline FLOAT3 math_div(FLOAT3 a, float scalar)
	{
		return FLOAT3{ a.x / scalar, a.y / scalar, a.z / scalar };
	}

	inline FLOAT4 math_div(FLOAT4 a, float scalar)
	{
		return FLOAT4{ a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar };
	}

	/**
	* @brief 벡터의 내적을 계산합니다.
	*/
	inline float math_dot(FLOAT2 a, FLOAT2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float math_dot(FLOAT3 a, FLOAT3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float math_dot(FLOAT4 a, FLOAT4 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	/**
	* @brief 벡터의 내적을 계산합니다. 결과를 out 매개변수에 저장합니다.
	*/
	inline void math_dot(FLOAT2 a, FLOAT2 b, float& out)
	{
		out = math_dot(a, b);
	}

	inline void math_dot(FLOAT3 a, FLOAT3 b, float& out)
	{
		out = math_dot(a, b);
	}

	inline void math_dot(FLOAT4 a, FLOAT4 b, float& out)
	{
		out = math_dot(a, b);
	}


	/**
	* @brief 벡터의 외적을 계산합니다.
	*/
	inline float math_cross(FLOAT2 a, FLOAT2 b) noexcept
	{
		// r > 0 : b媛 a???쇱そ (諛섏떆怨?諛⑺뼢)
		// r < 0 : b媛 a???ㅻⅨ履?(?쒓퀎 諛⑺뼢)
		// r = 0 : ??踰≫꽣媛 ?됲뻾 (媛숈? 諛⑺뼢 or 諛섎? 諛⑺뼢)
		return a.x * b.y - a.y * b.x;
	}

	inline FLOAT3 math_cross(FLOAT3 a, FLOAT3 b) noexcept
	{
		return FLOAT3{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	inline FLOAT4 math_cross(FLOAT4 a, FLOAT4 b, FLOAT4 c) noexcept
	{
		// 4D 외적은 세 벡터에 수직인 벡터를 반환합니다. 계산은 4x4 행렬식으로 수행됩니다.
		return FLOAT4{
			a.y * (b.z * c.w - b.w * c.z) - a.z * (b.y * c.w - b.w * c.y) + a.w * (b.y * c.z - b.z * c.y),
			-a.x * (b.z * c.w - b.w * c.z) + a.z * (b.x * c.w - b.w * c.x) - a.w * (b.x * c.z - b.z * c.x),
			a.x * (b.y * c.w - b.w * c.y) - a.y * (b.x * c.w - b.w * c.x) + a.w * (b.x * c.y - b.y * c.x),
			-a.x * (b.y * c.z - b.z * c.y) + a.y * (b.x * c.z - b.z * c.x) - a.z * (b.x * c.y - b.y * c.x)
		};
	}

	/**
	* @brief 벡터의 외적을 계산합니다. 결과를 out 매개변수에 저장합니다.
	*/
	inline void math_cross(FLOAT2 a, FLOAT2 b, float& out) noexcept
	{
		out = math_cross(a, b);
	}

	inline void math_cross(FLOAT3 a, FLOAT3 b, FLOAT3& out) noexcept
	{
		out = math_cross(a, b);
	}

	inline void math_cross(FLOAT4 a, FLOAT4 b, FLOAT4 c, FLOAT4& out) noexcept
	{
		out = math_cross(a, b, c);
	}

	/**
	* @brief 벡터의 길이를 계산합니다.
	*/
	inline float math_length(FLOAT2 v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	inline float math_length(FLOAT3 v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	inline float math_length(FLOAT4 v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	/**
	* @brief 벡터의 길이의 제곱을 계산합니다. sqrt를 생략하여 더 빠릅니다.
	* 결과는 벡터의 길이의 제곱입니다.
	*/
	inline float math_lengthsq(FLOAT2 v)
	{
		return v.x * v.x + v.y * v.y;
	}

	inline float math_lengthsq(FLOAT3 v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	inline float math_lengthsq(FLOAT4 v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
	}

	/**
	* @brief 벡터를 정규화합니다. 길이를 1로 만듭니다.
	*/
	inline FLOAT2 math_normalized(FLOAT2 v)
	{
		float length = math_length(v);
		return FLOAT2{ v.x / length, v.y / length };
	}

	inline FLOAT3 math_normalized(FLOAT3 v)
	{
		float length = math_length(v);
		return FLOAT3{ v.x / length, v.y / length, v.z / length };
	}

	inline FLOAT4 math_normalized(FLOAT4 v)
	{
		float length = math_length(v);
		return FLOAT4{ v.x / length, v.y / length, v.z / length, v.w / length };
	}

	/**
	* @brief 벡터를 정규화합니다. 원본 벡터를 직접 수정합니다.
	*/
	inline void math_normalize(FLOAT2& v)
	{
		float length = math_length(v);
		v.x /= length;
		v.y /= length;
	}

	inline void math_normalize(FLOAT3& v)
	{
		float length = math_length(v);
		v.x /= length;
		v.y /= length;
		v.z /= length;
	}

	inline void math_normalize(FLOAT4& v)
	{
		float length = math_length(v);
		v.x /= length;
		v.y /= length;
		v.z /= length;
		v.w /= length;
	}

	/**
	* @brief 벡터를 선형 보간합니다. 두 벡터 사이의 선형 보간 결과입니다.
	*/
	inline FLOAT2 math_lerp(FLOAT2 a, FLOAT2 b, float t)
	{
		return FLOAT2{ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
	}

	inline FLOAT3 math_lerp(FLOAT3 a, FLOAT3 b, float t)
	{
		return FLOAT3{ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
	}

	inline FLOAT4 math_lerp(FLOAT4 a, FLOAT4 b, float t)
	{
		return FLOAT4{ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t };
	}

	/**
	* @brief 벡터를 요소별로 클램프합니다. 최소값과 최대값 사이로 제한합니다.
	*/
	inline FLOAT2 math_clamp(FLOAT2 v, FLOAT2 vmin, FLOAT2 vmax)
	{
		return FLOAT2{
			T_MAX(vmin.x, T_MIN(v.x, vmax.x)),
			T_MAX(vmin.y, T_MIN(v.y, vmax.y))
		};
	}

	inline FLOAT3 math_clamp(FLOAT3 v, FLOAT3 vmin, FLOAT3 vmax)
	{
		return FLOAT3{
			T_MAX(vmin.x, T_MIN(v.x, vmax.x)),
			T_MAX(vmin.y, T_MIN(v.y, vmax.y)),
			T_MAX(vmin.z, T_MIN(v.z, vmax.z))
		};
	}

	inline FLOAT4 math_clamp(FLOAT4 v, FLOAT4 vmin, FLOAT4 vmax)
	{
		return FLOAT4{
			T_MAX(vmin.x, T_MIN(v.x, vmax.x)),
			T_MAX(vmin.y, T_MIN(v.y, vmax.y)),
			T_MAX(vmin.z, T_MIN(v.z, vmax.z)),
			T_MAX(vmin.w, T_MIN(v.w, vmax.w))
		};
	}

	/**
	* @brief 벡터를 요소별로 포화합니다. 각 요소를 0과 1 사이로 제한한 결과입니다.
	*/
	inline FLOAT2 math_saturate(FLOAT2 v)
	{
		return FLOAT2{
			T_MAX(0.0f, T_MIN(v.x, 1.0f)),
			T_MAX(0.0f, T_MIN(v.y, 1.0f))
		};
	}

	inline FLOAT3 math_saturate(FLOAT3 v)
	{
		return FLOAT3{
			T_MAX(0.0f, T_MIN(v.x, 1.0f)),
			T_MAX(0.0f, T_MIN(v.y, 1.0f)),
			T_MAX(0.0f, T_MIN(v.z, 1.0f))
		};
	}

	inline FLOAT4 math_saturate(FLOAT4 v)
	{
		return FLOAT4{
			T_MAX(0.0f, T_MIN(v.x, 1.0f)),
			T_MAX(0.0f, T_MIN(v.y, 1.0f)),
			T_MAX(0.0f, T_MIN(v.z, 1.0f)),
			T_MAX(0.0f, T_MIN(v.w, 1.0f))
		};
	}

	/**
	* @brief 벡터를 요소별로 최소값을 계산합니다.
	*/
	inline FLOAT2 math_min(FLOAT2 a, FLOAT2 b)
	{
		return FLOAT2{ T_MIN(a.x, b.x), T_MIN(a.y, b.y) };
	}

	inline FLOAT3 math_min(FLOAT3 a, FLOAT3 b)
	{
		return FLOAT3{ T_MIN(a.x, b.x), T_MIN(a.y, b.y), T_MIN(a.z, b.z) };
	}

	inline FLOAT4 math_min(FLOAT4 a, FLOAT4 b)
	{
		return FLOAT4{ T_MIN(a.x, b.x), T_MIN(a.y, b.y), T_MIN(a.z, b.z), T_MIN(a.w, b.w) };
	}

	/**
	* @brief 벡터를 요소별로 최대값을 계산합니다.
	*/
	inline FLOAT2 math_max(FLOAT2 a, FLOAT2 b)
	{
		return FLOAT2{ T_MAX(a.x, b.x), T_MAX(a.y, b.y) };
	}

	inline FLOAT3 math_max(FLOAT3 a, FLOAT3 b)
	{
		return FLOAT3{ T_MAX(a.x, b.x), T_MAX(a.y, b.y), T_MAX(a.z, b.z) };
	}

	inline FLOAT4 math_max(FLOAT4 a, FLOAT4 b)
	{
		return FLOAT4{ T_MAX(a.x, b.x), T_MAX(a.y, b.y), T_MAX(a.z, b.z), T_MAX(a.w, b.w) };
	}

	/**
	* @brief 벡터를 요소별로 올림합니다.
	*/
	inline FLOAT2 math_ceil(FLOAT2 v)
	{
		return FLOAT2{ std::ceil(v.x), std::ceil(v.y) };
	}

	inline FLOAT3 math_ceil(FLOAT3 v)
	{
		return FLOAT3{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) };
	}

	inline FLOAT4 math_ceil(FLOAT4 v)
	{
		return FLOAT4{ std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w) };
	}

	/**
	* @brief 벡터를 요소별로 내림합니다.
	*/
	inline FLOAT2 math_floor(FLOAT2 v)
	{
		return FLOAT2{ std::floor(v.x), std::floor(v.y) };
	}

	inline FLOAT3 math_floor(FLOAT3 v)
	{
		return FLOAT3{ std::floor(v.x), std::floor(v.y), std::floor(v.z) };
	}

	inline FLOAT4 math_floor(FLOAT4 v)
	{
		return FLOAT4{ std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w) };
	}

	/**
	* @brief 벡터를 요소별로 반올림합니다.
	*/
	inline FLOAT2 math_round(FLOAT2 v)
	{
		return FLOAT2{ std::round(v.x), std::round(v.y) };
	}

	inline FLOAT3 math_round(FLOAT3 v)
	{
		return FLOAT3{ std::round(v.x), std::round(v.y), std::round(v.z) };
	}

	inline FLOAT4 math_round(FLOAT4 v)
	{
		return FLOAT4{ std::round(v.x), std::round(v.y), std::round(v.z), std::round(v.w) };
	}

	/**
	* @brief 벡터를 요소별로 버림합니다.
	*/
	inline FLOAT2 math_trunc(FLOAT2 v)
	{
		return FLOAT2{ std::trunc(v.x), std::trunc(v.y) };
	}

	inline FLOAT3 math_trunc(FLOAT3 v)
	{
		return FLOAT3{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z) };
	}

	inline FLOAT4 math_trunc(FLOAT4 v)
	{
		return FLOAT4{ std::trunc(v.x), std::trunc(v.y), std::trunc(v.z), std::trunc(v.w) };
	}

	/**
	* @brief 쿼터니언의 항등원을 반환합니다.
	*/
	inline QUAT quat_ident()
	{
		return QUAT{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	/**
	* @brief 쿼터니언과 스칼라 값을 곱합니다.
	*/
	inline QUAT quat_mul(QUAT q, float scalar)
	{
		return QUAT{ q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar };
	}

	/**
	* @brief 두 쿼터니언을 곱합니다.
	*/
	inline QUAT quat_mul(QUAT q1, QUAT q2)
	{
		return QUAT{
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
			q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
		};
	}

	/**
	* @brief 쿼터니언과 벡터를 곱합니다. 벡터를 회전한 결과입니다.
	*/
	inline FLOAT3 quat_mul(QUAT q, FLOAT3 v)
	{
		// nVidia SDK implementation
		// FVector3 uv, uuv;
		// FVector3 qvec(x, y, z);
		// uv = qvec.Cross(v);
		// uuv = qvec.Cross(uv);
		// uv *= (2.0f * w);
		// uuv *= 2.0f;
		// return v + uv + uuv;
		FLOAT3 qvec = { q.x, q.y, q.z };

		FLOAT3 uv = math_cross(qvec, v);
		FLOAT3 uuv = math_cross(qvec, uv);

		float qww = q.w + q.w; // 2.0f * q.w

		uv.x *= qww;
		uv.y *= qww;
		uv.z *= qww;
		uuv.x *= 2.0f;
		uuv.y *= 2.0f;
		uuv.z *= 2.0f;

		return FLOAT3{
			v.x + uv.x + uuv.x,
			v.y + uv.y + uuv.y,
			v.z + uv.z + uuv.z
		};
	}

	/**
	* @brief 쿼터니언의 역원을 계산합니다.
	*/
	inline QUAT quat_inverse(QUAT q)
	{
		// Conjugate
		return QUAT{ -q.x, -q.y, -q.z, q.w };
	}

	/**
	* @brief 쿼터니언을 정규화합니다. 길이를 1로 만듭니다.
	*/
	inline QUAT quat_normalize(QUAT q)
	{
		float length = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return QUAT{ q.x / length, q.y / length, q.z / length, q.w / length };
	}

	/**
	* @brief 쿼터니언을 정규화합니다. 원본을 직접 수정합니다.
	*/
	inline void quat_normalized(QUAT& q)
	{
		float length = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		q.x = q.x / length;
		q.y = q.y / length;
		q.z = q.z / length;
		q.w = q.w / length;
	}

	/**
	* @brief 쿼터니언의 켤레를 계산합니다. 벡터 부분을 반전한 결과입니다.
	*/
	inline QUAT quat_conjugate(QUAT q)
	{
		return QUAT{ -q.x, -q.y, -q.z, q.w };
	}

	/**
	* @brief 두 쿼터니언 사이의 구면 선형 보간(Slerp)을 계산합니다.
	*/
	inline QUAT quat_slerp(QUAT q0, QUAT q1, float t)
	{
		// Calculate the dot product (cosine of the angle between the quaternions)
		float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

		// If the dot product is negative, the quaternions have opposite handed-ness and slerp won't take the shorter path. 
		// Fix by reversing one quaternion.
		if (dot < 0.0f)
		{
			q1.x = -q1.x;
			q1.y = -q1.y;
			q1.z = -q1.z;
			q1.w = -q1.w;
			dot = -dot;
		}

		const float DOT_THRESHOLD = 0.99995f;
		if (1.0f - dot <= MX_TINY)
		{
			// If the quaternions are very close, use linear interpolation to avoid division by zero
			return quat_normalize(QUAT{
				q0.x + t * (q1.x - q0.x),
				q0.y + t * (q1.y - q0.y),
				q0.z + t * (q1.z - q0.z),
				q0.w + t * (q1.w - q0.w)
				});
		}

		// Calculate the angle between the two quaternions
		float theta = acosf(dot);
		float sin_theta = sinf(theta);

		// Calculate the weights for each quaternion
		float w0 = sinf((1.0f - t) * theta) / sin_theta;
		float w1 = sinf(t * theta) / sin_theta;

		return QUAT
		{
			(w0 * q0.x) + (w1 * q1.x),
			(w0 * q0.y) + (w1 * q1.y),
			(w0 * q0.z) + (w1 * q1.z),
			(w0 * q0.w) + (w1 * q1.w)
		};
	}

	/**
	* @brief 롤, 피치, 요 각도로부터 회전 쿼터니언을 계산합니다. 각도는 라디안입니다.
	*/
	inline QUAT quat_rotation_roll_pitch_yaw(float roll, float pitch, float yaw)
	{
		// Convert angles from degrees to radians
		float cr = cosf(roll * 0.5f);
		float sr = sinf(roll * 0.5f);
		float cp = cosf(pitch * 0.5f);
		float sp = sinf(pitch * 0.5f);
		float cy = cosf(yaw * 0.5f);
		float sy = sinf(yaw * 0.5f);

		QUAT out;
		out.w = cr * cp * cy + sr * sp * sy;
		out.x = sr * cp * cy - cr * sp * sy;
		out.y = cr * sp * cy + sr * cp * sy;
		out.z = cr * cp * sy - sr * sp * cy;

		return out;
	}

	/**
	* @brief 축과 각도로부터 회전 쿼터니언을 계산합니다.
	*/
	inline QUAT quat_rotation_axis_angle(FLOAT3 axis, float angle)
	{
		float half_angle = angle * 0.5f;
		float s = sinf(half_angle);

		QUAT out;
		out.x = axis.x * s;
		out.y = axis.y * s;
		out.z = axis.z * s;
		out.w = cosf(half_angle);

		return out;
	}

	inline QUAT quat_rotation_lookat(FLOAT3 forward, FLOAT3 up)
	{
		FLOAT3 zaxis = math_normalized(forward);
		FLOAT3 xaxis = math_normalized(math_cross(up, zaxis));
		FLOAT3 yaxis = math_cross(zaxis, xaxis);

		return quat_rotation_axis_angle(xaxis, acosf(math_dot(zaxis, yaxis)));
	}

	/**
	* @brief 두 벡터 사이의 회전을 나타내는 쿼터니언을 계산합니다. from에서 to로 회전합니다.
	*/
	inline QUAT quat_rotation_between(FLOAT3 from, FLOAT3 to)
	{
		FLOAT3 f = math_normalized(from);
		FLOAT3 t = math_normalized(to);

		float cos_theta = math_dot(f, t);
		FLOAT3 rotation_axis;

		if (cos_theta < -1.0f + MX_TINY)
		{
			// 180 degree rotation around any orthogonal vector
			rotation_axis = math_cross(FLOAT3{ 0.0f, 0.0f, 1.0f }, f);
			if (math_lengthsq(rotation_axis) < MX_TINY)
			{
				rotation_axis = math_cross(FLOAT3{ 1.0f, 0.0f, 0.0f }, f);
			}

			return quat_rotation_axis_angle(math_normalized(rotation_axis), MX_PI);
		}

		rotation_axis = math_cross(f, t);
		float s = sqrtf((1.0f + cos_theta) * 2.0f);
		float invs = 1.0f / s;
		return QUAT{
			rotation_axis.x * invs,
			rotation_axis.y * invs,
			rotation_axis.z * invs,
			s * 0.5f
		};
	}

	/**
	* @brief 행렬에서 쿼터니언을 추출합니다. 행렬이 나타내는 회전을 쿼터니언으로 변환합니다.
	*/
	inline QUAT quat_from_matrix(const MATRIX4& m) noexcept
	{
		// [ 1 - 2*(y^2 + z^2), 2*(x*y - z*w), 2*(x*z + y*w), 0.0f ]
		// [ 2*(x*y + z*w), 1 - 2*(x^2 + z^2), 2*(y*z - x*w), 0.0f ]
		// [ 2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x^2 + y^2), 0.0f ]
		// [ 0.0f, 0.0f, 0.0f, 1.0f ]

		FLOAT4 q;
		float r22 = m.m22;
		if (r22 <= 0.f)  // x^2 + y^2 >= z^2 + w^2
		{
			float dif10 = m.m11 - m.m00;
			float omr22 = 1.f - r22;
			if (dif10 <= 0.f)  // x^2 >= y^2
			{
				float fourXSqr = omr22 - dif10;
				float inv4x = 0.5f / sqrtf(fourXSqr);
				q.v[0] = fourXSqr * inv4x;
				q.v[1] = (m.m01 + m.m10) * inv4x;
				q.v[2] = (m.m02 + m.m20) * inv4x;
				q.v[3] = (m.m12 - m.m21) * inv4x;
			}
			else  // y^2 >= x^2
			{
				float fourYSqr = omr22 + dif10;
				float inv4y = 0.5f / sqrtf(fourYSqr);
				q.v[0] = (m.m01 + m.m10) * inv4y;
				q.v[1] = fourYSqr * inv4y;
				q.v[2] = (m.m12 + m.m21) * inv4y;
				q.v[3] = (m.m20 - m.m02) * inv4y;
			}
		}
		else  // z^2 + w^2 >= x^2 + y^2
		{
			float suRight = m.m11 + m.m00;
			float opr22 = 1.f + r22;
			if (suRight <= 0.f)  // z^2 >= w^2
			{
				float fourZSqr = opr22 - suRight;
				float inv4z = 0.5f / sqrtf(fourZSqr);
				q.v[0] = (m.m02 + m.m20) * inv4z;
				q.v[1] = (m.m12 + m.m21) * inv4z;
				q.v[2] = fourZSqr * inv4z;
				q.v[3] = (m.m01 - m.m10) * inv4z;
			}
			else  // w^2 >= z^2
			{
				float fourWSqr = opr22 + suRight;
				float inv4w = 0.5f / sqrtf(fourWSqr);
				q.v[0] = (m.m12 - m.m21) * inv4w;
				q.v[1] = (m.m20 - m.m02) * inv4w;
				q.v[2] = (m.m01 - m.m10) * inv4w;
				q.v[3] = fourWSqr * inv4w;
			}
		}

		QUAT out;
		out.x = q.x;
		out.y = q.y;
		out.z = q.z;
		out.w = q.w;

		return out;
	}

	/**
	* @brief 쿼터니언을 롤, 피치, 요로 변환합니다.
	*/
	inline FLOAT3 quat_to_roll_pitch_yaw(QUAT q)
	{
		// roll = rotation around z-axis
		// pitch = rotation around x-axis
		// yaw = rotation around y-axis
		FLOAT3 out;
		out.z = atan2f(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)); // roll
		out.y = atan2f(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)); // yaw
		out.x = asinf(2.0f * (q.w * q.y - q.z * q.x)); // pitch
		return out;
	}

	/**
	* @brief 쿼터니언을 축과 각도로 변환합니다.
	*/
	inline FLOAT4 quat_to_axis_angle(QUAT q)
	{
		FLOAT4 out;
		float angle = 2.0f * acosf(q.w);
		float s = sqrtf(1.0f - q.w * q.w);
		if (s < MX_TINY)
		{
			out.x = 1.0f; // Arbitrary axis
			out.y = 0.0f;
			out.z = 0.0f;
		}
		else
		{
			out.x = q.x / s;
			out.y = q.y / s;
			out.z = q.z / s;
		}

		out.w = angle;

		return out;
	}

	inline MATRIX4 quat_to_matrix(QUAT q)
	{
		MATRIX4 out;

		const float tx = q.x + q.x;
		const float ty = q.y + q.y;
		const float tz = q.z + q.z;

		const float q_xx = tx * q.x;
		const float q_xy = tx * q.y;
		const float q_xz = tx * q.z;
		const float q_xw = tx * q.w;
		const float q_yy = ty * q.y;
		const float q_yz = ty * q.z;
		const float q_yw = ty * q.w;
		const float q_zz = tz * q.z;
		const float q_zw = tz * q.w;

		out.m00 = 1.0f - q_yy - q_zz;
		out.m01 = q_xy + q_zw;
		out.m02 = q_xz - q_yw;
		out.m03 = 0.0f;
		out.m10 = q_xy - q_zw;
		out.m11 = 1.0f - q_xx - q_zz;
		out.m12 = q_yz + q_xw;
		out.m13 = 0.0f;
		out.m20 = q_xz + q_yw;
		out.m21 = q_yz - q_xw;
		out.m22 = 1.0f - q_xx - q_yy;
		out.m23 = 0.0f;
		out.m30 = 0.0f;
		out.m31 = 0.0f;
		out.m32 = 0.0f;
		out.m33 = 1.0f;

		return out;
	}
}

