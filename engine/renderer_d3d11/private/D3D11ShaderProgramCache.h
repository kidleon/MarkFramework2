#pragma once


class D3D11ShaderProgram;

class D3D11ShaderProgramCache
{
	static D3D11ShaderProgramCache* s_pInstance;

public:
	D3D11ShaderProgramCache();
	~D3D11ShaderProgramCache() noexcept;

	BOOL Init();
	void Shutdown();

	void Register(D3D11ShaderProgram* pShaderProgram) noexcept;
	D3D11ShaderProgram* Find(SHADER_TYPE ShaderType, NameHash ShaderName, UINT32 ShaderDefinesHash) noexcept;

	static inline D3D11ShaderProgramCache* Get() noexcept { return s_pInstance; }

private:
	void DestroyHashTable(HASH_TABLE* pHashTable) noexcept;

private:
	HASH_TABLE* m_pHashTable_VS = nullptr;
	HASH_TABLE* m_pHashTable_PS = nullptr;
	HASH_TABLE* m_pHashTable_GS = nullptr;
	HASH_TABLE* m_pHashTable_CS = nullptr;

};

