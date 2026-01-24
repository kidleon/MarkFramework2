#pragma once


class D3D11ShaderProgram final : public IShaderProgram
{
public:
	explicit D3D11ShaderProgram(
		UINT64 ID,
		UINT32 Index,
		NameHash ShaderName,
		UINT32 ShaderDefinesHash,
		UINT32 InputVertexFormat,
		ID3D11VertexShader* pVertexShader
	);

	explicit D3D11ShaderProgram(
		UINT64 ID,
		UINT32 Index,
		NameHash ShaderName,
		UINT32 ShaderDefinesHash,
		ID3D11PixelShader* pPixelShader
	);

	explicit D3D11ShaderProgram(
		UINT64 ID,
		UINT32 Index,
		NameHash ShaderName,
		UINT32 ShaderDefinesHash,
		ID3D11GeometryShader* pGeometryShader
	);

	explicit D3D11ShaderProgram(
		UINT64 ID,
		UINT32 Index,
		NameHash ShaderName,
		UINT32 ShaderDefinesHash,
		ID3D11ComputeShader* pComputeShader
	);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	virtual SHADER_TYPE GetShaderType() const noexcept final;

	__FORCEINLINE UINT32 INL_GetShaderIndex() const noexcept { return m_ShaderIndex; }
	__FORCEINLINE SHADER_TYPE INL_GetShaderType() const noexcept { return m_ShaderType; }
	__FORCEINLINE NameHash INL_GetShaderName() const noexcept { return m_ShaderName; }
	__FORCEINLINE UINT32 INL_GetShaderDefinesHash() const noexcept { return m_ShaderDefinesHash; }
	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept { return &m_HashNode; }
	__FORCEINLINE UINT32 INL_GetInputVertexFormat() const noexcept { return m_InputVertexFormat; }

	__FORCEINLINE ID3D11VertexShader* INL_GetVertexShader() noexcept
	{
		return (m_ShaderType == SHADER_TYPE::VERTEX) ? m_pVertexShader : nullptr;
	}

	__FORCEINLINE ID3D11PixelShader* INL_GetPixelShader() noexcept
	{
		return (m_ShaderType == SHADER_TYPE::PIXEL) ? m_pPixelShader : nullptr;
	}

private:
	virtual ~D3D11ShaderProgram() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	NameHash m_ShaderName;
	SHADER_TYPE m_ShaderType;
	UINT32 m_ShaderDefinesHash;
	UINT32 m_InputVertexFormat = 0;
	UINT32 m_ShaderIndex = 0;
	union
	{
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11GeometryShader* m_pGeometryShader;
		ID3D11ComputeShader* m_pComputeShader;
		void* m_pShaderHandle;
	};

	HASH_NODE m_HashNode = {};
};
