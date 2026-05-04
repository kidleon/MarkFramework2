#include "pch.h"
#include "Mesh.h"
#include "PrimitiveBuffer.h"


namespace mark
{
	Mesh::Mesh()
	{
		m_lstPrimitives.reserve(8);
	}

	Mesh::~Mesh() noexcept
	{
		CORE_DELETE(PrimitiveBuffer, m_pPrimitiveBuffers);
		m_lstPrimitives.clear();
	}

	void Mesh::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void Mesh::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(Mesh, this);
		}
	}

	bool Mesh::Create(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount)
	{
		m_pPrimitiveBuffers = CORE_NEW(PrimitiveBuffer);
		if (!m_pPrimitiveBuffers->Create(VertexFormats, VertexCount, IndexFormat, IndexCount))
		{
			SYS_LOG_ERR("Failed to create primitive buffers for model.");
			CORE_DELETE(PrimitiveBuffer, m_pPrimitiveBuffers);
			return false;
		}

		return true;
	}

	int32_t Mesh::AddPrimitive(uint32_t VertexCount, uint32_t IndexCount)
	{
		uint32_t UsedVertexCount = 0;
		uint32_t UsedIndexCount = 0;
		for (size_t i = 0; i < m_lstPrimitives.size(); ++i)
		{
			UsedVertexCount += m_lstPrimitives[i].VertexCount;
			UsedIndexCount += m_lstPrimitives[i].IndexCount;
		}

		if (UsedVertexCount + VertexCount > m_pPrimitiveBuffers->INL_GetVertexCount())
		{
			SYS_LOG_ERR_F("Vertex buffer overflow: Used {}, Adding {}, Total {}, Buffer Size {}",
				UsedVertexCount, VertexCount, UsedVertexCount + VertexCount, m_pPrimitiveBuffers->INL_GetVertexCount());

			return -1;
		}

		if (UsedIndexCount + IndexCount > m_pPrimitiveBuffers->INL_GetIndexCount())
		{
			SYS_LOG_ERR_F("Index buffer overflow: Used {}, Adding {}, Total {}, Buffer Size {}",
				UsedIndexCount, IndexCount, UsedIndexCount + IndexCount, m_pPrimitiveBuffers->INL_GetIndexCount());
			return -1;
		}

		Primitive NewPrimitive = {};
		NewPrimitive.VertexStart = UsedVertexCount;
		NewPrimitive.IndexStart = UsedIndexCount;
		NewPrimitive.VertexCount = VertexCount;
		NewPrimitive.IndexCount = IndexCount;
		NewPrimitive.NumSubsets = 0;
		NewPrimitive.MaterialIndex = 0;
		m_lstPrimitives.push_back(NewPrimitive);

		return (int32_t)m_lstPrimitives.size() - 1;
	}

	int32_t Mesh::AddPrimitive(uint32_t VertexCount, uint32_t* IndexCountArray, size_t NumIndexCountArray)
	{
		if (NumIndexCountArray > MAX_SUBSET)
		{
			SYS_LOG_ERR_F("NumIndexCountArray {} exceeds MAX_SUBSET {}", NumIndexCountArray, MAX_SUBSET);
			return -1;
		}

		uint32_t UsedVertexCount = 0;
		uint32_t UsedIndexCount = 0;
		for (size_t i = 0; i < m_lstPrimitives.size(); ++i)
		{
			UsedVertexCount += m_lstPrimitives[i].VertexCount;
			UsedIndexCount += m_lstPrimitives[i].IndexCount;
		}

		if (UsedVertexCount + VertexCount > m_pPrimitiveBuffers->INL_GetVertexCount())
		{
			SYS_LOG_ERR_F("Vertex buffer overflow: Used {}, Adding {}, Total {}, Buffer Size {}",
				UsedVertexCount, VertexCount, UsedVertexCount + VertexCount, m_pPrimitiveBuffers->INL_GetVertexCount());

			return -1;
		}

		uint32_t TotalIndexCount = 0;
		for (size_t i = 0; i < NumIndexCountArray; ++i)
		{
			TotalIndexCount += IndexCountArray[i];
		}

		if (UsedIndexCount + TotalIndexCount > m_pPrimitiveBuffers->INL_GetIndexCount())
		{
			SYS_LOG_ERR_F("Index buffer overflow: Used {}, Adding {}, Total {}, Buffer Size {}",
				UsedIndexCount, TotalIndexCount, UsedIndexCount + TotalIndexCount, m_pPrimitiveBuffers->INL_GetIndexCount());
			return -1;
		}

		Primitive NewPrimitive = {};
		NewPrimitive.VertexStart = UsedVertexCount;
		NewPrimitive.IndexStart = UsedIndexCount;
		NewPrimitive.VertexCount = VertexCount;
		NewPrimitive.IndexCount = TotalIndexCount;
		NewPrimitive.NumSubsets = static_cast<uint32_t>(NumIndexCountArray);
		for (uint32_t i = 0; i < NumIndexCountArray; ++i)
			NewPrimitive.SubsetIndexCounts[i] = IndexCountArray[i];
		
		m_lstPrimitives.push_back(NewPrimitive);

		return (int32_t)m_lstPrimitives.size() - 1;
	}

	uint32_t Mesh::GetNumPrimitives() const noexcept
	{
		return static_cast<uint32_t>(m_lstPrimitives.size());
	}

	bool Mesh::UpdateVertex(VERTEX_FORMAT VertexFormat, const void* pVertexData, size_t DataSize)
	{
		return m_pPrimitiveBuffers->UpdateVertexDataImmediate(VertexFormat, pVertexData, DataSize);
	}

	bool Mesh::UpdateIndex(const void* pIndexData, size_t DataSize)
	{
		return m_pPrimitiveBuffers->UpdateIndexDataImmediate(pIndexData, DataSize);
	}

}
