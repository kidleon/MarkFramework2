#ifndef __D3D11_CONSTANT_BUFFER_H__
#define __D3D11_CONSTANT_BUFFER_H__


class D3D11ConstantBuffer
{
public:
	D3D11ConstantBuffer(
		size_t BufferSize,
		ID3D11Buffer* pD3D11Buffer
	);

	~D3D11ConstantBuffer() noexcept;


	__FORCEINLINE size_t INL_GetBufferSize() const noexcept
	{
		return m_BufferSize;
	}

	__FORCEINLINE ID3D11Buffer* INL_GetD3D11Buffer() const noexcept
	{
		return m_pD3D11Buffer;
	}

	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept
	{
		return &m_LinkNode;
	}

private:
	size_t m_BufferSize;
	ID3D11Buffer* m_pD3D11Buffer;
	LINK_NODE m_LinkNode;

};


#endif // __D3D11_CONSTANT_BUFFER_H__
