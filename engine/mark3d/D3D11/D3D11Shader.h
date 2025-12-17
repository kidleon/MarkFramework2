#ifndef __D3D11_SHADER_H__
#define __D3D11_SHADER_H__


class D3D11ShaderParamTable;

class D3D11Shader
{
public:
	D3D11Shader(
		const NameHash& ShaderName,
		D3D11ShaderParamTable* pParamTable,
		ID3D11VertexShader* pVS
	);

	D3D11Shader(
		const NameHash& ShaderName,
		D3D11ShaderParamTable* pParamTable,
		ID3D11PixelShader* pPS
	);

	D3D11Shader(
		const NameHash& ShaderName,
		D3D11ShaderParamTable* pParamTable,
		ID3D11ComputeShader* pCS
	);

	~D3D11Shader() noexcept;

	__FORCEINLINE const NameHash& INL_GetShaderName() const noexcept
	{
		return m_ShaderName;
	}

	__FORCEINLINE SHADER_TYPE INL_GetShaderType() const noexcept
	{
		return m_ShaderType;
	}

	__FORCEINLINE D3D11ShaderParamTable* INL_GetParamTable() const noexcept
	{
		return m_pParamTable;
	}

	__FORCEINLINE ID3D11VertexShader* INL_GetVertexShader() const noexcept
	{
		if (m_ShaderType == SHADER_TYPE::VERTEX)
			return m_pVS;
		return nullptr;
	}

	__FORCEINLINE ID3D11PixelShader* INL_GetPixelShader() const noexcept
	{
		if (m_ShaderType == SHADER_TYPE::PIXEL)
			return m_pPS;
		return nullptr;
	}

	__FORCEINLINE ID3D11ComputeShader* INL_GetComputeShader() const noexcept
	{
		if (m_ShaderType == SHADER_TYPE::COMPUTE)
			return m_pCS;
		return nullptr;
	}

	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept
	{
		return &m_HashNode;
	}

private:
	D3D11Shader() = delete;

private:
	NameHash m_ShaderName;
	SHADER_TYPE m_ShaderType;
	D3D11ShaderParamTable* m_pParamTable;

	union
	{
		ID3D11VertexShader* m_pVS;
		ID3D11PixelShader* m_pPS;
		ID3D11ComputeShader* m_pCS;
	};

	HASH_NODE m_HashNode;
};


#endif // __D3D11_SHADER_H__