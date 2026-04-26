namespace mark
{
	inline float repeat(float t, float length) noexcept
	{
		return T_CLAMP(t - std::floor(t / length) * length, 0.0f, length);
	}

	inline FLOAT2 repeat(FLOAT2 t, FLOAT2 length) noexcept
	{
		return FLOAT2
		{
			repeat(t.x, length.x),
			repeat(t.y, length.y)
		};
	}

	inline FLOAT3 repeat(FLOAT3 t, FLOAT3 length) noexcept
	{
		return FLOAT3
		{
			repeat(t.x, length.x),
			repeat(t.y, length.y),
			repeat(t.z, length.z)
		};
	}

	// current → target 사이의 최단 각도 차이를 라디안으로 반환
	// 반환 범위: (-π, π]
	// 예) current=350°(≈6.1rad), target=10°(≈0.17rad) → +20°(≈0.35rad) 반환 (359° 돌아가지 않음)
	inline float delta_angle(float current, float target) noexcept
	{
		// target - current 의 차이를 [0, 2π) 범위로 순환
		float delta = std::fmod(target - current, MX_2PI);

		// 최단 경로 보정: (-π, π] 범위로 변환
		if (delta > MX_PI)
			delta -= MX_2PI; // 반시계보다 시계가 더 가까운 경우
		else if (delta < -MX_PI)
			delta += MX_2PI; // 시계보다 반시계가 더 가까운 경우

		return delta;
	}

	inline FLOAT2 move_towards(FLOAT2 current, FLOAT2 target, float max_delta) noexcept
	{
		FLOAT2 delta = math_sub(target, current);
		float dist = math_length(delta);

		if (dist <= max_delta || 0.0f == dist)
			return target;

		FLOAT2 dir = math_normalized(delta);
		FLOAT2 moved = math_mul(dir, FLOAT2{ max_delta, max_delta });

		return math_add(current, moved);
	}

	inline FLOAT3 move_towards(FLOAT3 current, FLOAT3 target, float max_delta) noexcept
	{
		FLOAT3 delta = math_sub(target, current);
		float dist = math_length(delta);

		if (dist <= max_delta || 0.0f == dist)
			return target;

		FLOAT3 dir = math_normalized(delta);
		FLOAT3 moved = math_mul(dir, FLOAT3{ max_delta, max_delta, max_delta });

		return math_add(current, moved);
	}

	inline FLOAT4 move_towards(FLOAT4 current, FLOAT4 target, float max_delta) noexcept
	{
		FLOAT4 delta = math_sub(target, current);
		float dist = math_length(delta);

		if (dist <= max_delta || 0.0f == dist)
			return target;

		FLOAT4 dir = math_normalized(delta);
		FLOAT4 moved = math_mul(dir, FLOAT4{ max_delta, max_delta, max_delta, max_delta });

		return math_add(current, moved);
	}

	inline QUAT forward_to_quat(FLOAT2 forward) noexcept
	{
		// forward 벡터를 이용하여 2D 평면에서의 회전을 나타내는 쿼터니언을 계산한다.
		// forward 벡터가 (0, 1)인 경우에는 회전이 없으므로 (0, 0, 0, 1) 쿼터니언을 반환한다.
		if (forward.x == 0.0f && forward.y == 1.0f)
			return QUAT{ 0.0f, 0.0f, 0.0f, 1.0f };

		// forward 벡터의 방향에 따라 회전 축과 회전 각도를 계산한다.
		float angle = std::atan2(forward.y, forward.x); // forward 벡터의 방향에서 회전 각도를 계산한다.
		const float half = angle * 0.5f;

		// Z축 회전 쿼터니언 구성
		// q = (x, y, z, w)
		//   = (0, 0, sin(θ/2), cos(θ/2))
		// x, y는 0 (Z축 회전이므로 XY 성분 없음)
		return QUAT
		{
			0.0f,           // x: 0
			0.0f,           // y: 0
			std::sin(half), // z: sin(θ/2) — Z축 회전량
			std::cos(half)  // w: cos(θ/2) — 스칼라 성분
		};
	}

	// FLOAT3 forward/up 단위벡터로부터 3D 회전 쿼터니언을 생성
	//
	// 전제 조건:
	//   - forward, up 모두 정규화된 단위벡터
	//   - QUAT 구조체: { float x, y, z, w }
	//   - FLOAT3 구조체: { float x, y, z }
	//   - 좌표계: 왼손 좌표계 (Left-Handed)
	//
	// 수학적 원리:
	//   왼손 좌표계에서 right = forward × up (오른손과 반대)
	//   → forward/up/right 세 축으로 회전 행렬(3x3) 구성
	//   → Shepperd's method로 행렬 → 쿼터니언 변환
	//
	//   회전 행렬 R (왼손 좌표계, 열 기준 = local→world):
	//       | right.x   up.x   forward.x |
	//   R = | right.y   up.y   forward.y |
	//       | right.z   up.z   forward.z |
	inline QUAT forward_to_quat(FLOAT3 forward, FLOAT3 up) noexcept
	{
		// 왼손 좌표계: right = up × forward
		// 오른손 좌표계의 (forward × up)와 반대 방향
		// up과 forward에 수직인 오른쪽 축 생성
		const FLOAT3 right =
		{
			up.y * forward.z - up.z * forward.y, // x
			up.z * forward.x - up.x * forward.z, // y
			up.x * forward.y - up.y * forward.x  // z
		};

		// 회전 행렬 요소 명명 (열 기준: local→world 변환)
		//   m00 = right.x,   m01 = up.x,      m02 = forward.x
		//   m10 = right.y,   m11 = up.y,      m12 = forward.y
		//   m20 = right.z,   m21 = up.z,      m22 = forward.z
		const float m00 = right.x, m01 = up.x, m02 = forward.x;
		const float m10 = right.y, m11 = up.y, m12 = forward.y;
		const float m20 = right.z, m21 = up.z, m22 = forward.z;

		// 회전 행렬의 대각합(trace) 계산
		const float trace = m00 + m11 + m22;

		float x, y, z, w;

		if (trace > 0.0f)
		{
			// trace > 0: w가 가장 큰 성분 → w 기준 계산
			const float s = 0.5f / std::sqrt(trace + 1.0f);
			w = 0.25f / s;
			x = (m21 - m12) * s;
			y = (m02 - m20) * s;
			z = (m10 - m01) * s;
		}
		else if (m00 > m11 && m00 > m22)
		{
			// m00이 가장 큰 대각 성분 → x 기준 계산
			const float s = 0.5f / std::sqrt(1.0f + m00 - m11 - m22);
			w = (m21 - m12) * s;
			x = 0.25f / s;
			y = (m01 + m10) * s;
			z = (m02 + m20) * s;
		}
		else if (m11 > m22)
		{
			// m11이 가장 큰 대각 성분 → y 기준 계산
			const float s = 0.5f / std::sqrt(1.0f + m11 - m00 - m22);
			w = (m02 - m20) * s;
			x = (m01 + m10) * s;
			y = 0.25f / s;
			z = (m12 + m21) * s;
		}
		else
		{
			// m22가 가장 큰 대각 성분 → z 기준 계산
			const float s = 0.5f / std::sqrt(1.0f + m22 - m00 - m11);
			w = (m10 - m01) * s;
			x = (m02 + m20) * s;
			y = (m12 + m21) * s;
			z = 0.25f / s;
		}

		return QUAT{ x, y, z, w };
	}

	// 두 FLOAT2 forward 방향벡터 사이를 max_angle(라디안)만큼 회전
	//
	// 전제 조건:
	//   - current, target 모두 정규화된 단위벡터 (forward 방향)
	//   - max_angle: 한 번에 회전할 최대 각도 (라디안, 양수)
	//   - 반환값: max_angle만큼 target 방향으로 회전한 단위벡터
	//
	// 수학적 원리:
	//   current와 target 사이의 각도 θ = atan2(cross, dot)
	//   |θ| <= max_angle 이면 target을 그대로 반환
	//   |θ| >  max_angle 이면 current를 clamp된 각도만큼 회전
	//
	//   2D 회전 공식:
	//     rotated.x = current.x * cos(a) - current.y * sin(a)
	//     rotated.y = current.x * sin(a) + current.y * cos(a)
	inline FLOAT2 rotate_towards(FLOAT2 current, FLOAT2 target, float max_angle) noexcept
	{
		// 2D 외적(cross product z성분): current × target
		// 양수 → target이 current의 반시계 방향(CCW)
		// 음수 → target이 current의 시계 방향(CW)
		const float cross = current.x * target.y - current.y * target.x;

		// 내적(dot product): cos(θ) 값
		// 두 단위벡터 사이의 각도 코사인
		const float dot = current.x * target.x + current.y * target.y;

		// current → target 사이의 부호 있는 각도 계산 (라디안, 범위: [-π, π])
		// atan2(cross, dot): cross의 부호로 회전 방향 결정
		const float angle = std::atan2(cross, dot);

		// 이미 target에 도달했거나 max_angle 이내인 경우
		// → target을 그대로 반환 (더 이상 회전 불필요)
		if (std::abs(angle) <= max_angle)
		{
			return target;
		}

		// max_angle만큼만 회전 (부호 유지: 회전 방향 보존)
		// angle > 0: CCW, angle < 0: CW
		const float clamp_angle = std::copysign(max_angle, angle);

		// 2D 회전 행렬 적용
		// [ cos(a)  -sin(a) ] [ current.x ]
		// [ sin(a)   cos(a) ] [ current.y ]
		const float cos_a = std::cos(clamp_angle);
		const float sin_a = std::sin(clamp_angle);

		return FLOAT2
		{
			current.x * cos_a - current.y * sin_a, // x
			current.x * sin_a + current.y * cos_a  // y
		};
	}

	// 두 FLOAT3 forward 방향벡터 사이를 max_angle(라디안)만큼 회전
	//
	// 전제 조건:
	//   - current_forward, target_forward 모두 정규화된 단위벡터
	//   - max_angle: 한 번에 회전할 최대 각도 (라디안, 양수)
	//   - 반환값: max_angle만큼 target 방향으로 회전한 단위벡터
	//
	// 수학적 원리:
	//   1. dot   = current · target        → cos(θ) 
	//   2. cross = current × target        → 회전축 (수직 벡터)
	//   3. θ = acos(clamp(dot, -1, 1))     → 두 벡터 사이 각도
	//   4. |θ| <= max_angle → target 반환
	//   5. |θ| >  max_angle → 로드리게스 회전 공식으로 max_angle만큼만 회전
	//
	// 로드리게스 회전 공식 (단위 회전축 k, 각도 a):
	//   v' = v*cos(a) + (k × v)*sin(a) + k*(k·v)*(1 - cos(a))
	inline FLOAT3 rotate_towards(FLOAT3 current_forward, FLOAT3 target_forward, float max_angle) noexcept
	{
		// 내적: cos(θ) — 두 단위벡터 사이 각도의 코사인
		const float dot = current_forward.x * target_forward.x
			+ current_forward.y * target_forward.y
			+ current_forward.z * target_forward.z;

		// 부동소수점 오차로 [-1, 1] 범위를 벗어날 수 있으므로 clamp
		const float dot_clamped = std::fmax(-1.0f, std::fmin(1.0f, dot));

		// 두 벡터 사이의 각도 θ (라디안, 범위: [0, π])
		const float angle = std::acos(dot_clamped);

		// 이미 target에 도달했거나 max_angle 이내인 경우
		// → target을 그대로 반환
		if (angle <= max_angle)
		{
			return target_forward;
		}

		// 두 벡터가 거의 평행(반대 방향 포함)한 경우 외적이 zero vector가 됨
		// → 임의의 수직 벡터를 회전축으로 사용
		// sin(θ) ≈ 0 이면 평행 또는 반평행
		const float sin_angle = std::sqrt(1.0f - dot_clamped * dot_clamped);

		FLOAT3 axis;
		if (sin_angle < 1e-6f)
		{
			// 반대 방향(angle ≈ π): current에 수직인 임의 축 생성
			// +X축과 외적 시도, 평행이면 +Y축 사용
			if (std::abs(current_forward.x) < 0.9f)
			{
				// current × (1, 0, 0) 로 수직축 생성
				axis = FLOAT3
				{
					0.0f,
					 current_forward.z,
					-current_forward.y
				};
			}
			else
			{
				// current × (0, 1, 0) 로 수직축 생성
				axis = FLOAT3
				{
					-current_forward.z,
					 0.0f,
					 current_forward.x
				};
			}

			// 생성한 축 정규화
			const float len = std::sqrt(axis.x * axis.x
				+ axis.y * axis.y
				+ axis.z * axis.z);
			axis.x /= len;
			axis.y /= len;
			axis.z /= len;
		}
		else
		{
			// 일반적인 경우: 외적으로 회전축 계산
			// axis = current × target (정규화 불필요: 길이 = sin(θ))
			// sin_angle로 나눠 단위벡터로 만듦
			const float inv_sin = 1.0f / sin_angle;
			axis = FLOAT3
			{
				(current_forward.y * target_forward.z - current_forward.z * target_forward.y) * inv_sin,
				(current_forward.z * target_forward.x - current_forward.x * target_forward.z) * inv_sin,
				(current_forward.x * target_forward.y - current_forward.y * target_forward.x) * inv_sin
			};
		}

		// 로드리게스 회전 공식으로 current를 axis 기준 max_angle만큼 회전
		// v' = v*cos(a) + (k × v)*sin(a) + k*(k·v)*(1 - cos(a))
		const float cos_a = std::cos(max_angle);
		const float sin_a = std::sin(max_angle);
		const float k_dot_v = axis.x * current_forward.x  // k · v
			+ axis.y * current_forward.y
			+ axis.z * current_forward.z;

		// k × v (축과 current의 외적)
		const FLOAT3 k_cross_v =
		{
			axis.y * current_forward.z - axis.z * current_forward.y,
			axis.z * current_forward.x - axis.x * current_forward.z,
			axis.x * current_forward.y - axis.y * current_forward.x
		};

		// 최종 회전 결과
		// v' = v*cos(a) + (k×v)*sin(a) + k*(k·v)*(1-cos(a))
		return FLOAT3
		{
			current_forward.x * cos_a + k_cross_v.x * sin_a + axis.x * k_dot_v * (1.0f - cos_a),
			current_forward.y * cos_a + k_cross_v.y * sin_a + axis.y * k_dot_v * (1.0f - cos_a),
			current_forward.z * cos_a + k_cross_v.z * sin_a + axis.z * k_dot_v * (1.0f - cos_a)
		};
	}
}
