#ifndef __T_OBJECT_POOL_H__
#define __T_OBJECT_POOL_H__


template <typename T>
class TObjectPool
{
public:
	TObjectPool() = default;
	~TObjectPool() = default;

	T* Allocate()
	{
		if (!m_FreeList.empty())
		{
			T* obj = m_FreeList.back();
			m_FreeList.pop_back();
			return obj;
		}
		return new T();
	}
	void Deallocate(T* obj)
	{
		m_FreeList.push_back(obj);
	}

	void Clear()
	{
		for (T* obj : m_FreeList)
		{
			delete obj;
		}
		m_FreeList.clear();
	}

private:
	TArray<T*> m_FreeList;

};

#endif // __T_OBJECT_POOL_H__
