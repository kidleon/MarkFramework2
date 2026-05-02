#include "pch.h"
#include "D3D11RenderSystem.h"
#include "D3D11RenderDevice.h"
#include "D3D11GPUBuffer.h"


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
				false // DebugDevice
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

	void D3D11RenderSystem::Shutdown()
	{
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
}
