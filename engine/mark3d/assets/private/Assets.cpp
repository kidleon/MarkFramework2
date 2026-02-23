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

Assets::Assets()
	: m_pFileSystem(nullptr)
	, m_hThreadPool(nullptr)
	, m_hIDGen(nullptr)
	, m_Initialized(FALSE)
{
}

Assets::~Assets() noexcept
{
	Shutdown();
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

	CreateOSFileSystem(szRootPath, &m_pFileSystem);
	m_hThreadPool = threadpool_create(THREAD_POOL_SIZE);
	m_hIDGen = idgen_create(MIN_ID_COUNT, MAX_ID_COUNT);
	Assets::ID_GEN_HANDLE = m_hIDGen;

	m_hSyncLoadTempPool = temppool_create(1024 * 8192, FALSE);

	m_Initialized = TRUE;

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

	TextAsset* pTextAsset = CORE_NEW(TextAsset)(idgen_getid(m_hIDGen));

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

	TextAsset* pTextAsset = CORE_NEW(TextAsset)(idgen_getid(m_hIDGen));
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

	BinaryAsset* pBinaryAsset = CORE_NEW(BinaryAsset)(idgen_getid(m_hIDGen));

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

	BinaryAsset* pBinaryAsset = CORE_NEW(BinaryAsset)(idgen_getid(m_hIDGen));
	(*ppOut) = pBinaryAsset;

	AsyncAssetOp* pArg = (AsyncAssetOp*)CORE_POOL_ALLOC(sizeof(AsyncAssetOp));
	fstrlcpy(pArg->szRelativePath, szRelativePath, sizeof(pArg->szRelativePath));

	pArg->pFileSystem = m_pFileSystem;
	pArg->pAsset = pBinaryAsset;

	pBinaryAsset->AddRef(); // 비동기 작업에서 해제할 것이므로 참조 카운트 증가

	threadpool_add_task_arg(
		m_hThreadPool,
		AsyncLoadTextAssetFromFileSystem,
		(void*)pArg
	);

	return TRUE;
}

/*
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
	return TRUE;
}

BOOL Assets::Load(const char* szRelativePath, ITexture2D** ppOut)
{
	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, ITexture2D** ppOut)
{
	return TRUE;
}
*/

BOOL Assets::Load(const char* szRelativePath, IModelAsset** ppOut)
{
	if (!szRelativePath || !ppOut) return FALSE;

	ModelAsset* pModelAsset = CORE_NEW(ModelAsset)(idgen_getid(m_hIDGen));

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

	*ppOut = pModelAsset;
	
	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, IModelAsset** ppOut)
{
	return TRUE;
}