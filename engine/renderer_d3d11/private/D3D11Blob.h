#pragma once


class D3D11_BLOB : public IUNKNOWN
{
public:
	D3D11_BLOB(void* pBuffer, size_t size);
	
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	void Update(void* pBuffer, size_t size);
	void Update(void* pBuffer, size_t size, size_t offset);
	void Clear() noexcept;

	__FORCEINLINE void* INL_GetPointer() const noexcept { return m_pBuffer; }
	__FORCEINLINE size_t INL_GetSize() const noexcept { return m_BufferSize; }
	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }

private:
	D3D11_BLOB() = default;
	~D3D11_BLOB() noexcept;

private:
	long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	void* m_pBuffer = nullptr;
	size_t m_BufferSize = 0;

	LINK_NODE m_LinkNode = {};

};
