#include "pch.h"
#include "ModelAsset.h"
#include "fbx_loader.h"


ModelAsset::ModelAsset(UINT64 ID)
	: m_ID(ID)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
{
}

ModelAsset::~ModelAsset() noexcept
{
	m_Meshes.clear();
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

UINT32 ModelAsset::GetModelAttrib() const noexcept
{
	return m_ModelAttrib;
}

size_t ModelAsset::GetNumMesh() const noexcept
{
	return m_Meshes.size();
}

size_t ModelAsset::GetNumSubMesh(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex].NumSubMesh;
}

FLOAT3* ModelAsset::GetPositions(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex].pPositions;
}

size_t ModelAsset::GetNumPositions(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex].NumPositions;
}

FLOAT3* ModelAsset::GetNormals(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex].pNormals;
}

size_t ModelAsset::GetNumNormals(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex].NumNormals;
}

FLOAT2* ModelAsset::GetTexCoords(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex].pTexCoords;
}

size_t ModelAsset::GetNumTexCoords(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex].NumTexCoords;
}

FLOAT4* ModelAsset::GetColor(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex].pColors;
}

size_t ModelAsset::GetNumColor(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex].NumColors;
}

FLOAT3* ModelAsset::GetTangent(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex].pTangents;
}

size_t ModelAsset::GetNumTangent(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex].NumTangents;
}

FLOAT3* ModelAsset::GetBinormal(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;

	return m_Meshes[MeshIndex].pBinormals;
}

size_t ModelAsset::GetNumBinormal(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;

	return m_Meshes[MeshIndex].NumBinormals;
}

uint32* ModelAsset::GetIndices(int32 MeshIndex, int32 SubMeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_Meshes[MeshIndex].NumSubMesh)
		return nullptr;

	return m_Meshes[MeshIndex].pSubMeshes[SubMeshIndex].pIndices;
}

size_t ModelAsset::GetNumIndices(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;

	size_t TotalIndices = 0;
	for (size_t i = 0; i < m_Meshes[MeshIndex].NumSubMesh; i++)
		TotalIndices += m_Meshes[MeshIndex].pSubMeshes[i].NumIndices;

	return TotalIndices;
}

size_t ModelAsset::GetNumIndices(int32 MeshIndex, int32 SubMeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_Meshes[MeshIndex].NumSubMesh)
		return 0;

	return m_Meshes[MeshIndex].pSubMeshes[SubMeshIndex].NumIndices;
}


BOOL ModelAsset::LoadFromFBX(const FBX_SCENE* fbx_scene) noexcept
{
	if (!fbx_scene)		
		return FALSE;

	m_ModelAttrib = 0;
	m_Materials.clear();
	m_Meshes.clear();

	if (fbx_scene->num_materials)
	{
		m_ModelAttrib |= static_cast<uint32>(MODEL_ATTRIB::MATERIAL);
	}

	if (fbx_scene->model)
	{
		if (fbx_scene->model->num_meshes)
		{
			m_ModelAttrib |= static_cast<uint32>(MODEL_ATTRIB::MESH);
		}
	}

	return TRUE;
}