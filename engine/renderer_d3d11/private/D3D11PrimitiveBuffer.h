#ifndef __D3D11_PRIMITIVE_BUFFER_H__
#define __D3D11_PRIMITIVE_BUFFER_H__


class D3D11Buffer;

class D3D11PrimitiveBuffer final : public IPrimitiveBuffer
{
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

	D3D11Buffer* m_pVertexBuffer = nullptr;
	D3D11Buffer* m_pIndexBuffer = nullptr;
	size_t m_BufferSize = 0;

};


#endif // __D3D11_PRIMITIVE_BUFFER_H__
