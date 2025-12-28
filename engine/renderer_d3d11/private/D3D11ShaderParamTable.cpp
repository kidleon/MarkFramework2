#include "pch.h"
#include "D3D11ShaderParamTable.h"


BOOL D3D11ShaderParamTable::Build(
	D3D11_SHADER_PARAMS* pShaderParams,
	UINT32 NumShaderParams
)
{
	m_NumCBufferParams = 0;
	m_NumTextureParams = 0;
	m_NumSamplerParams = 0;

	for (UINT32 i = 0; i < NumShaderParams; ++i)
	{
		D3D11_SHADER_PARAMS& Param = pShaderParams[i];
		switch (Param.ParamType)
		{
			case CPARAM_CONSTANT:
			{
				if (m_NumCBufferParams < MAX_CONSTANT_SLOT)
				{
					m_CBufferParams[m_NumCBufferParams++] = Param;
				}
				break;
			}
				
			case CPARAM_TEXTURE:
			{
				if (m_NumTextureParams < MAX_TEXTURE_SLOT)
				{
					m_TextureParams[m_NumTextureParams++] = Param;
				}
				break;
			}
				
			case CPARAM_SAMPLER:
			{
				if (m_NumSamplerParams < MAX_SAMPLER_SLOT)
				{
					m_SamplerParams[m_NumSamplerParams++] = Param;
				}
				break;
			}
				
			default:
				break;
		}
	}
	return TRUE;
}