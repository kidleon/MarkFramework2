#ifndef __D3D11_CONSTANT_BUFFER_H__
#define __D3D11_CONSTANT_BUFFER_H__


class D3D11ConstantBuffer final : public IConstantBuffer
{
public:
	D3D11ConstantBuffer(ID3D11Buffer* pBuffer, void* pAlignedBuffer, size_t BufferSize);
	
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IConstantBuffer 인터페이스 구현
	virtual void UpdateData(void* pData, size_t DataSize) final;

	BOOL UploadToGPU(ID3D11DeviceContext* pDeviceContext, void* pBufferData, size_t BufferSize);

	__FORCEINLINE ID3D11Buffer* INL_GetD3D11Buffer() const noexcept { return m_pD3D11Buffer; }
	__FORCEINLINE size_t INL_GetBufferSize() const noexcept { return m_BufferSize; }

	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }

private:
	virtual ~D3D11ConstantBuffer() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	ID3D11Buffer* m_pD3D11Buffer;
	size_t m_BufferSize;

	void* m_pBufferData;
	void* m_pBufferDataRef;

	LINK_NODE m_LinkNode;
};


#endif // __D3D11_CONSTANT_BUFFER_H__