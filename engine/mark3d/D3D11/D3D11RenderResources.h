#ifndef __D3D11_SHADER_H__
#define __D3D11_SHADER_H__

class D3D11ShaderParams;

//----------------------------------------------------------------
struct D3D11InputLayout : public IUNKNOWN_HASH_IMPL
{
	ID3D11InputLayout* pIL = nullptr;
	UINT32 NumVertexFormat = 0;
	VERTEX_FORMAT VertexFormats[MAX_VERTEX_FORMAT] = {};
	VERTEX_FORMAT_INDEX VertexFormatIndices[MAX_VERTEX_FORMAT] = {};

protected:
	virtual ~D3D11InputLayout() noexcept;
	virtual void OnDestroy() override;

};

/*
//----------------------------------------------------------------
struct D3D11VertexShader : public IUNKNOWN_HASH_IMPL
{
	ID3D11VertexShader* pVS = nullptr;
	D3D11ShaderParams* pShaderParams = nullptr;
	D3D11InputLayout* pInputLayout = nullptr;

protected:
	virtual ~D3D11VertexShader() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11PixelShader : public IUNKNOWN_HASH_IMPL
{
	ID3D11PixelShader* pPS = nullptr;
	D3D11ShaderParams* pShaderParams = nullptr;

protected:
	virtual ~D3D11PixelShader() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11ComputeShader : public IUNKNOWN_HASH_IMPL
{
	ID3D11ComputeShader* pCS = nullptr;
	D3D11ShaderParams* pShaderParams = nullptr;

protected:
	virtual ~D3D11ComputeShader() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11ConstantBuffer : public IUNKNOWN_LIST_IMPL
{
	ID3D11Buffer* pConstantBuffer = nullptr;
	UINT32 SizeInBytes = 0;

protected:
	virtual ~D3D11ConstantBuffer() noexcept;
	virtual void OnDestroy() override;
};

//----------------------------------------------------------------
struct D3D11SamplerState : public IUNKNOWN_HASH_IMPL
{
	ID3D11SamplerState* pSamplerState = nullptr;
	///RS_SAMPLER_STA State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11SamplerState() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11RasterizerState : public IUNKNOWN_HASH_IMPL
{
	ID3D11RasterizerState* pRasterizerState = nullptr;
	RS_RASTERIZER_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11RasterizerState() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11BlendState : public IUNKNOWN_HASH_IMPL
{
	ID3D11BlendState* pBlendState = nullptr;
	RS_BLEND_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11BlendState() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11DepthStencilState : public IUNKNOWN_HASH_IMPL
{
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	RS_DEPTH_STENCIL_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11DepthStencilState() noexcept;
	virtual void OnDestroy() override;

};
*/

#endif // __D3D11_SHADER_H__


