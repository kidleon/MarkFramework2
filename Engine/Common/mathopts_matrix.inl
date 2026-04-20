namespace mark
{

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
		MATRIX4 r = MATRIX4_IDENT;
		r.m30 = pos.x;
		r.m31 = pos.y;
		r.m32 = pos.z;
		return r;
	}

	inline void mat4_trans(const FLOAT3& pos, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
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

	inline void mat4_trans(float x, float y, float z, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m30 = x;
		out.m31 = y;
		out.m32 = z;
	}

	inline MATRIX4 mat4_trans(float x, float y, float z) noexcept
	{
		MATRIX4 r;
		mat4_trans(x, y, z, r);
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

	inline MATRIX4 mat4_transpose(const MATRIX4& m) noexcept
	{
		MATRIX4 r;
		mat4_transpose(m, r);
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
			out = MATRIX4_IDENT;
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

	inline MATRIX4 mat4_inverse(const MATRIX4& m) noexcept
	{
		MATRIX4 r;
		mat4_inverse(m, r);
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
				temp.m[i][j] =
					m0.m[i][0] * m1.m[0][j] +
					m0.m[i][1] * m1.m[1][j] +
					m0.m[i][2] * m1.m[2][j] +
					m0.m[i][3] * m1.m[3][j];
			}
		}

		// 결과를 출력 행렬에 복사
		// (입력과 출력이 같은 행렬일 수 있으므로 임시 행렬 사용)
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				out.m[i][j] = temp.m[i][j];
			}
		}
	}

	inline MATRIX4 mat4_mul(const MATRIX4& m0, const MATRIX4& m1) noexcept
	{
		MATRIX4 r;
		mat4_mul(m0, m1, r);
		return r;
	}

	inline void mat4_mul(const MATRIX4& m, const FLOAT3& v, FLOAT3& out) noexcept
	{
		out.x = (m.m00 * v.x) + (m.m10 * v.y) + (m.m20 * v.z) + m.m30;
		out.y = (m.m01 * v.x) + (m.m11 * v.y) + (m.m21 * v.z) + m.m31;
		out.z = (m.m02 * v.x) + (m.m12 * v.y) + (m.m22 * v.z) + m.m32;
	}

	inline FLOAT3 mat4_mul(const MATRIX4& m, const FLOAT3& v) noexcept
	{
		FLOAT3 r;
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

	inline FLOAT4 mat4_mul(const MATRIX4& m, const FLOAT4& v) noexcept
	{
		FLOAT4 r;
		mat4_mul(m, v, r);
		return r;
	}

	inline void mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = sx;
		out.m11 = sy;
		out.m22 = sz;
	}

	inline MATRIX4 mat4_scale(FLOAT sx, FLOAT sy, FLOAT sz) noexcept
	{
		MATRIX4 r;
		mat4_scale(sx, sy, sz, r);
		return r;
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

	inline void mat4_rotation_x(FLOAT angle, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		float c = cosf(angle);
		float s = sinf(angle);
		out.m11 = c;
		out.m12 = s;
		out.m21 = -s;
		out.m22 = c;
	}

	inline MATRIX4 mat4_rotation_x(FLOAT angle) noexcept
	{
		MATRIX4 r;
		mat4_rotation_x(angle, r);
		return r;
	}

	inline void mat4_rotation_y(FLOAT angle, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		float c = cosf(angle);
		float s = sinf(angle);
		out.m00 = c;
		out.m02 = -s;
		out.m20 = s;
		out.m22 = c;
	}

	inline MATRIX4 mat4_rotation_y(FLOAT angle) noexcept
	{
		MATRIX4 r;
		mat4_rotation_y(angle, r);
		return r;
	}

	inline void mat4_rotation_z(FLOAT angle, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		float c = cosf(angle);
		float s = sinf(angle);
		out.m00 = c;
		out.m01 = s;
		out.m10 = -s;
		out.m11 = c;
	}

	inline MATRIX4 mat4_rotation_z(FLOAT angle) noexcept
	{
		MATRIX4 r;
		mat4_rotation_z(angle, r);
		return r;
	}

	inline void mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
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

	inline MATRIX4 mat4_rotation_yaw_pitch_roll(FLOAT yaw, FLOAT pitch, FLOAT roll) noexcept
	{
		MATRIX4 r;
		mat4_rotation_yaw_pitch_roll(yaw, pitch, roll, r);
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

	inline MATRIX4 mat4_rotation_axis(const FLOAT3& axis, FLOAT angle) noexcept
	{
		MATRIX4 r;
		mat4_rotation_axis(axis, angle, r);
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
		out.m01 = xy + wz;
		out.m02 = xz - wy;
		out.m03 = 0.0f;

		out.m10 = xy - wz;
		out.m11 = 1.0f - (xx + zz);
		out.m12 = yz + wx;
		out.m13 = 0.0f;

		out.m20 = xz + wy;
		out.m21 = yz - wx;
		out.m22 = 1.0f - (xx + yy);
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

	inline void mat4_lookat_lh(const FLOAT3& eye, const FLOAT3& dir, const FLOAT3& up, MATRIX4& out) noexcept
	{
		FLOAT3 ndir = dir;
		math_normalize(ndir);

		FLOAT3 nup = up;
		math_normalize(nup);

		// right = up × dir
		FLOAT3 right;
		math_cross(nup, ndir, right);

		// dir과 up이 거의 평행한지 확인
		float rightLen = math_length(right);
		if (rightLen < 1e-6f)
		{
			// 폴백: 임의의 직교 벡터 선택
			if (fabsf(ndir.y) < 0.999f)
			{
				FLOAT3 worldUp = { 0.0f, 1.0f, 0.0f };
				math_cross(worldUp, ndir, right);
			}
			else
			{
				FLOAT3 worldRight = { 1.0f, 0.0f, 0.0f };
				math_cross(ndir, worldRight, right);
			}
		}

		math_normalize(right);

		// up = dir × right (재직교화)
		math_cross(ndir, right, nup);
		math_normalize(nup);

		// 행렬 설정 (기존 코드와 동일)
		out.m00 = right.x;
		out.m10 = right.y;
		out.m20 = right.z;
		out.m30 = -math_dot(right, eye);

		out.m01 = nup.x;
		out.m11 = nup.y;
		out.m21 = nup.z;
		out.m31 = -math_dot(nup, eye);

		out.m02 = ndir.x;
		out.m12 = ndir.y;
		out.m22 = ndir.z;
		out.m32 = -math_dot(ndir, eye);

		out.m03 = 0.0f;
		out.m13 = 0.0f;
		out.m23 = 0.0f;
		out.m33 = 1.0f;
	}

	inline MATRIX4 mat4_lookat_lh(const FLOAT3& eye, const FLOAT3& dir, const FLOAT3& up) noexcept
	{
		MATRIX4 r;
		mat4_lookat_lh(eye, dir, up, r);
		return r;
	}

	inline void mat4_lookto_lh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept
	{
		FLOAT3 right, upn, dir;

		dir = math_sub(to, eye);
		math_normalize(dir);

		math_cross(up, dir, right);
		math_normalize(right);

		math_cross(dir, right, upn);
		math_normalize(upn);
		out.m00 = right.x;
		out.m10 = right.y;
		out.m20 = right.z;
		out.m30 = -math_dot(right, eye);
		out.m01 = upn.x;
		out.m11 = upn.y;
		out.m21 = upn.z;
		out.m31 = -math_dot(upn, eye);
		out.m02 = dir.x;
		out.m12 = dir.y;
		out.m22 = dir.z;
		out.m32 = -math_dot(dir, eye);
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

	inline void mat4_lookto_rh(const FLOAT3& eye, const FLOAT3& to, const FLOAT3& up, MATRIX4& out) noexcept
	{
		FLOAT3 right, upn, dir;

		dir = math_sub(to, eye);
		math_normalize(dir);

		math_cross(up, dir, right);
		math_normalize(right);
		math_cross(dir, right, upn);
		math_normalize(upn);

		out.m00 = right.x;
		out.m10 = right.y;
		out.m20 = right.z;
		out.m30 = -math_dot(right, eye);
		out.m01 = upn.x;
		out.m11 = upn.y;
		out.m21 = upn.z;
		out.m31 = -math_dot(upn, eye);
		out.m02 = -dir.x;
		out.m12 = -dir.y;
		out.m22 = -dir.z;
		out.m32 = math_dot(dir, eye);
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

	inline void mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;

		float yScale = 1.0f / tanf(fov * 0.5f);
		float xScale = yScale / aspect;
		float zRange = farZ / (farZ - nearZ);

		out.v[0] = xScale;  // [0,0]
		out.v[5] = yScale;  // [1,1]
		out.v[10] = zRange;  // [2,2]
		out.v[11] = 1.0f;    // [2,3]
		out.v[14] = -nearZ * zRange;  // [3,2]

		/*
		out->m00 = 1.0f / (aspect * tanf(fov * 0.5f));
		out->m11 = 1.0f / tanf(fov * 0.5f);
		out->m22 = farZ / (farZ - nearZ);
		out->m23 = 1.0f;
		out->m32 = (farZ * nearZ) / (nearZ - farZ);
		out->m33 = 0.0f;
		*/
	}

	inline MATRIX4 mat4_perspective_lh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_perspective_lh(fov, aspect, nearZ, farZ, r);
		return r;
	}

	inline void mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 1.0f / (aspect * tanf(fov * 0.5f));
		out.m11 = 1.0f / tanf(fov * 0.5f);
		out.m22 = farZ / (nearZ - farZ);
		out.m23 = -1.0f;
		out.m32 = (farZ * nearZ) / (nearZ - farZ);
		out.m33 = 0.0f;
	}

	inline MATRIX4 mat4_perspective_rh(FLOAT fov, FLOAT aspect, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_perspective_rh(fov, aspect, nearZ, farZ, r);
		return r;
	}

	inline void mat4_perspective_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 2.0f * nearZ / (right - left);
		out.m11 = -2.0f * nearZ / (bottom - top);
		out.m20 = -1.0f - 2.0f * left / (right - left);
		out.m21 = 1.0f + 2.0f * top / (bottom - top);
		out.m22 = -farZ / (nearZ - farZ);
		out.m32 = (nearZ * farZ) / (nearZ - farZ);
		out.m23 = 1.0f;
		out.m33 = 0.0f;
	}

	inline MATRIX4 mat4_perspective_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_perspective_lh(left, right, top, bottom, nearZ, farZ, r);
		return r;
	}

	inline void mat4_perspective_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 2.0f * nearZ / (right - left);
		out.m11 = -2.0f * nearZ / (bottom - top);
		out.m20 = 1.0f + 2.0f * left / (right - left);
		out.m21 = -1.0f - 2.0f * top / (bottom - top);
		out.m22 = farZ / (nearZ - farZ);
		out.m32 = (nearZ * farZ) / (nearZ - farZ);
		out.m23 = -1.0f;
		out.m33 = 0.0f;
	}

	inline MATRIX4 mat4_perspective_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_perspective_rh(left, right, top, bottom, nearZ, farZ, r);
		return r;
	}

	inline void mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 2.0f / width;
		out.m11 = 2.0f / height;
		out.m22 = 1.0f / (farZ - nearZ);
		out.m32 = nearZ / (nearZ - farZ);
	}

	inline MATRIX4 mat4_ortho_lh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_ortho_lh(width, height, nearZ, farZ, r);
		return r;
	}

	inline void mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 2.0f / width;
		out.m11 = 2.0f / height;
		out.m22 = 1.0f / (nearZ - farZ);
		out.m32 = nearZ / (nearZ - farZ);
	}

	inline MATRIX4 mat4_ortho_rh(FLOAT width, FLOAT height, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_ortho_rh(width, height, nearZ, farZ, r);
		return r;
	}

	inline void mat4_ortho_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 2.0f / (right - left);
		out.m11 = 2.0f / (top - bottom);
		out.m22 = 1.0f / (farZ - nearZ);
		out.m30 = -1.0f - 2.0f * left / (right - left);
		out.m31 = 1.0f + 2.0f * top / (bottom - top);
		out.m32 = nearZ / (nearZ - farZ);
	}

	inline MATRIX4 mat4_ortho_lh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_ortho_lh(left, right, top, bottom, nearZ, farZ, r);
		return r;
	}

	inline void mat4_ortho_rh(FLOAT left, FLOAT right, FLOAT top, FLOAT bottom, FLOAT nearZ, FLOAT farZ, MATRIX4& out) noexcept
	{
		out = MATRIX4_IDENT;
		out.m00 = 2.0f / (right - left);
		out.m11 = 2.0f / (top - bottom);
		out.m22 = 1.0f / (nearZ - farZ);
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

	inline MATRIX4 mat4_frustum_lh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_frustum_lh(left, right, bottom, top, nearZ, farZ, r);
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

	inline MATRIX4 mat4_frustum_rh(FLOAT left, FLOAT right, FLOAT bottom, FLOAT top, FLOAT nearZ, FLOAT farZ) noexcept
	{
		MATRIX4 r;
		mat4_frustum_rh(left, right, bottom, top, nearZ, farZ, r);
		return r;
	}
}