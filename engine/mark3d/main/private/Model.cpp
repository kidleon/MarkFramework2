#include "pch.h"
#include "Model.h"


Model::Model(UINT64 ID, IPrimitiveBuffer* pPrimitiveBuffer)
	: m_pPrimitiveBuffer(pPrimitiveBuffer)
{
}

Model::~Model() noexcept
{
	ClearMesh();
	CHECK_RELEASE(m_pPrimitiveBuffer);
}

long Model::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Model::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (!NewRefCnt)
	{
		CORE_POOL_DELETE(this, Model);
	}

	return NewRefCnt;
}

long Model::RefCnt()
{
	return m_RefCnt;
}

UINT64 Model::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE Model::GetAssetType() const noexcept
{
	return ASSET_TYPE::MODEL;
}

LOAD_STAT Model::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

int32 Model::AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 VertexFormat, size_t IndexStride, size_t NumVertex, size_t NumIndex) noexcept
{
	int32 PrimitiveIndex = m_pPrimitiveBuffer->AddPrimitive(PrimitiveType, (uint32)NumVertex, VertexFormat, (uint32)NumIndex, (uint32)IndexStride);
	if (-1 == PrimitiveIndex)
	{
		SYS_LOG_E("Model::AddMesh - Failed to add primitive to primitive buffer.");
		return -1;
	}

	MeshData NewMeshData = {};
	NewMeshData.Name = Name;
	NewMeshData.PrimitiveIndex = PrimitiveIndex;
	NewMeshData.Active = TRUE;
	NewMeshData.VertexFormat = VertexFormat;
	NewMeshData.VertexStride = (uint32)CalulateVertexStride(VertexFormat);
	NewMeshData.IndexStride = (uint32)IndexStride;
	NewMeshData.MaxVertex = (uint32)NumVertex;
	NewMeshData.MaxIndex = (uint32)NumIndex;
	NewMeshData.NumVertex = 0;
	NewMeshData.NumIndex = 0;
	m_lstMeshData.push_back(NewMeshData);

	return (int32)(m_lstMeshData.size() - 1);
}

void Model::ClearMesh() noexcept
{
	for(size_t i = 0; i < m_lstMeshData.size(); i++)
	{
		MeshData& meshData = m_lstMeshData[i];
		CHECK_RELEASE(meshData.pSurfaceMaterial);
	}
	m_lstMeshData.clear();
}

void Model::UpdateVertex(int32 MeshIndex, void* pVertexData, size_t VertexSize) noexcept
{
	m_lstMeshData[MeshIndex].NumVertex = (uint32)(VertexSize / m_lstMeshData[MeshIndex].VertexFormat);
	m_pPrimitiveBuffer->UpdateVertex(MeshIndex, pVertexData, VertexSize);
}

void Model::UpdateVertex(NameHash Name, void* pVertexData, size_t VertexSize) noexcept
{
	const int32 MeshIndex = FindMeshIndexByName(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::UpdateVertex - Cannot find mesh by name.");
		return;
	}

	size_t VertexStride = CalulateVertexStride(m_lstMeshData[MeshIndex].VertexFormat);
	m_lstMeshData[MeshIndex].NumVertex = (uint32)(VertexSize / VertexStride);
}

void Model::UpdateIndex(int32 MeshIndex, void* pIndexData, size_t IndexSize) noexcept
{
	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Invalid mesh index.");
		return;
	}

	const uint32 NumIndex = (uint32)(IndexSize / m_lstMeshData[MeshIndex].IndexStride);
	if (NumIndex > m_lstMeshData[MeshIndex].MaxIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Index size exceeds the maximum index count of the mesh.");
		return;
	}

	m_lstMeshData[MeshIndex].NumIndex = NumIndex;
	m_pPrimitiveBuffer->UpdateIndex(MeshIndex, pIndexData, IndexSize);
}

void Model::UpdateIndex(NameHash Name, void* pIndexData, size_t IndexSize) noexcept
{
	const int32 MeshIndex = FindMeshIndexByName(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Cannot find mesh by name.");
		return;
	}

	const uint32 NumIndex = (uint32)(IndexSize / m_lstMeshData[MeshIndex].IndexStride);
	if (NumIndex > m_lstMeshData[MeshIndex].MaxIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Index size exceeds the maximum index count of the mesh.");
		return;
	}

	m_lstMeshData[MeshIndex].NumIndex = NumIndex;
	m_pPrimitiveBuffer->UpdateIndex(MeshIndex, pIndexData, IndexSize);
}

void Model::SetMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept
{
	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Invalid mesh index.");
		return;
	}

	CHECK_RELEASE(m_lstMeshData[MeshIndex].pSurfaceMaterial);

	pSurfaceMaterial->AddRef();
	m_lstMeshData[MeshIndex].pSurfaceMaterial = pSurfaceMaterial;
}

void Model::SetMaterial(NameHash Name, ISurfaceMaterial* pSurfaceMaterial) noexcept
{
	const int32 MeshIndex = FindMeshIndexByName(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetMaterial - Cannot find mesh by name.");
		return;
	}

	CHECK_RELEASE(m_lstMeshData[MeshIndex].pSurfaceMaterial);
	pSurfaceMaterial->AddRef();
	m_lstMeshData[MeshIndex].pSurfaceMaterial = pSurfaceMaterial;
}

int32 Model::FindMeshIndexByName(NameHash Name) const noexcept
{
	for (size_t i = 0; i < m_lstMeshData.size(); i++)
	{
		if (m_lstMeshData[i].Name == Name)
			return (int32)i;
	}

	return -1;
}

ISurfaceMaterial* Model::GetMaterial(int32 MeshIndex) noexcept
{
	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Invalid mesh index.");
		return nullptr;
	}

	if (m_lstMeshData[MeshIndex].pSurfaceMaterial)
		m_lstMeshData[MeshIndex].pSurfaceMaterial->AddRef();

	return m_lstMeshData[MeshIndex].pSurfaceMaterial;
}

ISurfaceMaterial* Model::GetMaterial(NameHash Name) noexcept
{
	const int32 MeshIndex = FindMeshIndexByName(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetMaterial - Cannot find mesh by name.");
		return nullptr;
	}

	if (m_lstMeshData[MeshIndex].pSurfaceMaterial)
		m_lstMeshData[MeshIndex].pSurfaceMaterial->AddRef();

	return m_lstMeshData[MeshIndex].pSurfaceMaterial;
}

void Model::SetActiveMesh(int32 MeshIndex, BOOL Active) noexcept
{
	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Invalid mesh index.");
		return;
	}

	m_lstMeshData[MeshIndex].Active = Active;
}

void Model::SetActiveMesh(NameHash Name, BOOL Active) noexcept
{
	const int32 MeshIndex = FindMeshIndexByName(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetMaterial - Cannot find mesh by name.");
		return;
	}

	m_lstMeshData[MeshIndex].Active = Active;
}

BOOL Model::IsActiveMesh(int32 MeshIndex) const noexcept
{
	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::UpdateIndex - Invalid mesh index.");
		return FALSE;
	}

	return TRUE;
}

BOOL Model::IsActiveMesh(NameHash Name) const noexcept
{
	const int32 MeshIndex = FindMeshIndexByName(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetMaterial - Cannot find mesh by name.");
		return FALSE;
	}

	return m_lstMeshData[MeshIndex].Active;
}

size_t Model::GetNumMesh() const noexcept
{
	return m_lstMeshData.size();
}

size_t Model::CalulateVertexStride(uint32 VertexFormat) const noexcept
{
	size_t VertexStride = 0;
	if (VertexFormat & (uint32)VERTEX_FORMAT::POSITION)
		VertexStride += sizeof(FLOAT3);
	if (VertexFormat & (uint32)VERTEX_FORMAT::NORMAL)
		VertexStride += sizeof(FLOAT3);
	if (VertexFormat & (uint32)VERTEX_FORMAT::TEXCOORD)
		VertexStride += sizeof(FLOAT2);
	if (VertexFormat & (uint32)VERTEX_FORMAT::COLOR)
		VertexStride += sizeof(FLOAT4);
	if (VertexFormat & (uint32)VERTEX_FORMAT::TANGENT)
		VertexStride += sizeof(FLOAT3);
	if (VertexFormat & (uint32)VERTEX_FORMAT::BINORMAL)
		VertexStride += sizeof(FLOAT3);
	if (VertexFormat & (uint32)VERTEX_FORMAT::BONE)
		VertexStride += sizeof(UINT4);
	if (VertexFormat & (uint32)VERTEX_FORMAT::WEIGHT)
		VertexStride += sizeof(FLOAT4);
	if (VertexFormat & (uint32)VERTEX_FORMAT::TEXCOORD1)
		VertexStride += sizeof(FLOAT4);
	if (VertexFormat & (uint32)VERTEX_FORMAT::TEXCOORD2)
		VertexStride += sizeof(FLOAT4);
	if (VertexFormat & (uint32)VERTEX_FORMAT::TEXCOORD3)
		VertexStride += sizeof(FLOAT4);
	if (VertexFormat & (uint32)VERTEX_FORMAT::TEXCOORD4)
		VertexStride += sizeof(FLOAT4);
	return VertexStride;
}