#pragma once


class D3D11_BLOB;

class D3D11PrimitiveBuffer final : public IPrimitiveBuffer
{
	static constexpr size_t MAX_PRIMITIVES = 8;

	struct PRIMITIVE_DESC
	{
		PRIMITIVE_TYPE PrimitiveType;
		uint32 VertexOffset;
		uint32 VertexCount;
		uint32 IndexOffset;
		uint32 IndexCount;
	};;

public:
	// DEFAULT(STATIC) 버퍼용 생성자
	D3D11PrimitiveBuffer(
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer
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
		uint32 VertexOffset,
		uint32 VertexCount,
		uint32 IndexOffset,
		uint32 IndexCount
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

	D3D11_BLOB* m_pVertexBlob = nullptr;
	D3D11_BLOB* m_pIndexBlob = nullptr;

	ID3D11Buffer* m_pD3D11VertexBuffer = nullptr;
	ID3D11Buffer* m_pD3D11IndexBuffer = nullptr;

};