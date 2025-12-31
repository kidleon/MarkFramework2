#ifndef __D3D11_PRIMITIVE_BUFFER_H__
#define __D3D11_PRIMITIVE_BUFFER_H__


class D3D11Buffer;

class D3D11PrimitiveBuffer final : public IPrimitiveBuffer
{
public:
	struct PRIMITIVE
	{
		PRIMITIVE_TYPE PrimitiveType;
		UINT32 VertexCount;
		UINT32 VertexOffset;
		UINT32 IndexCount;
		UINT32 IndexOffset;

		VOID* pVertexData;
		VOID* pIndexData;
		UINT32 VertexDataSize;
		UINT32 IndexDataSize;
	};

public:
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT32 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IPrimitiveBuffer interface
	virtual void AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32 VertexOffset,
		uint32 VertexCount,
		uint32 IndexOffset,
		uint32 IndexCount
	) noexcept final;

	virtual uint32 GetNumPrimitives() const noexcept final;

	virtual void UpdateVertex(
		int32 PrimitiveIndex,
		const void* pVertexData,
		size_t VertexSize
	) final;

	virtual void UpdateIndex(
		int32 PrimitiveIndex,
		const void* pIndexData,
		size_t IndexSize
	) final;

	__FORCEINLINE size_t INL_GetNumPrimitives() const noexcept { return m_NumPrimitives; }
	__FORCEINLINE const PRIMITIVE& INL_GetPrimitive(size_t Index) const noexcept { return m_Primitives[Index]; }
	__FORCEINLINE BOOL INL_IsDirty() const noexcept { return m_Dirty; }
	
private:
	D3D11PrimitiveBuffer() = delete;
	virtual ~D3D11PrimitiveBuffer() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	UINT32 m_BufferSize = 0;
	UINT32 m_NumPrimitives = 0;
	BOOL m_Dirty = FALSE;
	PRIMITIVE m_Primitives[MAX_PRIMITIVE];

	D3D11Buffer* m_pVertexBuffer = nullptr;
	D3D11Buffer* m_pIndexBuffer = nullptr;

};


#endif // __D3D11_PRIMITIVE_BUFFER_H__
