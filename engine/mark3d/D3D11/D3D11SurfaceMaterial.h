#ifndef __D3D11_SURFACE_MATERIAL_H__
#define __D3D11_SURFACE_MATERIAL_H__

#include "D3D11RenderDef.h"


/*
* #include "ISurfaceMaterial.h"
* 
class D3D11VertexShader;
class D3D11PixelShader;

class D3D11SurfaceMaterial : public ISurfaceMaterial
{
public:
	D3D11SurfaceMaterial() = default;
	

	virtual int32 GetNumPass() const noexcept override;
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

private:
	virtual ~D3D11SurfaceMaterial() noexcept;
	virtual void OnDestroy() override;

private:
	RENDER_PASS m_Passes[MAX_RENDER_PASS];

};
*/


#endif // __D3D11_SURFACE_MATERIAL_H__
