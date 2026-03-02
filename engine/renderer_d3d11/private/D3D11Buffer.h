#ifndef __D3D11_BUFFER_H__
#define __D3D11_BUFFER_H__


class D3D11Buffer
{
public:
	D3D11Buffer(
		ID3D11Buffer* pD3D11Buffer,
		UINT32 BufferSize,
		BUFFER_USAGE Usage
	);
	~D3D11Buffer() noexcept;
	
	__FORCEINLINE ID3D11Buffer* INL_GetD3D11Buffer() const noexcept { return m_pD3D11Buffer; }
	__FORCEINLINE LINK_NODE* INL_GetPoolLinkNode() noexcept { return &m_PoolLinkNode; }
	__FORCEINLINE UINT32 INL_GetBufferSize() const noexcept { return m_BufferSize; }
	__FORCEINLINE BUFFER_USAGE INL_GetUsage() const noexcept { return m_Usage; }

private:
	D3D11Buffer() = delete;

	// D3D11Buffer는 복사나 이동이 불가능하다.
	D3D11Buffer(const D3D11Buffer&) = delete;
	D3D11Buffer& operator=(const D3D11Buffer&) = delete;
	D3D11Buffer(D3D11Buffer&&) = delete;
	D3D11Buffer& operator=(D3D11Buffer&&) = delete;

private:
	ID3D11Buffer* m_pD3D11Buffer = nullptr;
	UINT32 m_BufferSize = 0;
	BUFFER_USAGE m_Usage = BUFFER_USAGE::DEFAULT;
	LINK_NODE m_PoolLinkNode = {};

};


#endif // __D3D11_BUFFER_H__
