#include "pch.h"
#include "GPUGeometry.h"
#include "ModelAsset.h"
#include "PrimitiveBuffer.h"


namespace mark
{
	GPUGeometry::~GPUGeometry() noexcept
	{
		for (auto pBuffer : m_lstPrimitiveBuffers)
		{
			CORE_DELETE(PrimitiveBuffer, pBuffer);
		}
		m_lstPrimitiveBuffers.clear();
		m_pModelAsset.reset();
		m_lstMeshDescs.clear();
	}

	void GPUGeometry::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void GPUGeometry::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(GPUGeometry, this);
		}
	}

	bool GPUGeometry::Create(
		uint32_t VertexFormats,
		IModelAsset* pModelAsset,
		GPU_BUFFER_LAYOUT BufferLayout,
		BOOL HasModelAsset,
		BOOL ImmediateUploadToGPU
	)
	{
		if (!pModelAsset)
		{
			LOG_ERR("Model asset is null. Cannot create GPU geometry.");
			return false;
		}

		if (!(VertexFormats & (uint32_t)VERTEX_FORMAT::POSITION))
		{
			LOG_ERR("Vertex format must include POSITION.");
			return false;
		}

		ModelAsset* pModelAssetImpl = static_cast<ModelAsset*>(pModelAsset);
		pModelAssetImpl->AddRef();

		if (HasModelAsset)
		{
			m_pModelAsset = pModelAssetImpl;
		}

		m_BufferLayout = BufferLayout;

		uint32_t NumMesh = pModelAssetImpl->INL_GetNumMeshes();

		uint32_t TotalVertexCount = pModelAssetImpl->INL_GetTotalVertexCount();
		uint32_t TotalIndexCount = pModelAssetImpl->INL_GetTotalIndexCount();

		uint32_t ModelVertexFormats = pModelAssetImpl->INL_GetVertexFormats();

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
		{
			if ((ModelVertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) == 0)
			{
				LOG_ERR("Requested vertex format NORMAL is not available in the model asset.");
				pModelAsset->Release();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
		{
			if ((ModelVertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) == 0)
			{
				LOG_ERR("Requested vertex format TEXCOORD0 is not available in the model asset.");
				pModelAsset->Release();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
		{
			if ((ModelVertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) == 0)
			{
				LOG_ERR("Requested vertex format COLOR is not available in the model asset.");
				pModelAsset->Release();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
		{
			if (!pModelAssetImpl->INL_HasComputedTangent())
				pModelAssetImpl->ComputeTangent();
		}

		m_lstMeshDescs.resize(NumMesh);

		switch (m_BufferLayout)
		{
			case GPU_BUFFER_LAYOUT::MERGED:
			{
				PrimitiveBuffer* pMergedBuffer = CORE_NEW(PrimitiveBuffer);
				m_lstPrimitiveBuffers.push_back(pMergedBuffer);

				pMergedBuffer->Create(VertexFormats, TotalVertexCount, INDEX_FORMAT::AUTO, TotalIndexCount);

				uint32_t VertexStart = 0;
				uint32_t IndexStart = 0;

				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					const ModelAsset::Mesh* pMesh = pModelAssetImpl->INL_GetMesh(i);

					MeshDesc& Desc = m_lstMeshDescs[i];
					Desc.Name = name_hash(pMesh->szName);
					Desc.VertexStart = VertexStart;
					Desc.IndexStart = IndexStart;
					Desc.VertexCount = pMesh->NumVertex;
					Desc.IndexCount = pMesh->NumIndex;

					Desc.NumSubset = pMesh->NumSubMesh;
					
					for (uint32_t s = 0; s < pMesh->NumSubMesh; s++)
					{
						Desc.Subset_MatID[s] = pMesh->SubMesh[s].MatID;
						Desc.Subset_StartIndex[s] = pMesh->SubMesh[s].StartOffset;
						Desc.Subset_IndexCount[s] = pMesh->SubMesh[s].IndexCount;
					}

					VertexStart += pMesh->NumVertex;
					IndexStart += pMesh->NumIndex;
				}

				if (ImmediateUploadToGPU)
				{
					const ModelAsset::MeshData& MeshData = pModelAssetImpl->INL_GetMeshData();
					if ((VertexFormats & (uint32_t)VERTEX_FORMAT::POSITION) != 0)
						pMergedBuffer->UpdateVertexDataImmediate(VERTEX_FORMAT::POSITION, MeshData.pPosition, sizeof(FLOAT3) * TotalVertexCount);

					if ((VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
						pMergedBuffer->UpdateVertexDataImmediate(VERTEX_FORMAT::NORMAL, MeshData.pNormal, sizeof(FLOAT3) * TotalVertexCount);

					if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
						pMergedBuffer->UpdateVertexDataImmediate(VERTEX_FORMAT::TEXCOORD0, MeshData.pNormal, sizeof(FLOAT3) * TotalVertexCount);

					if ((VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
						pMergedBuffer->UpdateVertexDataImmediate(VERTEX_FORMAT::COLOR, MeshData.pColor, sizeof(FLOAT4) * TotalVertexCount);

					if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
						pMergedBuffer->UpdateVertexDataImmediate(VERTEX_FORMAT::TANGENT, MeshData.pTangent, sizeof(FLOAT4) * TotalVertexCount);

					pMergedBuffer->UpdateIndexDataImmediate(
						MeshData.pIndices,
						(MeshData.IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) * TotalIndexCount : sizeof(uint16_t) * TotalIndexCount
					);
				}

			} break;

			case GPU_BUFFER_LAYOUT::SEPARATE:
			{
				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					const ModelAsset::Mesh* pMesh = pModelAssetImpl->INL_GetMesh(i);

					PrimitiveBuffer* pPrimitiveBuffer = CORE_NEW(PrimitiveBuffer);
					m_lstPrimitiveBuffers.push_back(pPrimitiveBuffer);

					pPrimitiveBuffer->Create(VertexFormats, pMesh->NumVertex, INDEX_FORMAT::AUTO, pMesh->NumIndex);

					MeshDesc& Desc = m_lstMeshDescs[i];

					Desc.Name = name_hash(pMesh->szName);
					Desc.VertexStart = 0;
					Desc.IndexStart = 0;
					Desc.VertexCount = pMesh->NumVertex;
					Desc.IndexCount = pMesh->NumIndex;

					Desc.NumSubset = pMesh->NumSubMesh;
					for (uint32_t s = 0; s < pMesh->NumSubMesh; s++)
					{
						Desc.Subset_MatID[s] = pMesh->SubMesh[s].MatID;
						Desc.Subset_StartIndex[s] = pMesh->SubMesh[s].StartOffset;
						Desc.Subset_IndexCount[s] = pMesh->SubMesh[s].IndexCount;
					}

					if (ImmediateUploadToGPU)
					{
						if ((VertexFormats & (uint32_t)VERTEX_FORMAT::POSITION) != 0)
						{
							pPrimitiveBuffer->UpdateVertexDataImmediate(
								VERTEX_FORMAT::POSITION,
								pModelAssetImpl->INL_GetPosition(i),
								sizeof(FLOAT3)* pMesh->NumVertex
							);
						}

						if ((VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
						{
							pPrimitiveBuffer->UpdateVertexDataImmediate(
								VERTEX_FORMAT::NORMAL,
								pModelAssetImpl->INL_GetNormal(i),
								sizeof(FLOAT3) * pMesh->NumVertex
							);
						}

						if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
						{
							pPrimitiveBuffer->UpdateVertexDataImmediate(
								VERTEX_FORMAT::TEXCOORD0,
								pModelAssetImpl->INL_GetTexCoord0(i),
								sizeof(FLOAT2) * pMesh->NumVertex
							);
						}

						if ((VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
						{
							pPrimitiveBuffer->UpdateVertexDataImmediate(
								VERTEX_FORMAT::COLOR,
								pModelAssetImpl->INL_GetColor(i),
								sizeof(FLOAT4) * pMesh->NumVertex
							);
						}

						if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
						{
							pPrimitiveBuffer->UpdateVertexDataImmediate(
								VERTEX_FORMAT::TANGENT,
								pModelAssetImpl->INL_GetTangent(i),
								sizeof(FLOAT4) * pMesh->NumVertex
							);
						}

						uint32_t IndexSize = (pModelAssetImpl->INL_GetMeshData().IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t);

						pPrimitiveBuffer->UpdateIndexDataImmediate(
							pModelAssetImpl->INL_GetIndices(i),
							IndexSize * pMesh->NumIndex
						);
					}
				}

			} break;
		}
		

		pModelAsset->Release();

		return true;
	}


}
