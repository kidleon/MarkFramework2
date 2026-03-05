#pragma once


class D3D11_BLOB;
class D3D11Buffer;

class D3D11PrimitiveBuffer final : public IPrimitiveBuffer
{
	static constexpr INT32 MAX_PRIMITIVES = 8;
	static constexpr INT32 MAX_SUB_MESH = 8; // 하나의 프리미티브에 최대 4개의 인덱스 버퍼 (서브메시)

public:
	struct PRIMITIVE_DESC
	{
		PRIMITIVE_TYPE PrimitiveType;
		uint32 VertexStart;
		uint32 VertexCount;
		uint32 IndexStart;
		uint32 IndexCount;

		uint32 NumIndices = 0;
		uint32 IndexStarts[MAX_SUB_MESH] = {};
		uint32 IndexCounts[MAX_SUB_MESH] = {};
	};

	static PRIMITIVE_DESC INVALID_PRIMITIVE_DESC;

public:
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IPrimitiveBuffer interface
	virtual void ResetPrimitive() noexcept final;

	virtual INT32 AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32 VertexCount,
		uint32 IndexCount
	) noexcept final;

	virtual INT32 AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32 VertexCount,
		uint32 NumIndices,
		uint32* pIndices
	) noexcept final;

	virtual size_t GetNumPrimitives() const noexcept final;

	virtual BOOL UpdatePosition(
		int32 PrimitiveIndex,
		const FLOAT3* pPositions,
		UINT32 PositionCount
	) final;

	virtual BOOL UpdateNormal(
		int32 PrimitiveIndex,
		const FLOAT3* pNormals,
		UINT32 NormalCount
	) final;

	virtual BOOL UpdateColor(
		int32 PrimitiveIndex,
		const FLOAT4* pColors,
		UINT32 ColorCount
	) final;

	virtual BOOL UpdateTexCoord0(
		int32 PrimitiveIndex,
		const FLOAT2* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTangent(
		int32 PrimitiveIndex,
		const FLOAT3* pTangents,
		UINT32 TangentCount
	) final;

	virtual BOOL UpdateBinormal(
		int32 PrimitiveIndex,
		const FLOAT3* pBinormal,
		UINT32 BinormalCount
	) final;

	virtual BOOL UpdateBlendIndices(
		int32 PrimitiveIndex,
		const UINT4* pBlendIndices,
		UINT32 BlendIndexCount
	) final;

	virtual BOOL UpdateBlendWeights(
		int32 PrimitiveIndex,
		const FLOAT4* pBlendWeights,
		UINT32 BlendWeightCount
	) final;

	virtual BOOL UpdateTexCoord1(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord2(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord3(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord4(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord5(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord6(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord7(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateTexCoord8(
		int32 PrimitiveIndex,
		const FLOAT4* pTexCoords,
		UINT32 TexCoordCount
	) final;

	virtual BOOL UpdateIndex(
		int32 PrimitiveIndex,
		const uint32* pIndices,
		UINT32 IndexCount
	) final;

	virtual BOOL UpdateIndex(
		int32 PrimitiveIndex,
		UINT32 NumIndices,
		const uint32** ppIndices,
		UINT32* pIndexCounts
	) final;

	virtual UINT32 GetMaxVertexCount() const noexcept final;
	virtual UINT32 GetMaxIndexCount() const noexcept final;

	void UploadToGPU(ID3D11DeviceContext* pDeviceContext);

	BOOL Create(
		UINT32 VertexFormat,
		size_t MaxVertexCount,
		size_t MaxIndexCount,
		BUFFER_USAGE UsageVB,
		BUFFER_USAGE UsageIB
	) noexcept;

	void ComputeBindVBs(int PrimitiveIndex) noexcept;

	__FORCEINLINE const PRIMITIVE_DESC& INL_GetPrimitiveDesc(size_t PrimitiveIndex) const noexcept
	{
		if (PrimitiveIndex >= m_NumPrimitives)
			return INVALID_PRIMITIVE_DESC;
		return m_Primitives[PrimitiveIndex];
	}

	__FORCEINLINE D3D11Buffer* INL_GetVB(VERTEX_FORMAT_INDEX Index) const noexcept { return m_pVBs[(int)Index]; }
	__FORCEINLINE D3D11Buffer* INL_GetIB() const noexcept { return m_pIB; }
	__FORCEINLINE UINT32 INL_GetVertexFormat() const noexcept { return m_VertexFormat; }

	__FORCEINLINE BOOL INL_IsDirtyBuffer() const noexcept { return m_DirtyBuffer; }

	__FORCEINLINE ID3D11Buffer** INL_GetBindVBs() noexcept { return m_pBindVBs; }
	__FORCEINLINE UINT32* INL_GetBindVBStrides() noexcept { return m_BindVB_Strides; }
	__FORCEINLINE UINT32* INL_GetBindVBOffsets() noexcept { return m_BindVB_Offsets; }
	__FORCEINLINE UINT32 INL_GetBindVBCount() const noexcept { return m_BindVB_Count; }

private:
	virtual ~D3D11PrimitiveBuffer() noexcept;
	BOOL UpdateVertexBuffer(
		int32 PrimitiveIndex, 
		int VertexFormatIndex, 
		const void* pVertexData, 
		UINT32 VertexCount,
		UINT32 VertexStride
	);

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	PRIMITIVE_DESC m_Primitives[MAX_PRIMITIVES] = {};
	UINT32 m_CurrentVertexCount = 0;
	UINT32 m_CurrentIndexCount = 0;
	UINT32 m_MaxVertexCount = 0;
	UINT32 m_MaxIndexCount = 0;
	INT32 m_NumPrimitives = 0;
	uint32 m_VertexFormat = 0;
	BOOL m_DirtyBuffer = FALSE;
	UINT32 PADDING1 = 0;

	BOOL m_DirtyVBs[(size_t)VERTEX_FORMAT_INDEX::MAX] = {};
	BOOL m_DirtyIB = FALSE;
	UINT32 PADDING2 = 0;

	D3D11_BLOB* m_pVBlobs[(size_t)VERTEX_FORMAT_INDEX::MAX] = {};
	D3D11_BLOB* m_pIBlob = nullptr;

	D3D11Buffer* m_pVBs[(size_t)VERTEX_FORMAT_INDEX::MAX] = {};
	D3D11Buffer* m_pIB = nullptr;

	ID3D11Buffer* m_pBindVBs[(size_t)VERTEX_FORMAT_INDEX::MAX] = {};
	UINT32 m_BindVB_Strides[(size_t)VERTEX_FORMAT_INDEX::MAX] = {};
	UINT32 m_BindVB_Offsets[(size_t)VERTEX_FORMAT_INDEX::MAX] = {};
	UINT32 m_BindVB_Count = 0;

};