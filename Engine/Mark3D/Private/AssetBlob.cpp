#include "pch.h"
#include "AssetBlob.h"


namespace mark
{
	AssetBlob::AssetBlob(size_t DataSize, void* pData)
		: m_DataSize(DataSize), m_pData(pData)
	{
	}

	AssetBlob::AssetBlob(size_t DataSize, void* pData, BOOL OwnsData)
		: m_OwnsData(OwnsData), m_DataSize(DataSize), m_pData(pData)
	{
	}

	AssetBlob::~AssetBlob() noexcept
	{
		if (m_OwnsData && m_pData)
		{
			CORE_SYS_FREE(m_pData);
			m_pData = nullptr;
			m_DataSize = 0;
		}
	}

	AssetBlob::AssetBlob(AssetBlob&& other) noexcept
		: m_RefCount(other.m_RefCount.load(std::memory_order_relaxed)),
		  m_OwnsData(other.m_OwnsData),
		  m_DataSize(other.m_DataSize),
		  m_pData(other.m_pData)
	{
		other.m_OwnsData = FALSE;
		other.m_DataSize = 0;
		other.m_pData = nullptr;
		other.m_RefCount.store(1, std::memory_order_relaxed);
	}

	AssetBlob& AssetBlob::operator=(AssetBlob&& other) noexcept
	{
		if (this != &other)
		{
			m_RefCount.store(other.m_RefCount.load(std::memory_order_relaxed), std::memory_order_relaxed);
			m_OwnsData = other.m_OwnsData;
			m_DataSize = other.m_DataSize;
			m_pData = other.m_pData;

			other.m_OwnsData = FALSE;
			other.m_DataSize = 0;
			other.m_pData = nullptr;
		}
		return *this;
	}

	void AssetBlob::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void AssetBlob::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(AssetBlob, this);
		}
	}

	size_t AssetBlob::GetDataSize() const noexcept
	{
		return m_DataSize;
	}

	const void* AssetBlob::GetData() const noexcept
	{
		return m_pData;
	}


}
