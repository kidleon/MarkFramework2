#include "pch.h"

#include "Mesh.h"
#include "TArray.h"
#include "mathlib.h"
#include "Log.h"


Mesh::Mesh(uint32 m_BufferFormat, size_t TotalVertexCount, size_t TotalIndexCount)
	: m_TotalVertexCount(TotalVertexCount)
	, m_TotalIndexCount(TotalIndexCount)
{
	m_Primitives.reserve(8);
	AllocBuffer(m_BufferFormat, TotalVertexCount, TotalIndexCount);
}

Mesh::~Mesh() noexcept
{
}

long Mesh::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Mesh::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_POOL_DELETE(this, Mesh);
	}
	return NewRefCnt;
}

long Mesh::RefCnt()
{
	return m_RefCnt;
}

UINT32 Mesh::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE Mesh::GetAssetType() const noexcept
{
	return ASSET_TYPE::MESH;
}

LOAD_STAT Mesh::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

void Mesh::Clear() noexcept
{
	m_Primitives.clear();
	m_UsedVertexCount = 0;
	m_UsedIndexCount = 0;
	m_TotalVertexCount = 0;
	m_TotalIndexCount = 0;

	if (m_pPositions)
	{
		CORE_POOL_FREE(m_pPositions);
		m_pPositions = nullptr;
	}

	if (m_pNormals)
	{
		CORE_POOL_FREE(m_pNormals);
		m_pNormals = nullptr;
	}

	if (m_pColors)
	{
		CORE_POOL_FREE(m_pColors);
		m_pColors = nullptr;
	}

	if (m_pTexCoords)
	{
		CORE_POOL_FREE(m_pTexCoords);
		m_pTexCoords = nullptr;
	}

	if (m_pTexCoords1)
	{
		CORE_POOL_FREE(m_pTexCoords1);
		m_pTexCoords1 = nullptr;
	}

	if (m_pTangents)
	{
		CORE_POOL_FREE(m_pTangents);
		m_pTangents = nullptr;
	}

	if (m_pBinormals)
	{
		CORE_POOL_FREE(m_pBinormals);
		m_pBinormals = nullptr;
	}

	if (m_pIndices16)
	{
		CORE_POOL_FREE(m_pIndices16);
		m_pIndices16 = nullptr;
	}
}

void Mesh::AllocBuffer(
	uint32 m_BufferFormat,
	size_t TotalVertexCount,
	size_t TotalIndexCount
) noexcept
{
	Clear();
	m_TotalVertexCount = TotalVertexCount;
	m_TotalIndexCount = TotalIndexCount;

	if (0 < m_TotalVertexCount)
	{
		if (m_BufferFormat & (uint32)VERTEX_FORMAT::POSITION)
			m_pPositions = (FLOAT3*)CORE_POOL_ALLOC(sizeof(FLOAT3) * TotalVertexCount);

		if (m_BufferFormat & (uint32)VERTEX_FORMAT::NORMAL)
			m_pNormals = (FLOAT3*)CORE_POOL_ALLOC(sizeof(FLOAT3) * TotalVertexCount);

		if (m_BufferFormat & (uint32)VERTEX_FORMAT::COLOR)
			m_pColors = (COLOR_VAULE*)CORE_POOL_ALLOC(sizeof(COLOR_VAULE) * TotalVertexCount);

		if (m_BufferFormat & (uint32)VERTEX_FORMAT::TEXCOORD)
			m_pTexCoords = (FLOAT2*)CORE_POOL_ALLOC(sizeof(FLOAT2) * TotalVertexCount);

		if (m_BufferFormat & (uint32)VERTEX_FORMAT::TEXCOORD1)
			m_pTexCoords1 = (FLOAT4*)CORE_POOL_ALLOC(sizeof(FLOAT4) * TotalVertexCount);

		if (m_BufferFormat & (uint32)VERTEX_FORMAT::TANGENT)
			m_pTangents = (FLOAT3*)CORE_POOL_ALLOC(sizeof(FLOAT3) * TotalVertexCount);

		if (m_BufferFormat & (uint32)VERTEX_FORMAT::BINORMAL)
			m_pBinormals = (FLOAT3*)CORE_POOL_ALLOC(sizeof(FLOAT3) * TotalVertexCount);
	}
	
	if (0 < m_TotalIndexCount)
	{
		m_pIndices16 = (uint16*)CORE_POOL_ALLOC(sizeof(uint16) * TotalIndexCount);
	}
}

int32 Mesh::AddPrimitive(PRIMITIVE_TYPE PrimitiveType, size_t MaxVertex, size_t MaxIndex) noexcept
{
	if (m_UsedVertexCount + MaxVertex > m_TotalVertexCount)
	{
		SYS_LOG_E("Mesh::AddPrimitive - Exceeded total vertex count.");
		return -1;
	}

	if (m_UsedIndexCount + MaxIndex > m_TotalIndexCount)
	{
		SYS_LOG_E("Mesh::AddPrimitive - Exceeded total index count.");
		return -1;
	}
	
	PRIMITIVE Primitive = {};
	Primitive.PrimitiveType = PrimitiveType;
	Primitive.VertexStart = m_UsedVertexCount;
	Primitive.IndexStart = m_UsedIndexCount;
	Primitive.MaxVertex = MaxVertex;
	Primitive.MaxIndex = MaxIndex;
	m_Primitives.push_back(Primitive);

	m_UsedVertexCount += MaxVertex;
	m_UsedIndexCount += MaxIndex;

	return (int32)m_Primitives.size() - 1;
}

int32 Mesh::GetNumPrimitives() const noexcept
{
	return (int32)m_Primitives.size();
}

void Mesh::SetPrimitiveType(int32 PrimitiveIndex, PRIMITIVE_TYPE PrimitiveType)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;
	m_Primitives[PrimitiveIndex].PrimitiveType = PrimitiveType;
}

PRIMITIVE_TYPE Mesh::GetPrimitiveType(int32 PrimitiveIndex) const noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return PRIMITIVE_TYPE::UNKNOWN;
	return m_Primitives[PrimitiveIndex].PrimitiveType;
}

void Mesh::SetPositions(int32 PrimitiveIndex, const FLOAT3* pPositions, size_t NumPosition)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	FLOAT3* pPositionDest = m_pPositions + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pPositionDest, pPositions, sizeof(FLOAT3) * NumPosition);

	m_Primitives[PrimitiveIndex].PositionCount = NumPosition;
}

void Mesh::SetNormals(int32 PrimitiveIndex, const FLOAT3* pNormals, size_t NumNormal)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	FLOAT3* pDest = m_pNormals + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pDest, pNormals, sizeof(FLOAT3) * NumNormal);

	m_Primitives[PrimitiveIndex].NormalCount = NumNormal;
}

void Mesh::SetColors(int32 PrimitiveIndex, const COLOR_VAULE* pColors, size_t NumColor)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	COLOR_VAULE* pDest = m_pColors + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pDest, pColors, sizeof(COLOR_VAULE) * NumColor);

	m_Primitives[PrimitiveIndex].ColorCount = NumColor;
}

void Mesh::SetTexCoords(int32 PrimitiveIndex, const FLOAT2* pTexCoords, size_t NumTexCoord)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	FLOAT2* pDest = m_pTexCoords + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pDest, pTexCoords, sizeof(FLOAT2) * NumTexCoord);

	m_Primitives[PrimitiveIndex].TexCoordCount = NumTexCoord;
}

void Mesh::SetTexCoords1(int32 PrimitiveIndex, const FLOAT4* pTexCoords, size_t NumTexCoord)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	FLOAT4* pDest = m_pTexCoords1 + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pDest, pTexCoords, sizeof(FLOAT4) * NumTexCoord);

	m_Primitives[PrimitiveIndex].TexCoord1Count = NumTexCoord;
}

void Mesh::SetTangents(int32 PrimitiveIndex, const FLOAT3* pTangents, size_t NumTangent)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	FLOAT3* pDest = m_pTangents + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pDest, pTangents, sizeof(FLOAT3) * NumTangent);

	m_Primitives[PrimitiveIndex].TangentCount = NumTangent;
}

void Mesh::SetBinormals(int32 PrimitiveIndex, const FLOAT3* pBinormals, size_t NumBinormal)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxVertex < m_Primitives[PrimitiveIndex].MaxVertex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max vertex count.");
		return;
	}

	FLOAT3* pDest = m_pBinormals + m_Primitives[PrimitiveIndex].VertexStart;
	memcpy(pDest, pBinormals, sizeof(FLOAT3) * NumBinormal);

	m_Primitives[PrimitiveIndex].BinormalCount = NumBinormal;
}

void Mesh::SetIndices16(int32 PrimitiveIndex, const uint16* pIndices, size_t NumIndex)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return;

	if (m_Primitives[PrimitiveIndex].MaxIndex < m_Primitives[PrimitiveIndex].MaxIndex)
	{
		SYS_LOG_E("Mesh::SetPositions - Exceeded primitive max index count.");
		return;
	}

	uint16* pDest = m_pIndices16 + m_Primitives[PrimitiveIndex].IndexStart;
	memcpy(pDest, pIndices, sizeof(uint16) * NumIndex);

	m_Primitives[PrimitiveIndex].IndexCount = NumIndex;
}

int32 Mesh::AddPosition(int32 PrimitiveIndex, const FLOAT3& Position)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].PositionCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	FLOAT3* pDest = m_pPositions + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].PositionCount);
	*pDest = Position;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].PositionCount;
	m_Primitives[PrimitiveIndex].PositionCount++;

	return VertexPos;
}

int32 Mesh::AddNormal(int32 PrimitiveIndex, const FLOAT3& Normal)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].NormalCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	FLOAT3* pDest = m_pNormals + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].NormalCount);
	*pDest = Normal;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].NormalCount;
	m_Primitives[PrimitiveIndex].NormalCount++;

	return VertexPos;
}

int32 Mesh::AddColor(int32 PrimitiveIndex, const COLOR_VAULE& Color)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].ColorCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	COLOR_VAULE* pDest = m_pColors + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].ColorCount);
	*pDest = Color;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].ColorCount;
	m_Primitives[PrimitiveIndex].ColorCount++;

	return VertexPos;
}

int32 Mesh::AddTexCoord(int32 PrimitiveIndex, const FLOAT2& TexCoord)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].ColorCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	FLOAT2* pDest = m_pTexCoords + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].TexCoordCount);
	*pDest = TexCoord;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].TexCoordCount;
	m_Primitives[PrimitiveIndex].TexCoordCount++;

	return VertexPos;
}

int32 Mesh::AddTexCoord1(int32 PrimitiveIndex, const FLOAT4& TexCoord)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].TexCoord1Count)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	FLOAT4* pDest = m_pTexCoords1 + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].TexCoord1Count);
	*pDest = TexCoord;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].TexCoord1Count;
	m_Primitives[PrimitiveIndex].TexCoord1Count++;

	return VertexPos;
}

int32 Mesh::AddTangent(int32 PrimitiveIndex, const FLOAT3& Tangent)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].TangentCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	FLOAT3* pDest = m_pTangents + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].TangentCount);
	*pDest = Tangent;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].TangentCount;
	m_Primitives[PrimitiveIndex].TangentCount++;

	return VertexPos;
}

int32 Mesh::AddBinormal(int32 PrimitiveIndex, const FLOAT3& Binormal)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxVertex <= m_Primitives[PrimitiveIndex].TangentCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max vertex count.");
		return -1;
	}

	FLOAT3* pDest = m_pBinormals + (m_Primitives[PrimitiveIndex].VertexStart + m_Primitives[PrimitiveIndex].BinormalCount);
	*pDest = Binormal;

	int32 VertexPos = (int32)m_Primitives[PrimitiveIndex].BinormalCount;
	m_Primitives[PrimitiveIndex].BinormalCount++;

	return VertexPos;
}

int32 Mesh::AddIndex16(int32 PrimitiveIndex, uint16 Index)
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return -1;

	if (m_Primitives[PrimitiveIndex].MaxIndex <= m_Primitives[PrimitiveIndex].IndexCount)
	{
		SYS_LOG_E("Mesh::AddPosition - Exceeded primitive max index count.");
		return -1;
	}

	uint16* pDest = m_pIndices16 + (m_Primitives[PrimitiveIndex].IndexStart + m_Primitives[PrimitiveIndex].IndexCount);
	*pDest = Index;

	int32 IndexPos = (int32)m_Primitives[PrimitiveIndex].IndexCount;
	m_Primitives[PrimitiveIndex].IndexCount++;

	return IndexPos;
}

FLOAT3* Mesh::GetPosition(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pPositions + m_Primitives[PrimitiveIndex].VertexStart;
}

FLOAT3* Mesh::GetNormal(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pNormals + m_Primitives[PrimitiveIndex].VertexStart;
}

COLOR_VAULE* Mesh::GetColor(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pColors + m_Primitives[PrimitiveIndex].VertexStart;
}

FLOAT2* Mesh::GetTexCoord(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pTexCoords + m_Primitives[PrimitiveIndex].VertexStart;
}

FLOAT4* Mesh::GetTexCoord1(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pTexCoords1 + m_Primitives[PrimitiveIndex].VertexStart;
}

FLOAT3* Mesh::GetTangent(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pTangents + m_Primitives[PrimitiveIndex].VertexStart;
}

FLOAT3* Mesh::GetBinormal(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pBinormals + m_Primitives[PrimitiveIndex].VertexStart;
}

uint16* Mesh::GetIndex16(int32 PrimitiveIndex) noexcept
{
	if (m_Primitives.size() <= PrimitiveIndex)
		return nullptr;

	return m_pIndices16 + m_Primitives[PrimitiveIndex].IndexStart;
}

size_t Mesh::GetVertexCount(int32 PrimitiveIndex) const noexcept
{
	return m_Primitives[PrimitiveIndex].PositionCount;
}

size_t Mesh::GetIndexCount(int32 PrimitiveIndex) const noexcept
{
	return m_Primitives[PrimitiveIndex].IndexCount;
}