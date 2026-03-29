#include "pch.h"
#include "Assets.h"
#include "thread_pool.h"
#include "unicode.h"
#include "strings.h"
#include "idgen.h"
#include "Log.h"
#include "temp_pool.h"

#include "TextAsset.h"
#include "BinaryAsset.h"
#include "ModelAsset.h"

#include "TextureLoader.h"
#include "TextAssetLoader.h"
#include "BinaryAssetLoader.h"
#include "FBXModelLoader.h"
#include "ModelLoader.h"
#include "AsyncAssetOp.h"


constexpr static UINT32 MIN_ID_COUNT = 1;
constexpr static UINT32 MAX_ID_COUNT = 200000;
constexpr static size_t THREAD_POOL_SIZE = 4;

HANDLE Assets::ID_GEN_HANDLE = nullptr;
Assets* Assets::s_pInstance = nullptr;

Assets* Assets::Get() noexcept
{
	return s_pInstance;
}

Assets::Assets()
	: m_pFileSystem(nullptr)
	, m_hThreadPool(nullptr)
	, m_hIDGen(nullptr)
	, m_Initialized(FALSE)
{
	if (!s_pInstance)
		s_pInstance = this;
}

Assets::~Assets() noexcept
{
	Shutdown();

	if (s_pInstance == this)
		s_pInstance = nullptr;
}

long Assets::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Assets::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, Assets);
	}
	return NewRefCnt;
}

long Assets::RefCnt()
{
	return m_RefCnt;
}

BOOL Assets::Init(
	IRenderSystem* pRenderSystem,
	const char* szRootPath
)
{
	if (m_Initialized) return TRUE;

	constexpr size_t TEMP_POOL_SIZE = 1024 * 1024 * 20; // 20MB 크기의 임시 풀 생성

	CreateOSFileSystem(szRootPath, &m_pFileSystem);
	m_hThreadPool = threadpool_create(THREAD_POOL_SIZE, TEMP_POOL_SIZE);
	m_hIDGen = idgen_create(MIN_ID_COUNT, MAX_ID_COUNT);
	Assets::ID_GEN_HANDLE = m_hIDGen;

	m_hSyncLoadTempPool = temppool_create(TEMP_POOL_SIZE, FALSE); // 20MB 크기의 동기 로드용 임시 풀 생성

	m_Initialized = TRUE;

	fstrlcpy(m_szTexturePath[0], "assets/common/texture", sizeof(m_szTexturePath[0]));
	fstrlcpy(m_szTexturePath[1], "assets/model/texture", sizeof(m_szTexturePath[1]));

	m_pRenderSystem = pRenderSystem;

	return TRUE;
}

void Assets::Shutdown()
{
	if (!m_Initialized) return;

	if (m_hThreadPool)
	{
		threadpool_destroy(m_hThreadPool);
		m_hThreadPool = nullptr;
	}

	if (m_hSyncLoadTempPool)
	{
		temppool_destroy(m_hSyncLoadTempPool);
		m_hSyncLoadTempPool = nullptr;
	}

	if (m_hIDGen)
	{
		idgen_destroy(m_hIDGen);
		m_hIDGen = nullptr;
		Assets::ID_GEN_HANDLE = nullptr;
	}

	if (m_pFileSystem)
	{
		m_pFileSystem->Release();
		m_pFileSystem = nullptr;
	}

	m_Initialized = FALSE;
}															

BOOL Assets::Load(const char* szRelativePath, ITextAsset** ppOut)
{
	if (!szRelativePath || !(*ppOut)) return FALSE; 

	TextAsset* pTextAsset = CORE_NEW(TextAsset)(idgen_getid(m_hIDGen), szRelativePath);

	BOOL result = LoadTextAssetFromFileSystem(
		m_pFileSystem,
		szRelativePath,
		pTextAsset
	);

	if (!result)
	{
		pTextAsset->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pTextAsset;
	
	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, ITextAsset** ppOut)
{
	if (!szRelativePath || !(*ppOut)) return FALSE;

	TextAsset* pTextAsset = CORE_NEW(TextAsset)(idgen_getid(m_hIDGen), szRelativePath);
	(*ppOut) = pTextAsset;

	AsyncAssetOp* pArg = (AsyncAssetOp*)CORE_POOL_ALLOC(sizeof(AsyncAssetOp));
	fstrlcpy(pArg->szRelativePath, szRelativePath, sizeof(pArg->szRelativePath));

	pArg->pFileSystem = m_pFileSystem;
	pArg->pAsset = pTextAsset;

	pTextAsset->AddRef(); // 비동기 작업에서 해제할 것이므로 참조 카운트 증가

	threadpool_add_task_arg(
		m_hThreadPool,
		AsyncLoadTextAssetFromFileSystem,
		(void*)pArg
	);

	return TRUE;
}

BOOL Assets::Load(const char* szRelativePath, IBinaryAsset** ppOut)
{
	if (!szRelativePath || !(*ppOut)) return FALSE;

	BinaryAsset* pBinaryAsset = CORE_NEW(BinaryAsset)(idgen_getid(m_hIDGen), szRelativePath);

	BOOL result = LoadBinaryAssetFromFileSystem(
		m_pFileSystem,
		szRelativePath,
		pBinaryAsset
	);

	if (!result)
	{
		pBinaryAsset->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pBinaryAsset;

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut)
{
	if (!szRelativePath || !(*ppOut)) return FALSE;

	BinaryAsset* pBinaryAsset = CORE_NEW(BinaryAsset)(idgen_getid(m_hIDGen), szRelativePath);
	(*ppOut) = pBinaryAsset;

	pBinaryAsset->INL_SetLoadStat(LOAD_STAT::LOADING);

	AsyncAssetOp* pArg = (AsyncAssetOp*)CORE_POOL_ALLOC(sizeof(AsyncAssetOp));
	fstrlcpy(pArg->szRelativePath, szRelativePath, sizeof(pArg->szRelativePath));

	pArg->pFileSystem = m_pFileSystem;
	pArg->pAsset = pBinaryAsset;

	pBinaryAsset->AddRef(); // 비동기 작업중 해제 할 수 있으므로 참조 카운트 증가

	threadpool_add_task_arg(
		m_hThreadPool,
		AsyncLoadBinaryAssetFromFileSystem,
		(void*)pArg
	);

	return TRUE;
}

BOOL Assets::Load(HANDLE hTempPool, const char* szRelativePath, BOOL sRGB, ITexture1D** ppOut)
{
	if (!hTempPool || !szRelativePath || !ppOut) return FALSE;

	char szModifiedPath[MAX_FILE_LENGTH] = { 0 };

	if (!IsExistTextureFile(szRelativePath, szModifiedPath))
	{
		SYS_LOG_E("Assets::Load: Texture1D file does not exist: %s", szRelativePath);
		return FALSE;
	}

	ITexture1D* pTexture = nullptr;
	m_pRenderSystem->CreateTexture1D(&pTexture);

	BOOL result = LoadTexture1DFromFileSystem(
		hTempPool,
		m_pFileSystem,
		m_pRenderSystem,
		szModifiedPath,
		sRGB,
		pTexture
	);

	if (!result)
	{
		pTexture->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pTexture;

	return TRUE;
}

BOOL Assets::Load(HANDLE hTempPool, const char* szRelativePath, BOOL sRGB, ITexture2D** ppOut)
{
	if (!hTempPool || !szRelativePath || !ppOut) return FALSE;

	char szModifiedPath[MAX_FILE_LENGTH] = { 0 };

	if (!IsExistTextureFile(szRelativePath, szModifiedPath))
	{
		SYS_LOG_E("Assets::Load: Texture2D file does not exist: %s", szRelativePath);
		return FALSE;
	}

	ITexture2D* pTexture = nullptr;
	m_pRenderSystem->CreateTexture2D(&pTexture);

	BOOL result = LoadTexture2DFromFileSystem(
		hTempPool,
		m_pFileSystem,
		m_pRenderSystem,
		szModifiedPath,
		sRGB,
		pTexture
	);

	if (!result)
	{
		pTexture->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pTexture;

	return TRUE;
}


BOOL Assets::Load(const char* szRelativePath, BOOL sRGB, ITexture1D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;
	
	char szModifiedPath[MAX_FILE_LENGTH] = { 0 };
	if (!IsExistTextureFile(szRelativePath, szModifiedPath))
	{
		SYS_LOG_E("Assets::Load: Texture1D file does not exist: %s", szRelativePath);
		return FALSE;
	}

	ITexture1D* pTexture = nullptr;
	m_pRenderSystem->CreateTexture1D(&pTexture);

	BOOL result = LoadTexture1DFromFileSystem(
		m_hSyncLoadTempPool,
		m_pFileSystem,
		m_pRenderSystem,
		szModifiedPath,
		sRGB,
		pTexture
	);

	if (!result)
	{
		pTexture->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pTexture;

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, BOOL sRGB, ITexture1D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

	char szModifiedPath[MAX_FILE_LENGTH] = { 0 };
	if (!IsExistTextureFile(szRelativePath, szModifiedPath))
	{
		SYS_LOG_E("Assets::Load: Texture1D file does not exist: %s", szRelativePath);
		return FALSE;
	}

	ITexture1D* pTexture = nullptr;
	m_pRenderSystem->CreateTexture1D(&pTexture);

	pTexture->AddRef(); // 비동기 작업에서 해제할 것이므로 참조 카운트 증가

	AsyncAssetOp* pArg = (AsyncAssetOp*)CORE_POOL_ALLOC(sizeof(AsyncAssetOp));
	fstrlcpy(pArg->szRelativePath, szModifiedPath, sizeof(szModifiedPath));
	pArg->pFileSystem = m_pFileSystem;
	pArg->pRenderSystem = m_pRenderSystem;
	pArg->Argument1 = sRGB ? 1 : 0; // sRGB 여부를 Argument1에 저장
	pArg->pAsset = pTexture;

	threadpool_add_task_temppool_arg(
		m_hThreadPool,
		AsyncLoadTexture1DFromFileSystem,
		(void*)pArg
	);

	return TRUE;
}

BOOL Assets::Load(const char* szRelativePath, BOOL sRGB, ITexture2D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

	char szModifiedPath[MAX_FILE_LENGTH] = { 0 };
	if (!IsExistTextureFile(szRelativePath, szModifiedPath))
	{
		SYS_LOG_E("Assets::Load: Texture2D file does not exist: %s", szRelativePath);
		return FALSE;
	}

	ITexture2D* pTexture = nullptr;
	m_pRenderSystem->CreateTexture2D(&pTexture);

	BOOL result = LoadTexture2DFromFileSystem(
		m_hSyncLoadTempPool,
		m_pFileSystem,
		m_pRenderSystem,
		szModifiedPath,
		sRGB,
		pTexture
	);

	if (!result)
	{
		pTexture->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	*ppOut = pTexture;

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, BOOL sRGB, ITexture2D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

	char szModifiedPath[MAX_FILE_LENGTH] = { 0 };
	if (!IsExistTextureFile(szRelativePath, szModifiedPath))
	{
		SYS_LOG_E("Assets::Load: Texture2D file does not exist: %s", szRelativePath);
		return FALSE;
	}

	ITexture2D* pTexture = nullptr;
	m_pRenderSystem->CreateTexture2D(&pTexture);
	pTexture->AddRef(); // 비동기 작업에서 해제할 것이므로 참조 카운트 증가

	AsyncAssetOp* pArg = (AsyncAssetOp*)CORE_POOL_ALLOC(sizeof(AsyncAssetOp));
	fstrlcpy(pArg->szRelativePath, szModifiedPath, sizeof(szModifiedPath));
	pArg->pFileSystem = m_pFileSystem;
	pArg->pRenderSystem = m_pRenderSystem;
	pArg->Argument1 = sRGB ? 1 : 0; // sRGB 여부를 Argument1에 저장
	pArg->pAsset = pTexture;

	threadpool_add_task_temppool_arg(
		m_hThreadPool,
		AsyncLoadTexture2DFromFileSystem,
		(void*)pArg
	);

	return TRUE;
}

BOOL Assets::Load(const char* szRelativePath, IModelAsset** ppOut)
{
	if (!szRelativePath || !ppOut)
		return FALSE;

	ModelAsset* pModelAsset = CORE_NEW(ModelAsset)(idgen_getid(m_hIDGen), szRelativePath);
	pModelAsset->INL_SetLoadStat(LOAD_STAT::LOADING);

	BOOL result = LoadModelFromFBX(
		m_hSyncLoadTempPool,
		m_pFileSystem,
		szRelativePath,
		pModelAsset
	);

	if (!result)
	{
		pModelAsset->Release();
		*ppOut = nullptr;
		return FALSE;
	}

	temppool_clear(m_hSyncLoadTempPool);

	pModelAsset->INL_SetLoadStat(LOAD_STAT::LOADED);

	*ppOut = pModelAsset;
	
	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, IModelAsset** ppOut)
{
	if (!szRelativePath || !ppOut)
		return FALSE;

	ModelAsset* pModelAsset = CORE_NEW(ModelAsset)(idgen_getid(m_hIDGen), szRelativePath);
	(*ppOut) = pModelAsset;

	pModelAsset->INL_SetLoadStat(LOAD_STAT::LOADING);

	AsyncAssetOp* pArg = (AsyncAssetOp*)CORE_POOL_ALLOC(sizeof(AsyncAssetOp));
	fstrlcpy(pArg->szRelativePath, szRelativePath, sizeof(pArg->szRelativePath));

	pArg->pFileSystem = m_pFileSystem;
	pArg->pAsset = pModelAsset;

	pModelAsset->AddRef(); // 비동기 작업중 해제 할 수 있으므로 참조 카운트 증가

	threadpool_add_task_temppool_arg(
		m_hThreadPool,
		AsyncLoadModelFromFBX,
		(void*)pArg
	);

	return TRUE;
}

BOOL Assets::Load(const char* szRelativePath, IModel** ppOut)
{
	char szExtension[16] = { 0 };
	get_file_extension(szRelativePath, szExtension, sizeof(szExtension));
	fstrlcpy(szExtension, fstrlwr(szExtension), sizeof(szExtension) - 1);

	// fbx
	if (fstrstr(szExtension, "fbx"))
	{
		IModelAsset* pModelAsset = nullptr;
		if (!Load(szRelativePath, &pModelAsset))
		{
			*ppOut = nullptr;
			return FALSE;
		}

		temppool_clear(m_hSyncLoadTempPool);

		MODEL_CREATE_DESC ModelCreateDesc = {};
		get_filename(szRelativePath, ModelCreateDesc.szModelName, sizeof(ModelCreateDesc.szModelName));
		ModelCreateDesc.VertexFormat = pModelAsset->GetVertexFormat();
		ModelCreateDesc.MaxVertexCount = pModelAsset->GetTotalVertexCount();
		ModelCreateDesc.MaxIndexCount = pModelAsset->GetTotalIndexCount();

		IModel* pModel = nullptr;
		if (!m_pRenderSystem->CreateModel(ModelCreateDesc, &pModel))
		{
			pModelAsset->Release();
			*ppOut = nullptr;
			return FALSE;
		}
		
		BOOL Result= LoadModelFromModelAsset(
			m_hSyncLoadTempPool,
			m_pFileSystem,
			this,
			m_pRenderSystem,
			szRelativePath,
			pModelAsset,
			pModel
		);

		CHECK_RELEASE(pModelAsset);

		if (!Result)
		{
			pModel->Release();
			*ppOut = nullptr;
			return FALSE;
		}

		*ppOut = pModel;
	}

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, IModel** ppOut)
{
	return TRUE;
}

BOOL Assets::IsExistTextureFile(const char* szRelativePath, char* szModifiedPath) noexcept
{
	// 텍스처 파일이 존재하는지 확인하는 로직 구현
	// 예시: "texture/wood.png" -> "texture/wood_diffuse.png", "texture/wood_normal.png", "texture/wood_specular.png" 등으로 변환하여 존재 여부 확인
	char szLowerPath[MAX_FILE_LENGTH] = { 0 };
	fstrlcpy(szLowerPath, fstrlwr((char*)szRelativePath), sizeof(szLowerPath) - 1);
	if (m_pFileSystem->ExistFile(szLowerPath))
	{
		if (szModifiedPath)
			fstrlcpy(szModifiedPath, szLowerPath, MAX_FILE_LENGTH);
		return TRUE;
	}

	char szOnlyPath[MAX_FILE_LENGTH] = { 0 };
	char szOnlyFileName[MAX_FILE_LENGTH] = { 0 };
	char szTempPath[MAX_FILE_LENGTH] = { 0 };
	char szFullPath[MAX_FILE_LENGTH] = { 0 };

	get_path(szLowerPath, szOnlyPath, sizeof(szOnlyPath));
	get_filename(szRelativePath, szOnlyFileName, sizeof(szOnlyFileName));

	combine_path(szOnlyPath, "texture", szTempPath, sizeof(szTempPath));
	combine_path(szTempPath, szOnlyFileName, szFullPath, sizeof(szFullPath));
	if (m_pFileSystem->ExistFile(szFullPath))
	{
		if (szModifiedPath)
			fstrlcpy(szModifiedPath, szFullPath, MAX_FILE_LENGTH);
		return TRUE;
	}

	combine_path(szOnlyPath, "material", szTempPath, sizeof(szTempPath));
	combine_path(szTempPath, szOnlyFileName, szFullPath, sizeof(szFullPath));
	if (m_pFileSystem->ExistFile(szFullPath))
	{
		if (szModifiedPath)
			fstrlcpy(szModifiedPath, szFullPath, MAX_FILE_LENGTH);
		return TRUE;
	}

	for (int i = 0; i < 8; ++i)
	{
		if(m_szTexturePath[i][0] == '\0')
			continue;

		combine_path(m_szTexturePath[i], szOnlyFileName, szFullPath, sizeof(szFullPath));
		if (m_pFileSystem->ExistFile(szFullPath))
		{
			if (szModifiedPath)
				fstrlcpy(szModifiedPath, szFullPath, MAX_FILE_LENGTH);
			return TRUE;
		}
	}

	return FALSE;
}



/*
char szLowerPath[MAX_FILE_LENGTH] = { 0 };
fstrlcpy(szLowerPath, fstrlwr((char*)szRelativePath), sizeof(szLowerPath) - 1);

if (!m_pFileSystem->ExistFile(szLowerPath))
{
	char szOnlyPath[MAX_FILE_LENGTH] = { 0 };
	char szTempPath[MAX_FILE_LENGTH] = { 0 };
	get_path(szLowerPath, szOnlyPath, sizeof(szOnlyPath));

	combine_path(szOnlyPath, "texture", szTempPath, sizeof(szTempPath));
	if (!m_pFileSystem->ExistFile(szLowerPath))
	{
		combine_path(szOnlyPath, "material", szTempPath, sizeof(szTempPath));
		if (m_pFileSystem->ExistFile(szTempPath))
		{
			SYS_LOG_E("Assets::Load: Model file not found: %s", szRelativePath);
			(*ppOut) = nullptr;
			return FALSE;
		}
	}
}
*/