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

		/**
		 * @brief 셰이더 프로그램을 캐시에 등록합니다.
		 * @param pShaderProgram [Borrowed] 호출자의 ref는 유지됩니다. 캐시는 내부적으로 AddRef하여
		 *                       독립적인 owning ref를 보유합니다. 등록 후에도 호출자는 자기 ref를 유지/Release 합니다.
		 * @return 등록 성공 시 true. 동일 이름 해시가 이미 존재하면 false (이중 등록 차단).
		 */
		bool Register(IShaderProgram* pShaderProgram);

		/**
		 * @brief 셰이더 이름으로 캐시를 조회합니다.
		 * @return [Owning] 반환된 포인터는 AddRef된 상태입니다. 호출자가 Release()를 책임지거나
		 *         unknown_ptr<IShaderProgram>::attach()로 소유권을 넘겨받으세요. 없으면 nullptr.
		 */
		[[nodiscard]] IShaderProgram* Query(
			SHADER_TYPE ShaderType,
			const char* szShaderName
		);

		/**
		 * @brief 셰이더 이름 해시로 캐시를 조회합니다.
		 * @return [Owning] 반환된 포인터는 AddRef된 상태입니다. 호출자가 Release()를 책임지거나
		 *         unknown_ptr<IShaderProgram>::attach()로 소유권을 넘겨받으세요. 없으면 nullptr.
		 */
		[[nodiscard]] IShaderProgram* Query(
			SHADER_TYPE ShaderType,
			NameHash ShaderNameHash
		);

	private:
		ShaderProgramGroup m_SPGroups[(int)SHADER_TYPE::EMAX]; // 셰이더 타입별로 그룹화하여 관리

	};
}
