#include "pch.h"
#include "D3D11RenderSystem.h"
#include "D3D11RenderDevice.h"


namespace mark
{
	D3D11RenderSystem::~D3D11RenderSystem() noexcept
	{
		Shutdown();
	}

	void D3D11RenderSystem::AddRef()
	{
		m_RefCount++;
	}

	void D3D11RenderSystem::Release()
	{
		if (--m_RefCount == 0)
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
				nullptr, // TODO: 윈도우 핸들
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

	PrimitiveBufferHandle D3D11RenderSystem::CreatePrimitiveBuffer(const PrimitiveBufferCreateDesc& desc)
	{
		return 0;
	}

}
