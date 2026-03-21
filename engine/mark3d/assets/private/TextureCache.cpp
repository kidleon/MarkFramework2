#include "pch.h"
#include "TextureCache.h"
#include "AssetDef.h"


struct TextureCacheEntry
{
	int64 TextureHash;
	IUNKNOWN* pTexture; // ITexture1D 또는 ITexture2D 포인터

	HASH_NODE HashNode; // 해시 노드
};

struct TextureCache
{
	HASH_TABLE* pTexture1DCache;
	HASH_TABLE* pTexture2DCache;
};

TextureCache* g_pTextureCache = nullptr;


VOID InitTextureCache(
	size_t Texture1DBucketSize,
	size_t Texture2DBucketSize
)
{
	if (g_pTextureCache)
		return;

	Texture1DBucketSize = T_MAX(Texture1DBucketSize, 16);
	Texture2DBucketSize = T_MAX(Texture2DBucketSize, 32);

	// 해시 테이블 버킷 크기는 1024로 설정 (적절한 크기로 조정 가능)
	g_pTextureCache = (TextureCache*)CORE_SYS_ALLOC(sizeof(TextureCache));
	memset(g_pTextureCache, 0, sizeof(TextureCache));

	g_pTextureCache->pTexture1DCache = create_hash_table(Texture1DBucketSize);
	g_pTextureCache->pTexture2DCache = create_hash_table(Texture2DBucketSize);
}

VOID ShutdownTextureCache()
{
	if (!g_pTextureCache)
		return;

	if (g_pTextureCache->pTexture1DCache)
	{
		for (size_t b = 0; b < g_pTextureCache->pTexture1DCache->bucket_size; ++b)
		{
			HASH_NODE* node = g_pTextureCache->pTexture1DCache->buckets[b];
			while (node)
			{
				TextureCacheEntry* entry = (TextureCacheEntry*)node->data;
				if (entry)
				{
					CHECK_RELEASE(entry->pTexture);
					CORE_SYS_FREE(node->data);
				}

				node = node->next;
			}
			g_pTextureCache->pTexture1DCache->buckets[b] = nullptr;
		}

		delete_hash_table(g_pTextureCache->pTexture1DCache);
		g_pTextureCache->pTexture1DCache = nullptr;
	}

	if (g_pTextureCache->pTexture2DCache)
	{
		for (size_t b = 0; b < g_pTextureCache->pTexture2DCache->bucket_size; ++b)
		{
			HASH_NODE* node = g_pTextureCache->pTexture2DCache->buckets[b];
			while (node)
			{
				TextureCacheEntry* entry = (TextureCacheEntry*)node->data;
				if (entry)
				{
					CHECK_RELEASE(entry->pTexture);
					CORE_SYS_FREE(node->data);
				}
				node = node->next;
			}
			g_pTextureCache->pTexture2DCache->buckets[b] = nullptr;
		}

		delete_hash_table(g_pTextureCache->pTexture2DCache);
		g_pTextureCache->pTexture2DCache = nullptr;
	}

	CORE_SYS_FREE(g_pTextureCache);
	g_pTextureCache = nullptr;
}

VOID ClearTextureCache()
{
	if (g_pTextureCache->pTexture1DCache)
	{
		for (size_t b = 0; b < g_pTextureCache->pTexture1DCache->bucket_size; ++b)
		{
			HASH_NODE* node = g_pTextureCache->pTexture1DCache->buckets[b];
			while (node)
			{
				TextureCacheEntry* entry = (TextureCacheEntry*)node->data;
				if (entry)
				{
					CHECK_RELEASE(entry->pTexture);
					CORE_SYS_FREE(node->data);
				}

				node = node->next;
			}
			g_pTextureCache->pTexture1DCache->buckets[b] = nullptr;
		}

		g_pTextureCache->pTexture1DCache->node_count = 0;
	}

	if (g_pTextureCache->pTexture2DCache)
	{
		for (size_t b = 0; b < g_pTextureCache->pTexture2DCache->bucket_size; ++b)
		{
			HASH_NODE* node = g_pTextureCache->pTexture2DCache->buckets[b];
			while (node)
			{
				TextureCacheEntry* entry = (TextureCacheEntry*)node->data;
				if (entry)
				{
					CHECK_RELEASE(entry->pTexture);
					CORE_SYS_FREE(node->data);
				}
				HASH_NODE* next = node->next;
				node = next;
			}
			g_pTextureCache->pTexture2DCache->buckets[b] = nullptr;
		}

		g_pTextureCache->pTexture2DCache->node_count = 0;
	}
}

BOOL QueryTexture1D(
	const char* szTextureName,
	ITexture1D** ppOut
)
{
	return TRUE;
}

BOOL QueryTexture2D(
	const char* szTextureName,
	ITexture2D** ppOut
)
{
	return TRUE;
}

BOOL RegiTexture1D(
	const char* szTextureName,
	ITexture1D* pTexture
)
{
	return TRUE;
}

BOOL RegiTexture2D(
	const char* szTextureName,
	ITexture2D* pTexture
)
{
	return TRUE;
}

