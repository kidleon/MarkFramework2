#ifndef __D3D11_SHADER_PARAM_TABLE_H__
#define __D3D11_SHADER_PARAM_TABLE_H__

#include "D3D11RenderDef.h"


class D3D11ShaderParamTable
{
public:
	D3D11ShaderParamTable() = default;
	~D3D11ShaderParamTable() noexcept = default;

	BOOL Build(
		D3D11_SHADER_PARAMS* pShaderParams,
		UINT32 NumShaderParams
	);

	__FORCEINLINE size_t INL_GetNumCBufferParams() const noexcept
	{
		return m_NumCBufferParams;
	}

	__FORCEINLINE size_t INL_GetNumTextureParams() const noexcept
	{
		return m_NumTextureParams;
	}

	__FORCEINLINE size_t INL_GetNumSamplerParams() const noexcept
	{
		return m_NumSamplerParams;
	}

	// By Index
	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetCBufferParamByIndex(size_t Index) noexcept
	{
		if (Index < m_NumCBufferParams)
			return &m_CBufferParams[Index];
		return nullptr;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetTextureParamByIndex(size_t Index) noexcept
	{
		if (Index < m_NumTextureParams)
			return &m_TextureParams[Index];
		return nullptr;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetSamplerParamByIndex(size_t Index) noexcept
	{
		if (Index < m_NumSamplerParams)
			return &m_SamplerParams[Index];
		return nullptr;
	}

	// By BindPoint
	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetCBufferParamByBindPoint(uint32 BindPoint) noexcept
	{
		for (size_t i = 0; i < m_NumCBufferParams; ++i)
		{
			if (m_CBufferParams[i].BindPoint == BindPoint)
				return &m_CBufferParams[i];
		}

		return nullptr;
	}
	
	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetTextureParamByBindPoint(uint32 BindPoint) noexcept
	{
		for (size_t i = 0; i < m_NumTextureParams; ++i)
		{
			if (m_TextureParams[i].BindPoint == BindPoint)
				return &m_TextureParams[i];
		}
		return nullptr;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetSamplerParamByBindPoint(uint32 BindPoint) noexcept
	{
		for (size_t i = 0; i < m_NumSamplerParams; ++i)
		{
			if (m_SamplerParams[i].BindPoint == BindPoint)
				return &m_SamplerParams[i];
		}
		return nullptr;
	}

	// By HashName
	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetCBufferParamByName(const NameHash& ParamName) noexcept
	{
		for (size_t i = 0; i < m_NumCBufferParams; ++i)
		{
			if (m_CBufferParams[i].Name == ParamName)
				return &m_CBufferParams[i];
		}
		return nullptr;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetTextureParamByName(const NameHash& ParamName) noexcept
	{
		for (size_t i = 0; i < m_NumTextureParams; ++i)
		{
			if (m_TextureParams[i].Name == ParamName)
				return &m_TextureParams[i];
		}
		return nullptr;
	}

	__FORCEINLINE const D3D11_SHADER_PARAMS* INL_GetSamplerParamByName(const NameHash& ParamName) noexcept
	{
		for (size_t i = 0; i < m_NumSamplerParams; ++i)
		{
			if (m_SamplerParams[i].Name == ParamName)
				return &m_SamplerParams[i];
		}
		return nullptr;
	}

private:
	D3D11_SHADER_PARAMS m_CBufferParams[MAX_CONSTANT_SLOT] = {};
	D3D11_SHADER_PARAMS m_TextureParams[MAX_TEXTURE_SLOT] = {};
	D3D11_SHADER_PARAMS m_SamplerParams[MAX_SAMPLER_SLOT] = {};

	size_t m_NumCBufferParams = 0;
	size_t m_NumTextureParams = 0;
	size_t m_NumSamplerParams = 0;

};


#endif // __D3D11_SHADER_PARAM_TABLE_H__
