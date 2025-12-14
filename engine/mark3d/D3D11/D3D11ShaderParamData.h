#ifndef __D3D11_SHADER_PARAM_DATA_H__
#define __D3D11_SHADER_PARAM_DATA_H__


struct D3D11ConstantBuffer;
struct D3D11VertexShader;
struct D3D11PixelShader;

interface ITexture1D;
interface ITexture2D;

class D3D11ShaderParamData
{
	struct CONSTANT_BUFFER_DATA
	{
		D3D11ConstantBuffer* pCB;
		NameHash Name;
		int32 BindIndex;
	};

	struct TEXTURE1D_DATA
	{
		ITexture1D* pTexture1D;
		NameHash Name;
		int32 BindIndex;
	};

	struct TEXTURE2D_DATA
	{
		ITexture2D* pTexture2D;
		NameHash Name;
		int32 BindIndex;
	};

	CONSTANT_BUFFER_DATA m_VSConstantBuffers[16] = {};
	CONSTANT_BUFFER_DATA m_PSConstantBuffers[16] = {};
	TEXTURE1D_DATA m_PSTexture1Ds[16] = {};
	TEXTURE2D_DATA m_PSTexture2Ds[16] = {};

	LINK_NODE m_LinkNode;

public:
	D3D11ShaderParamData();

	void Build(D3D11VertexShader* pVS, D3D11PixelShader* pPS);
	void Reset();

	void VS_SetConstant(int32 BindIndex, D3D11ConstantBuffer* pCB);
	void PS_SetConstant(int32 BindIndex, D3D11ConstantBuffer* pCB);
	void VS_SetConstant(NameHash Name, D3D11ConstantBuffer* pCB);
	void PS_SetConstant(NameHash Name, D3D11ConstantBuffer* pCB);



};


#endif // __D3D11_SHADER_PARAM_DATA_H__
