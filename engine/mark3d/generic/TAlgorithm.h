#ifndef __TALGORITHM_H__
#define __TALGORITHM_H__

#include <algorithm>


namespace mark
{
	template<typename _Tval, typename Pred>
	inline int __private_quick_sort(_Tval* data, int lo, int hi, Pred pred)
	{
		int i = lo;
		const _Tval& pivot_value = data[hi];

		for (int j = lo; j < hi; ++j)
		{
			if (pred(data[j], pivot_value))
			{
				std::swap(data[i], data[j]);
				i++;
			}
		}

		std::swap(data[i], data[hi]);

		return i;
	}

	template<typename _Tval, typename Pred>
	inline void __private_insert_sort(_Tval* data, int lo, int hi, Pred pred)
	{
		int j;
		for (int i = lo; i < hi + 1; ++i)
		{
			j = i;
			while (j > 0 && pred(data[j], data[j - 1]))
			{
				std::swap(data[j], data[j - 1]);
				j--;
			}
		}
	}

	template<typename _Tval, typename Pred>
	inline void __private_heap_sort_self(_Tval* data, int base, int count, Pred pred)
	{
		int largest = base;
		int l = (base << 1) + 1; //left
		int r = (base << 1) + 2; //right

		// 왼쪽 자식이 현재 가장 큰 값보다 크면
		if (l < count && pred(data[largest], data[l]))
		{
			largest = l;
		}

		// 오른쪽 자식이 현재 가장 큰 값보다 크면
		if (r < count && pred(data[largest], data[r]))
		{
			largest = r;
		}

		// 가장 큰 값이 루트가 아니면
		if (largest != base)
		{
			std::swap(data[base], data[largest]);

			// 재귀적으로 힙 정렬 호출
			__private_heap_sort_self(data, largest, count, pred);
		}
	}

	template<typename _Tval, typename Pred>
	inline void __private_heap_sort(_Tval* data, int lo, int hi, Pred pred)
	{
		// 힙 생성 (배열 재정렬)
		int count = hi - lo + 1;

		for (int i = hi; i >= lo; i--)
		{
			__private_heap_sort_self(data, i, count, pred);
		}

		//힙에서 하나씩 요소 추출
		for (int i = hi; i >= lo; i--)
		{
			//현재 루트를 끝으로 이동
			std::swap(data[0], data[i]);

			//축소된 힙에서 최대 힙화 호출
			__private_heap_sort_self(data, 0, i, pred);
		}
	}

	template<typename _Tval, typename Pred>
	inline void __private_smart_sort(_Tval* data, int lo, int hi, int divlv, Pred pred)
	{
		if (lo >= hi)
			return;

		for (;;)
		{
			if (hi - lo <= 32)
			{
				// 요소가 32개 이하일 경우 삽입 정렬
				__private_insert_sort(data, lo, hi, pred);
				return;
			}

			if (divlv <= 0)
			{
				// 너무 많은 분할이 발생하면 힙 정렬 실행
				__private_heap_sort(data, lo, hi, pred);
				return;
			}

			divlv = (divlv >> 1) + (divlv >> 2); // 1.5 log2(N) 분할

			// 퀵소트 실행후 피벗 기준으로 스마트 정렬 재귀 호출
			int pivot = __private_quick_sort(data, lo, hi, pred);
			__private_smart_sort(data, lo, pivot - 1, divlv, pred);
			__private_smart_sort(data, pivot + 1, hi, divlv, pred);
		}
	}

	// 적응형 정렬 알고리즘
	template<typename _Tval, typename Pred>
	inline void sort(_Tval* data, size_t count, Pred pred)
	{
		__private_smart_sort(data, 0, (int)count - 1, (int)count, pred);
	}

	// 이진 탐색 알고리즘
	template<typename _Tval, typename Comp, typename ...Args>
	inline int binary_search(_Tval* data, size_t Count, Comp comp, Args... args)
	{
		int low = 0;
		int high = (int)Count - 1;

		while (low <= high)
		{
			int middle = (low + high) >> 1;
			if (comp(data[middle], std::forward<Args>(args)...) == 0)
			{
				return middle;
			}
			else if (comp(data[middle], std::forward<Args>(args)...) < 0)
			{
				low = middle + 1;
			}
			else if (comp(data[middle], std::forward<Args>(args)...) > 0)
			{
				high = middle - 1;
			}
		}

		return -1;
	}

	// 이진 탐색 알고리즘
	template<typename _Tval, typename Comp>
	inline int binary_search(_Tval* data, size_t count, const _Tval& target, Comp comp)
	{
		int low = 0;
		int high = (int)count - 1;

		while (low <= high)
		{
			int middle = (low + high) >> 1;
			if (comp(data[middle], target) == 0)
			{
				return middle;
			}
			else if (comp(data[middle], target) < 0)
			{
				low = middle + 1;
			}
			else if (comp(data[middle], target) > 0)
			{
				high = middle - 1;
			}
		}

		return -1;
	}

	// 이진 탐색 알고리즘
	template<typename _Tval, typename Comp>
	inline int binary_search_first(_Tval* data, size_t count, const _Tval& target, Comp comp)
	{
		int low = 0;
		int high = (int)count - 1;
		int first = -1;

		while (low <= high)
		{
			int middle = (low + high) >> 1;

			if (comp(data[middle], target) == 0)
			{
				first = middle;
				high = middle - 1;
			}
			else if (comp(data[middle], target) < 0)
			{
				low = middle + 1;
			}
			else if (comp(data[middle], target) > 0)
			{
				high = middle - 1;
			}
		}

		return first;
	}

	// 이진 탐색 알고리즘
	template<typename _Tval, typename Comp>
	inline int binary_search_last(_Tval* data, size_t count, const _Tval& target, Comp comp)
	{
		int low = 0;
		int high = (int)count - 1;
		int last = -1;

		while (low <= high)
		{
			int middle = (low + high) >> 1;

			if (comp(data[middle], target) == 0)
			{
				last = middle;
				low = middle + 1;
			}
			else if (comp(data[middle], target) < 0)
			{
				low = middle + 1;
			}
			else if (comp(data[middle], target) > 0)
			{
				high = middle - 1;
			}
		}

		return last;
	}
}

#endif // __TALGORITHM_H__
