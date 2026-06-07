#pragma once
#include "PrimitiveBuffer.h"


namespace mark
{
	class Model : public IModel
	{
	public:
		Model(PrimitiveBuffer* pPrimitiveBuffer);

		virtual void AddRef() override;
		virtual void Release() override;

		virtual bool IsLoaded() const noexcept override;

		virtual void SetEnable(bool Enable) noexcept final;
		virtual bool IsEnable() const noexcept final;

		virtual void SetVisible(bool Visible) noexcept final;
		virtual bool IsVisible() const noexcept final;

		virtual ISceneNode* GetSceneNode() const noexcept override;

		virtual int32_t AddPrimitive(
			PRIMITIVE_TYPE PrimitiveType,
			uint32_t VertexCount,
			uint32_t IndexCount,
			uint32_t MaterialSlot
		) noexcept override;

		virtual int32_t GetPrimitiveCount() const noexcept override;
		virtual void ClearPrimitive() noexcept override;

		virtual bool UpdateVertexData(
			int32_t PrimitiveIndex,
			VERTEX_FORMAT VertexFormat,
			const void* pData,
			size_t DataSize
		) noexcept override;

		virtual bool UpdateIndexData(
			int32_t PrimitiveIndex,
			const void* pData,
			size_t DataSize
		) noexcept override;

		virtual void SetSurfaceMaterial(
			uint32_t MaterialSlot,
			ISurfaceMaterial* pMaterial
		) noexcept override;

		[[nodiscard]] virtual ISurfaceMaterial* GetSurfaceMaterial(
			uint32_t MaterialSlot
		) const noexcept override;

		inline void INL_CompleteLoading() noexcept { m_Loaded.store(TRUE, std::memory_order_release); }
		inline ISceneNode* INL_GetSceneNode() const noexcept { return m_pSceneNode; }
		inline BOOL INL_IsEnable() const noexcept { return m_Enable; }
		inline BOOL INL_IsVisible() const noexcept { return m_Visible; }

	private:
		Model() = delete;
		virtual ~Model() noexcept;

		inline bool IsValidPrimitiveIndex(int32_t PrimitiveIndex) const noexcept
		{
			return PrimitiveIndex >= 0 && static_cast<size_t>(PrimitiveIndex) < m_lstPrimitives.size();
		}

		inline bool IsValidMaterialSlotIndex(uint32_t MaterialSlot) const noexcept
		{
			return MaterialSlot < m_lstMaterialSlots.size();
		}

		struct PRIMITIVE
		{
			PRIMITIVE_TYPE Type;
			uint32_t VertexCount;
			uint32_t IndexCount;
			uint32_t MaterialSlot;
		};

		struct MATERIAL_SLOT
		{
			ISurfaceMaterial* pMaterial;
		};

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		std::atomic<BOOL> m_Loaded{ FALSE };

		BOOL m_Enable = TRUE;
		BOOL m_Visible = TRUE;
		PrimitiveBuffer* m_pPrimitiveBuffer = nullptr;
		ISceneNode* m_pSceneNode = nullptr;

		mtl::fixed_vector<PRIMITIVE, 8> m_lstPrimitives;
		mtl::fixed_vector<MATERIAL_SLOT, 8> m_lstMaterialSlots;

	};
}
