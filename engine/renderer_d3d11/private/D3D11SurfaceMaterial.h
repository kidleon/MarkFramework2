#pragma once


class D3D11SurfaceMaterial final : public ISurfaceMaterial
{
public:
	virtual int32 AddPass(const char* szPassName) noexcept final;
	virtual int32 GetNumPass() const noexcept final;

	virtual void SetVertexShader(int32 Pass, IShaderProgram* pVS) final;
	virtual void SetVertexShader(IShaderProgram* pVS) final;

	virtual void SetPixelShader(int32 Pass, IShaderProgram* pPS) final;
	virtual void SetPixelShader(IShaderProgram* pPS) final;

	virtual IShaderProgram* GetVertexShader(int32 Pass) noexcept final;
	virtual IShaderProgram* GetVertexShader() noexcept final;

	virtual IShaderProgram* GetPixelShader(int32 Pass) noexcept final;
	virtual IShaderProgram* GetPixelShader() noexcept final;

	virtual void SetColor(int32 Pass, const FLOAT4& Color) final;
	virtual void SetColor(const FLOAT4& Color) final;

	virtual const FLOAT4& GetColor(int32 Pass) const noexcept final;
	virtual const FLOAT4& GetColor() const noexcept final;


};
