#ifndef __D3D11_BUFFER_H__
#define __D3D11_BUFFER_H__


class D3D11Buffer
{
public:
	D3D11Buffer(
		D3D11_BUFFER_TYPE BufferType,
		size_t BufferSize,
		size_t Offset,
		void* pCPUAddress,
		ID3D11Buffer* pBuffer
	) noexcept;

	~D3D11Buffer() noexcept;

	__FORCEINLINE size_t INL_GetBufferSize() const noexcept { return m_BufferSize; }
	__FORCEINLINE ID3D11Buffer* INL_GetD3D11Buffer() const noexcept { return m_pD3D11Buffer; }

private:
	D3D11_BUFFER_TYPE m_BufferType;
	uint32 _PADDING = 0;
	size_t m_BufferSize;
	size_t m_Offset;
	void* m_pCPUAddress;
	ID3D11Buffer* m_pD3D11Buffer;

};


#endif // __D3D11_BUFFER_H__
