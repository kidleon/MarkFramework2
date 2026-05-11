#include "pch.h"
#include "ShaderProgramCache.h"


namespace mark
{
	ShaderProgramCache::ShaderProgramCache()
	{
		for (int i = 0; i < (int)SHADER_TYPE::EMAX; ++i)
		{
			ShaderProgramGroup& SPGroup = m_SPGroups[i];
			init_spin_lock(&SPGroup.SpinLock);
		}
	}

	ShaderProgramCache::~ShaderProgramCache() noexcept
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

	bool ShaderProgramCache::Register(IShaderProgram* pShaderProgram)
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
		
		SPGroup.ShaderProgramMap.insert(sys_unordered_map<NameHash, IShaderProgram*>::value_type(ShaderNameHash, pShaderProgram));

		return true;
	}

	IShaderProgram* ShaderProgramCache::Query(
		SHADER_TYPE ShaderType,
		const char* szShaderName
	)
	{
		NameHash ShaderNameHash(szShaderName);
		return Query(ShaderType, ShaderNameHash);
	}

	IShaderProgram* ShaderProgramCache::Query(
		SHADER_TYPE ShaderType,
		NameHash ShaderNameHash
	)
	{
		ShaderProgramGroup& SPGroup = m_SPGroups[(int)ShaderType];

		IShaderProgram* pShaderProgram = nullptr;

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
