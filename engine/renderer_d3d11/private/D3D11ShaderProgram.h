#ifndef __D3D11_SHADER_PROGRAM_H__
#define __D3D11_SHADER_PROGRAM_H__


class D3D11RenderDevice;
struct D3D11VertexShader;
struct D3D11PixelShader;

class D3D11ShaderProgram : public IShaderProgram
{
public:
	D3D11ShaderProgram() = default;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT32 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IShaderProgram interface
	virtual int32 GetBindIndexByName(const NameHash& Name) const override;


	/*
	virtual void SetConstant(const NameHash& Name, const void* pData, uint32 DataSize) override;
	virtual void SetConstant(int32 BindIndex, const void* pData, uint32 DataSize) override;

	virtual void SetTexture1D(const NameHash& Name, ITexture1D* pTexture) override;
	virtual void SetTexture1D(int32 BindIndex, ITexture1D* pTexture) override;

	virtual void SetTexture2D(const NameHash& Name, ITexture2D* pTexture) override;
	virtual void SetTexture2D(int32 BindIndex, ITexture2D* pTexture) override;
	
	__FORCEINLINE void INL_SetShader(D3D11VertexShader* pVS) noexcept
	{
		if (m_pVS)
		{
			m_pVS->Release();
			m_pVS = nullptr;
		}

		if (m_pPS)
		{
			m_pPS->Release();
			m_pPS = nullptr;
		}

		m_ShaderType = SHADER_TYPE::UNKNOWN;
		
		if (pVS)
		{
			m_ShaderType = SHADER_TYPE::VERTEX;
			pVS->AddRef();
		}
			
		m_pVS = pVS;
		interlock_store_l((long*)&m_LoadStat, (long)LOAD_STAT::LOADED, MEMORY_ORDER_RELAXED);
	}

	__FORCEINLINE void INL_SetShader(D3D11PixelShader* pPS) noexcept
	{
		if (m_pVS)
		{
			m_pVS->Release();
			m_pVS = nullptr;
		}

		if (m_pPS)
		{
			m_pPS->Release();
			m_pPS = nullptr;
		}

		m_ShaderType = SHADER_TYPE::UNKNOWN;

		if (pPS)
		{
			m_ShaderType = SHADER_TYPE::PIXEL;
			pPS->AddRef();
		}

		m_pPS = pPS;
		interlock_store_l((long*)&m_LoadStat, (long)LOAD_STAT::LOADED, MEMORY_ORDER_RELAXED);
	}
	*/
protected:
	virtual ~D3D11ShaderProgram() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	/*
	SHADER_TYPE m_ShaderType = SHADER_TYPE::UNKNOWN;

	union
	{
		D3D11VertexShader* m_pVS = nullptr;
		D3D11PixelShader* m_pPS;
	};

	*/
};


#endif // __D3D11_SHADER_PROGRAM_H__
