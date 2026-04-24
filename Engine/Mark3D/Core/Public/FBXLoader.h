#pragma once
/*

namespace mark
{
	class MARKENGINE_API FBX
	{
	public:
		struct FBX_MATERIAL
		{
			int32_t id;
			char diffuse[_MAX_PATH];
			char normal[_MAX_PATH];
			char specular[_MAX_PATH];
			char emissive[_MAX_PATH];
			float color[4];
		};

		struct FBX_SUBMESH
		{
			uint32_t* indices;
			int32_t num_indices;
			int32_t material_id;
		};

		struct FBX_MESH
		{
			char name[64];
			float* positions;
			float* normals;
			float* uvs;
			float* colors;
			float* tangents;
			float* binormals;
			size_t num_vertices;

			size_t num_submesh;
			FBX_SUBMESH* submeshes;
		};

		struct FBX_MODEL
		{
			size_t num_meshes;
			FBX_MESH* meshes;
		};

		struct FBX_SCENE
		{
			FBX_MATERIAL* materials;
			size_t num_materials;

			FBX_MODEL* model;
			BOOL create_temppool;
		};

		[[nodiscard]] static FBX_SCENE* fbx_load(void* data, size_t size);
		[[nodiscard]] static FBX_SCENE* fbx_load(HANDLE temp_alloc_handle, void* data, size_t size);
		static void fbx_free(FBX_SCENE* scene);
	};
}
*/
