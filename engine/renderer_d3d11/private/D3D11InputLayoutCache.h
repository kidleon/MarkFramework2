#pragma once


class D3D11InputLayoutCache
{
	static D3D11InputLayoutCache* s_pInstance;

	struct CACHE_DATA
	{
		ID3D11InputLayout* pInputLayout;
		HASH_NODE HashNode;
	};

public:
	D3D11InputLayoutCache();
	~D3D11InputLayoutCache() noexcept;

	BOOL Init();
	void Shutdown();

	void Register(UINT32 VertexFormat, ID3D11InputLayout* pInputLayout) noexcept;
	ID3D11InputLayout* Find_RS(UINT32 VertexFormat) noexcept;

	static inline D3D11InputLayoutCache* Get() noexcept { return s_pInstance; }

private:
	HASH_TABLE* m_pHashTable = nullptr;

};
