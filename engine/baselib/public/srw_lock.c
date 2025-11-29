#include "pch.h"
#include "srw_lock.h"


#if defined(__TARGET_COMPILER_MSC)

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

#elif defined(__TARGET_COMPILER_GCC) || defined(__TARGET_COMPILER_CLANG)

#include <pthread.h>

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

#endif // __TARGET_COMPILER_GCC