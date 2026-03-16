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

#include "TextAssetLoader.h"
#include "BinaryAssetLoader.h"
#include "FBXModelLoader.h"
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

BOOL Assets::Init(const char* szRootPath)
{
	if (m_Initialized) return TRUE;

	constexpr size_t TEMP_POOL_SIZE = 1024 * 1024 * 20; // 20MB 크기의 임시 풀 생성

	CreateOSFileSystem(szRootPath, &m_pFileSystem);
	m_hThreadPool = threadpool_create(THREAD_POOL_SIZE, TEMP_POOL_SIZE);
	m_hIDGen = idgen_create(MIN_ID_COUNT, MAX_ID_COUNT);
	Assets::ID_GEN_HANDLE = m_hIDGen;

	m_hSyncLoadTempPool = temppool_create(TEMP_POOL_SIZE, FALSE); // 20MB 크기의 동기 로드용 임시 풀 생성

	m_Initialized = TRUE;

	fstrlcpy(m_szTexturePath[0], "texture", sizeof(m_szTexturePath[0]));
	fstrlcpy(m_szTexturePath[1], "common/texture", sizeof(m_szTexturePath[1]));
	fstrlcpy(m_szTexturePath[2], "model/texture", sizeof(m_szTexturePath[2]));

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

BOOL Assets::Load(const char* szRelativePath, ITexture1D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;
	
#if defined(__MARK3D_RENDERSYSTEM_D3D11__)
	
	IDataStream* pDataStream = m_pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Assets::Load: Failed to open texture1D file: %s", szRelativePath);
		return FALSE;
	}

	size_t DataSize = pDataStream->GetSize();
	void* pData = MARK_TEMP_ALLOC(DataSize);
	if (!pData)
	{
		pDataStream->Release();
		SYS_LOG_E("Assets::Load: Failed to allocate memory for texture1D data: %s", szRelativePath);
		return FALSE;
	}

	if (!pDataStream->Read(pData, DataSize))
	{
		SYS_LOG_E("Assets::Load: Failed to read texture1D data from file: %s", szRelativePath);

		MARK_TEMP_RESET();
		pDataStream->Release();
		
		return FALSE;
	}

	D3D11Texture1D* pD3D11Texture1D = nullptr;
	if (!LoadD3D11Texture1DFromMemory(
		GLOBAL_VARS::D3D11_RENDER_DEVICE,
		szRelativePath,
		pData,
		DataSize,
		&pD3D11Texture1D
	))
	{
		SYS_LOG_E("Assets::Load: Failed to load D3D11 texture1D from memory: %s", szRelativePath);

		MARK_TEMP_RESET();
		pDataStream->Release();

		return FALSE;
	}

	pDataStream->Release();
	pDataStream = nullptr;

	D3D11Texture1DProxy* pTexture1DProxy = MARK_POOL_NEW(D3D11Texture1DProxy)(pD3D11Texture1D);

	Texture2D* pTexture1D = MARK_POOL_NEW(Texture2D)(idgen_getid(m_hIDGen));
	pTexture1D->SetTexture1DProxy(pTexture1DProxy);
	
	(*ppOut) = pTexture1D;

#endif // __MARK3D_RENDERSYSTEM_D3D11__

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, ITexture1D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

	return TRUE;
}

BOOL Assets::Load(const char* szRelativePath, ITexture2D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, ITexture2D** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

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

		return TRUE;
	}

	// model

	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, IModel** ppOut)
{
	return TRUE;
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