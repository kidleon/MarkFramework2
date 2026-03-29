#include "pch.h"
#include "ModelAsset.h"
#include "fbx_loader.h"
#include "os_file.h"


ModelAsset::ModelAsset(UINT64 ID, const char* szRelativePath)
	: m_ID(ID)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
{
	if (szRelativePath && fstrlen(szRelativePath))
		fstrlcpy(m_szRelativePath, szRelativePath, MAX_FILE_LENGTH);
}

ModelAsset::~ModelAsset() noexcept
{
	if (m_pMaterials)
	{
		CORE_SYS_FREE(m_pMaterials);
		m_pMaterials = nullptr;
	}

	if (m_pMeshes)
	{
		for (size_t i = 0; i < m_NumMeshes; i++)
		{
			Mesh& mesh = m_pMeshes[i];

			if (mesh.pPositions)
				CORE_SYS_FREE(mesh.pPositions);

			if (mesh.pNormals)
				CORE_SYS_FREE(mesh.pNormals);

			if (mesh.pTexCoords)
				CORE_SYS_FREE(mesh.pTexCoords);

			if (mesh.pColors)
				CORE_SYS_FREE(mesh.pColors);

			if (mesh.pTangents)
				CORE_SYS_FREE(mesh.pTangents);

			if (mesh.pBinormals)
				CORE_SYS_FREE(mesh.pBinormals);

			if (mesh.pSubMeshes)
			{
				for (size_t j = 0; j < mesh.NumSubMesh; j++)
				{
					if (mesh.pSubMeshes[j].pIndices)
						CORE_SYS_FREE(mesh.pSubMeshes[j].pIndices);
				}
				CORE_SYS_FREE(mesh.pSubMeshes);
			}
		}

		CORE_SYS_FREE(m_pMeshes);
		m_pMeshes = nullptr;
	}
}

long ModelAsset::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long ModelAsset::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, ModelAsset);
	}
	return NewRefCnt;
}

long ModelAsset::RefCnt()
{
	return m_RefCnt;
}

UINT64 ModelAsset::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE ModelAsset::GetAssetType() const noexcept
{
	return ASSET_TYPE::MODEL_ASSET;
}

LOAD_STAT ModelAsset::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

BOOL ModelAsset::GetRelativePath(char* szBuffer, size_t BufferLen, BOOL IgnoreFileName) const noexcept
{
	if (!szBuffer || BufferLen == 0)
		return FALSE;

	if (IgnoreFileName)
	{
		char FilePath[MAX_FILE_LENGTH] = { 0 };
		if (!get_path(m_szRelativePath, FilePath, MAX_FILE_LENGTH))
			return FALSE;

		fstrlcpy(szBuffer, FilePath, BufferLen);
	}
	else
	{
		fstrlcpy(szBuffer, m_szRelativePath, BufferLen);
	}

	return TRUE;
}

UINT32 ModelAsset::GetModelAttrib() const noexcept
{
	return m_ModelAttrib;
}

size_t ModelAsset::GetNumMesh() const noexcept
{
	return m_NumMeshes;
}

UINT32 ModelAsset::GetVertexFormat(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return 0;

	UINT32 VertexFormat = 0;
	const Mesh& mesh = m_pMeshes[MeshIndex];

	if (mesh.pPositions)
		VertexFormat |= (uint32)VERTEX_FORMAT::POSITION;

	if (mesh.pNormals)
		VertexFormat |= (uint32)VERTEX_FORMAT::NORMAL;

	if (mesh.pTexCoords)
		VertexFormat |= (uint32)VERTEX_FORMAT::TEXCOORD;

	if (mesh.pColors)
		VertexFormat |= (uint32)VERTEX_FORMAT::COLOR;

	if (mesh.pTangents)
		VertexFormat |= (uint32)VERTEX_FORMAT::TANGENT;

	if (mesh.pBinormals)
		VertexFormat |= (uint32)VERTEX_FORMAT::BINORMAL;

	return VertexFormat;
}

UINT32 ModelAsset::GetVertexFormat() const noexcept
{
	UINT32 VertexFormat = 0;

	for (size_t i = 0; i < m_NumMeshes; i++)
	{
		const Mesh& mesh = m_pMeshes[i];
		if (mesh.pPositions)
			VertexFormat |= (uint32)VERTEX_FORMAT::POSITION;
		if (mesh.pNormals)
			VertexFormat |= (uint32)VERTEX_FORMAT::NORMAL;
		if (mesh.pTexCoords)
			VertexFormat |= (uint32)VERTEX_FORMAT::TEXCOORD;
		if (mesh.pColors)
			VertexFormat |= (uint32)VERTEX_FORMAT::COLOR;
		if (mesh.pTangents)
			VertexFormat |= (uint32)VERTEX_FORMAT::TANGENT;
		if (mesh.pBinormals)
			VertexFormat |= (uint32)VERTEX_FORMAT::BINORMAL;
	}

	return VertexFormat;
}

UINT32 ModelAsset::GetTotalVertexCount() const noexcept
{
	UINT32 TotalVertexCount = 0;

	for (size_t i = 0; i < m_NumMeshes; i++)
		TotalVertexCount += (UINT32)m_pMeshes[i].NumVertices;

	return TotalVertexCount;
}

UINT32 ModelAsset::GetTotalIndexCount() const noexcept
{
	UINT32 TotalIndexCount = 0;

	for (size_t i = 0; i < m_NumMeshes; i++)
	{
		for (size_t j = 0; j < m_pMeshes[i].NumSubMesh; j++)
			TotalIndexCount += (UINT32)m_pMeshes[i].pSubMeshes[j].NumIndices;
	}

	return TotalIndexCount;

}

const char* ModelAsset::GetMeshName(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;
	return m_pMeshes[MeshIndex].szName;
}

size_t ModelAsset::GetNumSubMesh(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return 0;
	return m_pMeshes[MeshIndex].NumSubMesh;
}

size_t ModelAsset::GetNumVertices(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return 0;
	return m_pMeshes[MeshIndex].NumVertices;
}

FLOAT3* ModelAsset::GetPositions(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;
	return m_pMeshes[MeshIndex].pPositions;
}

FLOAT3* ModelAsset::GetNormals(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;
	return m_pMeshes[MeshIndex].pNormals;
}

FLOAT2* ModelAsset::GetTexCoords(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;
	return m_pMeshes[MeshIndex].pTexCoords;
}

FLOAT4* ModelAsset::GetColor(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;
	return m_pMeshes[MeshIndex].pColors;
}

FLOAT3* ModelAsset::GetTangent(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;
	return m_pMeshes[MeshIndex].pTangents;
}

FLOAT3* ModelAsset::GetBinormal(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;

	return m_pMeshes[MeshIndex].pBinormals;
}

UINT32* ModelAsset::GetIndices(int32 MeshIndex, int32 SubMeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return nullptr;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_pMeshes[MeshIndex].NumSubMesh)
		return nullptr;

	return m_pMeshes[MeshIndex].pSubMeshes[SubMeshIndex].pIndices;
}

size_t ModelAsset::GetNumIndices(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return 0;

	size_t TotalIndices = 0;
	for (size_t i = 0; i < m_pMeshes[MeshIndex].NumSubMesh; i++)
		TotalIndices += m_pMeshes[MeshIndex].pSubMeshes[i].NumIndices;

	return TotalIndices;
}

size_t ModelAsset::GetNumIndices(int32 MeshIndex, int32 SubMeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return 0;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_pMeshes[MeshIndex].NumSubMesh)
		return 0;

	return m_pMeshes[MeshIndex].pSubMeshes[SubMeshIndex].NumIndices;
}

size_t ModelAsset::GetNumMaterials() const noexcept
{
	return m_NumMaterials;
}

int32 ModelAsset::GetMaterialIndex(int32 MeshIndex, int32 SubMeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_NumMeshes)
		return -1;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_pMeshes[MeshIndex].NumSubMesh)
		return -1;

	return m_pMeshes[MeshIndex].pSubMeshes[SubMeshIndex].MaterialIndex;
}

BOOL ModelAsset::HasDiffuseTexture(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return FALSE;
	return m_pMaterials[MaterialID].Diffuse[0] != '\0';
}

BOOL ModelAsset::HasNormalTexture(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return FALSE;
	return m_pMaterials[MaterialID].Normal[0] != '\0';
}

BOOL ModelAsset::HasSpecularTexture(int32 MaterialID) const noexcept
{
		if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return FALSE;
		return m_pMaterials[MaterialID].Specular[0] != '\0';
}

BOOL ModelAsset::HasEmissiveTexture(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return FALSE;
	return m_pMaterials[MaterialID].Emissive[0] != '\0';
}

BOOL ModelAsset::HasColor(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return FALSE;
	const FLOAT4& Color = m_pMaterials[MaterialID].Color;
	return !(Color.x == 0 && Color.y == 0 && Color.z == 0 && Color.w == 0);
}

const char* ModelAsset::GetMaterialDiffuse(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return nullptr;
	return m_pMaterials[MaterialID].Diffuse;
}

const char* ModelAsset::GetMaterialNormal(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return nullptr;
	return m_pMaterials[MaterialID].Normal;
}

const char* ModelAsset::GetMaterialSpecular(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return nullptr;
	return m_pMaterials[MaterialID].Specular;
}

const char* ModelAsset::GetMaterialEmissive(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return nullptr;
	return m_pMaterials[MaterialID].Emissive;
}

FLOAT4 ModelAsset::GetMaterialColor(int32 MaterialID) const noexcept
{
	if (MaterialID < 0 || static_cast<size_t>(MaterialID) >= m_NumMaterials)
		return FLOAT4{0, 0, 0, 0};
	return m_pMaterials[MaterialID].Color;
}

int32 ModelAsset::FindMaterialIndex(int32 MatID) const noexcept
{
	for(size_t i = 0; i < m_NumMaterials; i++)
	{
		if (m_pMaterials[i].ID == MatID)
			return static_cast<int32>(i);
	}

	return -1;
}

BOOL ModelAsset::LoadFromFBX(const FBX_SCENE* fbx_scene) noexcept
{
	if (!fbx_scene)		
		return FALSE;

	m_ModelAttrib = 0;

	if (fbx_scene->num_materials)
	{
		m_ModelAttrib |= static_cast<uint32>(MODEL_ATTRIB::MATERIAL);

		m_NumMaterials = fbx_scene->num_materials;
		m_pMaterials = (Material*)CORE_SYS_ALLOC(sizeof(Material) * fbx_scene->num_materials);
		memset(m_pMaterials, 0, sizeof(Material) * fbx_scene->num_materials);

		for (size_t i = 0; i < fbx_scene->num_materials; i++)
		{
			const FBX_MATERIAL& fbx_material = fbx_scene->materials[i];

			Material& material = m_pMaterials[i];
			material.ID = fbx_material.id;
			fstrlcpy(material.Diffuse, fbx_material.diffuse, sizeof(material.Diffuse));
			fstrlcpy(material.Normal, fbx_material.normal, sizeof(material.Normal));
			fstrlcpy(material.Specular, fbx_material.specular, sizeof(material.Specular));
			fstrlcpy(material.Emissive, fbx_material.emissive, sizeof(material.Emissive));
			memcpy(material.Color.v, fbx_material.color, sizeof(float) * 4);
		}
	}

	if (fbx_scene->model)
	{
		if (fbx_scene->model->meshes && 0 < fbx_scene->model->num_meshes)
		{
			m_ModelAttrib |= static_cast<uint32>(MODEL_ATTRIB::MESH);

			m_NumMeshes = fbx_scene->model->num_meshes;
			m_pMeshes = (Mesh*)CORE_SYS_ALLOC(sizeof(Mesh) * fbx_scene->model->num_meshes);
			memset(m_pMeshes, 0, sizeof(Mesh) * fbx_scene->model->num_meshes);

			for (size_t i = 0; i < fbx_scene->model->num_meshes; i++)
			{
				const FBX_MESH& fbx_mesh = fbx_scene->model->meshes[i];

				Mesh& mesh = m_pMeshes[i];
				fstrlcpy(mesh.szName, fbx_mesh.name, sizeof(mesh.szName));

				mesh.NumVertices = fbx_mesh.num_vertices;

				if (fbx_mesh.positions)
				{
					mesh.pPositions = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh.NumVertices);
					memcpy(mesh.pPositions, fbx_mesh.positions, sizeof(FLOAT3) * mesh.NumVertices);
				}

				if (fbx_mesh.normals)
				{
					mesh.pNormals = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh.NumVertices);
					memcpy(mesh.pNormals, fbx_mesh.normals, sizeof(FLOAT3) * mesh.NumVertices);
				}

				if (fbx_mesh.colors)
				{
					mesh.pColors = (FLOAT4*)CORE_SYS_ALLOC(sizeof(FLOAT4) * mesh.NumVertices);
					memcpy(mesh.pColors, fbx_mesh.colors, sizeof(FLOAT4) * mesh.NumVertices);
				}

				if (fbx_mesh.uvs)
				{
					mesh.pTexCoords = (FLOAT2*)CORE_SYS_ALLOC(sizeof(FLOAT2) * mesh.NumVertices);
					memcpy(mesh.pTexCoords, fbx_mesh.uvs, sizeof(FLOAT2) * mesh.NumVertices);
				}

				if (fbx_mesh.tangents)
				{
					mesh.pTangents = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh.NumVertices);
					memcpy(mesh.pTangents, fbx_mesh.tangents, sizeof(FLOAT3) * mesh.NumVertices);
				}

				if (fbx_mesh.binormals)
				{
					mesh.pBinormals = (FLOAT3*)CORE_SYS_ALLOC(sizeof(FLOAT3) * mesh.NumVertices);
					memcpy(mesh.pBinormals, fbx_mesh.binormals, sizeof(FLOAT3) * mesh.NumVertices);
				}

				if (fbx_mesh.num_submesh)
				{
					mesh.NumSubMesh = fbx_mesh.num_submesh;
					mesh.pSubMeshes = (Mesh::SubMesh*)CORE_SYS_ALLOC(sizeof(Mesh::SubMesh) * fbx_mesh.num_submesh);
					memset(mesh.pSubMeshes, 0, sizeof(Mesh::SubMesh) * fbx_mesh.num_submesh);

					for (size_t j = 0; j < fbx_mesh.num_submesh; j++)
					{
						const FBX_SUBMESH& fbx_submesh = fbx_mesh.submeshes[j];
						Mesh::SubMesh& submesh = mesh.pSubMeshes[j];
						submesh.MaterialIndex = FindMaterialIndex(fbx_submesh.material_id);
						submesh.NumIndices = fbx_submesh.num_indices;
						if (fbx_submesh.indices)
						{
							submesh.pIndices = (UINT32*)CORE_SYS_ALLOC(sizeof(UINT32) * submesh.NumIndices);
							memcpy(submesh.pIndices, fbx_submesh.indices, sizeof(UINT32) * submesh.NumIndices);
						}
					}
				}
			}
		}
	}

	return TRUE;
}