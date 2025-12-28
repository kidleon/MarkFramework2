#include "pch.h"
#include "D3D11RenderContext.h"


D3D11RenderContext::D3D11RenderContext()
{

}

D3D11RenderContext::~D3D11RenderContext() noexcept
{
}

long D3D11RenderContext::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11RenderContext::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11RenderContext);
	}
	return NewRefCnt;
}

long D3D11RenderContext::RefCnt()
{
	return m_RefCnt;
}

void D3D11RenderContext::BeginRender(IRenderCamera* pRenderCamera)
{

}

void D3D11RenderContext::EndRender()
{

}

void D3D11RenderContext::SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial)
{

}

void D3D11RenderContext::SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer)
{

}

void D3D11RenderContext::DrawPrimitive(int32 PrimitiveIndex)
{

}