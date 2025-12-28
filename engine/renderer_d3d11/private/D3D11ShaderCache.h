#ifndef __D3D11_SHADER_CACHE_H__
#define __D3D11_SHADER_CACHE_H__


class D3D11Shader;

class D3D11ShaderCache
{
public:
	D3D11ShaderCache();
	~D3D11ShaderCache() noexcept;

	void Init();
	void Add(NameHash Name, D3D11Shader* pShader);
	D3D11Shader* Get(NameHash Name);

private:
	HASH_TABLE* m_pCacheTable;

};


#endif // __D3D11_SHADER_CACHE_H__