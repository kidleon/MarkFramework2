#include "pch.h"
#include "GPUGeometryFactory.h"
#include "GPUGeometry.h"
#include "ModelAsset.h"
#include "PrimitiveBuffer.h"


namespace mark
{
	bool GPUGeometryFactory::CreateGeometry(RenderSystem* pRenderSystem, const GPUGeometryCreateDesc& CreateDesc, unknown_ptr<GPUGeometry> pGPUGeometry)
	{
		if (!pRenderSystem)
		{
			LOG_ERR("RenderSystem is null. Cannot create GPU geometry.");
			return false;
		}

		if (!CreateDesc.pModelAsset.ptr())
		{
			LOG_ERR("Model asset is null in GPUGeometryCreateDesc. Cannot create GPU geometry.");
			return false;
		}

		if (!pGPUGeometry)
		{
			LOG_ERR("Output GPUGeometry pointer is null. Cannot create GPU geometry.");
			return false;
		}

		IModelAsset* pModelAssetInterface = const_cast<IModelAsset*>(CreateDesc.pModelAsset.ptr());
		ModelAsset* pModelAssetRef = static_cast<ModelAsset*>(pModelAssetInterface);

		uint32_t NumMesh = pModelAssetRef->INL_GetNumMeshes();
		uint32_t TotalVertexCount = pModelAssetRef->INL_GetTotalVertexCount();
		uint32_t TotalIndexCount = pModelAssetRef->INL_GetTotalIndexCount();
		uint32_t ModelVertexFormats = pModelAssetRef->INL_GetVertexFormats();

		if (CreateDesc.VertexFormats > 0)
		{
			if ((CreateDesc.VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
			{
				if ((ModelVertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) == 0)
				{
					LOG_ERR("Requested vertex format NORMAL is not available in the model asset.");
					return false;
				}
			}

			if ((CreateDesc.VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
			{
				if ((ModelVertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) == 0)
				{
					LOG_ERR("Requested vertex format TEXCOORD0 is not available in the model asset.");
					return false;
				}
			}

			if ((CreateDesc.VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
			{
				if ((ModelVertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) == 0)
				{
					LOG_ERR("Requested vertex format COLOR is not available in the model asset.");
					return false;
				}
			}

			if ((CreateDesc.VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
			{
				if (!pModelAssetRef->INL_HasComputedTangent())
					pModelAssetRef->ComputeTangent();
			}
		}

		uint32_t CreateVertexFormats = (CreateDesc.VertexFormats > 0) ? CreateDesc.VertexFormats : ModelVertexFormats;

		pGPUGeometry->INL_SetBufferLayout(CreateDesc.BufferLayout);

		switch (CreateDesc.BufferLayout)
		{
			case GPU_BUFFER_LAYOUT::MERGED:
			{
				PrimitiveBuffer* pMergedBuffer = CORE_NEW(PrimitiveBuffer);
				pGPUGeometry->INL_AddPrimitiveBuffer(pMergedBuffer);

				if (!pMergedBuffer->Create(
					pRenderSystem,
					CreateVertexFormats,
					TotalVertexCount,
					INDEX_FORMAT::AUTO,
					TotalIndexCount
				))
				{
					LOG_ERR("Failed to create merged primitive buffer for GPU geometry.");
					pGPUGeometry->Release();
					return false;
				}

				uint32_t VertexStart = 0;
				uint32_t IndexStart = 0;

				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					const ModelAsset::Mesh* pMesh = pModelAssetRef->INL_GetMesh(i);

					GPUGeometry::MeshDesc& Desc = pGPUGeometry->INL_AddAndGetMeshDesc();
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

				const ModelAsset::MeshData& MeshData = pModelAssetRef->INL_GetMeshData();
				if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::POSITION) != 0)
				{
					pMergedBuffer->UpdateVertexDataImmediate(
						VERTEX_FORMAT::POSITION,
						MeshData.pPosition,
						sizeof(FLOAT3) * TotalVertexCount
					);
				}

				if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
				{
					pMergedBuffer->UpdateVertexDataImmediate(
						VERTEX_FORMAT::NORMAL,
						MeshData.pNormal,
						sizeof(FLOAT3) * TotalVertexCount
					);
				}

				if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
				{
					pMergedBuffer->UpdateVertexDataImmediate(
						VERTEX_FORMAT::TEXCOORD0,
						MeshData.pTexCoord0,
						sizeof(FLOAT2) * TotalVertexCount
					);
				}

				if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
				{
					pMergedBuffer->UpdateVertexDataImmediate(
						VERTEX_FORMAT::COLOR,
						MeshData.pColor,
						sizeof(FLOAT4)* TotalVertexCount
					);
				}

				if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
				{
					pMergedBuffer->UpdateVertexDataImmediate(
						VERTEX_FORMAT::TANGENT,
						MeshData.pTangent,
						sizeof(FLOAT4)* TotalVertexCount
					);
				}

				pMergedBuffer->UpdateIndexDataImmediate(
					MeshData.pIndices,
					(MeshData.IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) * TotalIndexCount : sizeof(uint16_t) * TotalIndexCount
				);

			} break;

			case GPU_BUFFER_LAYOUT::SEPARATE:
			{
				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					const ModelAsset::Mesh* pMesh = pModelAssetRef->INL_GetMesh(i);

					GPUGeometry::MeshDesc& Desc = pGPUGeometry->INL_AddAndGetMeshDesc();
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

					PrimitiveBuffer* pPrimitiveBuffer = CORE_NEW(PrimitiveBuffer);
					pGPUGeometry->INL_AddPrimitiveBuffer(pPrimitiveBuffer);

					pPrimitiveBuffer->Create(pRenderSystem, CreateVertexFormats, pMesh->NumVertex, INDEX_FORMAT::AUTO, pMesh->NumIndex);

					if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::POSITION) != 0)
					{
						pPrimitiveBuffer->UpdateVertexDataImmediate(
							VERTEX_FORMAT::POSITION,
							pModelAssetRef->INL_GetPosition(i),
							sizeof(FLOAT3) * pMesh->NumVertex
						);
					}

					if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
					{
						pPrimitiveBuffer->UpdateVertexDataImmediate(
							VERTEX_FORMAT::NORMAL,
							pModelAssetRef->INL_GetNormal(i),
							sizeof(FLOAT3) * pMesh->NumVertex
						);
					}

					if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
					{
						pPrimitiveBuffer->UpdateVertexDataImmediate(
							VERTEX_FORMAT::TEXCOORD0,
							pModelAssetRef->INL_GetTexCoord0(i),
							sizeof(FLOAT2) * pMesh->NumVertex
						);
					}

					if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
					{
						pPrimitiveBuffer->UpdateVertexDataImmediate(
							VERTEX_FORMAT::COLOR,
							pModelAssetRef->INL_GetColor(i),
							sizeof(FLOAT4) * pMesh->NumVertex
						);
					}

					if ((CreateVertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
					{
						pPrimitiveBuffer->UpdateVertexDataImmediate(
							VERTEX_FORMAT::TANGENT,
							pModelAssetRef->INL_GetTangent(i),
							sizeof(FLOAT4) * pMesh->NumVertex
						);
					}

					uint32_t IndexSize = (pModelAssetRef->INL_GetMeshData().IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t);

					pPrimitiveBuffer->UpdateIndexDataImmediate(
						pModelAssetRef->INL_GetIndices(i),
						IndexSize * pMesh->NumIndex
					);
				}
			} break;
		}

		if ((CreateDesc.CreateFlags & (uint32_t)GPUGEOMETRY_CREATE_FLAGS::HAS_MODEL_ASSET) != 0)
		{
			pGPUGeometry->INL_SetModelAsset(pModelAssetRef);
		}

		return true;
	}
}
