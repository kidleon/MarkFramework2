#include "pch.h"
#include "D3D11ShaderProgramCache.h"
#include "D3D11ShaderProgram.h"


namespace mark
{
	D3D11ShaderProgramCache::D3D11ShaderProgramCache()
	{
		for (int i = 0; i < (int)SHADER_TYPE::EMAX; ++i)
		{
			ShaderProgramGroup& SPGroup = m_SPGroups[i];
			init_spin_lock(&SPGroup.SpinLock);
		}
	}

	D3D11ShaderProgramCache::~D3D11ShaderProgramCache() noexcept
	{
		for (int i = 0; i < (int)SHADER_TYPE::EMAX; ++i)
		{
			ShaderProgramGroup& SPGroup = m_SPGroups[i];

			AUTO_SPIN_LOCK Guard(&SPGroup.SpinLock);
			for (auto& pair : SPGroup.ShaderProgramMap)
			{
				if (pair.second)
				{
					pair.second->Release(); // 참조 카운트 감소
					pair.second = nullptr;
				}
			}

			SPGroup.ShaderProgramMap.clear();
		}
	}

	bool D3D11ShaderProgramCache::Register(D3D11ShaderProgram* pShaderProgram)
	{
		if (!pShaderProgram) return false;

		pShaderProgram->AddRef();

		SHADER_TYPE ShaderType = pShaderProgram->GetShaderType();
		NameHash ShaderNameHash = pShaderProgram->GetShaderNameHash();

		ShaderProgramGroup& SPGroup = m_SPGroups[(int)ShaderType];

		AUTO_SPIN_LOCK Guard(&(SPGroup.SpinLock));

		auto it = SPGroup.ShaderProgramMap.find(ShaderNameHash);
		if (it != SPGroup.ShaderProgramMap.end())
		{
			pShaderProgram->Release();
			SYS_LOG_F("Shader program with name hash {} already exists in cache for shader type {}.", ShaderNameHash.get_hash(), (int)ShaderType);
			return false; // 이미 존재하는 셰이더 프로그램이 있음
		}

		SPGroup.ShaderProgramMap.insert(sys_unordered_map<NameHash, D3D11ShaderProgram*>::value_type(ShaderNameHash, pShaderProgram));

		return true;
	}

	D3D11ShaderProgram* D3D11ShaderProgramCache::Query(
		SHADER_TYPE ShaderType,
		const char* szShaderName
	)
	{
		NameHash ShaderNameHash(szShaderName);
		return Query(ShaderType, ShaderNameHash);
	}

	D3D11ShaderProgram* D3D11ShaderProgramCache::Query(
		SHADER_TYPE ShaderType,
		NameHash ShaderNameHash
	)
	{
		ShaderProgramGroup& SPGroup = m_SPGroups[(int)ShaderType];

		D3D11ShaderProgram* pShaderProgram = nullptr;

		{
			AUTO_SPIN_LOCK Guard(&(SPGroup.SpinLock));

			auto it = SPGroup.ShaderProgramMap.find(ShaderNameHash);
			if (SPGroup.ShaderProgramMap.end() == it)
				return nullptr;

			pShaderProgram = it->second;
		}

		pShaderProgram->AddRef(); // 참조 카운트 증가

		return pShaderProgram;
	}
}
