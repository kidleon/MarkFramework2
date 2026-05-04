#pragma once


namespace mark
{
	class D3D11RenderDevice;
	class D3D11ShaderProgram;

	class D3D11ShaderProgramCache
	{
	public:
		D3D11ShaderProgramCache() = default;
		~D3D11ShaderProgramCache() noexcept;

		bool Register(D3D11ShaderProgram* pShaderProgram);

		[[nodiscard]] D3D11ShaderProgram* Query(
			SHADER_TYPE ShaderType,
			const char* szShaderName
		);

	private:
		sys_unordered_map<name_hash, D3D11ShaderProgram*> m_ShaderProgramMap[(int)SHADER_TYPE::EMAX];
		D3D11RenderDevice* m_pRenderDevice = nullptr;
	};
}
