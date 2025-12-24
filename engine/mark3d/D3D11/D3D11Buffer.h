#ifndef __D3D11_BUFFER_H__
#define __D3D11_BUFFER_H__


class D3D11Buffer
{
public:
	// 독립 버퍼용 생성자
	D3D11Buffer(
		D3D11_BUFFER_TYPE BufferType,
		size_t BufferSize,
		ID3D11Buffer* pBuffer
	) noexcept;

	// 버퍼 풀용 생성자.
	D3D11Buffer(
		D3D11_BUFFER_TYPE BufferType,
		size_t BufferSize,
		uint32 Format,
		uint32 PoolIndex,
		ID3D11Buffer* pBuffer
	) noexcept;

	// TRANSIENT 버퍼용 생성자
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
	D3D11_BUFFER_STRATEGY m_BufferStrategy;
	size_t m_BufferSize;
	size_t m_Offset;
	uint32 m_BufferFormat;
	uint32 m_PoolIndex;
	void* m_pCPUAddress;
	ID3D11Buffer* m_pD3D11Buffer;

};


#endif // __D3D11_BUFFER_H__
