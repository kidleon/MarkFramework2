#include "pch.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderCamera.h"
#include "D3D11SurfaceMaterial.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderManager.h"


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
		D3D11_DELETE(this, D3D11RenderContext);
	}
	return NewRefCnt;
}

long D3D11RenderContext::RefCnt()
{
	return m_RefCnt;
}

void D3D11RenderContext::BeginRender(IRenderCamera* pRenderCamera)
{
	if (!pRenderCamera)
		return;

	CHECK_RELEASE(m_pSetRenderCamera);

	m_pSetRenderCamera = static_cast<D3D11RenderCamera*>(pRenderCamera);
	m_pSetRenderCamera->AddRef();

	// RENDER_QUEUE 초기화
	D3D11RenderQueue* pRQ = D3D11RenderManager::Get()->PrepareRQ();
	if (!pRQ)
	{
		SYS_LOG_E("D3D11RenderContext::BeginRender: Failed to get D3D11RenderQueue from pool");
		return;
	}

	m_pSetRQ = pRQ;
	m_pSetRQ->INL_SetRenderCamera(m_pSetRenderCamera);
}

void D3D11RenderContext::EndRender()
{
	if(!m_pSetRQ) return;

	D3D11RenderManager::Get()->PostRQ(m_pSetRQ);
	m_pSetRQ = nullptr;
	CHECK_RELEASE(m_pSetPrimitiveBuffer);
	CHECK_RELEASE(m_pSetSurfaceMaterial);
}

void D3D11RenderContext::SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial)
{
	if (!pSurfaceMaterial)
	{
		CHECK_RELEASE(m_pSetPrimitiveBuffer);
		return;
	}

	m_pSetSurfaceMaterial = static_cast<D3D11SurfaceMaterial*>(pSurfaceMaterial);
	m_pSetSurfaceMaterial->AddRef();
}

void D3D11RenderContext::SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer)
{
	if (!pPrimitiveBuffer)
	{
		CHECK_RELEASE(m_pSetPrimitiveBuffer);
		return;
	}
	
	m_pSetPrimitiveBuffer = static_cast<D3D11PrimitiveBuffer*>(pPrimitiveBuffer);
	m_pSetPrimitiveBuffer->AddRef();
}

void D3D11RenderContext::DrawPrimitive(int32 PrimitiveIndex)
{
	// Set D3D11RenderPipelineState
	
	// Set D3D11DynamicRenderPipelineState

	// Set PrimitiveBuffer & Index

	// m_pSetRQ->Add();
}