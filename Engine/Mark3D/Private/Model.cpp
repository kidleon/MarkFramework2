#include "pch.h"
#include "Model.h"
#include "PrimitiveBuffer.h"
#include "RenderSystem.h"
#include "GPUGeometry.h"


namespace mark
{
	Model::Model(PrimitiveBuffer* pPrimitiveBuffer)
		: m_pPrimitiveBuffer(pPrimitiveBuffer)
	{
	}

	Model::~Model() noexcept
	{
		CORE_DELETE(PrimitiveBuffer, m_pPrimitiveBuffer);
	}

	void Model::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void Model::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(Model, this);
		}
	}

	bool Model::IsLoaded() const noexcept
	{
		return m_Loaded.load(std::memory_order_acquire) == TRUE;
	}

	void Model::SetEnable(bool Enable) noexcept
	{
		m_Enable = Enable;
	}

	bool Model::IsEnable() const noexcept
	{
		return m_Enable;
	}

	void Model::SetVisible(bool Visible) noexcept
	{
		m_Visible = Visible;
	}

	bool Model::IsVisible() const noexcept
	{
		return m_Visible;
	}

	ISceneNode* Model::GetSceneNode() const noexcept
	{
		return m_pSceneNode;
	}

	int32_t Model::AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32_t VertexCount,
		uint32_t IndexCount,
		uint32_t MaterialSlot
	) noexcept
	{
		m_lstPrimitives.emplace_back(
			PRIMITIVE {
				PrimitiveType,
				VertexCount,
				IndexCount,
				MaterialSlot
			}
		);

		return (int32_t)m_lstPrimitives.size() - 1;
	}

	int32_t Model::GetPrimitiveCount() const noexcept
	{
		return (int32_t)m_lstPrimitives.size();
	}

	void Model::ClearPrimitive() noexcept
	{
		m_lstPrimitives.clear();
	}

	bool Model::UpdateVertexData(
		int32_t PrimitiveIndex,
		VERTEX_FORMAT VertexFormat,
		const void* pData,
		size_t DataSize
	) noexcept
	{
		if (!IsValidPrimitiveIndex(PrimitiveIndex))
		{
			SYS_LOG_ERR_F("Invalid PrimitiveIndex: {}", PrimitiveIndex);
			return false;
		}

		return true;
	}

	bool Model::UpdateIndexData(
		int32_t PrimitiveIndex,
		const void* pData,
		size_t DataSize
	) noexcept
	{
		return false;
	}

	void Model::SetSurfaceMaterial(
		uint32_t MaterialSlot,
		ISurfaceMaterial* pMaterial
	) noexcept
	{
		if (!pMaterial)
		{
		}
		
		m_lstMaterialSlots.emplace_back(
			MATERIAL_SLOT{
				pMaterial
			}
		);
	}

	ISurfaceMaterial* Model::GetSurfaceMaterial(
		uint32_t MaterialSlot
	) const noexcept
	{
		return nullptr;
	}
}
