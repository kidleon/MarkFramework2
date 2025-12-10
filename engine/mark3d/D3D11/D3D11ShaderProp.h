#ifndef __D3D11_SHADER_PROP_H__
#define __D3D11_SHADER_PROP_H__

#include "D3D11RenderDef.h"


class D3D11ShaderProp
{
public:
	D3D11ShaderProp(
		D3D11_SHADER_PARAMS* pShaderParams,
		UINT32 NumShaderParams
	);

	~D3D11ShaderProp();

	__FORCEINLINE BOOL INL_HasShaderParam(NameHash ParamName) const
	{
		for (UINT32 i = 0; i < m_NumShaderParams; ++i)
		{
			if (m_pShaderParams[i].Name == ParamName)
				return TRUE;
		}

		return FALSE;
	}

	__FORCEINLINE int32 INL_GetShaderParamIndexByName(NameHash ParamName) const
	{
		for (UINT32 i = 0; i < m_NumShaderParams; ++i)
		{
			if (m_pShaderParams[i].Name == ParamName)
				return static_cast<int32>(i);
		}

		return -1;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetShaderParamByIndex(UINT32 Index) const
	{
		if (Index < m_NumShaderParams)
			return &m_pShaderParams[Index];
		return nullptr;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetShaderParamByName(NameHash ParamName) const
	{
		for (UINT32 i = 0; i < m_NumShaderParams; ++i)
		{
			if (m_pShaderParams[i].Name == ParamName)
				return &m_pShaderParams[i];
		}
		return nullptr;
	}

	__FORCEINLINE UINT32 INL_GetNumShaderParams() const
	{
		return m_NumShaderParams;
	}

private:
	D3D11_SHADER_PARAMS* m_pShaderParams = nullptr;
	UINT32 m_NumShaderParams = 0;

};


#endif // __D3D11_SHADER_PROP_H__
