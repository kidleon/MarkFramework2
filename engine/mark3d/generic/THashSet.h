#ifndef __THASH_SET_H__
#define __THASH_SET_H__

#include "TAllocator.h"
#include "TCommon.h"
#include "THash.h"


namespace mark
{
	/**
	* THashSet
	* @brief 템플릿 해시 셋 클래스.
	* 해시 셋은 고유한 값을 저장하는 데이터 구조로, 값의 존재 여부를 빠르게 확인할 수 있습니다.
	* 이 구현은 메모리 할당 유형을 지정할 수 있는 템플릿 클래스로, 다양한 데이터 타입에 대해 사용할 수 있습니다.
	* @tparam _T 해시 셋의 값 타입
	* @tparam HashType 해시 함수 유형 (기본값: HASH_TYPE::FNV)
	* @tparam NUM_BUCKET 버킷 수 (기본값: 32)
	* @tparam _AllocType 메모리 할당 유형 (기본값: ALLOC_TYPE::SYSCALL)
	*/
	template<typename _Tval, HASH_TYPE HashType = HASH_TYPE::FNV, size_t NUM_BUCKET = 32, ALLOC_TYPE _AllocType = ALLOC_TYPE::SYSCALL>
	struct THashSet
	{
		template<typename _Tval>
		struct __HashSet_Node
		{
			_Tval val;
			HASH hash;

			__HashSet_Node* prev;
			__HashSet_Node* next;
			__HashSet_Node* chain_prev;
			__HashSet_Node* chain_next;
		};
	};

}

#endif // __THASH_SET_H__