#ifndef __SPIN_LOCK_H__
#define __SPIN_LOCK_H__


/**
* @brief spin_lock은 atomic 연산을 이용하여 쓰레드 잠금을 시도한다. 잠금이 실패 할 경우 일정 횟수만큼 시도하다가 yield(대기)로 넘어간다. 해당 lock은 재귀 잠금을 지원하지 않는다.
*/
struct spin_lock_t
{
	volatile LONG stat;
};


/**
* @brief spin_lock을 초기화 합니다
* @param spin_lock 초기화 할 spin_lock객체의 포인터
* @return 없음
*/
MARKENGINE_C_API void init_spin_lock(
	struct spin_lock_t* spin_lock
);

/**
* @brief spin_lock 잠금을 시도 합니다. 잠금이 실패 할경우 다른 Thread로 작업이 넘어갑니다.
* @param spin_lock 잠금을 시도 할 spin_lock객체의 포인터
* @return 없음
*/
MARKENGINE_C_API void acquire_spin_lock(
	struct spin_lock_t* spin_lock
);

/**
* @brief spin_lock 잠금을 해제 힙니다.
* @param spin_lock 잠금을 해제 할 spin_lock객체의 포인터
* @return 없음
*/
MARKENGINE_C_API void release_spin_lock(
	struct spin_lock_t* spin_lock
);


/**
* @brief AUTO_SPIN_LOCK은 소멸자에 의한 자동 잠금 해제를 지원한다. 일반적인 상황에서 해당 객체 사용을 추천한다.
*/
struct AUTO_SPIN_LOCK
{
	AUTO_SPIN_LOCK(struct spin_lock_t* spin_lock)
		: _sp(spin_lock)
	{
		acquire_spin_lock(_sp);
	}

	~AUTO_SPIN_LOCK()
	{
		if (_sp)
		{
			release_spin_lock(_sp);
		}
	}

	struct spin_lock_t* _sp;
};


#endif // __SPIN_LOCK_H__
