#ifndef __MESH_DATA_H__
#define __MESH_DATA_H__

#include "IMesh.h"


class Mesh : public IMesh
{
public:
	Mesh() = default;
	Mesh(uint32 m_VertexFormat, size_t TotalVertexCount, size_t TotalIndexCount);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT32 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	virtual int32 AddPrimitive(PRIMITIVE_TYPE PrimitiveType, size_t MaxVertex, size_t MaxIndex) noexcept override;
	virtual int32 GetNumPrimitives() const noexcept override;

	virtual void SetPrimitiveType(int32 PrimitiveIndex, PRIMITIVE_TYPE PrimitiveType) override;
	virtual PRIMITIVE_TYPE GetPrimitiveType(int32 PrimitiveIndex) const noexcept override;

	virtual void SetPositions(int32 PrimitiveIndex, const FLOAT3* pPositions, size_t NumPosition) override;
	virtual void SetNormals(int32 PrimitiveIndex, const FLOAT3* pNormals, size_t NumNormal) override;
	virtual void SetColors(int32 PrimitiveIndex, const COLOR_VAULE* pColors, size_t NumColor) override;
	virtual void SetTexCoords(int32 PrimitiveIndex, const FLOAT2* pTexCoords, size_t NumTexCoord) override;
	virtual void SetTexCoords1(int32 PrimitiveIndex, const FLOAT4* pTexCoords, size_t NumTexCoord) override;
	virtual void SetTangents(int32 PrimitiveIndex, const FLOAT3* pTangents, size_t NumTangent) override;
	virtual void SetBinormals(int32 PrimitiveIndex, const FLOAT3* pBinormals, size_t NumBinormal) override;
	virtual void SetIndices16(int32 PrimitiveIndex, const uint16* pIndices, size_t NumIndex) override;

	virtual int32 AddPosition(int32 PrimitiveIndex, const FLOAT3& Position) override;
	virtual int32 AddNormal(int32 PrimitiveIndex, const FLOAT3& Normal) override;
	virtual int32 AddColor(int32 PrimitiveIndex, const COLOR_VAULE& Color) override;
	virtual int32 AddTexCoord(int32 PrimitiveIndex, const FLOAT2& TexCoord) override;
	virtual int32 AddTexCoord1(int32 PrimitiveIndex, const FLOAT4& TexCoord) override;
	virtual int32 AddTangent(int32 PrimitiveIndex, const FLOAT3& Tangent) override;
	virtual int32 AddBinormal(int32 PrimitiveIndex, const FLOAT3& Binormal) override;
	virtual int32 AddIndex16(int32 PrimitiveIndex, uint16 Index) override;

	virtual FLOAT3* GetPosition(int32 PrimitiveIndex) noexcept override;
	virtual FLOAT3* GetNormal(int32 PrimitiveIndex) noexcept override;
	virtual COLOR_VAULE* GetColor(int32 PrimitiveIndex) noexcept override;
	virtual FLOAT2* GetTexCoord(int32 PrimitiveIndex) noexcept override;
	virtual FLOAT4* GetTexCoord1(int32 PrimitiveIndex) noexcept override;
	virtual FLOAT3* GetTangent(int32 PrimitiveIndex) noexcept override;
	virtual FLOAT3* GetBinormal(int32 PrimitiveIndex) noexcept override;
	virtual uint16* GetIndex16(int32 PrimitiveIndex) noexcept override;

	virtual size_t GetVertexCount(int32 PrimitiveIndex) const noexcept override;
	virtual size_t GetIndexCount(int32 PrimitiveIndex) const noexcept override;

	// Private line functions
	__INLINE int32 INL_GetNumPrimitives() const noexcept { return (int32)m_Primitives.size(); }

	__INLINE const PRIMITIVE_TYPE INL_GetPrimitiveType(int32 PrimitiveIndex) const noexcept 
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return PRIMITIVE_TYPE::UNKNOWN;
		return m_Primitives[PrimitiveIndex].PrimitiveType;
	}

	__INLINE FLOAT3* INL_GetPosition(int32 PrimitiveIndex) noexcept 
	{ 
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pPositions + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE FLOAT3* INL_GetNormal(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pNormals + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE COLOR_VAULE* INL_GetColor(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pColors + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE FLOAT2* INL_GetTexCoord(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pTexCoords + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE FLOAT4* INL_GetTexCoord1(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pTexCoords1 + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE FLOAT3* INL_GetTangent(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pTangents + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE FLOAT3* INL_GetBinormal(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pBinormals + m_Primitives[PrimitiveIndex].VertexStart;
	}

	__INLINE uint16* INL_GetIndex16(int32 PrimitiveIndex) noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return nullptr;
		return m_pIndices16 + m_Primitives[PrimitiveIndex].IndexStart;
	}

	__INLINE size_t INL_GetVertexCount(int32 PrimitiveIndex) const noexcept
	{
		if (m_Primitives.size() <= PrimitiveIndex)
			return 0;
		return m_Primitives[PrimitiveIndex].PositionCount;
	}

	__INLINE size_t INL_GetIndexCount(int32 PrimitiveIndex) const noexcept
	{
		if(m_Primitives.size() <= PrimitiveIndex)
			return 0;
		return m_Primitives[PrimitiveIndex].IndexCount;
	}

protected:
	virtual ~Mesh() noexcept;
	void Clear() noexcept;
	void AllocBuffer(
		uint32 m_VertexFormat, 
		size_t TotalVertexCount, 
		size_t TotalIndexCount
	) noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID;
	LOAD_STAT m_LoadStat;


	struct PRIMITIVE
	{
		PRIMITIVE_TYPE PrimitiveType;
		unsigned int PADDING;
		size_t VertexStart;
		size_t IndexStart;
		size_t MaxVertex;
		size_t MaxIndex;
		size_t IndexCount;

		size_t PositionCount;
		size_t NormalCount;
		size_t ColorCount;
		size_t TexCoordCount;
		size_t TexCoord1Count;
		size_t TangentCount;
		size_t BinormalCount;
	};

	size_t m_TotalVertexCount = 0;
	size_t m_TotalIndexCount = 0;
	size_t m_UsedVertexCount = 0;
	size_t m_UsedIndexCount = 0;

	TArray<PRIMITIVE, ALLOC_TYPE::POOL> m_Primitives;

	FLOAT3* m_pPositions = nullptr;
	FLOAT3* m_pNormals = nullptr;
	COLOR_VAULE* m_pColors = nullptr;
	FLOAT2* m_pTexCoords = nullptr;
	FLOAT4* m_pTexCoords1 = nullptr;
	FLOAT3* m_pTangents = nullptr;
	FLOAT3* m_pBinormals = nullptr;

	uint16* m_pIndices16 = nullptr;

};


#endif // __MESH_DATA_H__