#include "pch.h"
#include "Mesh.h"


Mesh::Mesh(
	IPrimitiveBuffer* pPrimitiveBuffer,
	INT32 PrimitiveIndex,
	UINT32 TotalVertexCount,
	UINT32 TotalIndexCount
)
	: m_pPrimitiveBuffer(pPrimitiveBuffer)
	, m_PrimitiveIndex(PrimitiveIndex)
	, m_TotalVertexCount(TotalVertexCount)
	, m_TotalIndexCount(TotalIndexCount)
{
}

Mesh::~Mesh() noexcept
{
	for (UINT32 i = 0; i < MAX_MESH_PART; i++)
	{
		CHECK_RELEASE(m_pMaterials[i]);
	}
}

long Mesh::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Mesh::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (!NewRefCnt)
	{
		CORE_POOL_DELETE(this, Mesh);
	}

	return NewRefCnt;
}

long Mesh::RefCnt()
{
	return m_RefCnt;
}

void Mesh::SetName(const char* szName) noexcept
{
	m_NameHash = NameHash(szName);
}

const NameHash& Mesh::GetName() const noexcept
{
	return m_NameHash;
}

void Mesh::SetPosition(const FLOAT3* pPositions, UINT32 NumPosition) noexcept
{
	__ASSERT(m_TotalVertexCount >= NumPosition, "Number of positions exceeds total vertex count");

	if (m_TotalVertexCount <= NumPosition)
		m_pPrimitiveBuffer->UpdatePosition(m_PrimitiveIndex, pPositions, NumPosition);
	m_UsedVertexCount = T_MAX(m_UsedVertexCount, NumPosition);
}

void Mesh::SetNormal(const FLOAT3* pNormals, UINT32 NumNormal) noexcept
{
	__ASSERT(m_TotalVertexCount >= NumNormal, "Number of normals exceeds total vertex count");

	if (m_TotalVertexCount <= NumNormal)
		m_pPrimitiveBuffer->UpdateNormal(m_PrimitiveIndex, pNormals, NumNormal);

	m_UsedVertexCount = T_MAX(m_UsedVertexCount, NumNormal);
}

void Mesh::SetTexCoord(const FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept
{
	__ASSERT(m_TotalVertexCount >= NumTexCoord, "Number of texture coordinates exceeds total vertex count");

	if (m_TotalVertexCount <= NumTexCoord)
		m_pPrimitiveBuffer->UpdateTexCoord0(m_PrimitiveIndex, pTexCoords, NumTexCoord);

	m_UsedVertexCount = T_MAX(m_UsedVertexCount, NumTexCoord);
}

void Mesh::SetColor(const FLOAT4* pColors, UINT32 NumColor) noexcept
{
	__ASSERT(m_TotalVertexCount >= NumColor, "Number of colors exceeds total vertex count");

	if (m_TotalVertexCount <= NumColor)
		m_pPrimitiveBuffer->UpdateColor(m_PrimitiveIndex, pColors, NumColor);

	m_UsedVertexCount = T_MAX(m_UsedVertexCount, NumColor);
}

void Mesh::SetTangent(const FLOAT3* pTangents, UINT32 NumTangent) noexcept
{
	__ASSERT(m_TotalVertexCount >= NumTangent, "Number of tangents exceeds total vertex count");

	if (m_TotalVertexCount <= NumTangent)
		m_pPrimitiveBuffer->UpdateTangent(m_PrimitiveIndex, pTangents, NumTangent);

	m_UsedVertexCount = T_MAX(m_UsedVertexCount, NumTangent);
}

void Mesh::SetBinormal(const FLOAT3* pBinormals, UINT32 NumBinormal) noexcept
{
	__ASSERT(m_TotalVertexCount >= NumBinormal, "Number of binormals exceeds total vertex count");

	if (m_TotalVertexCount <= NumBinormal)
		m_pPrimitiveBuffer->UpdateBinormal(m_PrimitiveIndex, pBinormals, NumBinormal);

	m_UsedVertexCount = T_MAX(m_UsedVertexCount, NumBinormal);
}

UINT32 Mesh::GetNumMeshPart() const noexcept
{
	return m_NumMeshPart;
}

void Mesh::SetIndex(const UINT16* pIndices, UINT32 NumIndices) noexcept
{
	m_NumMeshPart = 1;
	m_pNumIndices[0] = NumIndices;
	m_pPrimitiveBuffer->UpdateIndex(m_PrimitiveIndex, pIndices, NumIndices);
}

void Mesh::SetIndex(UINT32 NumMeshParts, const UINT16** ppIndices, UINT32* pNumIndices) noexcept
{
	__ASSERT(NumMeshParts <= MAX_MESH_PART, "Number of mesh parts exceeds maximum");
	if (NumMeshParts > MAX_MESH_PART)
		return;

	m_NumMeshPart = NumMeshParts;
	for (UINT32 i = 0; i < NumMeshParts; i++)
		m_pNumIndices[i] = pNumIndices[i];

	m_pPrimitiveBuffer->UpdateIndex(m_PrimitiveIndex, NumMeshParts, ppIndices, pNumIndices);
}

UINT32 Mesh::GetNumIndex(INT32 MeshPartIndex) const noexcept
{
	__ASSERT(MeshPartIndex < m_NumMeshPart, "Invalid mesh part index");

	if (MeshPartIndex >= m_NumMeshPart)
		return 0;

	return m_pNumIndices[MeshPartIndex];
}

void Mesh::SetMaterial(INT32 MeshPartIndex, ISurfaceMaterial* pMaterial) noexcept
{
	__ASSERT(MeshPartIndex < m_NumMeshPart, "Invalid mesh part index");
	if (MeshPartIndex >= m_NumMeshPart)
		return;

	if (m_pMaterials[MeshPartIndex] != pMaterial)
	{
		CHECK_RELEASE(m_pMaterials[MeshPartIndex]);
		CHECK_ADDREF(pMaterial);
		m_pMaterials[MeshPartIndex] = pMaterial;
	}
}

void Mesh::GetMaterial(INT32 MeshPartIndex, ISurfaceMaterial** ppMaterial) noexcept
{
	__ASSERT(MeshPartIndex < m_NumMeshPart, "Invalid mesh part index");

	if (MeshPartIndex >= m_NumMeshPart)
	{
		*ppMaterial = nullptr;
		return;
	}

	*ppMaterial = m_pMaterials[MeshPartIndex];
}

ISurfaceMaterial* Mesh::GetMaterial(INT32 MeshPartIndex) noexcept
{
	__ASSERT(MeshPartIndex < m_NumMeshPart, "Invalid mesh part index");

	if (MeshPartIndex >= m_NumMeshPart)
		return nullptr;

	return m_pMaterials[MeshPartIndex];
}