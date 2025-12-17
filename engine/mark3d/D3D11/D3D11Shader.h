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
};


#endif // __D3D11_SHADER_H__