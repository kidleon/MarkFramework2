#ifndef __D3D11_INPUT_LAYOUT_H__
#define __D3D11_INPUT_LAYOUT_H__


class D3D11InputLayout 
{
public:
	D3D11InputLayout(
		uint32 VertexFormat, 
		ID3D11InputLayout* pInputLayout
	);

	~D3D11InputLayout() noexcept;

	__FORCEINLINE ID3D11InputLayout* INL_GetD3D11InputLayout() const noexcept
	{
		return m_pD3D11InputLayout;
	}

	__FORCEINLINE uint32 INL_GetVertexFormat() const noexcept
	{
		return m_BufferFormat;
	}

	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept
	{
		return &m_HashNode;
	}

private:
	ID3D11InputLayout* m_pD3D11InputLayout;
	uint32 m_BufferFormat;
	uint32 PADDING = 0;
	HASH_NODE m_HashNode;

};

#endif // __D3D11_INPUT_LAYOUT_H__
