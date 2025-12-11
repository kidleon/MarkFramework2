#ifndef __D3D11_SHADER_H__
#define __D3D11_SHADER_H__


class D3D11ShaderParams;
class D3D11InputLayout;

//----------------------------------------------------------------
struct D3D11VertexShader : public IUNKNOWN_IMPL
{
	ID3D11VertexShader* pVS = nullptr;
	D3D11ShaderParams* pShaderParams = nullptr;
	D3D11InputLayout* pInputLayout = nullptr;

protected:
	virtual ~D3D11VertexShader() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11PixelShader : public IUNKNOWN_IMPL
{
	ID3D11PixelShader* pPS = nullptr;
	D3D11ShaderParams* pShaderParams = nullptr;

protected:
	virtual ~D3D11PixelShader() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11ComputeShader : public IUNKNOWN_IMPL
{
	ID3D11ComputeShader* pCS = nullptr;
	D3D11ShaderParams* pShaderParams = nullptr;

protected:
	virtual ~D3D11ComputeShader() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11SamplerState : public IUNKNOWN_IMPL
{
	ID3D11SamplerState* pSamplerState = nullptr;
	RS_SAMPLER_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11SamplerState() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11RasterizerState : public IUNKNOWN_IMPL
{
	ID3D11RasterizerState* pRasterizerState = nullptr;
	RS_RASTERIZER_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11RasterizerState() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11BlendState : public IUNKNOWN_IMPL
{
	ID3D11BlendState* pBlendState = nullptr;
	RS_BLEND_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11BlendState() noexcept;
	virtual void OnDestroy() override;

};

//----------------------------------------------------------------
struct D3D11DepthStencilState : public IUNKNOWN_IMPL
{
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	RS_DEPTH_STENCIL_STATE State = {};
	UINT32 Hash = 0;

protected:
	virtual ~D3D11DepthStencilState() noexcept;
	virtual void OnDestroy() override;

};


#endif // __D3D11_SHADER_H__


