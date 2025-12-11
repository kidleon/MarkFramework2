#include "pch.h"
#include "Assets.h"
#include "thread_pool.h"
#include "unicode.h"
#include "strings.h"
#include "idgen.h"
#include "Log.h"

#include "IDataStream.h"
#include "IFileSystem.h"

#include "TextAsset.h"
#include "BinaryAsset.h"
#include "ITexture1D.h"
#include "ITexture2D.h"

#include "TextAssetLoader.h"
#include "BinaryAssetLoader.h"
#include "AsyncAssetArgument.h"


constexpr static UINT32 MIN_ID_COUNT = 1;
constexpr static UINT32 MAX_ID_COUNT = 200000;
constexpr static size_t THREAD_POOL_SIZE = 4;

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

void Assets::OnDestroy()
{
	MARK_DELETE(this, Assets);
}

BOOL Assets::Init(const char* szRootPath)
{
	if (m_Initialized) return TRUE;

	CreateOSFileSystem(szRootPath, &m_pFileSystem);
	m_hThreadPool = threadpool_create(THREAD_POOL_SIZE);
	m_hIDGen = idgen_create(MIN_ID_COUNT, MAX_ID_COUNT);
	GLOBAL_VARS::ID_GEN_HANDLE = m_hIDGen;

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

	if (m_hIDGen)
	{
		idgen_destroy(m_hIDGen);
		m_hIDGen = nullptr;
		GLOBAL_VARS::ID_GEN_HANDLE = nullptr;
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

	TextAsset* pTextAsset = MARK_NEW(TextAsset)(idgen_getid(m_hIDGen));

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

	TextAsset* pTextAsset = MARK_NEW(TextAsset)(idgen_getid(m_hIDGen));
	(*ppOut) = pTextAsset;

	AsyncArgument* pArg = (AsyncArgument*)MARK_POOL_ALLOC(sizeof(AsyncArgument));
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

	BinaryAsset* pBinaryAsset = MARK_NEW(BinaryAsset)(idgen_getid(m_hIDGen));

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

	BinaryAsset* pBinaryAsset = MARK_NEW(BinaryAsset)(idgen_getid(m_hIDGen));
	(*ppOut) = pBinaryAsset;

	AsyncArgument* pArg = (AsyncArgument*)MARK_POOL_ALLOC(sizeof(AsyncArgument));
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

BOOL Assets::Load(const char* szRelativePath, ITexture1D** ppOut)
{
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


