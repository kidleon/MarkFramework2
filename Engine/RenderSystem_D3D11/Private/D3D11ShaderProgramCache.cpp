#include "pch.h"
#include "D3D11ShaderProgramCache.h"
#include "D3D11ShaderProgram.h"


namespace mark
{
	D3D11ShaderProgramCache::~D3D11ShaderProgramCache() noexcept
	{
		for (int i = 0; i < (int)SHADER_TYPE::EMAX; ++i)
		{
			for (auto& pair : m_ShaderProgramMap[i])
			{
				IShaderProgram* pShaderProgram = pair.second;
				if (pShaderProgram)
				{
					pShaderProgram->Release();
				}
			}
			m_ShaderProgramMap[i].clear();
		}
	}

	bool D3D11ShaderProgramCache::Register(D3D11ShaderProgram* pShaderProgram)
	{
		if (!pShaderProgram)
			return false;

		SHADER_TYPE ShaderType = pShaderProgram->INL_GetShaderType();
		name_hash NameHash = pShaderProgram->INL_GetNameHash();

		auto& ShaderMap = m_ShaderProgramMap[(int)ShaderType];

		if (ShaderMap.find(NameHash) != ShaderMap.end())
		{
			SYS_LOG_WRN_F("Shader program with name hash {} already exists in the cache for shader type {}.", NameHash.get_hash(), (int)ShaderType);
			return false;
		}
		ShaderMap[NameHash] = pShaderProgram;

		return true;
	}

	[[nodiscard]] D3D11ShaderProgram* D3D11ShaderProgramCache::Query(
		SHADER_TYPE ShaderType,
		const char* szShaderName
	)
	{
		name_hash NameHash(szShaderName);

		auto& ShaderMap = m_ShaderProgramMap[(int)ShaderType];

		auto it = ShaderMap.find(NameHash);
		if (it != ShaderMap.end())
		{
			D3D11ShaderProgram* pShaderProgram = it->second;
			if (pShaderProgram)
			{
				pShaderProgram->AddRef();
				return pShaderProgram;
			}
		}

		return nullptr;
	}


}
