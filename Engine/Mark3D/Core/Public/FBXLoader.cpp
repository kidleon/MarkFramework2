#include "pch.h"
#include "FBXLoader.h"
#include "ufbx/ufbx.h"
#include "FileSystem.h"


namespace mark
{
	inline static void* ufbx_alloc(void* user, size_t size)
	{
		if (!user)
		{
			return CORE_SYS_ALLOC(size);
		}
		else
		{
			HANDLE temp_alloc_handle = (HANDLE)user;
			return temppool_alloc(temp_alloc_handle, size, sizeof(uintptr_t));
		}
	}

	static void change_psd_to_tag(const char* filename, char* out_filename, size_t out_size)
	{
		char extension[16];
		file_system::get_file_extension(filename, extension, sizeof(extension));
		if (!strcmp(extension, ".psd"))
		{
			file_system::change_extension(
				filename,
				".tga",
				out_filename,
				out_size
			);

			return;
		}

		safe_strcpy(out_filename, out_size, filename);
	}

	static void fbx_load_memory(HANDLE temp_pool_handle, ufbx_scene* scene, FBX::FBX_SCENE* fbx_scene);

	FBX::FBX_SCENE* FBX::fbx_load(void* data, size_t size)
	{
		if (!data || size == 0)
			return nullptr;

		ufbx_load_opts load_opts = {};

		load_opts.obj_axes = ufbx_axes_left_handed_y_up;
		load_opts.obj_unit_meters = 1.0f;
		load_opts.normalize_tangents = true;
		load_opts.normalize_normals = true;

		ufbx_error error = {};

		ufbx_scene* scene = ufbx_load_memory(data, size, &load_opts, &error);
		if (!scene)
			return NULL;

		struct FBX_SCENE* fbx_scene = (struct FBX_SCENE*)CORE_SYS_ALLOC(sizeof(struct FBX_SCENE));

		fbx_load_memory(nullptr, scene, fbx_scene);

		ufbx_free_scene(scene);

		fbx_scene->create_temppool = false;

		return fbx_scene;
	}

	FBX::FBX_SCENE* FBX::fbx_load(HANDLE temp_alloc_handle, void* data, size_t size)
	{
		if (!temp_alloc_handle || !data || size == 0)
			return nullptr;

		ufbx_load_opts load_opts = {};

		load_opts.obj_axes = ufbx_axes_left_handed_y_up;
		load_opts.obj_unit_meters = 1.0f;
		load_opts.normalize_tangents = true;
		load_opts.normalize_normals = true;

		ufbx_error error = {};

		ufbx_scene* scene = ufbx_load_memory(data, size, &load_opts, &error);
		if (!scene)
			return NULL;

		FBX::FBX_SCENE* fbx_scene = (FBX::FBX_SCENE*)temppool_alloc(
			temp_alloc_handle,
			sizeof(FBX::FBX_SCENE),
			sizeof(uintptr_t)
		);

		fbx_load_memory(temp_alloc_handle, scene, fbx_scene);

		ufbx_free_scene(scene);

		fbx_scene->create_temppool = true;

		return fbx_scene;
	}

	void FBX::fbx_free(FBX_SCENE* scene)
	{
		if (!scene) return;

		if (!scene->create_temppool)
			return;

		if (scene->materials)
		{
			CORE_SYS_FREE(scene->materials);
			scene->materials = nullptr;
			scene->num_materials = 0;
		}

		if (scene->model)
		{
			if (scene->model->meshes)
			{
				for (size_t i = 0; i < scene->model->num_meshes; ++i)
				{
					FBX::FBX_MESH* mesh = &scene->model->meshes[i];
					if (mesh->positions)
					{
						CORE_SYS_FREE(mesh->positions);
						mesh->positions = nullptr;
					}

					if (mesh->normals)
					{
						CORE_SYS_FREE(mesh->normals);
						mesh->normals = nullptr;
					}

					if (mesh->uvs)
					{
						CORE_SYS_FREE(mesh->uvs);
						mesh->uvs = nullptr;
					}

					if (mesh->colors)
					{
						CORE_SYS_FREE(mesh->colors);
						mesh->colors = nullptr;
					}

					if (mesh->tangents)
					{
						CORE_SYS_FREE(mesh->tangents);
						mesh->tangents = nullptr;
					}

					if (mesh->binormals)
					{
						CORE_SYS_FREE(mesh->binormals);
						mesh->binormals = nullptr;
					}

					if (mesh->submeshes)
					{
						for (size_t s = 0; s < mesh->num_submesh; ++s)
						{
							if (mesh->submeshes[s].indices)
							{
								CORE_SYS_FREE(mesh->submeshes[s].indices);
								mesh->submeshes[s].indices = nullptr;
							}
						}
						CORE_SYS_FREE(mesh->submeshes);
						mesh->submeshes = nullptr;
					}
				}

				CORE_SYS_FREE(scene->model->meshes);
				scene->model->meshes = nullptr;
			}

			CORE_SYS_FREE(scene->model);
			scene->model = nullptr;
		}
	}

	static void fbx_load_memory(HANDLE temp_pool_handle, ufbx_scene* scene, FBX::FBX_SCENE* fbx_scene)
	{
		if (0 < scene->materials.count)
		{
			size_t mat_count = scene->materials.count;
			fbx_scene->num_materials = mat_count;
			fbx_scene->materials = (FBX::FBX_MATERIAL*)ufbx_alloc(
				temp_pool_handle,
				sizeof(FBX::FBX_MATERIAL) * fbx_scene->num_materials
			);

			if (!fbx_scene->materials)
			{
				fbx_scene->num_materials = 0;
				return;
			}

			for (size_t i = 0; i < scene->materials.count; ++i)
			{
				ufbx_material* material = scene->materials.data[i];
				struct FBX::FBX_MATERIAL* fbx_material = &fbx_scene->materials[i];

				fbx_material->id = (int32_t)material->element_id;
				fbx_material->color[0] = (float)material->fbx.diffuse_color.value_vec4.x;
				fbx_material->color[1] = (float)material->fbx.diffuse_color.value_vec4.y;
				fbx_material->color[2] = (float)material->fbx.diffuse_color.value_vec4.z;
				fbx_material->color[3] = (float)material->fbx.diffuse_color.value_vec4.w;

				if (material->fbx.diffuse_color.texture_enabled && material->fbx.diffuse_color.texture)
				{
					file_system::get_filename(
						material->fbx.diffuse_color.texture->filename.data,
						fbx_material->diffuse,
						_MAX_PATH
					);

					change_psd_to_tag(
						fbx_material->diffuse,
						fbx_material->diffuse,
						_MAX_PATH
					);
				}
				else
					fbx_material->diffuse[0] = '\0';

				if (material->fbx.normal_map.texture_enabled && material->fbx.normal_map.texture)
				{
					file_system::get_filename(
						material->fbx.normal_map.texture->filename.data,
						fbx_material->normal,
						_MAX_PATH
					);

					change_psd_to_tag(
						fbx_material->normal,
						fbx_material->normal,
						_MAX_PATH
					);
				}
				else
					fbx_material->normal[0] = '\0';

				if (material->fbx.specular_color.texture_enabled && material->fbx.specular_color.texture)
				{
					file_system::get_filename(
						material->fbx.specular_color.texture->filename.data,
						fbx_material->specular,
						_MAX_PATH
					);

					change_psd_to_tag(
						fbx_material->specular,
						fbx_material->specular,
						_MAX_PATH
					);
				}
				else
					fbx_material->specular[0] = '\0';

				if (material->fbx.emission_color.texture_enabled && material->fbx.emission_color.texture)
				{
					file_system::get_filename(
						material->fbx.emission_color.texture->filename.data,
						fbx_material->emissive,
						_MAX_PATH
					);

					change_psd_to_tag(
						fbx_material->emissive,
						fbx_material->emissive,
						_MAX_PATH
					);
				}
				else
					fbx_material->emissive[0] = '\0';
			}
		}

		if (0 < scene->meshes.count)
		{
			fbx_scene->model = (struct FBX::FBX_MODEL*)ufbx_alloc(
				temp_pool_handle,
				sizeof(FBX::FBX_MODEL)
			);
			memset(fbx_scene->model, 0, sizeof(FBX::FBX_MODEL));

			fbx_scene->model->meshes = (struct FBX::FBX_MESH*)ufbx_alloc(
				temp_pool_handle,
				sizeof(FBX::FBX_MESH) * scene->meshes.count
			);
			fbx_scene->model->num_meshes = scene->meshes.count;
			memset(fbx_scene->model->meshes, 0, sizeof(FBX::FBX_MESH)* scene->meshes.count);

			FBX::FBX_MESH* meshes = fbx_scene->model->meshes;

			for (size_t i = 0; i < scene->meshes.count; ++i)
			{
				ufbx_mesh* mesh = scene->meshes.data[i];

				if (0 < mesh->name.length)
					safe_strcpy(meshes[i].name, 64, mesh->name.data);

				if (!mesh->num_vertices)
					continue;

				meshes[i].num_vertices = mesh->num_vertices;

				if (mesh->vertex_position.exists)
				{
					meshes[i].positions = (float*)ufbx_alloc(
						temp_pool_handle,
						sizeof(float) * 3 * mesh->num_vertices
					);

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
					meshes[i].normals = (float*)ufbx_alloc(
						temp_pool_handle,
						sizeof(float) * 3 * mesh->num_vertices
					);

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
					meshes[i].colors = (float*)ufbx_alloc(
						temp_pool_handle,
						sizeof(float) * 4 * mesh->num_vertices
					);

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
					meshes[i].uvs = (float*)ufbx_alloc(
						temp_pool_handle,
						sizeof(float) * 2 * mesh->num_vertices
					);

					for (size_t v = 0; v < mesh->num_vertices; ++v)
					{
						ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, v);
						meshes[i].uvs[v * 2 + 0] = (float)uv.x;
						meshes[i].uvs[v * 2 + 1] = (float)uv.y;
					}
				}

				if (mesh->vertex_tangent.exists)
				{
					meshes[i].tangents = (float*)ufbx_alloc(
						temp_pool_handle,
						sizeof(float) * 3 * mesh->num_vertices
					);

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
					meshes[i].binormals = (float*)ufbx_alloc(
						temp_pool_handle,
						sizeof(float) * 3 * mesh->num_vertices
					);

					for (size_t v = 0; v < mesh->num_vertices; ++v)
					{
						ufbx_vec3 binormals = ufbx_get_vertex_vec3(&mesh->vertex_bitangent, v);
						meshes[i].binormals[v * 3 + 0] = (float)binormals.x;
						meshes[i].binormals[v * 3 + 1] = (float)binormals.y;
						meshes[i].binormals[v * 3 + 2] = (float)binormals.z;
					}
				}

				if (0 < mesh->material_parts.count)
				{
					FBX::FBX_SUBMESH* submeshes = (FBX::FBX_SUBMESH*)ufbx_alloc(
						temp_pool_handle,
						sizeof(FBX::FBX_SUBMESH) * mesh->material_parts.count
					);

					memset(submeshes, 0, sizeof(FBX::FBX_SUBMESH) * mesh->material_parts.count);

					for (size_t m = 0; m < mesh->material_parts.count; ++m)
					{
						ufbx_mesh_part* part = &mesh->material_parts.data[m];

						ufbx_material* material = mesh->materials.data[part->index];

						submeshes[m].material_id = (int32_t)material->element_id;
						submeshes[m].num_indices = (int32_t)part->num_triangles * 3;
						submeshes[m].indices = (uint32_t*)ufbx_alloc(
							temp_pool_handle,
							sizeof(uint32_t) * part->num_triangles * 3
						);

						int32_t num_indices = 0;

						for (size_t f = 0; f < part->face_indices.count; ++f)
						{
							uint32_t face_index = part->face_indices.data[f];

							ufbx_face face = mesh->faces.data[face_index];

							if (!face.num_indices)
								continue;

							size_t num_tris = face.num_indices - 2;

							for (size_t tri = 0; tri < num_tris; ++tri)
							{
								if (num_indices + 3 > submeshes[m].num_indices)
									break;

								uint32_t tri_indices[3];
								ufbx_triangulate_face(tri_indices, 3, mesh, face);

								uint32_t idx0 = tri_indices[0];
								uint32_t idx1 = tri_indices[1];
								uint32_t idx2 = tri_indices[2];

								submeshes[m].indices[num_indices++] = (uint32_t)idx0;
								submeshes[m].indices[num_indices++] = (uint32_t)idx1;
								submeshes[m].indices[num_indices++] = (uint32_t)idx2;
							}
						}
					}

					meshes[i].num_submesh = mesh->material_parts.count;
					meshes[i].submeshes = submeshes;
				}
			}
		}
	}


}
