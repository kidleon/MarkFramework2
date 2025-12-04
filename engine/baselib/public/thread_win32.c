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
	HANDLE work_event;
	HANDLE wait_event;
	volatile BOOL terminate;
	struct spin_lock_t lock;
};

unsigned __stdcall func_thread(void* arg)
{
	struct thread_t* thread = (struct thread_t*)arg;

	for (;;)
	{
		DWORD result = WaitForSingleObject(thread->work_event, INFINITE); // 작업 또는 종료 신호 대기

		if (result != WAIT_OBJECT_0)
			break;

		if (thread->terminate)
			break;

		thread->thread_func(thread->arg);

		ResetEvent(thread->work_event);
		SetEvent(thread->wait_event);
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
	thread->work_event = CreateEvent(NULL, TRUE, FALSE, NULL);
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

	interlock_store_l((LONG*)&thread->terminate, TRUE, MEMORY_ORDER_ACQ_REL);

	SetEvent(thread->wait_event);

	// 종료까지 무한 대기
	WaitForSingleObject(thread->termiate_event, INFINITE);

	// 핸들 닫기
	CloseHandle(thread->wait_event);
	CloseHandle(thread->work_event);
	CloseHandle(thread->termiate_event);
	CloseHandle((HANDLE)thread->handle);

	crt_free(thread);
}

void thread_execute(
	HANDLE handle
)
{
	struct thread_t* thread = (struct thread_t*)handle;

	if (!is_valid_thread(thread))
		return;

	if (thread->terminate)
		return;

	ResetEvent(thread->wait_event);
	SetEvent(thread->work_event);
}

void thread_wait(
	HANDLE handle
)
{
	struct thread_t* thread = (struct thread_t*)handle;
	if (!is_valid_thread(thread))
		return;

	WaitForSingleObject(thread->wait_event, INFINITE);
}


#endif // __TARGET_OS_WINDOWS