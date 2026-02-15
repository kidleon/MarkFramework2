#include "pch.h"
#include "FBXModelLoader.h"
#define UFBX_REAL_IS_FLOAT
#include "ufbx.h"
#include "ModelAsset.h"
#include "temp_pool.h"
#include "TArray.h"


static inline ufbx_scene* load_fbx_from_memory(const void* data, size_t data_size)
{
	ufbx_load_opts opts = { 0 };

	// 필요에 따라 옵션 설정
	opts.target_axes = ufbx_axes_left_handed_y_up;	// 좌표계 변환 (선택)
	opts.target_unit_meters = 1.0f;                 // 단위 변환 (선택)

	ufbx_error error;
	ufbx_scene* scene = ufbx_load_memory(data, data_size, &opts, &error);

	return scene;
}

/// Skin Weight와 Bone Index 추출
void extract_skin_data(ufbx_mesh * mesh, size_t vertex_index, float out_weights[4], int out_indices[4]) 
{
	// 초기화
	for (int i = 0; i < 4; i++) 
	{
		out_weights[i] = 0.0f;
		out_indices[i] = 0;
	}

	// 스킨 데이터가 있는지 확인
	if (mesh->skin_deformers.count > 0) 
	{
		ufbx_skin_deformer* skin = mesh->skin_deformers.data[0];

		// 해당 버텍스의 스킨 가중치 가져오기
		ufbx_skin_vertex skin_vertex = skin->vertices.data[vertex_index];

		// 최대 4개의 가중치만 사용
		size_t num_weights = skin_vertex.num_weights < 4 ? skin_vertex.num_weights : 4;

		for (size_t i = 0; i < num_weights; i++) 
		{
			ufbx_skin_weight weight = skin->weights.data[skin_vertex.weight_begin + i];
			out_weights[i] = (float)weight.weight;
			out_indices[i] = (int)weight.cluster_index;
		}

		// 가중치 정규화 (합이 1.0이 되도록)
		float total_weight = 0.0f;
		for (int i = 0; i < 4; i++) 
		{
			total_weight += out_weights[i];
		}

		if (total_weight > 0.0f) 
		{
			for (int i = 0; i < 4; i++) 
			{
				out_weights[i] /= total_weight;
			}
		}
	}
}

BOOL LoadModelFromFBX(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	IModelAsset* pModelAsset
)
{
	ModelAsset* pModelAssetImpl = static_cast<ModelAsset*>(pModelAsset);

	IDataStream* pDataStream = pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Failed to open FBX file: %s", szRelativePath);
		return FALSE;
	}

	size_t streamSize = pDataStream->GetSize();
	//char* pBuffer = (char*)CORE_SYS_ALLOC(streamSize);
	char* pBuffer = (char*)temppool_alloc(hTempPool, streamSize);
	if (!pBuffer)
	{
		SYS_LOG_E("Failed to allocate memory for FBX file: %s", szRelativePath);
		pDataStream->Release();
		return FALSE;
	}

	pDataStream->Read(pBuffer, streamSize);

	ufbx_scene* scene = load_fbx_from_memory(pBuffer, streamSize);
	if (!scene)
	{
		pModelAssetImpl->INL_SetLoadStat(LOAD_STAT::FAILED);

		return FALSE;
	}

	temppool_clear(hTempPool);


	if (scene->meshes.count > 0)
	{
		// 메시 로드
		for (size_t i = 0; i < scene->meshes.count; ++i)
		{
			ufbx_mesh* mesh = scene->meshes.data[i];

			// 메시 데이터를 ModelAsset에 추가하는 코드 작성
			FLOAT3* positions = nullptr;
			FLOAT3* normals = nullptr;
			FLOAT2* texUV = nullptr;
			FLOAT4* colors = nullptr;
			FLOAT3* tangents = nullptr;
			FLOAT3* binormals = nullptr;

			// Positions
			if (mesh->vertex_position.exists)
			{
				positions = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, v);
					positions[v] = { pos.x, pos.y, pos.z };
				}
			}

			// Normals
			if (mesh->vertex_normal.exists)
			{
				normals = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 norm = ufbx_get_vertex_vec3(&mesh->vertex_normal, v);
					normals[v] = { norm.x, norm.y, norm.z };
				}
			}

			// UVs
			if (mesh->vertex_uv.exists)
			{
				texUV = (FLOAT2*)CORE_SYS_ALLOC(sizeof(FLOAT2) * mesh->num_vertices);
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, v);
					texUV[v] = { uv.x, uv.y };
				}
			}

			// Colors
			if(mesh->vertex_color.exists)
			{
				colors = (FLOAT4*)CORE_SYS_ALLOC(sizeof(FLOAT4) * mesh->num_vertices);

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec4 color = ufbx_get_vertex_vec4(&mesh->vertex_color, v);
					colors[v] = { color.x, color.y, color.z, color.w };
				}
			}

			// Tangent
			if (mesh->vertex_tangent.exists)
			{
				tangents = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);

				// Tangent이 존재하는 경우 처리
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, v);
					tangents[v] = { tangent.x, tangent.y, tangent.z };
				}
			}

			// Binormals
			if (mesh->vertex_bitangent.exists)
			{
				binormals = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh->num_vertices);

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 binormal = ufbx_get_vertex_vec3(&mesh->vertex_bitangent, v);
					binormals[v] = { binormal.x, binormal.y, binormal.z };
				}
			}
			
			/*
			// Skin Weights and Joints
			if (mesh->skin_deformers.count > 0)
			{
				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					float weights[4];
					int indices[4];
					extract_skin_data(mesh, v, weights, indices);

					// Add Weights and Indices
				}
			}
			*/

			size_t num_material = mesh->materials.count;

		}
	}

	if(scene->materials.count > 0)
	{
		// 머티리얼 로드
		for (size_t i = 0; i < scene->materials.count; ++i)
		{
			ufbx_material* material = scene->materials.data[i];

			// 머티리얼 데이터를 ModelAsset에 추가하는 코드 작성
			// material->fbx.diffuse_color.texture
			/*material->element_id
			material->fbx.normal_map.texture_enabled)
			material->fbx.specular_color.texture;
			material->fbx.emission_color.texture;
			material->fbx.transparency_color.texture;
			material->fbx.ambient_color.value_vec4;
			*/
		}
	}

	return TRUE;
}


void AsyncLoadModelFromFBX(void* pArg)
{

}