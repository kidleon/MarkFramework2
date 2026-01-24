#pragma once


class D3D11_BLOB;

class D3D11PrimitiveBuffer final : public IPrimitiveBuffer
{
	static constexpr size_t MAX_PRIMITIVES = 8;


public:
	struct PRIMITIVE_DESC
	{
		PRIMITIVE_TYPE PrimitiveType;
		uint32 VertexOffset;
		uint32 VertexCount;
		uint32 VertexStride;
		uint32 IndexOffset;
		uint32 IndexCount;
		uint32 IndexStride;
	};

	static PRIMITIVE_DESC INVALID_PRIMITIVE_DESC;

public:
	D3D11PrimitiveBuffer(
		BUFFER_USAGE Usage,
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		size_t VertexBufferSize,
		size_t IndexBufferSize
	) noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IPrimitiveBuffer interface
	virtual BUFFER_USAGE GetUsage() const noexcept final;

	virtual void ResetPrimitive() noexcept final;

	virtual INT32 AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32 VertexCount,
		uint32 VertexStride,
		uint32 IndexCount,
		uint32 IndexStride
	) noexcept final;

	virtual size_t GetNumPrimitives() const noexcept final;

	virtual BOOL UpdateVertex(
		int32 PrimitiveIndex,
		const void* pVertexData,
		size_t VertexSize
	) final;

	virtual BOOL UpdateIndex(
		int32 PrimitiveIndex,
		const void* pIndexData,
		size_t IndexSize
	) final;

	void UploadToGPU_VB(ID3D11DeviceContext* pDeviceContext);
	void UploadToGPU_IB(ID3D11DeviceContext* pDeviceContext);

	__FORCEINLINE const PRIMITIVE_DESC& INL_GetPrimitiveDesc(size_t PrimitiveIndex) const noexcept
	{
		if (PrimitiveIndex >= m_NumPrimitives)
			return INVALID_PRIMITIVE_DESC;
		return m_Primitives[PrimitiveIndex];
	}

	__FORCEINLINE ID3D11Buffer* INL_GetD3D11VertexBuffer() const noexcept
	{
		return m_pD3D11VertexBuffer;
	}

	__FORCEINLINE ID3D11Buffer* INL_GetD3D11IndexBuffer() const noexcept
	{
		return m_pD3D11IndexBuffer;
	}

	__FORCEINLINE BOOL INL_IsDirtyVertexBuffer() const noexcept { return m_DirtyVertexBuffer; }
	__FORCEINLINE BOOL INL_IsDirtyIndexBuffer() const noexcept { return m_DirtyIndexBuffer; }

private:
	virtual ~D3D11PrimitiveBuffer() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	BUFFER_USAGE m_Usage = BUFFER_USAGE::DEFAULT;

	PRIMITIVE_DESC m_Primitives[MAX_PRIMITIVES] = {};
	size_t m_NumPrimitives = 0;

	BOOL m_DirtyVertexBuffer = FALSE;
	BOOL m_DirtyIndexBuffer = FALSE;

	D3D11_BLOB* m_pVertexBlob = nullptr;
	D3D11_BLOB* m_pIndexBlob = nullptr;

	size_t m_CurrentVertexSize = 0;
	size_t m_CurrentIndexSize = 0;

	size_t m_VertexBufferSize = 0;
	size_t m_IndexBufferSize = 0;
	ID3D11Buffer* m_pD3D11VertexBuffer = nullptr;
	ID3D11Buffer* m_pD3D11IndexBuffer = nullptr;

};