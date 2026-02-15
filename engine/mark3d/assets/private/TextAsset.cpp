#include "pch.h"
#include "TextAsset.h"
#include "idgen.h"
#include "Log.h"
#include "Assets.h"


TextAsset::TextAsset(UINT64 ID)
	: m_pData(nullptr)
	, m_Size(0)
{
	m_ID = ID;
	m_LoadStat = LOAD_STAT::NOT_LOADED;
}

TextAsset::~TextAsset() noexcept
{
	if (m_pData)
	{
		CORE_SYS_FREE(m_pData);
		m_pData = nullptr;
	}
}

long TextAsset::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long TextAsset::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, TextAsset);
	}
	return NewRefCnt;
}

long TextAsset::RefCnt()
{
	return m_RefCnt;
}

UINT64 TextAsset::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE TextAsset::GetAssetType() const noexcept
{
	return ASSET_TYPE::TEXT_ASSET;
}

LOAD_STAT TextAsset::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

const char* TextAsset::GetData() const noexcept
{
	return m_pData;
}

size_t TextAsset::GetSize() const noexcept
{
	return m_Size;
}

BOOL TextAsset::IsUTF8() const noexcept
{
	// BOM 검사
	ENCODING_TYPE encoding = detect_bom(
		(const unsigned char*)m_pData,
		m_Size
	);

	if (encoding == ENCODING_UTF8)
		return TRUE;

	// BOM이 없으면 유효성 검사
	return is_valid_utf8(
		(const unsigned char*)m_pData,
		m_Size
	);
}

ENCODING_TYPE TextAsset::GetEncodingType() const noexcept
{
	return detect_bom(
		(const unsigned char*)m_pData,
		m_Size
	);
}

BOOL TextAsset::ConvertUTF32(char32_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept
{
	if (nullptr == pBuffer || BufferSize == 0 || nullptr == pResultSize)
		return FALSE;

	// 변환 수행
	BOOL result = conv_ansi_to_utf32(
		m_pData,
		m_Size,
		pBuffer,
		BufferSize
	);

	if (!result)
	{
		SYS_LOG_E("TextAsset::ConvertUTF32 - Conversion failed.");
		return FALSE;
	}

	// 변환된 크기 계산 (문자 단위)
	size_t converted_size = 0;
	for (size_t i = 0; i < BufferSize / sizeof(char32_t); ++i)
	{
		if (pBuffer[i] == 0)
			break;
		converted_size++;
	}

	*pResultSize = converted_size;

	return TRUE;
}

BOOL TextAsset::ConvertUTF16(char16_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept
{
	// ConvertUTF32 구현을 16버전으로 변경
	if (nullptr == pBuffer || BufferSize == 0 || nullptr == pResultSize)
		return FALSE;

	// 변환 수행
	BOOL result = conv_ansi_to_utf16(
		m_pData,
		m_Size,
		pBuffer,
		BufferSize
	);

	if (!result)
	{
		SYS_LOG_E("TextAsset::ConvertUTF16 - Conversion failed.");
		return FALSE;
	}

	// 변환된 크기 계산 (문자 단위)
	size_t converted_size = 0;
	for (size_t i = 0; i < BufferSize / sizeof(char16_t); ++i)
	{
		if (pBuffer[i] == 0)
			break;
		converted_size++;
	}

	*pResultSize = converted_size;

	return TRUE;
}

BOOL TextAsset::ConvertWCHAR(wchar_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept
{
	// ConvertUTF32 구현을 WCHAR버전으로 변경
	if (nullptr == pBuffer || BufferSize == 0 || nullptr == pResultSize)
		return FALSE;

	// 변환 수행
	BOOL result = conv_ansi_to_wchar(
		m_pData,
		m_Size,
		pBuffer,
		BufferSize
	);

	if (!result)
	{
		SYS_LOG_E("TextAsset::ConvertWCHAR - Conversion failed.");
		return FALSE;
	}

	// 변환된 크기 계산 (문자 단위)

	size_t converted_size = 0;
	for (size_t i = 0; i < BufferSize / sizeof(wchar_t); ++i)
	{
		if (pBuffer[i] == 0)
			break;
		converted_size++;
	}

	*pResultSize = converted_size;

	return TRUE;
}