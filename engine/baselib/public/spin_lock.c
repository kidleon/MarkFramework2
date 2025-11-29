#include "pch.h"
#include "spin_lock.h"
#include "interlock_atomic.h"

#if defined(__TARGET_COMPILER_GCC)
#	include <sched.h>
#endif // __TARGET_COMPILER_GCC


#define SPIN_THREAD_WAIT 0
#define SPIN_THREAD_WORKING 1
#define MAX_YIELD_ROUND 4
#define MAX_YIELD_COUNT 32

static inline void YEILD_PROCESSOR()
{
#if defined(__TARGET_COMPILER_MSC)
	YieldProcessor();
#elif defined(__TARGET_COMPILER_GCC)
#	if defined(__X86_64__) || defined(i386) || defined(__amd64__)
	__asm__ volatile("pause");
#	elif defined(__arm__) || defined(__aarch64__)
	__asm__ volatile("yield");
#	else
#	warning Unsupprted architecture
#	endif // X64_64...
#endif // TARGET_COMPILER_MSC
}

static inline void SWITH_TO_THREAD()
{
#if defined(__TARGET_COMPILER_MSC)
	SwitchToThread();
#elif defined(__TARGET_COMPILER_GCC)
	sched_yield();
#endif
}

void init_spin_lock(
	struct spin_lock_t* spin_lock
)
{
	spin_lock->stat = SPIN_THREAD_WAIT;
}

void acquire_spin_lock(
	struct spin_lock_t* spin_lock
)
{
	BOOL successed = FALSE;
	volatile long round = 0;

LOCK_TRY:
	successed = interlock_compare_exchange_l(
		&spin_lock->stat, 
		SPIN_THREAD_WORKING, 
		SPIN_THREAD_WAIT, 
		MEMORY_ORDER_ACQUIRE
	);

	if (!successed)
	{
		if (round++ < MAX_YIELD_ROUND)
		{
			for (UINT32 i = 0; i < MAX_YIELD_COUNT; ++i)
				YEILD_PROCESSOR();
		}
		else
		{
			round = 0;
			SWITH_TO_THREAD();
		}

		goto LOCK_TRY;
	}
}

void release_spin_lock(
	struct spin_lock_t* spin_lock
)
{
	interlock_store_l(
		&spin_lock->stat, 
		SPIN_THREAD_WAIT, 
		MEMORY_ORDER_RELEASE
	);
}

