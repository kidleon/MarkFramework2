#ifndef __D3D11_SHADER_PROGRAM_H__
#define __D3D11_SHADER_PROGRAM_H__

#include "IShaderProgram.h"


class D3D11VertexShader;
class D3D11PixelShader;

class D3D11ShaderProgram : public IShaderProgram
{
public:
	D3D11ShaderProgram(D3D11VertexShader* pVS, D3D11PixelShader* pPS) noexcept;
	virtual int32 VS_GetBindIndexByName(const NameHash& Name) const override;
	virtual int32 PS_GetBindIndexByName(const NameHash& Name) const override;
	virtual void VS_SetConstant(const NameHash& Name, const void* pData, uint32 DataSize) override;
	virtual void VS_SetConstant(int32 BindIndex, const void* pData, uint32 DataSize) override;
	virtual void PS_SetConstant(const NameHash& Name, const void* pData, uint32 DataSize) override;
	virtual void PS_SetConstant(int32 BindIndex, const void* pData, uint32 DataSize) override;
	virtual void PS_SetTexture1D(const NameHash& Name, ITexture1D* pTexture) override;
	virtual void PS_SetTexture1D(int32 BindIndex, ITexture1D* pTexture) override;
	virtual void PS_SetTexture2D(const NameHash& Name, ITexture2D* pTexture) override;
	virtual void PS_SetTexture2D(int32 BindIndex, ITexture2D* pTexture) override;

protected:
	virtual ~D3D11ShaderProgram() noexcept;
	virtual void OnDestroy() override;

private:
	D3D11VertexShader* m_pVS;
	D3D11PixelShader* m_pPS;

};


#endif // __D3D11_SHADER_PROGRAM_H__
