#ifndef __TEXTASSET_H__
#define __TEXTASSET_H__

#include "ITextAsset.h"


class TextAsset : public ITextAsset
{
public:
	TextAsset(UINT32 ID);
	virtual ~TextAsset() noexcept;

	// ITextAsset interface
	virtual const char* GetData() const noexcept override;
	virtual size_t GetSize() const noexcept override;

	virtual BOOL IsUTF8() const noexcept override;
	virtual ENCODING_TYPE GetEncodingType() const noexcept override;

	virtual BOOL ConvertUTF32(char32_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept override;
	virtual BOOL ConvertUTF16(char16_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept override;
	virtual BOOL ConvertWCHAR(wchar_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept override;
 
	// Private inline methods
	__FORCEINLINE UINT32 INL_GetID() const noexcept
	{
		return m_ID;
	}

	__FORCEINLINE ASSET_TYPE INL_GetAssetType() const noexcept
	{
		return ASSET_TYPE::TEXT;
	}

	__FORCEINLINE LOAD_STAT INL_GetLoadStat() const noexcept
	{
		return m_LoadStat;
	}

	__FORCEINLINE const char* INL_GetData() const noexcept
	{
		return m_pData;
	}

	__FORCEINLINE size_t INL_GetSize() const noexcept
	{
		return m_Size;
	}

	__FORCEINLINE void INL_SetData(char* pData, size_t Size) noexcept
	{
		m_pData = pData;
		m_Size = Size;

		LOAD_STAT loadStat = (nullptr != pData) ? LOAD_STAT::LOADED : LOAD_STAT::FAILED;

		interlock_store_l((long*)&m_LoadStat, (long)loadStat, MEMORY_ORDER_RELAXED);
	}

protected:
	void OnDestroy();

private:
	char* m_pData;
	size_t m_Size;

};


#endif // __TEXTASSET_H__
