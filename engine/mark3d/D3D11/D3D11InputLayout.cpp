#include "pch.h"
#include "D3D11InputLayout.h"


D3D11InputLayout::D3D11InputLayout(
	uint32 VertexFormat,
	ID3D11InputLayout* pInputLayout
)
	: m_BufferFormat(VertexFormat)
	, m_pD3D11InputLayout(pInputLayout)
	, m_HashNode{}
{
	m_HashNode.data = this;
}

D3D11InputLayout::~D3D11InputLayout() noexcept
{
	if (m_pD3D11InputLayout)
	{
		m_pD3D11InputLayout->Release();
		m_pD3D11InputLayout = nullptr;
	}
}