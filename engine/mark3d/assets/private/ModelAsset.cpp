#include "pch.h"
#include "ModelAsset.h"


ModelAsset::ModelAsset(UINT64 ID)
	: m_ID(ID)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
{
}

ModelAsset::~ModelAsset() noexcept
{
	for (size_t i = 0; i < m_Meshes.size(); i++)
	{
		if (m_Meshes[i]->pPositions)
			CORE_SYS_FREE(m_Meshes[i]->pPositions);

		if (m_Meshes[i]->pNormals)
			CORE_SYS_FREE(m_Meshes[i]->pNormals);

		if (m_Meshes[i]->pTexCoords)
			CORE_SYS_FREE(m_Meshes[i]->pTexCoords);

		if (m_Meshes[i]->pColors)
			CORE_SYS_FREE(m_Meshes[i]->pColors);

		if (m_Meshes[i]->pTangents)
			CORE_SYS_FREE(m_Meshes[i]->pTangents);

		if (m_Meshes[i]->pBinormals)
			CORE_SYS_FREE(m_Meshes[i]->pBinormals);

		for (size_t j = 0; j < m_Meshes[i]->NumSubMesh; j++)
		{
			if (m_Meshes[i]->pSubMeshes[j].pIndices)
				CORE_SYS_FREE(m_Meshes[i]->pSubMeshes[j].pIndices);
		}

		if (m_Meshes[i]->pSubMeshes)
			CORE_SYS_FREE(m_Meshes[i]->pSubMeshes);
	}

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

size_t ModelAsset::GetNumMesh() const noexcept
{
	return m_Meshes.size();
}

size_t ModelAsset::GetNumSubMesh(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex]->NumSubMesh;
}

FLOAT3* ModelAsset::GetPositions(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex]->pPositions;
}

size_t ModelAsset::GetNumPositions(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex]->NumPositions;
}

FLOAT3* ModelAsset::GetNormals(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex]->pNormals;
}

size_t ModelAsset::GetNumNormals(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex]->NumNormals;
}

FLOAT2* ModelAsset::GetTexCoords(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex]->pTexCoords;
}

size_t ModelAsset::GetNumTexCoords(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex]->NumTexCoords;
}

FLOAT4* ModelAsset::GetColor(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex]->pColors;
}

size_t ModelAsset::GetNumColor(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex]->NumColors;
}

FLOAT3* ModelAsset::GetTangent(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;
	return m_Meshes[MeshIndex]->pTangents;
}

size_t ModelAsset::GetNumTangent(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;
	return m_Meshes[MeshIndex]->NumTangents;
}

FLOAT3* ModelAsset::GetBinormal(int32 MeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;

	return m_Meshes[MeshIndex]->pBinormals;
}

size_t ModelAsset::GetNumBinormal(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;

	return m_Meshes[MeshIndex]->NumBinormals;
}

uint32* ModelAsset::GetIndices(int32 MeshIndex, int32 SubMeshIndex) noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return nullptr;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_Meshes[MeshIndex]->NumSubMesh)
		return nullptr;

	return m_Meshes[MeshIndex]->pSubMeshes[SubMeshIndex].pIndices;
}

size_t ModelAsset::GetNumIndices(int32 MeshIndex, int32 SubMeshIndex) const noexcept
{
	if (MeshIndex < 0 || static_cast<size_t>(MeshIndex) >= m_Meshes.size())
		return 0;

	if (SubMeshIndex < 0 || static_cast<size_t>(SubMeshIndex) >= m_Meshes[MeshIndex]->NumSubMesh)
		return 0;

	return m_Meshes[MeshIndex]->pSubMeshes[SubMeshIndex].NumIndices;
}

void ModelAsset::AddMesh(const char* szName, uint32 VertexFormat, size_t NumVertices, size_t NumSubMeshes) noexcept 
{
	Mesh* pNewMesh = CORE_NEW(Mesh);

	fstrlcpy(pNewMesh->szName, szName, sizeof(pNewMesh->szName) - 1);

	if (NumSubMeshes > 0)
	{
		pNewMesh->pSubMeshes = static_cast<Mesh::SubMesh*>(CORE_SYS_ALLOC(sizeof(Mesh::SubMesh) * NumSubMeshes));
		for (size_t i = 0; i < NumSubMeshes; i++)
		{
			pNewMesh->pSubMeshes[i].pIndices = nullptr;
			pNewMesh->pSubMeshes[i].NumIndices = 0;
		}
	}
	else
	{
		pNewMesh->pSubMeshes = nullptr;
	}

	pNewMesh->NumSubMesh = NumSubMeshes;

	m_Meshes.push_back(pNewMesh);
}
