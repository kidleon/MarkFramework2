#include "pch.h"
#include "ModelAsset.h"

#include "ufbx/ufbx.h"
#include "file_system.h"


namespace mark
{
	ModelAsset::~ModelAsset() noexcept
	{
		for (size_t i = 0; i < m_lstMaterial.size(); ++i)
		{
			CORE_SYS_FREE(m_lstMaterial[i]);
		}

		for (size_t i = 0; i < m_lstMesh.size(); ++i)
		{
			m_lstMesh[i]->Cleanup();
			CORE_SYS_FREE(m_lstMesh[i]);
		}
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

		safe_strcpy(MeshDesc.szName, MESH_NAME_SIZE, m_lstMesh[MeshIndex]->szName);
		MeshDesc.NumVertex = m_lstMesh[MeshIndex]->NumVertex;
		MeshDesc.pPosition = m_lstMesh[MeshIndex]->pPosition;
		MeshDesc.pNormal = m_lstMesh[MeshIndex]->pNormal;
		MeshDesc.pColor = m_lstMesh[MeshIndex]->pColor;
		MeshDesc.pTangent = m_lstMesh[MeshIndex]->pTangent;
		MeshDesc.pTexCoord0 = m_lstMesh[MeshIndex]->pTexCoord0;
		MeshDesc.NumSubset = m_lstMesh[MeshIndex]->NumSubMesh;
		MeshDesc.IndexFormat = (m_lstMesh[MeshIndex]->IndexSize == 2) ? INDEX_FORMAT::UINT16 : INDEX_FORMAT::UINT32;
		MeshDesc.NumIndex = m_lstMesh[MeshIndex]->NumIndex;
		MeshDesc.pIndices = m_lstMesh[MeshIndex]->pIndices;

		for(uint32_t s = 0; s < m_lstMesh[MeshIndex]->NumSubMesh; s++)
		{
			MeshDesc.SubsetIndexStart [s] = m_lstMesh[MeshIndex]->pSubMesh[s].StartIndex;
			MeshDesc.SubsetIndexCount[s] = m_lstMesh[MeshIndex]->pSubMesh[s].IndexCount;
		}
	}

	void ModelAsset::Reset()
	{
		for (size_t i = 0; i < m_lstMaterial.size(); ++i)
		{
			CORE_SYS_FREE(m_lstMaterial[i]);
		}
		m_lstMaterial.clear();

		for (size_t i = 0; i < m_lstMesh.size(); ++i)
		{
			m_lstMesh[i]->Cleanup();
			CORE_SYS_FREE(m_lstMesh[i]);
		}

		m_lstMesh.clear();
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
		for (size_t i = 0; i < scene->meshes.count; ++i)
		{
			ufbx_mesh* mesh = scene->meshes.data[i];
			if (!mesh->num_vertices)
				continue;

			TotalVetexCount += (uint32_t)mesh->num_vertices;

			ModelAsset::Mesh* pMesh = (ModelAsset::Mesh*)CORE_SYS_CALLOC(sizeof(ModelAsset::Mesh));
			safe_strcpy(pMesh->szName, MESH_NAME_SIZE, mesh->name.data);

			if (mesh->vertex_position.exists)
			{
				pMesh->pPosition = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 position = ufbx_get_vertex_vec3(&mesh->vertex_position, v);
					pMesh->pPosition[v].x = (float)position.x;
					pMesh->pPosition[v].y = (float)position.y;
					pMesh->pPosition[v].z = (float)position.z;
				}
			}

			if (mesh->vertex_normal.exists)
			{
				pMesh->pNormal = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, v);
					pMesh->pNormal[v].x = (float)normal.x;
					pMesh->pNormal[v].y = (float)normal.y;
					pMesh->pNormal[v].z = (float)normal.z;
				}
			}

			if (mesh->vertex_uv.exists)
			{
				pMesh->pTexCoord0 = (FLOAT2*)CORE_SYS_ALLOC(sizeof(FLOAT2) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, v);
					pMesh->pTexCoord0[v].x = (float)uv.x;
					pMesh->pTexCoord0[v].y = (float)uv.y;
				}
			}

			if (mesh->vertex_tangent.exists)
			{
				pMesh->pTangent = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, v);
					pMesh->pTangent[v].x = (float)tangent.x;
					pMesh->pTangent[v].y = (float)tangent.y;
					pMesh->pTangent[v].z = (float)tangent.z;
				}
			}

			INDEX_FORMAT IndexFormat = (TotalVetexCount > 65500) ? INDEX_FORMAT::UINT32 : INDEX_FORMAT::UINT16;

			sys_vector<uint32_t> Indices32;
			sys_vector<uint16_t> Indices16;
			size_t IndexSize = (IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t);

			if(IndexFormat == INDEX_FORMAT::UINT32)
			{
				Indices32.reserve(mesh->num_triangles * 3);
			}
			else
			{
				Indices16.reserve(mesh->num_triangles * 3);
			}

			if (0 < mesh->material_parts.count)
			{
				ufbx_mesh_part* part = &mesh->material_parts.data[0];
				ufbx_material* material = mesh->materials.data[part->index];

				pMesh->pSubMesh = (ModelAsset::SubMesh*)CORE_SYS_ALLOC(sizeof(ModelAsset::SubMesh) * mesh->material_parts.count);
				pMesh->NumSubMesh = (uint32_t)mesh->material_parts.count;

				uint32_t StartIndex = 0;
				for (uint32_t s = 0; s < pMesh->NumSubMesh; ++s)
				{
					ufbx_mesh_part* part = &mesh->material_parts.data[s];
					pMesh->pSubMesh[s].MatID = (int32_t)material->element_id;
					pMesh->pSubMesh[s].StartIndex = StartIndex;
					
					uint32_t FaceIndexCount = 0;
					for (size_t f = 0; f < part->face_indices.count; ++f)
					{
						uint32_t face_index = part->face_indices.data[f];

						ufbx_face face = mesh->faces.data[face_index];

						if (!face.num_indices)
							continue;

						size_t num_tris = face.num_indices - 2;

						if (IndexFormat == INDEX_FORMAT::UINT32)
						{
							for (size_t tri = 0; tri < num_tris; ++tri)
							{
								uint32_t tri_indices[3];
								ufbx_triangulate_face(tri_indices, 3, mesh, face);

								uint32_t idx0 = tri_indices[0];
								uint32_t idx1 = tri_indices[1];
								uint32_t idx2 = tri_indices[2];

								Indices32.push_back(idx0);
								Indices32.push_back(idx1);
								Indices32.push_back(idx2);
								FaceIndexCount += 3;
							}
						}
						else
						{
							for (size_t tri = 0; tri < num_tris; ++tri)
							{
								uint32_t tri_indices[3];
								ufbx_triangulate_face(tri_indices, 3, mesh, face);

								uint16_t idx0 = (uint16_t)tri_indices[0];
								uint16_t idx1 = (uint16_t)tri_indices[1];
								uint16_t idx2 = (uint16_t)tri_indices[2];

								Indices16.push_back(idx0);
								Indices16.push_back(idx1);
								Indices16.push_back(idx2);
								FaceIndexCount += 3;
							}
						}
					}

					pMesh->pSubMesh[s].IndexCount = FaceIndexCount;
					StartIndex += FaceIndexCount;
				}
			}

			size_t IndexCount = (IndexFormat == INDEX_FORMAT::UINT32) ? Indices32.size() : Indices16.size();
			pMesh->NumIndex = (uint32_t)IndexCount;
			pMesh->IndexSize = (uint32_t)IndexSize;
			pMesh->pIndices = CORE_SYS_ALLOC(IndexSize * IndexCount);
			memcpy(pMesh->pIndices, (IndexFormat == INDEX_FORMAT::UINT32) ? (void*)Indices32.data() : (void*)Indices16.data(), IndexSize * IndexCount);

			pModelAsset->m_lstMesh.push_back(pMesh);
		}

		if (scene)
			ufbx_free_scene(scene);

		pModelAsset->Release();
		pBlob->Release();

		return true;
	}
}
