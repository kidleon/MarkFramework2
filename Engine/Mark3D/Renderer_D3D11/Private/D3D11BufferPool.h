#pragma once
#include "SpinLock.h"


namespace mark
{
	struct D3D11Buffer;
	enum class BUFFER_SIZE : uint8_t;

	template<typename _T, size_t SlotCount>
	struct PagedResourcePool
	{
		struct Page
		{
			_T* pResources = nullptr;
			size_t Capacity = 0;
		};

		sys_vector<Page> m_Pages;
		sys_deque<ResourceEntity> m_FreeSlots;

		inline _T* Acquire()
		{
			if (m_FreeSlots.empty())
				AllocPage();

			ResourceEntity entity = m_FreeSlots.front();
			m_FreeSlots.pop_front();
			Page& page = m_Pages[entity.PageIndex];
			return &page.pResources[entity.SlotIndex];
		}

		inline void Release(_T* pResource)
		{
			if (!pResource)
				return;

			pResource->Reset(); // 리소스 초기화 (예: UsedBytes = 0
			m_FreeSlots.push_back(pResource->ResourceId);
		}

		bool AllocPage()
		{
			Page newPage;
			newPage.pResources = CORE_NEW_ARRAY(_T, SlotCount);
			newPage.Capacity = SlotCount;

			size_t pageIndex = m_Pages.size();
			m_Pages.push_back(std::move(newPage));

			for (size_t slotIndex = 0; slotIndex < SlotCount; ++slotIndex)
			{
				ResourceID entity;
				entity.PageIndex = static_cast<uint16_t>(pageIndex);
				entity.SlotIndex = static_cast<uint16_t>(slotIndex);
				m_FreeSlots.push_back(entity);
			}

			return true;
		}

		void RemovePage(const Page& page)
		{
			if (page.pResources)
			{
				CORE_DELETE_ARRAY(_T, page.pResources, page.Capacity);
				page.pResources = nullptr;
			}
		}
	};

	class D3D11BufferPool
	{
	public:
		D3D11Buffer* Acquire();
		void Release(D3D11Buffer* pBuffer);

	private:
		struct D3D11BufferPage
		{
			D3D11Buffer** ppBuffers = nullptr;
			size_t Capacity = 0;
		};

		sys_vector<D3D11BufferPage> m_BufferPages;
		sys_deque<ResourceEntity> m_FreeBuffers;

	};

	
		struct BufferPool
		{
			sys_list<D3D11Buffer*> lstBuffers;
			spin_lock_t lock = { 0 };
		};

	public:
		D3D11BufferPool() = default;
		~D3D11BufferPool() noexcept;

		bool Initialize();
		void Shutdown();

		D3D11Buffer* Acquire(
			BUFFER_TYPE BufferType,
			BUFFER_USAGE Usage,
			size_t BufferSize
		) noexcept;

		void Release(D3D11Buffer* pBuffer);

	private:
		BufferPool m_VBPool[(int)BUFFER_USAGE::EMAX][(int)BUFFER_SIZE::SIZE_COUNT];
		BufferPool m_IBPool[(int)BUFFER_USAGE::EMAX][(int)BUFFER_SIZE::SIZE_COUNT];
		BufferPool m_CBPool[(int)BUFFER_USAGE::EMAX][(int)BUFFER_SIZE::SIZE_COUNT];

	};
}
