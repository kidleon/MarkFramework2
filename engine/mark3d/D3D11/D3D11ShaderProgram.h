#ifndef __D3D11_SHADER_PROGRAM_H__
#define __D3D11_SHADER_PROGRAM_H__

#include "IShaderProgram.h"
#include "RenderDef.h"


class D3D11RenderDevice;
struct D3D11VertexShader;
struct D3D11PixelShader;

class D3D11ShaderProgram : public IShaderProgram
{
public:
	D3D11ShaderProgram() = default;

	virtual int32 GetBindIndexByName(const NameHash& Name) const override;

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

protected:
	virtual ~D3D11ShaderProgram() noexcept;
	virtual void OnDestroy() override;

private:
	SHADER_TYPE m_ShaderType = SHADER_TYPE::UNKNOWN;

	union
	{
		D3D11VertexShader* m_pVS = nullptr;
		D3D11PixelShader* m_pPS;
	};

	
};


#endif // __D3D11_SHADER_PROGRAM_H__
