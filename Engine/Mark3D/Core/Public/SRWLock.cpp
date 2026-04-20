#include "pch.h"
#include "SRWLock.h"

#if defined(__COMPILER_GCC) || defined(__COMPILER_CLANG)
#include <pthread.h>
#endif // __TARGET_COMPILER_GCC || __TARGET_COMPILER_CLANG


namespace mark
{
#if defined(__COMPILER_MSVC)

	void init_srw_lock(srw_lock* lock)
	{
		InitializeSRWLock(lock);
	}

	void destroy_srw_lock(srw_lock* lock)
	{
		// NO IMPLEMENT
	}

	void lock_read(srw_lock* lock)
	{
		AcquireSRWLockShared(lock);
	}

	void unlock_read(srw_lock* lock)
	{
		ReleaseSRWLockShared(lock);
	}

	void lock_write(srw_lock* lock)
	{
		AcquireSRWLockExclusive(lock);
	}

	void unlock_write(srw_lock* lock)
	{
		ReleaseSRWLockExclusive(lock);
	}

#elif defined(__COMPILER_GCC) || defined(__COMPILER_CLANG)
	void init_srw_lock(srw_lock* lock)
	{
		pthread_rwlock_init(lock, NULL);
	}

	void destroy_srw_lock(srw_lock* lock)
	{
		pthread_rwlock_destroy(lock);
	}

	void lock_read(srw_lock* lock)
	{
		pthread_rwlock_rdlock(lock);
	}

	void unlock_read(srw_lock* lock)
	{
		pthread_rwlock_unlock(lock);
	}

	void lock_write(srw_lock* lock)
	{
		pthread_rwlock_wrlock(lock);
	}

	void unlock_write(srw_lock* lock)
	{
		pthread_rwlock_unlock(lock);
	}

#endif // __COMPILER_GCC || __COMPILER_CLANG
}
