#ifndef __T_OBJECT_POOL_H__
#define __T_OBJECT_POOL_H__

#include "TCommon.h"
#include "TAllocator.h"
#include "TList.h"


namespace mark
{
	template <typename T, typename _Alloc , size_t _NumGrowup = 16>
	class TObjectPool
	{
	public:
		TObjectPool() = default;
		virtual ~TObjectPool()
		{
			clear();
		}

		__INLINE T* alloc()
		{
			if (m_FreeList.empty())
			{
				for (size_t i = 0; i < _NumGrowup; ++i)
				{
					T* obj = _Alloc::alloc(sizeof(T));
					m_FreeList.push_front(newObj);
					m_AllocList.push_back(newObj);
				}
			}

			T* obj = m_FreeList.front();
			m_FreeList.pop_front();
			return obj;
		}

		__INLINE void release(T* obj)
		{
			m_FreeList.push_front(obj);
		}

		void clear()
		{
			for (T* obj : m_AllocList)
				_Alloc::free(obj);
			m_AllocList.clear();
			m_FreeList.clear();
		}

	private:
		TList<T*, _Alloc> m_FreeList;
		TList<T*, _Alloc> m_AllocList;

	};
}

#endif // __T_OBJECT_POOL_H__
