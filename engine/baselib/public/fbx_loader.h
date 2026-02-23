#ifndef __FBX_LOADER_H__
#define __FBX_LOADER_H__


#define MAX_TEXTURE_FILENAME 256

struct FBX_MATERIAL
{
	int32 id;
	char diffuse[MAX_TEXTURE_FILENAME];
	char normal[MAX_TEXTURE_FILENAME];
	char specular[MAX_TEXTURE_FILENAME];
	char emissive[MAX_TEXTURE_FILENAME];
	float color[4];
};

struct FBX_MESH
{
	struct FBX_SUBMESH
	{
		uint32* indices;
		int32 num_indices;
		int32 material_id;
	};

	char name[64];
	float* positions;
	float* normals;
	float* uvs;
	float* colors;
	float* tangents;
	float* binormal;

	size_t num_submesh;
	struct FBX_SUBMESH* submeshes;

};

struct FBX_MODEL
{
	size_t num_meshes;
	struct FBX_MESH* meshes;
};

struct FBX_SCENE
{
	struct FBX_MATERIAL* materials;
	size_t num_materials;

	struct FBX_MODEL* model;
};

MARK_BASELIB_C_API struct FBX_SCENE* fbx_load(void* data, size_t size);

MARK_BASELIB_C_API void fbx_unload(struct FBX_SCENE* scene);


#endif // __FBX_LOADER_H__
