#include "pch.h"
#include "D3D11Application.h"
#include "RenderSystemFactory.h"


D3D11Application::~D3D11Application() noexcept
{
	OnDestroy();
}

BOOL D3D11Application::OnInit(HWND hWnd, int width, int height)
{
	m_hWnd = hWnd;
	m_Width = width;
	m_Height = height;

	ENGINE_CREATE_DESC CreateDesc = {};
	CreateDesc.hWnd = hWnd;
	CreateDesc.ScreenWidth = width;
	CreateDesc.ScreenHeight = height;
	CreateDesc.Fullscreen = FALSE;
	CreateDesc.RenderAPI = RENDER_API::D3D11;
	BOOL result = CreateAndInitRenderModule(CreateDesc, &m_pRenderSystem);
	if (!result)
	{
		// 실패 처리
		return FALSE;
	}

	RENDERCAMERA_CREATE_DESC CameraDesc = {};
	CameraDesc.CameraMode = CAMERA_MODE::PERSPECTIVE;
	CameraDesc.FOVY = 3.14159265f / 4.0f;
	CameraDesc.AspectRatio = static_cast<FLOAT>(width) / static_cast<FLOAT>(height);
	CameraDesc.NearZ = 0.1f;
	CameraDesc.FarZ = 100.0f;
	CameraDesc.ClearColor = FLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f };
	CameraDesc.ClearFlags = static_cast<UINT32>(CLEAR_BUFFER::ALL);
	CameraDesc.Depth = 1.0f;
	CameraDesc.Stencil = 0;
	CameraDesc.CameraOrder = 0;
	m_pRenderSystem->CreateRenderCamera(CameraDesc, &m_pRenderCamera);

	m_pRenderCamera->SetView(
		FLOAT3{ 0.0f, 0.0f, -5.0f },
		FLOAT3{ 0.0f, 0.0f, 1.0f },
		FLOAT3{ 0.0f, 1.0f, 0.0f }
	);

	PRIMITIVEBUFFER_CREATE_DESC PBDesc = {};
	PBDesc.Usage = BUFFER_USAGE::DEFAULT;
	PBDesc.VertexBufferSize = 256;
	PBDesc.IndexBufferSize = 256;

	if (m_pRenderSystem->CreatePrimitiveBuffer(PBDesc, &m_pPrimitiveBuffer))
	{
		FLOAT3 Vertices[3] = {
			{ -0.5f, -0.5f, 0.0f },
			{ 0.5f, -0.5f, 0.0f },
			{ 0.0f, 0.5f, 0.0f }
		};

		UINT32 Indices[3] = { 0, 1, 2 };

		m_pPrimitiveBuffer->AddPrimitive(PRIMITIVE_TYPE::TRIANGLE_LIST, 3, sizeof(FLOAT3), 3, sizeof(UINT32));

		m_pPrimitiveBuffer->UpdateVertex(
			0,
			Vertices,
			sizeof(FLOAT3) * 3
		);
		m_pPrimitiveBuffer->UpdateIndex(
			0,
			Indices,
			sizeof(UINT32) * 3
		);
	}

	char szCurrentDir[MAX_FILE_LENGTH];
	GetCurrentDirectoryA(MAX_FILE_LENGTH, szCurrentDir);

	fstrcat(szCurrentDir, "/..");

	IFileSystem* pFileSystem = nullptr;
	if (!CreateOSFileSystem(szCurrentDir, &pFileSystem))
	{
		return FALSE;
	}

	IDataStream* pDataStream = pFileSystem->OpenFile("/assets/shader/OnlyColor.hlsl", TRUE);

	if (pDataStream)
	{
		size_t ShaderFileSize = pDataStream->GetSize() + 1;
		char* pShaderSource = (char*)malloc(ShaderFileSize);

		memset(pShaderSource, 0, sizeof(char) * ShaderFileSize);
		pDataStream->Read(pShaderSource, ShaderFileSize);
		pShaderSource[ShaderFileSize - 1] = '\0';

		SHADER_PROGRAM_CREATE_DESC ShaderDesc_VS = {};
		ShaderDesc_VS.ShaderType = SHADER_TYPE::VERTEX;
		ShaderDesc_VS.ShaderName = NameHash("OnlyColor_VS");
		fstrlcpy(ShaderDesc_VS.pEntryPoint, "VS_MAIN", 31);
		fstrlcpy(ShaderDesc_VS.pTargetProfile, "vs_5_0", 31);
		ShaderDesc_VS.pShaderSource = pShaderSource;
		ShaderDesc_VS.ShaderSourceSize = ShaderFileSize;
		m_pRenderSystem->GetOrCreateShaderProgram(ShaderDesc_VS, &m_pShaderProgram_VS);

		SHADER_PROGRAM_CREATE_DESC ShaderDesc_PS = {};
		ShaderDesc_PS.ShaderType = SHADER_TYPE::PIXEL;
		ShaderDesc_PS.ShaderName = NameHash("OnlyColor_PS");
		fstrlcpy(ShaderDesc_PS.pEntryPoint, "PS_MAIN", 31);
		fstrlcpy(ShaderDesc_PS.pTargetProfile, "ps_5_0", 31);
		ShaderDesc_PS.pShaderSource = pShaderSource;
		ShaderDesc_PS.ShaderSourceSize = ShaderFileSize;
		m_pRenderSystem->GetOrCreateShaderProgram(ShaderDesc_PS, &m_pShaderProgram_PS);

		free(pShaderSource);
		pDataStream->Release();
	}

	pFileSystem->Release();

	m_pRenderSystem->CreateSurfaceMaterial(&m_pSurfaceMaterial);
	m_pSurfaceMaterial->AddPass("MainPass");
	m_pSurfaceMaterial->SetVertexShader(m_pShaderProgram_VS);
	m_pSurfaceMaterial->SetPixelShader(m_pShaderProgram_PS);

	RS_RASTERIZER_STATE RasterizerState = GetRS_TwoSide();
	RasterizerState.SetFrontCounterClockwise(false);
	m_pSurfaceMaterial->SetRasterizerState(RasterizerState);
	m_pSurfaceMaterial->SetSampleMask(0xFFFFFFFF);

	RS_DEPTH_STENCIL_STATE DepthStencilState = GetDSS_Default();
	m_pSurfaceMaterial->SetDepthStencilState(DepthStencilState);

	RS_BLEND_STATE BlendState = GetBS_Default();
	m_pSurfaceMaterial->SetBlendState(BlendState);
	m_pSurfaceMaterial->SetBlendFactor(FLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });

	return TRUE;
}

void D3D11Application::OnUpdate()
{
	if (!m_pRenderSystem || !m_pRenderCamera)
		return;

	IRenderContext* pRenderContext = nullptr;
	if (!m_pRenderSystem->GetOrCreateRenderContext(&pRenderContext))
		return;

	pRenderContext->BeginFrame();
	pRenderContext->BeginRenderCamera(m_pRenderCamera);
	
	pRenderContext->SetSurfaceMaterial(m_pSurfaceMaterial);
	pRenderContext->SetPrimitiveBuffer(m_pPrimitiveBuffer);

	LOCAL_TRANSFORM Transform = {};
	Transform.Position = FLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f };
	Transform.Rotation = QUAT{ 0.0f, 0.0f, 0.0f, 1.0f };
	Transform.Scale = FLOAT4{ 1.0f, 1.0f, 1.0f, 0.0f };
	compute_transform(&Transform);

	pRenderContext->DrawPrimitive(Transform, 0);

	pRenderContext->EndRenderCamera();
	pRenderContext->EndFrame();
	
	pRenderContext->Release();
	
	m_pRenderSystem->Update();
}

void D3D11Application::OnDestroy()
{
	CHECK_RELEASE(m_pPrimitiveBuffer);
	CHECK_RELEASE(m_pSurfaceMaterial);
	CHECK_RELEASE(m_pShaderProgram_VS);
	CHECK_RELEASE(m_pShaderProgram_PS);
	CHECK_RELEASE(m_pRenderCamera);
	CHECK_RELEASE(m_pRenderSystem);
	CleanupRenderModule();
}