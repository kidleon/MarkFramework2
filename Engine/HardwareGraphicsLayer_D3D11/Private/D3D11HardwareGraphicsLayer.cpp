#include "pch.h"
#include "D3D11HardwareGraphicsLayer.h"
#include "D3D11RenderDevice.h"
#include "D3D11GPUBuffer.h"
#include "D3D11ShaderProgram.h"


namespace mark
{
	D3D11HardwareGraphicsLayer::~D3D11HardwareGraphicsLayer() noexcept
	{
		Shutdown();
	}

	void D3D11HardwareGraphicsLayer::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void D3D11HardwareGraphicsLayer::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(D3D11HardwareGraphicsLayer, this);
		}
	}

	bool D3D11HardwareGraphicsLayer::Initialize(const RenderSystemCreateDesc& desc)
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

		return true;
	}

	void D3D11HardwareGraphicsLayer::Shutdown()
	{
		if (m_pRenderDevice)
		{
			CORE_DELETE(D3D11RenderDevice, m_pRenderDevice);
			m_pRenderDevice = nullptr;
		}
	}

	IGPUBuffer* D3D11HardwareGraphicsLayer::CreateGPUBuffer(const GPUBufferCreateDesc& desc)
	{
		ID3D11Buffer* pD3D11Buffer = m_pRenderDevice->CreateBuffer(desc);
		if (!pD3D11Buffer)
		{
			SYS_LOG_ERR("D3D11RenderSystem::CreateGPUBuffer - Failed to create D3D11 buffer.");
			return nullptr;
		}

		return CORE_NEW(D3D11GPUBuffer)(pD3D11Buffer);
	}

	IShaderProgram* D3D11HardwareGraphicsLayer::CreateShaderProgram(const ShaderProgramCreateDesc& desc)
	{
		if (!desc.pShaderBytecode || 0 == desc.BytecodeSize)
		{
			SYS_LOG_ERR("D3D11RenderSystem::CreateShaderProgram - Invalid shader bytecode.");
			return nullptr;
		}

		name_hash NameHash(desc.szShaderName);

		D3D11ShaderProgram* pShaderProgram = m_pRenderDevice->CompileShaderProgram(desc);
		if (!pShaderProgram)
			return nullptr;

		return static_cast<IShaderProgram*>(pShaderProgram);
	}

}
