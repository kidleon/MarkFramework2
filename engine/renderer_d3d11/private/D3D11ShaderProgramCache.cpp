#include "pch.h"
#include "D3D11ShaderProgramCache.h"
#include "D3D11ShaderProgram.h"
#include "fnv.h"


D3D11ShaderProgramCache* D3D11ShaderProgramCache::s_pInstance = nullptr;

D3D11ShaderProgramCache::D3D11ShaderProgramCache()
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11ShaderProgramCache::~D3D11ShaderProgramCache() noexcept
{
	Shutdown();
	s_pInstance = nullptr;
}

BOOL D3D11ShaderProgramCache::Init()
{
	m_pHashTable_VS = create_hash_table(64);
	m_pHashTable_PS = create_hash_table(64);
	m_pHashTable_GS = create_hash_table(64);
	m_pHashTable_CS = create_hash_table(64);

	return TRUE;
}

void D3D11ShaderProgramCache::Shutdown()
{
	if (m_pHashTable_VS)
	{
		DestroyHashTable(m_pHashTable_VS);
		m_pHashTable_VS = nullptr;
	}

	if (m_pHashTable_PS)
	{
		DestroyHashTable(m_pHashTable_PS);
		m_pHashTable_PS = nullptr;
	}

	if (m_pHashTable_GS)
	{
		DestroyHashTable(m_pHashTable_GS);
		m_pHashTable_GS = nullptr;
	}

	if (m_pHashTable_CS)
	{
		DestroyHashTable(m_pHashTable_CS);
		m_pHashTable_CS = nullptr;
	}
}

void D3D11ShaderProgramCache::DestroyHashTable(HASH_TABLE* pHashTable) noexcept
{
	if (!pHashTable)
		return;

	for (size_t b = 0; b < pHashTable->bucket_size; ++b)
	{
		HASH_NODE* pNode = pHashTable->buckets[b];
		while (pNode)
		{
			HASH_NODE* pNextNode = pNode->next;

			IUNKNOWN* pShaderProgram = static_cast<IUNKNOWN*>(pNode->data);
			if (pShaderProgram)
			{
				pShaderProgram->Release();
				pShaderProgram = nullptr;
			}

			pNode = pNextNode;
		}
		pHashTable->buckets[b] = nullptr;
	}

	delete_hash_table(pHashTable);
}

static inline uint64 MakeShaderHash(NameHash ShaderName, UINT32 ShaderDefinesHash)
{
	return ((uint64)ShaderName.value() << 32) | (uint64)ShaderDefinesHash;
}

void D3D11ShaderProgramCache::Register(D3D11ShaderProgram* pShaderProgram) noexcept
{
	if (!pShaderProgram)
		return;

	SHADER_TYPE ShaderType = pShaderProgram->INL_GetShaderType();

	HASH_TABLE* pHashTable = nullptr;
	switch (ShaderType)
	{
		case SHADER_TYPE::VERTEX:
			pHashTable = m_pHashTable_VS;
			break;

		case SHADER_TYPE::PIXEL:
			pHashTable = m_pHashTable_PS;
			break;

		case SHADER_TYPE::GEOMETRY:
			pHashTable = m_pHashTable_GS;
			break;

		case SHADER_TYPE::COMPUTE:
			pHashTable = m_pHashTable_CS;
			break;
	}

	if (pHashTable)
	{
		uint64 ShaderHash = MakeShaderHash(
			pShaderProgram->INL_GetShaderName(),
			pShaderProgram->INL_GetShaderDefinesHash()
		);

		insert_hash_node(
			pHashTable,
			ShaderHash,
			pShaderProgram->INL_GetHashNode()
		);
	}
}

D3D11ShaderProgram* D3D11ShaderProgramCache::Find_RS(SHADER_TYPE ShaderType, NameHash ShaderName, UINT32 ShaderDefinesHash) noexcept
{
	HASH_TABLE* pHashTable = nullptr;

	switch (ShaderType)
	{
		case SHADER_TYPE::VERTEX:
			pHashTable = m_pHashTable_VS;
			break;

		case SHADER_TYPE::PIXEL:
			pHashTable = m_pHashTable_PS;
			break;

		case SHADER_TYPE::GEOMETRY:
			pHashTable = m_pHashTable_GS;
			break;

		case SHADER_TYPE::COMPUTE:
			pHashTable = m_pHashTable_CS;
			break;
	}

	if (!pHashTable)
		return nullptr;

	uint64 ShaderHash = MakeShaderHash(ShaderName, ShaderDefinesHash);

	D3D11ShaderProgram* pSP = (D3D11ShaderProgram*)query_hash_node(
		pHashTable,
		ShaderHash
	);

	if (pSP)
		pSP->AddRef();

	return pSP;
}