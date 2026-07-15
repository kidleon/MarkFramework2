#include "pch.h"
#include "ModelFactory.h"
#include "Model.h"
#include "ModelAsset.h"
#include "PrimitiveBuffer.h"
#include "RenderSystem.h"


namespace mark
{
	namespace
	{
		// 메쉬의 서브메쉬 등록 + 머티리얼 연결 + 정점/인덱스 데이터 업로드를 수행한다.
		// MERGED / SEPARATE 레이아웃 모두 동일한 Model 공개 API 를 사용하므로 공통화한다.
		bool INL_BuildMeshData(
			Model* pModel,
			ModelAsset* pModelAsset,
			uint32_t MeshIndexInAsset,
			int32_t MeshIndex,
			uint32_t VertexFormats,
			INDEX_FORMAT IndexFormat
		)
		{
			const ModelAsset::Mesh* pMesh = pModelAsset->INL_GetMesh(MeshIndexInAsset);

			// 서브메쉬 등록 및 머티리얼 슬롯 연결.
			for (uint32_t s = 0; s < pMesh->NumSubMesh; ++s)
			{
				const ModelAsset::SubMesh& Sub = pMesh->SubMesh[s];

				const int32_t SubMeshIndex = pModel->CreateSubMesh(MeshIndex, 0, Sub.IndexCount);
				if (SubMeshIndex < 0)
				{
					LOG_ERR_F("Failed to create sub-mesh {} of mesh {}.", s, MeshIndexInAsset);
					return false;
				}

				pModel->LinkMeshToMaterial(MeshIndex, SubMeshIndex, Sub.MatID);
			}

			// 메쉬 단위 머티리얼 슬롯 연결.
			pModel->LinkMeshToMaterial(MeshIndex, pMesh->MatID);

			// 정점 데이터 업로드 (각 포맷을 예약된 정점 오프셋에 부분 기록).
			static constexpr VERTEX_FORMAT kVertexChannels[] =
			{
				VERTEX_FORMAT::POSITION,
				VERTEX_FORMAT::NORMAL,
				VERTEX_FORMAT::TEXCOORD0,
				VERTEX_FORMAT::COLOR,
				VERTEX_FORMAT::TANGENT,
			};

			for (VERTEX_FORMAT Channel : kVertexChannels)
			{
				if ((VertexFormats & (uint32_t)Channel) == 0)
					continue;

				const void* pChannelData = nullptr;
				switch (Channel)
				{
					case VERTEX_FORMAT::POSITION:  pChannelData = pModelAsset->INL_GetPosition(MeshIndexInAsset); break;
					case VERTEX_FORMAT::NORMAL:    pChannelData = pModelAsset->INL_GetNormal(MeshIndexInAsset); break;
					case VERTEX_FORMAT::TEXCOORD0: pChannelData = pModelAsset->INL_GetTexCoord0(MeshIndexInAsset); break;
					case VERTEX_FORMAT::COLOR:     pChannelData = pModelAsset->INL_GetColor(MeshIndexInAsset); break;
					case VERTEX_FORMAT::TANGENT:   pChannelData = pModelAsset->INL_GetTangent(MeshIndexInAsset); break;
					default: break;
				}

				if (!pChannelData)
					continue;

				pModel->UpdateVertex(
					MeshIndex,
					Channel,
					pChannelData,
					(size_t)GetVertexSizeFromFormat(Channel) * pMesh->NumVertex
				);
			}

			// 인덱스 데이터 업로드 (예약된 인덱스 오프셋에 부분 기록).
			const uint32_t IndexStride = (IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t);
			pModel->UpdateIndex(
				MeshIndex,
				pModelAsset->INL_GetIndices(MeshIndexInAsset),
				(size_t)IndexStride * pMesh->NumIndex
			);

			return true;
		}
	}


	Model* ModelFactory::CreateModel(
		RenderSystem* pRenderSystem,
		ModelAsset* pModelAsset,
		MODEL_LAYOUT Layout
	)
	{
		if (!pRenderSystem)
		{
			LOG_ERR("RenderSystem is null. Cannot create model.");
			return nullptr;
		}

		if (!pModelAsset)
		{
			LOG_ERR("Model asset is null. Cannot create model.");
			return nullptr;
		}

		const uint32_t NumMesh = pModelAsset->INL_GetNumMeshes();
		const uint32_t TotalVertexCount = pModelAsset->INL_GetTotalVertexCount();
		const uint32_t TotalIndexCount = pModelAsset->INL_GetTotalIndexCount();
		const uint32_t VertexFormats = pModelAsset->INL_GetVertexFormats();

		if (NumMesh == 0 || TotalVertexCount == 0 || TotalIndexCount == 0 || VertexFormats == 0)
		{
			LOG_ERR("Model asset has no renderable geometry. Cannot create model.");
			return nullptr;
		}

		// TANGENT 포맷을 요구하지만 아직 계산되지 않았다면 사전 계산.
		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0 && !pModelAsset->INL_HasComputedTangent())
			pModelAsset->ComputeTangent();

		const INDEX_FORMAT IndexFormat = pModelAsset->INL_GetMeshData().IndexFormat;

		switch (Layout)
		{
			case MODEL_LAYOUT::MERGED:
			{
				// 모든 메쉬가 구간을 공유하는 단일 병합(MERGED) PrimitiveBuffer 를 생성한다.
				PrimitiveBuffer* pPrimitiveBuffer = CORE_NEW(PrimitiveBuffer);
				if (!pPrimitiveBuffer->Create(
					pRenderSystem,
					VertexFormats,
					TotalVertexCount,
					IndexFormat,
					TotalIndexCount
				))
				{
					LOG_ERR("Failed to create merged primitive buffer for model.");
					CORE_DELETE(PrimitiveBuffer, pPrimitiveBuffer);
					return nullptr;
				}

				Model* pModel = CORE_NEW(Model)(pPrimitiveBuffer);

				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					const ModelAsset::Mesh* pMesh = pModelAsset->INL_GetMesh(i);

					// 메쉬 예약 (커서 기반으로 병합 버퍼 내 구간이 순차 할당됨 = 에셋의 StartVertex/StartIndex 와 일치).
					const int32_t MeshIndex = pModel->CreateMesh(
						name_hash(pMesh->szName),
						PRIMITIVE_TYPE::TRIANGLE_LIST,
						VertexFormats,
						pMesh->NumVertex,
						pMesh->NumIndex
					);

					if (MeshIndex < 0)
					{
						LOG_ERR_F("Failed to create mesh {} while building model.", i);
						pModel->Release();
						return nullptr;
					}

					if (!INL_BuildMeshData(pModel, pModelAsset, i, MeshIndex, VertexFormats, IndexFormat))
					{
						pModel->Release();
						return nullptr;
					}
				}

				return pModel;
			}

			case MODEL_LAYOUT::SEPARATE:
			{
				// 각 메쉬가 독립적인 PrimitiveBuffer 를 소유하는 분리(SEPARATE) 레이아웃.
				// 공유 버퍼는 없으므로 Model 의 병합 버퍼는 nullptr 로 둔다.
				Model* pModel = CORE_NEW(Model)(nullptr);

				for (uint32_t i = 0; i < NumMesh; ++i)
				{
					const ModelAsset::Mesh* pMesh = pModelAsset->INL_GetMesh(i);

					// 메쉬 크기에 정확히 맞춘 독립 PrimitiveBuffer 를 생성한다.
					PrimitiveBuffer* pMeshBuffer = CORE_NEW(PrimitiveBuffer);
					if (!pMeshBuffer->Create(
						pRenderSystem,
						VertexFormats,
						pMesh->NumVertex,
						IndexFormat,
						pMesh->NumIndex
					))
					{
						LOG_ERR_F("Failed to create separate primitive buffer for mesh {}.", i);
						CORE_DELETE(PrimitiveBuffer, pMeshBuffer);
						pModel->Release();
						return nullptr;
					}

					// 메쉬가 소유 버퍼를 갖도록 등록 (오프셋 0 기준).
					const int32_t MeshIndex = pModel->CreateMeshWithBuffer(
						pMeshBuffer,
						name_hash(pMesh->szName),
						PRIMITIVE_TYPE::TRIANGLE_LIST,
						VertexFormats,
						pMesh->NumVertex,
						pMesh->NumIndex
					);

					if (MeshIndex < 0)
					{
						LOG_ERR_F("Failed to create mesh {} while building model.", i);
						CORE_DELETE(PrimitiveBuffer, pMeshBuffer);
						pModel->Release();
						return nullptr;
					}

					if (!INL_BuildMeshData(pModel, pModelAsset, i, MeshIndex, VertexFormats, IndexFormat))
					{
						pModel->Release();
						return nullptr;
					}
				}

				return pModel;
			}

			default:
			{
				LOG_ERR("Unknown MODEL_LAYOUT. Cannot create model.");
				return nullptr;
			}
		}
	}
}
