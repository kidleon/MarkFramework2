#include "pch.h"
#include "ModelAsset.h"
#include "file_system.h"

#include <ufbx/ufbx.h>
#include <mikktspace/mikktspace.h>


namespace mark
{
	ModelAsset::ModelAsset(const char* szAssetPath)
		: m_AssetPath(szAssetPath)
	{
	}

	ModelAsset::~ModelAsset() noexcept
	{
		for (size_t i = 0; i < m_lstMaterial.size(); ++i)
		{
			CORE_SYS_FREE(m_lstMaterial[i]);
		}

		m_lstMesh.clear();
		m_MeshData.clear();
			
	}

	void ModelAsset::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void ModelAsset::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(ModelAsset, this);
		}
	}

	uint32_t ModelAsset::GetNumMeshes() const noexcept
	{
		return (uint32_t)m_lstMesh.size();
	}

	void ModelAsset::GetMeshDesc(uint32_t MeshIndex, MESH_DESC& MeshDesc) const
	{
		assert(MeshIndex < (uint32_t)m_lstMesh.size() && "MeshIndex out of range");

		const Mesh& mesh = m_lstMesh[MeshIndex];

		safe_strcpy(MeshDesc.szName, MESH_NAME_SIZE, mesh.szName);
		MeshDesc.NumVertex = mesh.NumVertex;
		MeshDesc.NumIndex = mesh.NumIndex;
		MeshDesc.IndexFormat = m_MeshData.IndexFormat;
		MeshDesc.NumSubset = mesh.NumSubMesh;
		MeshDesc.pPosition = m_MeshData.pPosition + mesh.StartVertex;
		MeshDesc.pNormal = m_MeshData.pNormal + mesh.StartVertex;
		MeshDesc.pColor = m_MeshData.pColor + mesh.StartVertex;
		MeshDesc.pTangent = m_MeshData.pTangent + mesh.StartVertex;
		MeshDesc.pTexCoord0 = m_MeshData.pTexCoord0 + mesh.StartVertex;

		const size_t IndexSize = (MeshDesc.IndexFormat == INDEX_FORMAT::UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
		MeshDesc.pIndices = m_MeshData.pIndices ? (static_cast<uint8_t*>(m_MeshData.pIndices) + mesh.StartIndex * IndexSize) : nullptr;

		for(uint32_t s = 0; s < mesh.NumSubMesh; s++)
		{
			MeshDesc.SubsetIndexStart[s] = mesh.SubMesh[s].StartOffset;
			MeshDesc.SubsetIndexCount[s] = mesh.SubMesh[s].IndexCount;
		}
	}

	void ModelAsset::Reset()
	{
		for (size_t i = 0; i < m_lstMaterial.size(); ++i)
		{
			CORE_SYS_FREE(m_lstMaterial[i]);
		}
		m_lstMaterial.clear();

		m_lstMesh.clear();
		m_MeshData.clear();
	}

	struct MikkTSpaceContext
	{
		ModelAsset::Mesh* pMesh;        // 현재 처리 중인 메시
		ModelAsset::MeshData* pMeshData; // 메시 데이터 (위치, 노멀, UV, 인덱스 등)
		uint32_t TriangleCount;          // 삼각형 개수 (인덱스 / 3)
	};

	static inline uint32_t GetVertexIndex(const SMikkTSpaceContext* pContext, int iFace, int iVert)
	{
		const auto* ctx = static_cast<const MikkTSpaceContext*>(pContext->m_pUserData);
		const ModelAsset::Mesh* pMesh = ctx->pMesh;

		// 삼각형의 N번째 정점 = (iFace * 3 + iVert) 위치의 인덱스 값
		//const uint32_t indexPos = iFace * 3 + iVert + pMesh->StartIndex; // 메시의 시작 인덱스 오프셋 추가
		const uint32_t indexPos = iFace * 3 + iVert; // 메시의 시작 인덱스 오프셋 추가

		if (ctx->pMeshData->IndexFormat == INDEX_FORMAT::UINT16)
		{
			const uint16_t* pIndices16 = static_cast<const uint16_t*>(ctx->pMeshData->pIndices) + pMesh->StartIndex; // 메시의 시작 인덱스 오프셋 추가
			return static_cast<uint32_t>(pIndices16[indexPos]);
		}
		else
		{
			const uint32_t* pIndices32 = static_cast<const uint32_t*>(ctx->pMeshData->pIndices) + pMesh->StartIndex; // 메시의 시작 인덱스 오프셋 추가
			return pIndices32[indexPos];
		}
	}

	static int MikkGetNumFaces(const SMikkTSpaceContext* pContext)
	{
		const auto* ctx = static_cast<const MikkTSpaceContext*>(pContext->m_pUserData);
		return static_cast<int>(ctx->TriangleCount);
	}

	static int MikkGetNumVerticesOfFace(const SMikkTSpaceContext* /*pContext*/, const int /*iFace*/)
	{
		return 3;
	}

	static void MikkGetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
	{
		const uint32_t vertIdx = GetVertexIndex(pContext, iFace, iVert);

		const auto* ctx = static_cast<const MikkTSpaceContext*>(pContext->m_pUserData);
		const FLOAT3& pos = ctx->pMeshData->pPosition[vertIdx + ctx->pMesh->StartVertex];
		fvPosOut[0] = pos.x;
		fvPosOut[1] = pos.y;
		fvPosOut[2] = pos.z;
	}

	static void MikkGetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
	{
		const uint32_t vertIdx = GetVertexIndex(pContext, iFace, iVert);

		const auto* ctx = static_cast<const MikkTSpaceContext*>(pContext->m_pUserData);
		const FLOAT3& nrm = ctx->pMeshData->pNormal[vertIdx + ctx->pMesh->StartVertex];
		fvNormOut[0] = nrm.x;
		fvNormOut[1] = nrm.y;
		fvNormOut[2] = nrm.z;
	}

	static void MikkGetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
	{
		const uint32_t vertIdx = GetVertexIndex(pContext, iFace, iVert);

		const auto* ctx = static_cast<const MikkTSpaceContext*>(pContext->m_pUserData);
		const FLOAT2& uv = ctx->pMeshData->pTexCoord0[vertIdx + ctx->pMesh->StartVertex];
		fvTexcOut[0] = uv.x;
		fvTexcOut[1] = uv.y;
	}

	static void MikkSetTSpaceBasic(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
	{
		const uint32_t vertIdx = GetVertexIndex(pContext, iFace, iVert);

		auto* ctx = static_cast<MikkTSpaceContext*>(pContext->m_pUserData);
		ctx->pMeshData->pTangent[vertIdx + ctx->pMesh->StartVertex].x = fvTangent[0];
		ctx->pMeshData->pTangent[vertIdx + ctx->pMesh->StartVertex].y = fvTangent[1];
		ctx->pMeshData->pTangent[vertIdx + ctx->pMesh->StartVertex].z = fvTangent[2];
		ctx->pMeshData->pTangent[vertIdx + ctx->pMesh->StartVertex].w = fSign; // w 채널에 fSign 저장 (1.0f 또는 -1.0f)
	}

	void ModelAsset::ComputeTangent()
	{
		if (m_ComputedTangent) return;

		if (!m_MeshData.pPosition ||
			!m_MeshData.pNormal ||
			!m_MeshData.pTexCoord0 ||
			!m_MeshData.pIndices)
			return;


		// MikkTSpace 인터페이스 콜백 등록
		SMikkTSpaceInterface mikkInterface = {};
		mikkInterface.m_getNumFaces = MikkGetNumFaces;
		mikkInterface.m_getNumVerticesOfFace = MikkGetNumVerticesOfFace;
		mikkInterface.m_getPosition = MikkGetPosition;
		mikkInterface.m_getNormal = MikkGetNormal;
		mikkInterface.m_getTexCoord = MikkGetTexCoord;
		mikkInterface.m_setTSpaceBasic = MikkSetTSpaceBasic;
		mikkInterface.m_setTSpace = nullptr;  // basic 버전만 사용
		
		if (m_MeshData.pTangent)
			CORE_SYS_FREE(m_MeshData.pTangent);

		m_MeshData.pTangent = static_cast<FLOAT4*>(CORE_SYS_CALLOC(sizeof(FLOAT4) * m_MeshData.TotalVertexCount));
		if (!m_MeshData.pTangent) [[unlikely]]
			return;

		// 모든 메시 순회
		for (ModelAsset::Mesh& Mesh : m_lstMesh)
		{
			// 인덱스 개수가 3의 배수가 아니면 삼각형 메시가 아님
			if (Mesh.NumIndex % 3 != 0)
				continue;

			// MikkTSpace 컨텍스트 준비
			MikkTSpaceContext userContext = {};
			userContext.pMesh = &Mesh;
			userContext.TriangleCount = Mesh.NumIndex / 3;

			SMikkTSpaceContext mikkContext = {};
			mikkContext.m_pInterface = &mikkInterface;
			mikkContext.m_pUserData = &userContext;

			// 탄젠트 생성 실행
			// 두 번째 인자는 angular threshold (기본값 180.0f, 부드러운 결과)
			genTangSpaceDefault(&mikkContext);
		}

		m_ComputedTangent = TRUE;
	}


	inline static void change_psd_to_tag(const char* filename, char* out_filename, size_t out_size)
	{
		char extension[16];
		file_system::get_file_extension(filename, extension, sizeof(extension));
		if (!_strcmpi(extension, ".psd"))
		{
			file_system::change_extension(
				filename,
				".tga",
				out_filename,
				out_size
			);
		}
	}

	bool LoadModelFromFBX(IAssetBlob* pBlob, ModelAsset* pModelAsset)
	{
		pBlob->AddRef();
		pModelAsset->AddRef();

		ufbx_load_opts load_opts = {};
		load_opts.obj_axes = ufbx_axes_left_handed_y_up;
		load_opts.obj_unit_meters = 1.0f;
		load_opts.normalize_tangents = true;
		load_opts.normalize_normals = true;

		ufbx_error error = {};
		ufbx_scene* scene = ufbx_load_memory(pBlob->GetData(), pBlob->GetDataSize(), &load_opts, &error);
		if (!scene)
		{
			pBlob->Release();
			pModelAsset->Release();
			return false;
		}

		char szTempPath[MAX_TEXTURE_FILENAME] = {};

		// Load materials
		for (size_t i = 0; i < scene->materials.count; ++i)
		{
			ModelAsset::Material* pMaterial = (ModelAsset::Material*)CORE_SYS_CALLOC(sizeof(ModelAsset::Material));

			ufbx_material* material = scene->materials.data[i];

			pMaterial->MatID = (int32_t)material->element_id;
			if (material->fbx.diffuse_color.texture_enabled && material->fbx.diffuse_color.texture)
			{
				file_system::get_filename(
					material->fbx.diffuse_color.texture->filename.data,
					szTempPath,
					MAX_TEXTURE_FILENAME
				);

				change_psd_to_tag(
					szTempPath,
					pMaterial->szDiffuse,
					MAX_TEXTURE_FILENAME
				);
			}

			if (material->fbx.normal_map.texture_enabled && material->fbx.normal_map.texture)
			{
				file_system::get_filename(
					material->fbx.normal_map.texture->filename.data,
					szTempPath,
					MAX_TEXTURE_FILENAME
				);

				change_psd_to_tag(
					szTempPath,
					pMaterial->szNormal,
					MAX_TEXTURE_FILENAME
				);
			}

			if (material->fbx.specular_color.texture_enabled && material->fbx.specular_color.texture)
			{
				file_system::get_filename(
					material->fbx.specular_color.texture->filename.data,
					szTempPath,
					MAX_TEXTURE_FILENAME
				);

				change_psd_to_tag(
					szTempPath,
					pMaterial->szSpecular,
					MAX_TEXTURE_FILENAME
				);
			}

			if (material->fbx.emission_color.texture_enabled && material->fbx.emission_color.texture)
			{
				file_system::get_filename(
					material->fbx.emission_color.texture->filename.data,
					szTempPath,
					MAX_TEXTURE_FILENAME
				);

				change_psd_to_tag(
					szTempPath,
					pMaterial->szEmissive,
					MAX_TEXTURE_FILENAME
				);
			}

			pModelAsset->m_lstMaterial.push_back(pMaterial);
		}

		// Load meshes
		pModelAsset->m_lstMesh.reserve(scene->meshes.count);

		uint32_t TotalVetexCount = 0;
		uint32_t TotalIndexCount = 0;
		uint32_t VertexFormats = 0;

		// Mesh정보 수집 및 전체 버텍스/인덱스 개수 계산
		for (size_t i = 0; i < scene->meshes.count; ++i)
		{
			ufbx_mesh* mesh = scene->meshes.data[i];
			if (!mesh->num_vertices)
				continue;

			ModelAsset::Mesh Mesh = {};
			Mesh.StartVertex = TotalVetexCount;
			Mesh.StartIndex = TotalIndexCount;
			Mesh.NumVertex = (uint32_t)mesh->num_vertices;
			
			if (mesh->vertex_position.exists)
				VertexFormats |= (uint32_t)VERTEX_FORMAT::POSITION;
			if (mesh->vertex_normal.exists)
				VertexFormats |= (uint32_t)VERTEX_FORMAT::NORMAL;
			if (mesh->vertex_color.exists)
				VertexFormats |= (uint32_t)VERTEX_FORMAT::COLOR;
			if (mesh->vertex_uv.exists)
				VertexFormats |= (uint32_t)VERTEX_FORMAT::TEXCOORD0;
			// 탄젠트는 MikkTSpace로 계산할 예정이므로, 메시 자체에 탄젠트 정보가 있더라도 무시하고 MikkTSpace에서 생성하도록 함
			//if (mesh->vertex_tangent.exists)
				//VertexFormats |= (uint32_t)VERTEX_FORMAT::TANGENT;

			Mesh.NumSubMesh = (uint32_t)std::min(mesh->material_parts.count, MAX_SUBMESH_COUNT);
			
			if (0 < Mesh.NumSubMesh)
			{
				uint32_t MeshIndexCount = 0;

				for (uint32_t s = 0; s < Mesh.NumSubMesh; ++s)
				{
					ufbx_mesh_part* part = &mesh->material_parts.data[s];
					if (part->face_indices.count == 0)
						continue;

					ufbx_material* material = mesh->materials.data[part->index];
					Mesh.SubMesh[s].MatID = (int32_t)material->element_id;
					Mesh.SubMesh[s].StartOffset = MeshIndexCount;
					Mesh.SubMesh[s].IndexCount = 0;

					uint32_t SubsetIndexCount = 0;
					for (size_t f = 0; f < part->face_indices.count; ++f)
					{
						uint32_t face_index = part->face_indices.data[f];
						ufbx_face face = mesh->faces.data[face_index];

						if (!face.num_indices)
							continue;

						uint32_t num_face_indices = (uint32_t)(face.num_indices - 2) * 3; // 삼각형으로 분할된 인덱스 개수

						Mesh.SubMesh[s].IndexCount += num_face_indices;
						SubsetIndexCount += num_face_indices;
					}

					Mesh.SubMesh[s].IndexCount = SubsetIndexCount;
					MeshIndexCount += SubsetIndexCount;
				}

				Mesh.NumIndex = MeshIndexCount;
			}

			TotalVetexCount += Mesh.NumVertex;
			TotalIndexCount += Mesh.NumIndex;
		}

		// 메시 데이터 버퍼 할당
		pModelAsset->m_MeshData.TotalVertexCount = TotalVetexCount;
		pModelAsset->m_MeshData.VertexFormats = VertexFormats;
		pModelAsset->m_MeshData.IndexFormat = (TotalVetexCount > 65500) ? INDEX_FORMAT::UINT32 : INDEX_FORMAT::UINT16;

		if (VertexFormats & (uint32_t)VERTEX_FORMAT::POSITION)
			pModelAsset->m_MeshData.pPosition = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * TotalVetexCount);

		if (VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL)
			pModelAsset->m_MeshData.pNormal = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * TotalVetexCount);

		if (VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR)
			pModelAsset->m_MeshData.pColor = (FLOAT4*)CORE_SYS_ALLOC(sizeof(FLOAT4) * TotalVetexCount);

		if (VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0)
			pModelAsset->m_MeshData.pTexCoord0 = (FLOAT2*)CORE_SYS_ALLOC(sizeof(FLOAT2) * TotalVetexCount);

		//if (VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT)
			//pModelAsset->m_MeshData.pTangent = (FLOAT4*)CORE_SYS_ALLOC(sizeof(FLOAT4) * TotalVetexCount);

		size_t IndexSize = (pModelAsset->m_MeshData.IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t);
		pModelAsset->m_MeshData.pIndices = CORE_SYS_CALLOC(IndexSize * TotalIndexCount);

		// Vertex
		size_t PositionIndex = 0;
		size_t NormalIndex = 0;
		size_t ColorIndex = 0;
		size_t TexCoord0Index = 0;
		size_t TangentIndex = 0;
		size_t IndicesIndex = 0;

		for (size_t i = 0; i < scene->meshes.count; ++i)
		{
			ufbx_mesh* mesh = scene->meshes.data[i];
			if (!mesh->num_vertices)
				continue;

			if (VertexFormats & (uint32_t)VERTEX_FORMAT::POSITION)
			{
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 position = ufbx_get_vertex_vec3(&mesh->vertex_position, v);
					pModelAsset->m_MeshData.pPosition[PositionIndex].x = (float)position.x;
					pModelAsset->m_MeshData.pPosition[PositionIndex].y = (float)position.y;
					pModelAsset->m_MeshData.pPosition[PositionIndex].z = (float)position.z;
					++PositionIndex;
				}
			}

			if (VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL)
			{
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, v);
					pModelAsset->m_MeshData.pNormal[NormalIndex].x = (float)normal.x;
					pModelAsset->m_MeshData.pNormal[NormalIndex].y = (float)normal.y;
					pModelAsset->m_MeshData.pNormal[NormalIndex].z = (float)normal.z;
					++NormalIndex;
				}
			}

			if (VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR)
			{
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec4 color = ufbx_get_vertex_vec4(&mesh->vertex_color, v);
					pModelAsset->m_MeshData.pColor[ColorIndex].x = (float)color.x;
					pModelAsset->m_MeshData.pColor[ColorIndex].y = (float)color.y;
					pModelAsset->m_MeshData.pColor[ColorIndex].z = (float)color.z;
					pModelAsset->m_MeshData.pColor[ColorIndex].w = (float)color.w;
					++ColorIndex;
				}
			}

			if (VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0)
			{
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, v);
					pModelAsset->m_MeshData.pTexCoord0[TexCoord0Index].x = (float)uv.x;
					pModelAsset->m_MeshData.pTexCoord0[TexCoord0Index].y = (float)uv.y;
					++TexCoord0Index;
				}
			}

			/*
			* 탄젠트는 MikkTSpace로 계산할 예정이므로, 메시 자체에 탄젠트 정보가 있더라도 무시하고 MikkTSpace에서 생성하도록 함
			if (VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT)
			{
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, v);
					pModelAsset->m_MeshData.pTangent[TangentIndex].x = (float)tangent.x;
					pModelAsset->m_MeshData.pTangent[TangentIndex].y = (float)tangent.y;
					pModelAsset->m_MeshData.pTangent[TangentIndex].z = (float)tangent.z;
					++TangentIndex;
				}
			}
			*/

			size_t NumSubset = std::min(mesh->material_parts.count, MAX_SUBMESH_COUNT);
			if (0 < NumSubset)
			{
				for (size_t s = 0; s < NumSubset; ++s)
				{
					ufbx_mesh_part* part = &mesh->material_parts.data[s];
					for (size_t f = 0; f < part->face_indices.count; ++f)
					{
						uint32_t face_index = part->face_indices.data[f];

						ufbx_face face = mesh->faces.data[face_index];

						if (!face.num_indices)
							continue;

						size_t num_tris = face.num_indices - 2;

						for (size_t tri = 0; tri < num_tris; ++tri)
						{
							uint32_t tri_indices[3];
							ufbx_triangulate_face(tri_indices, 3, mesh, face);

							uint32_t idx0 = tri_indices[0];
							uint32_t idx1 = tri_indices[1];
							uint32_t idx2 = tri_indices[2];

							if (INDEX_FORMAT::UINT32 == pModelAsset->m_MeshData.IndexFormat)
							{
								((uint32_t*)pModelAsset->m_MeshData.pIndices)[IndicesIndex++] = idx0;
								((uint32_t*)pModelAsset->m_MeshData.pIndices)[IndicesIndex++] = idx1;
								((uint32_t*)pModelAsset->m_MeshData.pIndices)[IndicesIndex++] = idx2;
							}
							else
							{
								((uint16_t*)pModelAsset->m_MeshData.pIndices)[IndicesIndex++] = (uint16_t)idx0;
								((uint16_t*)pModelAsset->m_MeshData.pIndices)[IndicesIndex++] = (uint16_t)idx1;
								((uint16_t*)pModelAsset->m_MeshData.pIndices)[IndicesIndex++] = (uint16_t)idx2;
							}
						}
					}
				}
			}
		}

		if (scene)
			ufbx_free_scene(scene);

		pModelAsset->Release();
		pBlob->Release();

		return true;
	}
}
