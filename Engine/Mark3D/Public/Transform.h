#pragma once
#include "predefine.h"
#include "mathlib.h"


namespace mark
{
	struct MARKENGINE_API Transform
	{
		Transform() noexcept;

		void Reset() noexcept;

		void SetLocalPosition(const FLOAT3& Position) noexcept;
		void SetLocalRotation(const QUAT& Rotation) noexcept;
		void SetLocalScale(const FLOAT3& Scale) noexcept;
		void SetLocalTransform(const FLOAT3& Position, const QUAT& Rotation, const FLOAT3& Scale) noexcept;
		void SetLocalMatrix(const MATRIX4& Matrix) noexcept;

		[[nodiscard]] const FLOAT3& GetLocalPosition() const noexcept;
		[[nodiscard]] const QUAT& GetLocalRotation() const noexcept;
		[[nodiscard]] const FLOAT3& GetLocalScale() const noexcept;
		[[nodiscard]] const MATRIX4& GetLocalMatrix() const noexcept;

		void SetWorldMatrix(const MATRIX4& Matrix) noexcept;
		[[nodiscard]] const FLOAT3& GetWorldPosition() const noexcept;
		[[nodiscard]] const QUAT& GetWorldRotation() const noexcept;
		[[nodiscard]] const FLOAT3& GetWorldScale() const noexcept;
		[[nodiscard]] const MATRIX4& GetWorldMatrix() const noexcept;

		void Translate(const FLOAT3& Delta) noexcept;
		void Rotate(const QUAT& Delta) noexcept;
		void Scale(const FLOAT3& Multiplier) noexcept;
		void LookAt(const FLOAT3& Target, const FLOAT3& Up = FLOAT3_UNIT_Y) noexcept;

		void MarkDirty() noexcept;
		void UpdateWorldMatrix(const MATRIX4* pParentWorldMatrix = nullptr) noexcept;
		[[nodiscard]] bool IsDirty() const noexcept;

	private:
		void RebuildLocalMatrix() noexcept;
		void DecomposeMatrix(const MATRIX4& Matrix, FLOAT3& Position, QUAT& Rotation, FLOAT3& Scale) const noexcept;

	private:
		FLOAT3 m_LocalPosition = FLOAT3_ZERO;
		QUAT m_LocalRotation = QUAT_IDENTITY;
		FLOAT3 m_LocalScale = FLOAT3_ONE;
		MATRIX4 m_LocalMatrix = MATRIX4_IDENT;

		FLOAT3 m_WorldPosition = FLOAT3_ZERO;
		QUAT m_WorldRotation = QUAT_IDENTITY;
		FLOAT3 m_WorldScale = FLOAT3_ONE;
		MATRIX4 m_WorldMatrix = MATRIX4_IDENT;

		bool m_Dirty = true;
	};
}
