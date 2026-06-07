#include "pch.h"
#include "Model.h"
#include "PrimitiveBuffer.h"
#include "GPUGeometry.h"
#include "PrimitiveBuffer.h"


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

	int32_t Model::AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32_t VertexCount,
		uint32_t IndexCount,
		uint32_t MaterialSlot
	) noexcept
	{
		return -1;
	}

	int32_t Model::GetPrimitiveCount() const noexcept
	{
		return -1;
	}

	void Model::ClearPrimitive() noexcept
	{
	}

	bool Model::UpdateVertexData(
		int32_t PrimitiveIndex,
		uint32_t VertexFormat,
		const void* pData,
		size_t DataSize
	) noexcept
	{
		return false;
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
	}

	ISurfaceMaterial* Model::GetSurfaceMaterial(
		uint32_t MaterialSlot
	) const noexcept
	{
		return nullptr;
	}
}
