#ifndef __D3D11_VERTEX_SHADER_H__
#define __D3D11_VERTEX_SHADER_H__


class D3D11ShaderProp;
class D3D11InputLayout;

class D3D11VertexShader
{
public:
	D3D11VertexShader(
		ID3D11VertexShader* pVS,
		D3D11ShaderProp* pShaderProp,
		D3D11InputLayout* pInputLayout
	);

	~D3D11VertexShader() noexcept;

	__FORCEINLINE ID3D11VertexShader* INL_GetVS() const noexcept
	{
		return m_pVS;
	}

	__FORCEINLINE D3D11ShaderProp* INL_GetShaderProp() const noexcept
	{
		return m_pShaderProp;
	}

	__FORCEINLINE D3D11InputLayout* INL_GetInputLayout() const noexcept
	{
		return m_pInputLayout;
	}

private:
	ID3D11VertexShader* m_pVS;
	D3D11ShaderProp* m_pShaderProp;
	D3D11InputLayout* m_pInputLayout;

};

//---------------------------------------------------------------------
class D3D11PixelShader
{
public:
	D3D11PixelShader(
		ID3D11PixelShader* pPS,
		D3D11ShaderProp* pShaderProp
	);

	~D3D11PixelShader() noexcept;

	__FORCEINLINE ID3D11PixelShader* INL_GetPS() const
	{
		return m_pPS;
	}

	__FORCEINLINE D3D11ShaderProp* INL_GetShaderProp() const
	{
		return m_pShaderProp;
	}

private:
	ID3D11PixelShader* m_pPS;
	D3D11ShaderProp* m_pShaderProp;

};

//---------------------------------------------------------------------
class D3D11ComputeShader
{
public:
	D3D11ComputeShader(
		ID3D11ComputeShader* pCS,
		D3D11ShaderProp* pShaderProp
	);
	~D3D11ComputeShader() noexcept;

	__FORCEINLINE ID3D11ComputeShader* INL_GetCS() const
	{
		return m_pCS;
	}

	__FORCEINLINE D3D11ShaderProp* INL_GetShaderProp() const
	{
		return m_pShaderProp;
	}

private:
	ID3D11ComputeShader* m_pCS;
	D3D11ShaderProp* m_pShaderProp;

};

#endif // __D3D11_VERTEX_SHADER_H__


