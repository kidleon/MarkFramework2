#pragma once


namespace mark
{
	class D3D11ShaderProgram final : public IShaderProgram
	{
		constexpr static size_t MAX_SHADER_NAME = 64;

	public:
		explicit D3D11ShaderProgram(const char* szShaderName, ID3D11VertexShader* pVertexShader);
		explicit D3D11ShaderProgram(const char* szShaderName, ID3D11PixelShader* pPixelShader);
		explicit D3D11ShaderProgram(const char* szShaderName, ID3D11ComputeShader* pComputeShader);

		void AddRef();
		void Release();

		SHADER_TYPE GetShaderType() const noexcept;

#if defined(_DEBUG)
		inline const char* INL_GetShaderName() const noexcept { return m_szShaderName; }
#endif // #if defined(_DEBUG)

		inline name_hash INL_GetNameHash() const noexcept { return m_HashName; }
		inline SHADER_TYPE INL_GetShaderType() const noexcept { return m_ShaderType; }
		inline ID3D11VertexShader* INL_GetVertexShader() const noexcept { return m_pVertexShader; }
		inline ID3D11PixelShader* INL_GetPixelShader() const noexcept { return m_pPixelShader; }
		inline ID3D11ComputeShader* INL_GetComputeShader() const noexcept { return m_pComputeShader; }

	private:
		D3D11ShaderProgram() = delete;
		virtual ~D3D11ShaderProgram() noexcept;

	private:
		std::atomic<uint32_t> m_RefCount = { 1 };
		const SHADER_TYPE m_ShaderType;
		const name_hash m_HashName;

#if defined(_DEBUG)
		char m_szShaderName[MAX_SHADER_NAME] = {};
#endif // #if defined(_DEBUG)

		union
		{
			ID3D11DeviceChild* m_pShader = nullptr;
			ID3D11VertexShader* m_pVertexShader;
			ID3D11PixelShader* m_pPixelShader;
			ID3D11ComputeShader* m_pComputeShader;
		};
	};
}
