#include "pch.h"
#include "Transform.h"

#include <cmath>


namespace mark
{
	Transform::Transform() noexcept
	{
		Reset();
	}

	void Transform::Reset() noexcept
	{
		m_LocalPosition = FLOAT3_ZERO;
		m_LocalRotation = QUAT_IDENTITY;
		m_LocalScale = FLOAT3_ONE;
		m_LocalMatrix = MATRIX4_IDENT;

		m_WorldPosition = FLOAT3_ZERO;
		m_WorldRotation = QUAT_IDENTITY;
		m_WorldScale = FLOAT3_ONE;
		m_WorldMatrix = MATRIX4_IDENT;

		m_Dirty = true;
	}

	void Transform::SetLocalPosition(const FLOAT3& Position) noexcept
	{
		m_LocalPosition = Position;
		RebuildLocalMatrix();
	}

	void Transform::SetLocalRotation(const QUAT& Rotation) noexcept
	{
		m_LocalRotation = quat_normalize(Rotation);
		RebuildLocalMatrix();
	}

	void Transform::SetLocalScale(const FLOAT3& Scale) noexcept
	{
		m_LocalScale = Scale;
		RebuildLocalMatrix();
	}

	void Transform::SetLocalTransform(const FLOAT3& Position, const QUAT& Rotation, const FLOAT3& Scale) noexcept
	{
		m_LocalPosition = Position;
		m_LocalRotation = quat_normalize(Rotation);
		m_LocalScale = Scale;
		RebuildLocalMatrix();
	}

	void Transform::SetLocalMatrix(const MATRIX4& Matrix) noexcept
	{
		m_LocalMatrix = Matrix;
		DecomposeMatrix(m_LocalMatrix, m_LocalPosition, m_LocalRotation, m_LocalScale);
		MarkDirty();
	}

	const FLOAT3& Transform::GetLocalPosition() const noexcept
	{
		return m_LocalPosition;
	}

	const QUAT& Transform::GetLocalRotation() const noexcept
	{
		return m_LocalRotation;
	}

	const FLOAT3& Transform::GetLocalScale() const noexcept
	{
		return m_LocalScale;
	}

	const MATRIX4& Transform::GetLocalMatrix() const noexcept
	{
		return m_LocalMatrix;
	}

	void Transform::SetWorldMatrix(const MATRIX4& Matrix) noexcept
	{
		m_WorldMatrix = Matrix;
		DecomposeMatrix(m_WorldMatrix, m_WorldPosition, m_WorldRotation, m_WorldScale);
		m_Dirty = false;
	}

	const FLOAT3& Transform::GetWorldPosition() const noexcept
	{
		return m_WorldPosition;
	}

	const QUAT& Transform::GetWorldRotation() const noexcept
	{
		return m_WorldRotation;
	}

	const FLOAT3& Transform::GetWorldScale() const noexcept
	{
		return m_WorldScale;
	}

	const MATRIX4& Transform::GetWorldMatrix() const noexcept
	{
		return m_WorldMatrix;
	}

	void Transform::Translate(const FLOAT3& Delta) noexcept
	{
		m_LocalPosition = math_add(m_LocalPosition, Delta);
		RebuildLocalMatrix();
	}

	void Transform::Rotate(const QUAT& Delta) noexcept
	{
		m_LocalRotation = quat_normalize(quat_mul(m_LocalRotation, Delta));
		RebuildLocalMatrix();
	}

	void Transform::Scale(const FLOAT3& Multiplier) noexcept
	{
		m_LocalScale = math_mul(m_LocalScale, Multiplier);
		RebuildLocalMatrix();
	}

	void Transform::LookAt(const FLOAT3& Target, const FLOAT3& Up) noexcept
	{
		const FLOAT3 Forward = math_sub(Target, m_LocalPosition);
		if (math_length(Forward) <= 0.000001f)
			return;

		m_LocalRotation = forward_to_quat(math_normalized(Forward), math_normalized(Up));
		RebuildLocalMatrix();
	}

	void Transform::MarkDirty() noexcept
	{
		m_Dirty = true;
	}

	void Transform::UpdateWorldMatrix(const MATRIX4* pParentWorldMatrix) noexcept
	{
		if (pParentWorldMatrix)
			mat4_mul(m_LocalMatrix, *pParentWorldMatrix, m_WorldMatrix);
		else
			m_WorldMatrix = m_LocalMatrix;

		DecomposeMatrix(m_WorldMatrix, m_WorldPosition, m_WorldRotation, m_WorldScale);
		m_Dirty = false;
	}

	bool Transform::IsDirty() const noexcept
	{
		return m_Dirty;
	}

	void Transform::RebuildLocalMatrix() noexcept
	{
		const MATRIX4 ScaleMatrix = mat4_scale(m_LocalScale);
		const MATRIX4 RotationMatrix = mat4_rotation_quat(m_LocalRotation);
		const MATRIX4 TranslationMatrix = mat4_trans(m_LocalPosition);

		MATRIX4 ScaleRotation;
		mat4_mul(ScaleMatrix, RotationMatrix, ScaleRotation);
		mat4_mul(ScaleRotation, TranslationMatrix, m_LocalMatrix);

		MarkDirty();
	}

	void Transform::DecomposeMatrix(const MATRIX4& Matrix, FLOAT3& Position, QUAT& Rotation, FLOAT3& Scale) const noexcept
	{
		Position = mat4_get_trans(Matrix);

		Scale.x = std::sqrt((Matrix.m00 * Matrix.m00) + (Matrix.m01 * Matrix.m01) + (Matrix.m02 * Matrix.m02));
		Scale.y = std::sqrt((Matrix.m10 * Matrix.m10) + (Matrix.m11 * Matrix.m11) + (Matrix.m12 * Matrix.m12));
		Scale.z = std::sqrt((Matrix.m20 * Matrix.m20) + (Matrix.m21 * Matrix.m21) + (Matrix.m22 * Matrix.m22));

		MATRIX4 RotationMatrix = Matrix;
		RotationMatrix.m30 = 0.0f;
		RotationMatrix.m31 = 0.0f;
		RotationMatrix.m32 = 0.0f;
		RotationMatrix.m33 = 1.0f;

		if (Scale.x > 0.000001f)
		{
			RotationMatrix.m00 /= Scale.x;
			RotationMatrix.m01 /= Scale.x;
			RotationMatrix.m02 /= Scale.x;
		}

		if (Scale.y > 0.000001f)
		{
			RotationMatrix.m10 /= Scale.y;
			RotationMatrix.m11 /= Scale.y;
			RotationMatrix.m12 /= Scale.y;
		}

		if (Scale.z > 0.000001f)
		{
			RotationMatrix.m20 /= Scale.z;
			RotationMatrix.m21 /= Scale.z;
			RotationMatrix.m22 /= Scale.z;
		}

		Rotation = quat_normalize(quat_from_matrix(RotationMatrix));
	}
}
