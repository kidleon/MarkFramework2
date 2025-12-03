#include "pch.h"
#include "thread.h"

#if defined(__TARGET_OS_WINDOWS)

#include <process.h>
#include "spin_lock.h"
#include "interlock_atomic.h"
#include "crt_memory.h"


#define THREAD_MAGIC "THREAD!_"

struct thread_t
{
#if defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_)
	char magic[8]; // "THREAD!_"
#endif // _DEBUG

	void (*thread_func)(void*);
	void* arg;
	uintptr_t handle;
	HANDLE termiate_event;
	HANDLE wait_event;
	volatile BOOL terminate;
	struct spin_lock_t lock;
};

unsigned __stdcall func_thread(void* arg)
{
	struct thread_t* thread = (struct thread_t*)arg;

	for (;;)
	{
		acquire_spin_lock(&thread->lock);
		BOOL term = thread->terminate;
		release_spin_lock(&thread->lock);

		if (term)
			break;

		thread->thread_func(thread->arg);

		Sleep(1);
	}

	SetEvent(thread->termiate_event);

	return 0;
}

inline BOOL is_valid_thread(struct thread_t* thread)
{
#if defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_)
	return (thread && memcmp(thread->magic, THREAD_MAGIC, sizeof(thread->magic)) == 0);
#else 
	return (thread != NULL);
#endif 
}

HANDLE thread_create(
	void (*thread_func)(void*),
	void* arg
)
{
	struct thread_t* thread = (struct thread_t*)crt_malloc(sizeof(struct thread_t));

#if defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_)
	memcpy(thread->magic, THREAD_MAGIC, sizeof(thread->magic));
#endif // _DEBUG

	thread->thread_func = thread_func;
	thread->arg = arg;
	thread->terminate = FALSE;
	init_spin_lock(&thread->lock);

	thread->termiate_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	thread->wait_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	thread->handle = _beginthreadex(
		NULL,
		0,
		func_thread,
		thread,
		0,
		NULL
	);

	return (HANDLE)thread;
}

void thread_destroy(
	HANDLE handle
)
{
	struct thread_t* thread = (struct thread_t*)handle;

	if (!is_valid_thread(thread))
		return;

	acquire_spin_lock(&thread->lock);
	thread->terminate = TRUE;
	release_spin_lock(&thread->lock);

	// 종료까지 무한 대기
	WaitForSingleObject(thread->termiate_event, INFINITE);

	// 핸들 닫기
	CloseHandle(thread->wait_event);
	CloseHandle(thread->termiate_event);
	CloseHandle((HANDLE)thread->handle);

	crt_free(thread);

}

#endif // __TARGET_OS_WINDOWS