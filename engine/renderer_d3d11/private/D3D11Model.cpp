#include "pch.h"
#include "D3D11Model.h"
#include "D3D11Mesh.h"


D3D11Model::D3D11Model(UINT64 ID, uint32 VertexFormat, IPrimitiveBuffer* pPrimitiveBuffer)
	: m_ID(ID)
	, m_pPrimitiveBuffer(pPrimitiveBuffer)
	, m_VertexFormat(VertexFormat)
{
}

D3D11Model::~D3D11Model() noexcept
{
	CHECK_RELEASE(m_pPrimitiveBuffer);
}

long D3D11Model::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11Model::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (!NewRefCnt)
	{
		D3D11_POOL_DELETE(this, D3D11Model);
	}

	return NewRefCnt;
}

long D3D11Model::RefCnt()
{
	return m_RefCnt;
}

UINT64 D3D11Model::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11Model::GetAssetType() const noexcept
{
	return ASSET_TYPE::MODEL;
}

LOAD_STAT D3D11Model::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

UINT32 D3D11Model::GetNumMesh() const noexcept
{
	return m_NumMesh;
}

INT32 D3D11Model::AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, UINT32 NumVertex, UINT32 NumIndex) noexcept
{
	__ASSERT(m_NumMesh < MAX_MESH_PER_MODEL, "Exceeded maximum number of meshes per model.");

	if (m_NumMesh >= MAX_MESH_PER_MODEL)
	{
		SYS_LOG_E("Model::AddMesh - Exceeded maximum number of meshes per model.");
		return -1;
	}

	INT32 PrimitiveIndex = m_pPrimitiveBuffer->AddPrimitive(PrimitiveType, NumVertex, NumIndex);
	if (-1 == PrimitiveIndex)
	{
		SYS_LOG_E("Model::AddMesh - Failed to add primitive to primitive buffer.");
		return -1;
	}

	m_pMeshes[m_NumMesh] = D3D11_POOL_NEW(D3D11Mesh)(m_pPrimitiveBuffer, PrimitiveIndex, NumVertex, NumIndex);
	m_NumMesh++;

	return (INT32)(m_NumMesh - 1);
}

IMesh* D3D11Model::GetMesh(INT32 MeshIndex) noexcept
{
	__ASSERT(MeshIndex >= 0 && MeshIndex < (INT32)m_NumMesh, "Mesh index is out of range.");

	if (MeshIndex < 0 || MeshIndex >= (INT32)m_NumMesh)
	{
		SYS_LOG_E("Model::GetMesh - Mesh index is out of range.");
		return nullptr;
	}

	return m_pMeshes[MeshIndex];
}

IMesh* D3D11Model::GetMesh(NameHash Name) noexcept
{
	for (UINT32 i = 0; i < m_NumMesh; i++)
	{
		if (m_pMeshes[i] && m_pMeshes[i]->GetName() == Name)
			return m_pMeshes[i];
	}

	return nullptr;
}
