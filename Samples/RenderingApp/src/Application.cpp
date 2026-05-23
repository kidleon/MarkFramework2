#include "pch.h"
#include "Application.h"


using namespace mark;

Application::~Application() noexcept
{
	OnDestroy();
}

BOOL Application::OnInit(HWND hWnd, int width, int height)
{
	m_hWnd = hWnd;
	m_Width = width;
	m_Height = height;

	mark::EngineCreateDesc CreateDesc = {};

	CreateDesc.ScreenWidth = static_cast<uint32_t>(width);
	CreateDesc.ScreenHeight = static_cast<uint32_t>(height);
	CreateDesc.WindowHandle = m_hWnd;
	safe_strcpy(CreateDesc.szAssetRootPath, _MAX_PATH, "../Assets/");

	if (!CreateMark3DEngine(CreateDesc, &m_pMark3D))
		return FALSE;

	IRenderSystem* pRenderSystem = nullptr;
	m_pMark3D->GetRenderSystemInterface(&pRenderSystem);

	GPUBufferCreateDesc BufferDesc = {};
	BufferDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
	BufferDesc.Usage = BUFFER_USAGE::DEFAULT;
	BufferDesc.BufferSize = 256;

	IGPUBuffer* pGPUBuffer = pRenderSystem->CreateGPUBuffer(BufferDesc);

	struct VERTEX
	{
		FLOAT3 Position;
	};

	VERTEX Pos[4] = {
		{ FLOAT3{ -0.5f, -0.5f, 0.0f } },
		{ FLOAT3{  0.5f, -0.5f, 0.0f } },
		{ FLOAT3{  0.5f,  0.5f, 0.0f } },
		{ FLOAT3{ -0.5f,  0.5f, 0.0f } },
	};

	size_t BufferSize = pGPUBuffer->GetBufferSize();
	BUFFER_TYPE BufferType = pGPUBuffer->GetBufferType();
	bool result = pGPUBuffer->UpdateBuffer(nullptr, 0);

	size_t WrittenOffset = 0;
	result = pGPUBuffer->UpdateBuffer(Pos, sizeof(VERTEX) * 4, &WrittenOffset);
	pGPUBuffer->Release();
	pRenderSystem->Release();

	IAssetManager* pAssetManager = nullptr;
	m_pMark3D->GetAssetManagerInterface(&pAssetManager);

	IModelAsset* pModelAsset = static_cast<IModelAsset*>(pAssetManager->LoadAsset(ASSET_TYPE::MODEL, "Model/ogre/m_ogre00.FBX"));

	MESH_DESC MeshDesc = {};

	size_t MeshCount = pModelAsset->GetNumMeshes();
	for (size_t i = 0; i < MeshCount; ++i)
	{
		pModelAsset->GetMeshDesc((uint32_t)i, MeshDesc);
	}

	{
		GPUGeometryCreateDesc GeoCreateDesc = {};
		GeoCreateDesc.pModelAsset = pModelAsset;
		GeoCreateDesc.BufferLayout = GPU_BUFFER_LAYOUT::MERGED;
		GeoCreateDesc.CreateFlags = (uint32_t)GPUGEOMETRY_CREATE_FLAGS::HAS_MODEL_ASSET;
		IGPUGeometry* pGPUGeometry = m_pMark3D->CreateGPUGeometry(GeoCreateDesc);
		pGPUGeometry->Release();
	}

	pModelAsset->Release();
	pAssetManager->Release();

	ISurfaceMaterial* pSurfaceMaterial = m_pMark3D->CreateSurfaceMaterial();
	if (pSurfaceMaterial)
	{
		pSurfaceMaterial->Release();
	}

	return TRUE;
}

void Application::OnUpdate()
{
}

void Application::OnDestroy()
{
	if (m_pMark3D)
	{
		DestroyMark3DEngine(m_pMark3D);
		m_pMark3D = nullptr;
	}
}
