#include "pch.h"
#include "D3D11InputLayout.h"


D3D11InputLayout::D3D11InputLayout(
	INT32 NumVertexFormat,
	VERTEX_FORMAT* pVertexFormats,
	VERTEX_FORMAT_INDEX* pVertexFormatIndices,
	ID3D11InputLayout* pD3D11InputLayout
)
	: m_NumVertexFormat(NumVertexFormat)
	, m_pD3D11InputLayout(pD3D11InputLayout)
{
	for (INT32 v = 0; v < NumVertexFormat; ++v)
	{
		m_VertexFormats[v] = pVertexFormats[v];
		m_VertexFormatIndcies[v] = pVertexFormatIndices[v];
	}

	//m_HashNode.data = this;
}

D3D11InputLayout::~D3D11InputLayout()
{
	CHECK_RELEASE(m_pD3D11InputLayout);
}