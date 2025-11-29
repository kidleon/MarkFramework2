#ifndef __SRW_LOCK_H__
#define __SRW_LOCK_H__


#if defined(__TARGET_COMPILER_MSC)
	typedef SRWLOCK srw_lock;
#elif defined(__TARGET_COMPILER_GCC) || defined(__TARGET_COMPILER_CLANG)
	typedef pthread_rwlock_t srw_lock;
#endif // __TARGET_OS_WINDOWS

/**
* @brief srw_lock을 초기화 합니다
* @param lock 초기화 할 srw_lock객체의 포인터
* @return 없음
*/
MARK_BASELIB_C_API void init_srw_lock(srw_lock* lock);


/**
* @brief srw_lock을 메모리로 부터 파괴 합니다. 이 API는 윈도우(MSVC) 플랫폼에서는 작동하지 않습니다.
* @param lock 초기화 할 srw_lock객체의 포인터
* @return 없음
*/
MARK_BASELIB_C_API void destroy_srw_lock(srw_lock* lock);


/**
* @brief read전용 lock을 겁니다.
* @param 읽기 전용 잠금 작업을 위한 srw_lock객체의 포인터
* @return 없음
*/
MARK_BASELIB_C_API void lock_read(srw_lock* lock);


/**
* @brief read lock을 unlock합니다.
* @param 읽기 전용 잠금을 해제하기 위한 srw_lock객체의 포인터
* @return 없음
*/
MARK_BASELIB_C_API void unlock_read(srw_lock* lock);


/**
* @brief write lock을 lock합니다
* @param 읽기/쓰기 잠금 작업을 위한 srw_lock객체의 포인터
* @return 없음
*/
MARK_BASELIB_C_API void lock_write(srw_lock* lock);


/**
* @brief write lock을 unlock 합니다
* @param 읽기/쓰기 잠금을 해제하기 위한 srw_lock객체의 포인터
* @return 없음
*/
MARK_BASELIB_C_API void unlock_write(srw_lock* lock);


/**
* @brief AUTO_LOCK_READ은 소멸자에 의한 자동 잠금 해제를 지원한다. 일반적인 상황에서 해당 객체 사용을 추천한다.
*/
/*
struct AUTO_LOCK_READ
{
    AUTO_LOCK_READ(srw_lock* lock)
        : _lock(lock)
    {
        lock_read(_lock);
    }

    ~AUTO_LOCK_READ()
    {
        unlock_read(_lock);
    }

    srw_lock* _lock;
};
*/

/**
* @brief AUTO_LOCK_RW은 소멸자에 의한 자동 잠금 해제를 지원한다. 일반적인 상황에서 해당 객체 사용을 추천한다.
*/
/*
struct AUTO_LOCK_RW
{
    AUTO_LOCK_RW(srw_lock* lock)
        : _lock(lock)
    {
        lock_write(_lock);
    }

    ~AUTO_LOCK_RW()
    {
        unlock_write(_lock);
    }

    srw_lock* _lock;
};
*/


#endif // __SRW_LOCK_H__