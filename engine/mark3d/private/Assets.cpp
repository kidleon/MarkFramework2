#include "pch.h"
#include "Assets.h"
#include "thread_pool.h"
#include "unicode.h"
#include "strings.h"
#include "Log.h"

#include "IDataStream.h"
#include "IFileSystem.h"

#include "TextAsset.h"
#include "BinaryAsset.h"
#include "ITexture1D.h"

#include "TextAssetLoader.h"
#include "AsyncAssetArgument.h"


IMPLEMENTATION_IUNKNOWN_INTERFACE(Assets);

Assets::Assets()
	: m_pFileSystem(nullptr)
	, m_hThreadPool(nullptr)
	, m_Initialized(FALSE)
{
}

Assets::~Assets() noexcept
{
	Shutdown();
}

void Assets::OnDestroy()
{
	MARK_DELETE_AUTO(this, Assets, HEAP_TYPE::SYSCALL);
}

BOOL Assets::Init(const char* szRootPath)
{
	if (m_Initialized) return TRUE;

	CreateOSFileSystem(szRootPath, &m_pFileSystem);
	m_hThreadPool = threadpool_create(THREAD_POOL_SIZE);

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

	if (m_pFileSystem)
	{
		m_pFileSystem->Release();
		m_pFileSystem = nullptr;
	}

	m_Initialized = FALSE;
}															

BOOL Assets::Load(const char* szRelativePath, ITextAsset** ppOut)
{
	TextAsset* pTextAsset = MARK_NEW(TextAsset);

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
	TextAsset* pTextAsset = MARK_NEW(TextAsset);
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
	return TRUE;
}

BOOL Assets::LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut)
{
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


