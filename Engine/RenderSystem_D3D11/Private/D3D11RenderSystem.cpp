#include "pch.h"
#include "D3D11RenderSystem.h"
#include "D3D11RenderDevice.h"
#include "D3D11GPUBuffer.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgramCache.h"


namespace mark
{
	D3D11RenderSystem::~D3D11RenderSystem() noexcept
	{
		Shutdown();
	}

	void D3D11RenderSystem::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void D3D11RenderSystem::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(D3D11RenderSystem, this);
		}
	}

	bool D3D11RenderSystem::Initialize(const RenderSystemCreateDesc& desc)
	{
		if (!m_pRenderDevice)
		{
			m_pRenderDevice = CORE_NEW(D3D11RenderDevice);
			if (!m_pRenderDevice->CreateDevice(
				desc.WindowHandle, // TODO: 윈도우 핸들
				desc.ScreenWidth,
				desc.ScreenHeight,
				desc.DebugMode // DebugDevice
			))
			{
				SYS_LOG_ERR("D3D11RenderSystem::Initialize - Failed to create D3D11 device.");

				CORE_DELETE(D3D11RenderDevice, m_pRenderDevice);
				m_pRenderDevice = nullptr;

				return false;
			}
		}

		if (!m_pShaderProgramCache)
		{
			m_pShaderProgramCache = CORE_NEW(D3D11ShaderProgramCache);
		}

		return true;
	}

	void D3D11RenderSystem::Shutdown()
	{
		CORE_DELETE(D3D11ShaderProgramCache, m_pShaderProgramCache);

		if (m_pRenderDevice)
		{
			CORE_DELETE(D3D11RenderDevice, m_pRenderDevice);
			m_pRenderDevice = nullptr;
		}
	}

	IGPUBuffer* D3D11RenderSystem::CreateGPUBuffer(const GPUBufferCreateDesc& desc)
	{
		ID3D11Buffer* pD3D11Buffer = m_pRenderDevice->CreateBuffer(desc);
		if (!pD3D11Buffer)
		{
			SYS_LOG_ERR("D3D11RenderSystem::CreateGPUBuffer - Failed to create D3D11 buffer.");
			return nullptr;
		}

		return CORE_NEW(D3D11GPUBuffer)(pD3D11Buffer);
	}

	IShaderProgram* D3D11RenderSystem::CreateShaderProgram(const ShaderProgramCreateDesc& desc)
	{
		name_hash NameHash(desc.szShaderName);

		D3D11ShaderProgram* pShaderProgram = m_pShaderProgramCache->Query(desc.ShaderType, desc.szShaderName);
		if (pShaderProgram)
		{
			return static_cast<IShaderProgram*>(pShaderProgram);
		}

		if (!desc.pShaderBytecode || 0 == desc.BytecodeSize)
		{
			SYS_LOG_ERR("D3D11RenderSystem::CreateShaderProgram - Invalid shader bytecode.");
			return nullptr;
		}

		pShaderProgram = m_pRenderDevice->CompileShaderProgram(desc);
		if (!pShaderProgram)
			return nullptr;

		if (!m_pShaderProgramCache->Register(pShaderProgram))
		{
			SYS_LOG_ERR("D3D11RenderSystem::CreateShaderProgram - Failed to register shader program in cache.");
			pShaderProgram->Release();
			return nullptr;
		}

		return static_cast<IShaderProgram*>(pShaderProgram);
	}

	IShaderProgram* D3D11RenderSystem::GetShaderProgram(
		SHADER_TYPE ShaderType,
		const char* szShaderName
	)
	{
		name_hash NameHash(szShaderName);

		D3D11ShaderProgram* pShaderProgram = m_pShaderProgramCache->Query(ShaderType, szShaderName);
		if (!pShaderProgram)
		{
			SYS_LOG_ERR_F("D3D11RenderSystem::GetShaderProgram - Shader program not found: {}", szShaderName);
			return nullptr;
		}

		return static_cast<IShaderProgram*>(pShaderProgram);
	}
}
