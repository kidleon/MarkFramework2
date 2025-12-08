#ifndef __D3D11_INPUT_LAYOUT_H__
#define __D3D11_INPUT_LAYOUT_H__


class D3D11InputLayout
{
public:
	D3D11InputLayout(
		INT32 NumVertexFormat,
		VERTEX_FORMAT* pVertexFormats,
		VERTEX_FORMAT_INDEX* pVertexFormatIndices,
		ID3D11InputLayout* pD3D11InputLayout
	);

	~D3D11InputLayout() noexcept;

	__INLINE UINT32 INL_GetNumVertexFormat() const noexcept { return m_NumVertexFormat; }
	__INLINE VERTEX_FORMAT INL_GetVertexFormat(UINT32 Index) const noexcept { return m_VertexFormats[Index]; }
	__INLINE VERTEX_FORMAT_INDEX INL_GetVertexFormatIndex(UINT32 Index) const noexcept { return m_VertexFormatIndcies[Index]; }
	__INLINE ID3D11InputLayout* INL_GetD3D11InputLayout() const noexcept { return m_pD3D11InputLayout; }

private:
	ID3D11InputLayout* m_pD3D11InputLayout;

	UINT32 m_NumVertexFormat = 0;
	UINT32 m_PADDING = 0;
	VERTEX_FORMAT m_VertexFormats[MAX_VERTEX_FORMAT] = {};
	VERTEX_FORMAT_INDEX  m_VertexFormatIndcies[MAX_VERTEX_FORMAT] = {};

};


#endif // __D3D11_INPUT_LAYOUT_H__
