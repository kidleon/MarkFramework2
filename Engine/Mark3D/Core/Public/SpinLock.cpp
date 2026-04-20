#include "pch.h"
#include "SpinLock.h"

#ifndef __TARGET_OS_WINDOWS
#	include <sched.h>
#	include <stdio.h>
#endif // __TARGET_OS_WINDOWS

namespace mark
{
	constexpr static long SPIN_THREAD_WAIT = 0;
	constexpr static long SPIN_THREAD_WORKING = 1;
	constexpr static long MAX_YIELD_ROUND = 4;
	constexpr static long MAX_YIELD_COUNT = 32;

	static inline void YEILD_PROCESSOR()
	{
#if defined(__COMPILER_MSVC)
		YieldProcessor();
#elif defined(__COMPILER_GCC) || defined(__COMPILER_CLANG)
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
#if defined(__COMPILER_MSVC)
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
#if defined(__COMPILER_MSVC)
		successed = ::InterlockedCompareExchange(
			&spin_lock->stat,
			SPIN_THREAD_WORKING,
			SPIN_THREAD_WAIT
		) == SPIN_THREAD_WAIT;
#else
		successed = __atomic_compare_exchange_n
			&spin_lock->stat,
			SPIN_THREAD_WAIT,
			SPIN_THREAD_WORKING,
			__ATOMIC_ACQUIRE,
			__ATOMIC_RELAXED
		);
#endif // TARGET_COMPILER_MSC

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
#if defined(__COMPILER_MSVC)
		::InterlockedExchange(&spin_lock->stat, SPIN_THREAD_WAIT);
#else
		_atomic_exchange_n(&spin_lock->stat, SPIN_THREAD_WAIT, __ATOMIC_RELEASE);
#endif // TARGET_COMPILER_MSC
	}

}
