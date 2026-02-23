#include "pch.h"
#include "fbx_loader.h"

#include "strings.h"
#include "os_file.h"
#include "ufbx.h"


struct FBX_SCENE* fbx_load(void* data, size_t size)
{
	ufbx_load_opts load_opts;
	memset(&load_opts, 0, sizeof(ufbx_load_opts));

	load_opts.obj_axes = ufbx_axes_left_handed_y_up;
	load_opts.obj_unit_meters = 1.0f;
	load_opts.normalize_tangents = true;
	load_opts.normalize_normals = true;

	ufbx_error error;
	memset(&error, 0, sizeof(ufbx_error));
	
	ufbx_scene* scene = ufbx_load_memory(data, size, &load_opts, &error);
	if (!scene)
		return NULL;

	struct FBX_SCENE* fbx_scene = (struct FBX_SCENE*)malloc(sizeof(struct FBX_SCENE));
	if (!fbx_scene)
		goto lb_error;

	memset(fbx_scene, 0, sizeof(struct FBX_SCENE));

	if (0 < scene->materials.count)
	{
		size_t mat_count = scene->materials.count;
		fbx_scene->num_materials = mat_count;
		fbx_scene->materials = (struct FBX_MATERIAL*)malloc(sizeof(struct FBX_MATERIAL) * fbx_scene->num_materials);

		if (!fbx_scene->materials)
			goto lb_error;

		for (size_t i = 0; i < scene->materials.count; ++i)
		{
			ufbx_material* material = scene->materials.data[i];
			struct FBX_MATERIAL* fbx_material = &fbx_scene->materials[i];

			fbx_material->id = (int32)material->element_id;
			fbx_material->color[0] = (float)material->fbx.diffuse_color.value_vec4.x;
			fbx_material->color[1] = (float)material->fbx.diffuse_color.value_vec4.y;
			fbx_material->color[2] = (float)material->fbx.diffuse_color.value_vec4.z;
			fbx_material->color[3] = (float)material->fbx.diffuse_color.value_vec4.w;

			if (material->fbx.diffuse_color.texture_enabled && material->fbx.diffuse_color.texture)
			{
				get_filename(
					material->fbx.diffuse_color.texture->filename.data,
					fbx_material->diffuse,
					MAX_TEXTURE_FILENAME
				);
			}
			else
				fbx_material->diffuse[0] = '\0';

			if (material->fbx.normal_map.texture_enabled && material->fbx.normal_map.texture)
			{
				get_filename(
					material->fbx.normal_map.texture->filename.data,
					fbx_material->normal,
					MAX_TEXTURE_FILENAME
				);
			}
			else
				fbx_material->normal[0] = '\0';

			if (material->fbx.specular_color.texture_enabled && material->fbx.specular_color.texture)
			{
				get_filename(
					material->fbx.specular_color.texture->filename.data,
					fbx_material->specular,
					MAX_TEXTURE_FILENAME
				);
			}
			else
				fbx_material->specular[0] = '\0';

			if (material->fbx.emission_color.texture_enabled && material->fbx.emission_color.texture)
			{
				get_filename(
					material->fbx.emission_color.texture->filename.data,
					fbx_material->emissive,
					MAX_TEXTURE_FILENAME
				);
			}
			else
				fbx_material->emissive[0] = '\0';
		}
	}

	if (0 < scene->meshes.count)
	{
		fbx_scene->model = (struct FBX_MODEL*)malloc(sizeof(struct FBX_MODEL));
		memset(fbx_scene->model, 0, sizeof(struct FBX_MODEL));

		fbx_scene->model->meshes = (struct FBX_MESH*)malloc(sizeof(struct FBX_MESH) * scene->meshes.count);
		fbx_scene->model->num_meshes = scene->meshes.count;

		memset(fbx_scene->model->meshes, 0, sizeof(struct FBX_MESH) * scene->meshes.count);

		struct FBX_MESH* meshes = fbx_scene->model->meshes;

		for (size_t i = 0; i < scene->meshes.count; ++i)
		{
			ufbx_mesh* mesh = scene->meshes.data[i];

			if (0 < mesh->name.length)
				fstrlcpy(meshes[i].name, mesh->name.data, 63);

			if (!mesh->num_vertices)
				continue;

			if (mesh->vertex_position.exists)
			{
				meshes[i].positions = (float*)malloc(sizeof(float) * 3 * mesh->num_vertices);
				if (!meshes[i].positions)
					goto lb_error;

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 position = ufbx_get_vertex_vec3(&mesh->vertex_position, v);
					meshes[i].positions[v * 3 + 0] = (float)position.x;
					meshes[i].positions[v * 3 + 1] = (float)position.y;
					meshes[i].positions[v * 3 + 2] = (float)position.z;
				}
			}

			if (mesh->vertex_normal.exists)
			{
				meshes[i].normals = (float*)malloc(sizeof(float) * 3 * mesh->num_vertices);
				if (!meshes[i].normals)
					goto lb_error;

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, v);
					meshes[i].normals[v * 3 + 0] = (float)normal.x;
					meshes[i].normals[v * 3 + 1] = (float)normal.y;
					meshes[i].normals[v * 3 + 2] = (float)normal.z;
				}
			}

			if (mesh->vertex_color.exists)
			{
				meshes[i].colors = (float*)malloc(sizeof(float) * 4 * mesh->num_vertices);
				if (!meshes[i].colors)
					goto lb_error;

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec4 color = ufbx_get_vertex_vec4(&mesh->vertex_color, v);
					meshes[i].colors[v * 4 + 0] = (float)color.x;
					meshes[i].colors[v * 4 + 1] = (float)color.y;
					meshes[i].colors[v * 4 + 2] = (float)color.z;
					meshes[i].colors[v * 4 + 3] = (float)color.w;
				}
			}

			if (mesh->vertex_uv.exists)
			{
				meshes[i].uvs = (float*)malloc(sizeof(float) * 2 * mesh->num_vertices);
				if (!meshes[i].uvs)
					goto lb_error;

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, v);
					meshes[i].uvs[v * 2 + 0] = (float)uv.x;
					meshes[i].uvs[v * 2 + 1] = (float)uv.y;
				}
			}

			if (mesh->vertex_tangent.exists)
			{
				meshes[i].tangents = (float*)malloc(sizeof(float) * 3 * mesh->num_vertices);
				if (!meshes[i].tangents)
					goto lb_error;

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, v);
					meshes[i].tangents[v * 3 + 0] = (float)tangent.x;
					meshes[i].tangents[v * 3 + 1] = (float)tangent.y;
					meshes[i].tangents[v * 3 + 2] = (float)tangent.z;
				}
			}

			if (mesh->vertex_bitangent.exists)
			{
				meshes[i].binormal = (float*)malloc(sizeof(float) * 3 * mesh->num_vertices);
				if (!meshes[i].binormal)
					goto lb_error;

				for (size_t v = 0; v < mesh->num_vertices; ++v)
				{
					ufbx_vec3 binormal = ufbx_get_vertex_vec3(&mesh->vertex_bitangent, v);
					meshes[i].binormal[v * 3 + 0] = (float)binormal.x;
					meshes[i].binormal[v * 3 + 1] = (float)binormal.y;
					meshes[i].binormal[v * 3 + 2] = (float)binormal.z;
				}
			}

			if (0 < mesh->material_parts.count)
			{
				struct FBX_SUBMESH* submeshes = (struct FBX_SUBMESH*)malloc(sizeof(struct FBX_SUBMESH) * mesh->material_parts.count);
				memset(submeshes, 0, sizeof(struct FBX_SUBMESH) * mesh->material_parts.count);

				for (size_t m = 0; m < mesh->material_parts.count; ++m)
				{
					ufbx_mesh_part* part = &mesh->material_parts.data[m];

					ufbx_material* material = mesh->materials.data[part->index];

					submeshes[m].material_id = (int32)material->element_id;
					submeshes[m].num_indices = (int32)part->num_triangles * 3;
					submeshes[m].indices = (uint32*)malloc(sizeof(uint32) * part->num_triangles * 3);

					int32 num_indices = 0;

					for (size_t f = 0; f < part->face_indices.count; ++f)
					{
						uint32 face_index = part->face_indices.data[f];
						
						ufbx_face face = mesh->faces.data[face_index];

						if (!face.num_indices) 							
							continue;

						size_t num_tris = face.num_indices - 2;

						for (size_t tri = 0; tri < num_tris; ++tri)
						{
							uint32_t tri_indices[3];
							ufbx_triangulate_face(tri_indices, 3, mesh, face);

							uint32 idx0 = tri_indices[0];
							uint32 idx1 = tri_indices[1];
							uint32 idx2 = tri_indices[2];

							if (num_indices >= submeshes[m].num_indices)
								break;

							submeshes[m].indices[num_indices++] = idx0;
							submeshes[m].indices[num_indices++] = idx1;
							submeshes[m].indices[num_indices++] = idx2;
						}
					}
				}

				meshes[i].num_submesh = mesh->material_parts.count;
				meshes[i].submeshes = submeshes;
			}
		}
	}

	if (scene)
		ufbx_free_scene(scene);

	return fbx_scene;

lb_error:
	if (fbx_scene)
	{
		if (fbx_scene->materials)
			free(fbx_scene->materials);

		if (fbx_scene->model)
		{
			for (size_t i = 0; i < fbx_scene->model->num_meshes; ++i)
			{
				struct FBX_MESH* mesh = &fbx_scene->model->meshes[i];
				if (mesh->positions)
					free(mesh->positions);
				if (mesh->normals)
					free(mesh->normals);
				if (mesh->uvs)
					free(mesh->uvs);
				if (mesh->colors)
					free(mesh->colors);
				if (mesh->tangents)
					free(mesh->tangents);
				if (mesh->binormal)
					free(mesh->binormal);
				if (mesh->submeshes)
				{
					for (size_t s = 0; s < mesh->num_submesh; ++s)
					{
						struct FBX_SUBMESH* submesh = &mesh->submeshes[s];
						if (submesh->indices)
							free(submesh->indices);
					}
					free(mesh->submeshes);
				}
			}

			if (fbx_scene->model->meshes)
				free(fbx_scene->model->meshes);

			free(fbx_scene->model);
		}

		if (fbx_scene->materials)
			free(fbx_scene->materials);

		free(fbx_scene);
	}

	if (scene)
	{
		ufbx_free_scene(scene);
		return NULL;
	}

	return NULL;
}

void fbx_unload(struct FBX_SCENE* scene)
{
	if (!scene)
		return;

	if (scene->materials)
	{
		free(scene->materials);
		scene->materials = NULL;
	}

	if (scene->model)
	{
		for (size_t i = 0; i < scene->model->num_meshes; ++i)
		{
			struct FBX_MESH* mesh = &scene->model->meshes[i];

			if (mesh->positions)
				free(mesh->positions);

			if (mesh->normals)
				free(mesh->normals);

			if (mesh->uvs)
				free(mesh->uvs);

			if (mesh->colors)
				free(mesh->colors);

			if (mesh->tangents)
				free(mesh->tangents);

			if (mesh->binormal)
				free(mesh->binormal);

			if (mesh->submeshes)
			{
				for (size_t s = 0; s < mesh->num_submesh; ++s)
				{
					struct FBX_SUBMESH* submesh = &mesh->submeshes[s];
					if (submesh->indices)
						free(submesh->indices);
				}

				free(mesh->submeshes);
			}
		}

		if (scene->model->meshes)
			free(scene->model->meshes);

		free(scene->model);
		scene->model = NULL;
	}

	free(scene);
}

/*
/// Skin Weight와 Bone Index 추출
void extract_skin_data(ufbx_mesh* mesh, size_t vertex_index, float out_weights[4], int out_indices[4])
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
*/