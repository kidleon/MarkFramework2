#include "pch.h"
#include "D3D11ShaderParamData.h"
#include "D3D11RenderResources.h"


D3D11ShaderParamData::D3D11ShaderParamData()
{
	m_LinkNode.data = this;
}

void D3D11ShaderParamData::Reset()
{
}

void D3D11ShaderParamData::Build(D3D11VertexShader* pVS, D3D11PixelShader* pPS)
{
	pVS->pShaderParams->INL_GetShaderParamByIndex()
}