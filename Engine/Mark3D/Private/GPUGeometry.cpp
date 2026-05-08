#include "pch.h"
#include "GPUGeometry.h"
#include "ModelAsset.h"
#include "PrimitiveBuffer.h"


namespace mark
{
	GPUGeometry::~GPUGeometry() noexcept
	{
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
					ModelAsset::Mesh* pMesh = pModelAssetImpl->INL_GetMesh(i);

					MeshDesc& Desc = m_lstMeshDescs[i];
					Desc.Name = name_hash(pMesh->szName);
					Desc.VertexStart = VertexStart;
					Desc.IndexStart = IndexStart;
					Desc.VertexCount = pMesh->NumVertex;
					Desc.IndexCount = pMesh->NumIndex;

					Desc.NumSubset = pMesh->NumSubMesh;
					/*
					for (uint32_t s = 0; s < pMesh->NumSubMesh; s++)
					{
						Desc.Subset_MatID[s] = pMesh->pSubMesh[s].MatID;
						Desc.Subset_StartIndex[s] = pMesh->pSubMesh[s].StartIndex;
						Desc.Subset_IndexCount[s] = pMesh->pSubMesh[s].IndexCount;
					}
					*/

					VertexStart += pMesh->NumVertex;
					IndexStart += pMesh->NumIndex;
				}

				if (ImmediateUploadToGPU)
				{

				}

			} break;

			case GPU_BUFFER_LAYOUT::SEPARATE:
			{
				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					ModelAsset::Mesh* pMesh = pModelAssetImpl->INL_GetMesh(i);

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
					/*
					for (uint32_t s = 0; s < pMesh->NumSubMesh; s++)
					{
						Desc.Subset_MatID[s] = pMesh->pSubMesh[s].MatID;
						Desc.Subset_StartIndex[s] = pMesh->pSubMesh[s].StartIndex;
						Desc.Subset_IndexCount[s] = pMesh->pSubMesh[s].IndexCount;
					}
					*/
				}

				if (ImmediateUploadToGPU)
				{

				}

			} break;
		}
		

		pModelAsset->Release();

		return true;
	}


}
