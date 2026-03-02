#include "pch.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11RenderCommand.h"
#include "D3D11ResourceCommand.h"
#include "D3D11RenderFrame.h"
#include "D3D11SurfaceMaterial.h"
#include "D3D11PrimitiveBuffer.h"
#include "stack_pool.h"


D3D11RenderContext::~D3D11RenderContext() noexcept
{
	Destroy();
}

void D3D11RenderContext::Init()
{
	for (int32 i = 0; i < MAX_RENDER_FRAME; ++i)
		m_RenderFrames[i].Init();
}

void D3D11RenderContext::Destroy()
{
	for (int32 i = 0; i < MAX_RENDER_FRAME; ++i)
		m_RenderFrames[i].Destroy();
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

void D3D11RenderContext::BeginFrame() noexcept
{
	m_CurrentFrameIndex = (m_LastFrameIndex + 1) % MAX_RENDER_FRAME;
	m_RenderFrames[m_CurrentFrameIndex].Reset();
}

void D3D11RenderContext::EndFrame() noexcept
{
	// 현재 프레임의 렌더 큐들을 렌더 커맨드 실행기로 전달
	D3D11RenderCommandExecutor::Get().Push(&m_RenderFrames[m_CurrentFrameIndex]);
	m_LastFrameIndex = m_CurrentFrameIndex;
	m_CurrentFrameIndex = -1;
}

static inline int32 FindRQ(
	const D3D11_RENDER_QUEUE_GROUP* pRQs,
	size_t NumRQs,
	IRenderCamera* pRenderCamera
) noexcept
{
	for (size_t i = 0; i < NumRQs; ++i)
	{
		if (pRQs[i].INL_GetRenderCamera() == pRenderCamera)
			return static_cast<int32>(i);
	}

	return -1;
}

void D3D11RenderContext::BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept
{
	__ASSERT(0 <= m_CurrentFrameIndex, "Invalid render frame index.");

	if (!pRenderCamera)
	{
		SYS_LOG_E("D3D11RenderContext::BeginRenderCamera: pRenderCamera is nullptr.");
		m_pCurRQs = nullptr;
		return;
	}

	// 현재 프레임의 렌더 큐에서 해당 렌더 카메라에 대한 렌더 큐가 있는지 검색
	int32 find_index = FindRQ(
		m_RenderFrames[m_CurrentFrameIndex].RQs, 
		m_RenderFrames[m_CurrentFrameIndex].NumRQs, 
		pRenderCamera
	);

	D3D11RenderCamera* pD3D11RenderCamera = static_cast<D3D11RenderCamera*>(pRenderCamera);
	pD3D11RenderCamera->ComputeCamera(); // 카메라 행렬 계산

	// 있으면 해당 렌더 큐를 사용, 없으면 새로 생성
	if (-1 != find_index)
	{
		m_pCurRQs = &m_RenderFrames[m_CurrentFrameIndex].RQs[find_index];
	}
	else
	{
		if (m_RenderFrames[m_CurrentFrameIndex].NumRQs >= D3D11_RENDER_FRAME::MAX_RQ_GROUPS)
		{
			SYS_LOG_E("D3D11RenderContext::BeginRenderCamera: Exceeded maximum render queue groups per frame.");
			m_pCurRQs = nullptr;
			return;
		}

		++m_RenderFrames[m_CurrentFrameIndex].NumRQs;
		int32 RQ_Index = static_cast<int32>(m_RenderFrames[m_CurrentFrameIndex].NumRQs - 1);
		m_pCurRQs = &m_RenderFrames[m_CurrentFrameIndex].RQs[RQ_Index];
	}

	m_pCurRQs->PrepareRQ(static_cast<D3D11RenderCamera*>(pRenderCamera));
	m_pCurRenderCamera = static_cast<D3D11RenderCamera*>(pRenderCamera);
	m_pCurRenderCamera->AddRef();
}

void D3D11RenderContext::EndRenderCamera() noexcept
{
	m_pCurRQs = nullptr;
	
	CHECK_RELEASE(m_pCurPrimitiveBuffer);
	CHECK_RELEASE(m_pCurSurfaceMaterial);
	CHECK_RELEASE(m_pCurRenderCamera);
}

void D3D11RenderContext::SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial)
{
	CHECK_RELEASE(m_pCurSurfaceMaterial);

	if (pSurfaceMaterial)
	{
		m_pCurSurfaceMaterial = static_cast<D3D11SurfaceMaterial*>(pSurfaceMaterial);
		m_pCurSurfaceMaterial->AddRef();
	}
}

void D3D11RenderContext::SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer)
{
	CHECK_RELEASE(m_pCurPrimitiveBuffer);

	if (pPrimitiveBuffer)
	{
		m_pCurPrimitiveBuffer = static_cast<D3D11PrimitiveBuffer*>(pPrimitiveBuffer);
		m_pCurPrimitiveBuffer->AddRef();

		if (m_pCurPrimitiveBuffer->INL_IsDirtyBuffer())
		{
			D3D11_RESOURCE_COMMAND* pResCmd = (D3D11_RESOURCE_COMMAND*)stackpool_alloc(
				m_RenderFrames[m_CurrentFrameIndex].CommandStackPool, 
				sizeof(D3D11_RESOURCE_COMMAND)
			);

			memset(pResCmd, 0, sizeof(D3D11_RESOURCE_COMMAND));
			pResCmd->LinkNode.data = pResCmd;
			pResCmd->CommandFunc = D3D11_RESOURCE_COMMAND::COMMAND_FUNC::UPDATE_PRIMITIVE_BUFFER;
			pResCmd->pPrimitiveBuffer = m_pCurPrimitiveBuffer;
			m_pCurPrimitiveBuffer->AddRef();

			linked_list_push_back(
				&m_RenderFrames[m_CurrentFrameIndex].ResourceCommandQueue,
				&pResCmd->LinkNode
			);
		}
	}
}

void D3D11RenderContext::DrawPrimitive(const LOCAL_TRANSFORM& Transform, int32 PrimitiveIndex)
{
	if (!m_pCurSurfaceMaterial || !m_pCurPrimitiveBuffer || !m_pCurRQs)
		return;

	int32 NumPass = m_pCurSurfaceMaterial->INL_GetNumPass();

	if (0 >= NumPass)
		return;

	FLOAT3 WorldPos = { 
		Transform.TM.m30, 
		Transform.TM.m31,
		Transform.TM.m32
	};

	FLOAT3 CameraPos = m_pCurRenderCamera->INL_GetEyePos();
	FLOAT DepthFarZ = m_pCurRenderCamera->INL_GetDepthFarZ();

	FLOAT3 CameraDist = vec3_sub(CameraPos, WorldPos);
	FLOAT3 DistLen = vec3_length(CameraDist);
	FLOAT DepthValueF = (DistLen.x / DepthFarZ) * ((1 << 8) - 1); // 0 ~ 255
	UINT16 DepthValue = static_cast<UINT16>(DepthValueF);

	const MATRIX4& ViewProjMatrix = m_pCurRenderCamera->INL_GetViewProjectionMatrix();

	for (int32 p = 0; p < NumPass; ++p)
	{
		D3D11_DRAW_COMMAND* pDrawCommand = (D3D11_DRAW_COMMAND*)stackpool_alloc(
			m_RenderFrames[m_CurrentFrameIndex].CommandStackPool, 
			sizeof(D3D11_DRAW_COMMAND)
		);

		if (!pDrawCommand)
		{
			SYS_LOG_E("D3D11RenderContext::DrawPrimitive: Failed to allocate draw command.");
			continue;
		}

		memset(pDrawCommand, 0, sizeof(D3D11_DRAW_COMMAND));

		m_pCurPrimitiveBuffer->AddRef();
		pDrawCommand->pPrimitiveBuffer = m_pCurPrimitiveBuffer;
		pDrawCommand->DrawPrimitiveIndex = PrimitiveIndex;

		pDrawCommand->ObjectConstant.World = mat4_transpose(Transform.TM);
		pDrawCommand->ObjectConstant.WorldViewProj = mat4_transpose(mat4_mul(Transform.TM, ViewProjMatrix)); // 월드-뷰-투영 행렬 계산
		pDrawCommand->ObjectConstant.Color = m_pCurSurfaceMaterial->INL_GetColor(p);
		//pDrawCommand->ObjectConstant.WorldViewProjection = mat4_transpose(&WorldViewProjTM);

		pDrawCommand->RenderPipeline.pVertexShader = m_pCurSurfaceMaterial->INL_GetVertexShader(p);
		pDrawCommand->RenderPipeline.pPixelShader = m_pCurSurfaceMaterial->INL_GetPixelShader(p);
		pDrawCommand->RenderPipeline.pRasterizerState = m_pCurSurfaceMaterial->INL_GetRasterizerState(p);
		pDrawCommand->RenderPipeline.pBlendState = m_pCurSurfaceMaterial->INL_GetBlendState(p);
		pDrawCommand->RenderPipeline.pDepthStencilState = m_pCurSurfaceMaterial->INL_GetDepthStencilState(p);
		pDrawCommand->RenderPipeline.Color = m_pCurSurfaceMaterial->INL_GetColor(p);

		pDrawCommand->DynamicRenderPipeline.BlendFactor = m_pCurSurfaceMaterial->INL_GetBlendFactor(p);
		pDrawCommand->DynamicRenderPipeline.SampleMask = m_pCurSurfaceMaterial->INL_GetSampleMask(p);
		pDrawCommand->DynamicRenderPipeline.StencilRef = m_pCurSurfaceMaterial->INL_GetStencilRef(p);

		// 정렬 키 설정
		pDrawCommand->SortKey.Value = 0;

		pDrawCommand->SortKey.Pass = p;

		// 인덱스 설정등을 한다...................
		UINT64 VS_Index = reinterpret_cast<UINT64>(pDrawCommand->RenderPipeline.pVertexShader);
		pDrawCommand->SortKey.VertexShaderIndex = m_RenderFrames[m_CurrentFrameIndex].RenderSortIndexer.GetVertexShaderIndex(
			m_RenderFrames[m_CurrentFrameIndex].IndexerStackPool, 
			VS_Index
		);

		UINT64 PS_Index = reinterpret_cast<UINT64>(pDrawCommand->RenderPipeline.pPixelShader);
		pDrawCommand->SortKey.PixelShaderIndex = m_RenderFrames[m_CurrentFrameIndex].RenderSortIndexer.GetPixelShaderIndex(
			m_RenderFrames[m_CurrentFrameIndex].IndexerStackPool, 
			PS_Index
		);

		UINT64 TextureID = 0; // TODO: 텍스처 ID 해시 계산

		UINT64 BlendStateHash = m_pCurSurfaceMaterial->INL_GetBlendStateHash(p);
		UINT64 RasterizerStateHash = m_pCurSurfaceMaterial->INL_GetRasterizerStateHash(p);
		UINT64 DepthStencilStateHash = m_pCurSurfaceMaterial->INL_GetDepthStencilStateHash(p);

		pDrawCommand->SortKey.StateIndex = m_RenderFrames[m_CurrentFrameIndex].RenderSortIndexer.GetRenderStateIndex(
			m_RenderFrames[m_CurrentFrameIndex].IndexerStackPool,
			BlendStateHash,
			RasterizerStateHash,
			DepthStencilStateHash
		);
			
		pDrawCommand->SortKey.Depth = DepthValue;


		// 일단 임시로 불투명 렌더 큐에 추가
		D3D11_RENDER_QUEUE* pRQ = m_pCurRQs->INL_GetOpaqueRQ();
		pRQ->Add(RENDER_QUEUE_TYPE::RQ_OPAQUE, pDrawCommand);
	}
}