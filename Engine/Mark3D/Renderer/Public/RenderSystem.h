#pragma once


namespace mark
{
	class MARKENGINE_API RenderSystem
	{
	public:
		RenderSystem();
		virtual ~RenderSystem() noexcept;

		bool Initialize(const RENDERER_CREATE_DESC& CreateDesc);
		void Shutdown();

		ResourceHandle CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& CreateDesc);

	private:
		IRenderer* m_Renderer; // 렌더러 인터페이스 포인터

	};
}
