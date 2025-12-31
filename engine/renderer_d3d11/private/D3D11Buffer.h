#ifndef __D3D11_BUFFER_H__
#define __D3D11_BUFFER_H__


class D3D11BufferPool;
class D3D11RenderDevice;

class D3D11Buffer
{
	ID3D11Buffer* pD3D11Buffer;

public:
	// 독립 버퍼용 생성자
	D3D11Buffer(
		D3D11_BUFFER_TYPE BufferType,
		BUFFER_USAGE Usage,
		size_t BufferSize,
		ID3D11Buffer* pBuffer
	) noexcept;

	// 버퍼 풀용 생성자.
	D3D11Buffer(
		D3D11_BUFFER_TYPE BufferType,
		D3D11BufferPool* pBufferPool,
		int PoolIndex,
		uint32 BufferSize,
		uint32 Offset,
		ID3D11Buffer* pBuffer
	) noexcept;

	// TRANSIENT 버퍼용 생성자
	D3D11Buffer(
		D3D11_BUFFER_TYPE BufferType,
		size_t BufferSize,
		size_t Offset,
		ID3D11Buffer* pBuffer
	) noexcept;

	~D3D11Buffer() noexcept;

	__FORCEINLINE size_t INL_GetBufferSize() const noexcept { return m_BufferSize; }
	__FORCEINLINE ID3D11Buffer* INL_GetD3D11Buffer() const noexcept { return m_pD3D11Buffer; }

private:
	ID3D11Buffer* m_pD3D11Buffer = nullptr;

	D3D11_BUFFER_TYPE m_BufferType = D3D11_BUFFER_TYPE::VERTEX_BUFFER;
	D3D11_BUFFER_STRATEGY m_BufferStrategy = D3D11_BUFFER_STRATEGY::INDEPENDENT;
	BUFFER_USAGE m_Usage = BUFFER_USAGE::DEFAULT;

	uint32 m_PoolIndex = 0;
	D3D11BufferPool* m_pBufferPool = nullptr;

	uint32 m_BufferSize = 0;
	uint32 m_Offset = 0;

};


#endif // __D3D11_BUFFER_H__
