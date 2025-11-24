#ifndef __INTERLOCK_ATOMIC_H__
#define __INTERLOCK_ATOMIC_H__


enum class MEMORY_ORDER
{
	MEMORY_ORDER_RELAXED, // 동기화 또는 순서 보장이 없이 원자적 연산을 수행합니다. 원자적 연산은 보장하지만 순서의 보장은 하지 않기 때문에 다른 쓰레드에서 값이 원하는 값이 아닐수 있습니다. 가장 빠른 속도를 가지며 주로 단순한 통계 업데이트에 주로 쓰인다.
	MEMORY_ORDER_ACQUIRE, // 원자적 연산의 메모리 순서를 보장하는 옵션으로 Acquire-Release와 함께 사용해야 의미가 있다. Acquire와 Release사이의 연산은 메모리 순서 보장으로 인해 Thread Safe합니다.
	MEMORY_ORDER_RELEASE, // Acquire를 사용한 이후 Release를 반드시 호출하여 메모리 순서를 보장하며 일종의 unlock기능을 수행할 수 있다.
	MEMORY_ORDER_ACQ_REL  // Acquire와 Release가 동시에 일어난다, 읽기-변경-쓰기가 동시에 필요한 경우에 사용한다.
};

/**
* @brief LONG값을 target에 원자적 연산으로 저장 합니다.
* @param target 저장하고자 하는 volatile long 목표변수
* @param value 저장할 값 (long)
* @param order Memory 순서의 방식을 정한다.
* @return value파라미터가 target에 저장 된 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API LONG interlock_store_l(
	LONG volatile* target,
	LONG value,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief ULONG값을 target에 원자적 연산으로 저장 합니다.
* @param target 저장하고자 하는 volatile ULONG 목표변수
* @param value 저장할 값 (ULONG)
* @param order Memory 순서의 방식을 정한다.
* @return value파라미터가 target에 저장 된 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API ULONG interlock_store_ul(
	ULONG volatile* target,
	ULONG value,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief LONG64값을 target에 원자적 연산으로 저장 합니다.
* @param target 저장하고자 하는 volatile LONG64 목표변수
* @param value 저장할 값 (LONG64)
* @param order Memory 순서의 방식을 정한다.
* @return value파라미터가 target에 저장 된 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API LONG64 interlock_store_l64(
	LONG64 volatile* target,
	LONG64 value,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief ULONG64값을 target에 원자적 연산으로 저장 합니다.
* @param target 저장하고자 하는 volatile ULONG64 목표변수
* @param value 저장할 값 (ULONG64)
* @param order Memory 순서의 방식을 정한다.
* @return value파라미터가 target에 저장 된 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API ULONG64 interlock_store_ul64(
	ULONG64 volatile* target,
	ULONG64 value,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 증가 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1증가된 연산 '이후 값'을 리턴한다. 
*/
MARKENGINE_C_API LONG interlock_increment_l(
	LONG volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 증가 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1증가된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API ULONG interlock_increment_ul(
	ULONG volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 증가 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1증가된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API LONG64 interlock_increment_l64(
	LONG64 volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 증가 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1증가된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API ULONG64 interlock_increment_ul64(
	ULONG64 volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 감소 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1 감소 된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API LONG interlock_decrement_l(
	LONG volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 감소 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1 감소 된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API ULONG interlock_decrement_ul(
	ULONG volatile* target,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 감소 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1 감소 된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API LONG64 interlock_decrement_l64(
	LONG64 volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값을 원자적 연산으로 1을 감소 시킨다.
* @param target 연산 하고자 하는 목표변수
* @param order Memory 순서의 방식을 정한다.
* @return store연산과 달리 1 감소 된 연산 '이후 값'을 리턴한다.
*/
MARKENGINE_C_API ULONG64 interlock_decrement_ul64(
	ULONG64 volatile* target, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 ADD연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 ADD를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API LONG interlock_fetch_add_l(
	LONG volatile* target, 
	LONG value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 ADD연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 ADD를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API ULONG interlock_fetch_add_ul(
	ULONG volatile* target, 
	ULONG value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 ADD연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 ADD를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API LONG64 interlock_fetch_add_l64(
	LONG64 volatile* target, 
	LONG64 value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 ADD연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 ADD를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API ULONG64 interlock_fetch_add_ul64(
	ULONG64 volatile* target, 
	ULONG64 value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 SUB연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 SUB를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API LONG interlock_fetch_sub_l(
	LONG volatile* target, 
	LONG value,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 SUB연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 SUB를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API ULONG interlock_fetch_sub_ul(
	ULONG volatile* target,
	ULONG value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 SUB연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 SUB를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API LONG64 interlock_fetch_sub_l64(
	LONG64 volatile* target, 
	LONG64 value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target에 value값을 원자적 SUB연산을 한다.
* @param target 연산이 저장되는 목표변수
* @param value 원자적 연산을 하기 위한 Value값
* @param order Memory 순서의 방식을 정한다.
* @return value과 target의 값이 원자적 SUB를 하기 '이전 값'을 리턴한다. 리턴값을 통해 실제로 바뀌었는지 비교가 가능하다.
*/
MARKENGINE_C_API ULONG64 interlock_fetch_sub_ul64(
	ULONG64 volatile* target, 
	ULONG64 value, 
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_RELAXED
);

/**
* @brief target의 값이 comperand값과 같으면 exchange값을 target에 원자적 연산을 통해 저장한다.
* @param target 원자적 비교 및 저장을 위한 파라메터
* @param exchange target에 세팅하고자 하는 값
* @param comperand target과 비교하고자 하는 값. 이 값과 target의 값이 같으면 exchange값이 target에 저장된다.
* @param order Memory 순서의 방식을 정한다.
* @return target의 값과 comperand의 값이 같아 exchange값이 target에 저장되면 TRUE, 값이 달라 저장이 실패하면 FALSE를 리턴한다.
*/
MARKENGINE_C_API BOOL interlock_compare_exchange_l(
	LONG volatile* target,
	LONG exchange,
	LONG comperand,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_ACQ_REL
);

/**
* @brief target의 값이 comperand값과 같으면 exchange값을 target에 원자적 연산을 통해 저장한다.
* @param target 원자적 비교 및 저장을 위한 파라메터
* @param exchange target에 세팅하고자 하는 값
* @param comperand target과 비교하고자 하는 값. 이 값과 target의 값이 같으면 exchange값이 target에 저장된다.
* @param order Memory 순서의 방식을 정한다.
* @return target의 값과 comperand의 값이 같아 exchange값이 target에 저장되면 TRUE, 값이 달라 저장이 실패하면 FALSE를 리턴한다.
*/
MARKENGINE_C_API BOOL interlock_compare_exchange_ul(
	ULONG volatile* target,
	ULONG exchange,
	ULONG comperand,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_ACQ_REL
);

/**
* @brief target의 값이 comperand값과 같으면 exchange값을 target에 원자적 연산을 통해 저장한다.
* @param target 원자적 비교 및 저장을 위한 파라메터
* @param exchange target에 세팅하고자 하는 값
* @param comperand target과 비교하고자 하는 값. 이 값과 target의 값이 같으면 exchange값이 target에 저장된다.
* @param order Memory 순서의 방식을 정한다.
* @return target의 값과 comperand의 값이 같아 exchange값이 target에 저장되면 TRUE, 값이 달라 저장이 실패하면 FALSE를 리턴한다.
*/
MARKENGINE_C_API BOOL interlock_compare_exchange_l64(
	LONG64 volatile* target,
	LONG64 exchange,
	LONG64 comperand,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_ACQ_REL
);

/**
* @brief target의 값이 comperand값과 같으면 exchange값을 target에 원자적 연산을 통해 저장한다.
* @param target 원자적 비교 및 저장을 위한 파라메터
* @param exchange target에 세팅하고자 하는 값
* @param comperand target과 비교하고자 하는 값. 이 값과 target의 값이 같으면 exchange값이 target에 저장된다.
* @param order Memory 순서의 방식을 정한다.
* @return target의 값과 comperand의 값이 같아 exchange값이 target에 저장되면 TRUE, 값이 달라 저장이 실패하면 FALSE를 리턴한다.
*/
MARKENGINE_C_API BOOL interlock_compare_exchange_ul64(
	ULONG64 volatile* target,
	ULONG64 exchange,
	ULONG64 comperand,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_ACQ_REL
);

/**
* @brief target의 포인터와 comperand 포인터가 같으면 exchange포인터를 target에 원자적 연산을 통해 저장한다.
* @param target 원자적 비교 및 저장을 위한 파라미터
* @param exchange target에 세팅하고자 하는 포인터
* @param comperand target과 비교하고자 하는 포인터. 이 포인터와 target의 포인터가 같으면 exchange 포인터가 target에 저장된다.
* @param order Memory 순서의 방식을 정한다.
* @return target의 포인터와 comperand의 포인터가 같아 exchange 포인터가 target에 저장되면 TRUE, 포인터가 달라 저장이 실패하면 FALSE를 리턴한다.
*/
MARKENGINE_C_API BOOL interlock_compare_exchange_ptr(
	void* volatile* target,
	void* exchange,
	void* comperand,
	MEMORY_ORDER order = MEMORY_ORDER::MEMORY_ORDER_ACQ_REL
);


#endif // __INTERLOCK_ATOMIC_H__
