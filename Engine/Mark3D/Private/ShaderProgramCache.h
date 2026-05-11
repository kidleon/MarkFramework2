#pragma once


namespace mark
{
	class ShaderProgramCache
	{
		struct ShaderProgramGroup
		{
			sys_unordered_map<NameHash, IShaderProgram*> ShaderProgramMap;
			spin_lock_t SpinLock;
		};

	public:
		ShaderProgramCache();
		~ShaderProgramCache() noexcept;

		bool Register(IShaderProgram* pShaderProgram);

		[[nodiscard]] IShaderProgram* Query(
			SHADER_TYPE ShaderType,
			const char* szShaderName
		);

		[[nodiscard]] IShaderProgram* Query(
			SHADER_TYPE ShaderType,
			NameHash ShaderNameHash
		);

	private:
		ShaderProgramGroup m_SPGroups[(int)SHADER_TYPE::EMAX]; // 셰이더 타입별로 그룹화하여 관리

	};
}
