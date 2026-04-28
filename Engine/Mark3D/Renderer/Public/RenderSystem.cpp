#include "pch.h"
#include "RenderSystem.h"


namespace mark
{
	RenderSystem::RenderSystem()
		: m_Renderer(nullptr)
	{
	}

	RenderSystem::~RenderSystem() noexcept
	{

	}

	bool RenderSystem::Initialize(const RENDERER_CREATE_DESC& CreateDesc)
	{
		// Initialization code here
		return true;
	}

	void RenderSystem::Shutdown()
	{
		// Shutdown code here
	}

	ResourceHandle RenderSystem::CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& CreateDesc)
	{
		return ResourceHandle{ 0 };
	}
	
}
