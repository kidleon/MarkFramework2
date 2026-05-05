#pragma once
#include "spin_lock.h"
#include "core_heap.h"
#include "core_generic.h"


namespace mark
{
	template<typename _T, size_t _AllocCount = 128>
	struct object_pool
	{
		~object_pool()
		{
			AUTO_SPIN_LOCK lock(&_lock);
			for (auto obj : _pool)
			{
				obj->~_T(); // 객체 소멸자 호출
				CORE_SYS_FREE(obj); // 메모리 해제
			}
			_pool.clear();
		}

		[[nodiscard]] inline _T* acquire()
		{
			AUTO_SPIN_LOCK lock(&_lock);
			if (_pool.empty())
			{
				for (size_t i = 0; i < _AllocCount; ++i)
				{
					void* raw = CORE_SYS_ALLOC(sizeof(_T));
					_T* obj = new (raw) _T(); // placement new로 객체 생성
					_pool.push_back(obj);
				}
			}

			_T* obj = _pool.front();
			_pool.pop_front();
			return obj;
		}

		inline void release(_T* obj)
		{
			AUTO_SPIN_LOCK lock(&_lock);
			_pool.push_front(obj);
		}

	private:
		spin_lock_t _lock = { 0 };
		sys_deque<_T*> _pool;
		

	};

} // namespace mark

