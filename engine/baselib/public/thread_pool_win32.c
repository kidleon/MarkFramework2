#include "pch.h"
#include "thread_pool.h"

#if defined(__TARGET_OS_WINDOWS)
#include <process.h>
#include "spin_lock.h"
#include "object_pool.h"
#include "interlock_atomic.h"
#include "crt_memory.h"


#define MAX_QUEUE_SIZE 128
#define THREADPOOL_MAGIC "THRDPOOL"

struct threadpool_task_t
{
	void (*task_func_arg)(void*);
	void (*task_func)();
	void* arg;
	struct threadpool_task_t* next;
};

struct threadpool_queue_t
{
	struct threadpool_task_t* front;
	struct threadpool_task_t* rear;
	size_t num_task;
	struct spin_lock_t lock;

	int padding;
};

struct threadpool_t
{
#if defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_)
	char magic[8]; // "THRDPOOL"
#endif // _DEBUG

	HANDLE* threads;
	
	struct threadpool_queue_t task_queue;
	HANDLE task_available_event;
	HANDLE shutdown_event;
	HANDLE all_tasks_done_event;  // 모든 작업 완료 이벤트
	volatile int shutdown;
	volatile LONG pending_tasks;  // 대기 중인 작업 수
	size_t num_thread;
	struct spin_lock_t pending_lock;
};

static HANDLE g_hTaskObjectPool = NULL;

void pool_init(
    size_t obj_size, 
    size_t pool_size
)
{
    g_hTaskObjectPool = create_object_pool(obj_size, pool_size, TRUE);
}

void pool_destroy()
{
    if (g_hTaskObjectPool) 
    {
        destroy_object_pool(g_hTaskObjectPool);
        g_hTaskObjectPool = NULL;
    }
}

struct threadpool_task_t* pool_alloc_task()
{
    return (struct threadpool_task_t*)object_pool_alloc(g_hTaskObjectPool);
}

void pool_release_task(
    struct threadpool_task_t* task
) 
{
    object_pool_release(g_hTaskObjectPool, task);
}

// 작업 큐 초기화
void queue_init(struct threadpool_queue_t* queue) 
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->num_task = 0;
	queue->padding = 0;
	init_spin_lock(&queue->lock);
}

// 작업 큐에 추가
int queue_push(
    struct threadpool_queue_t* queue,
    struct threadpool_task_t* task
) 
{
	acquire_spin_lock(&queue->lock);

    if (queue->num_task >= MAX_QUEUE_SIZE) 
    {
		release_spin_lock(&queue->lock);
        return 0;
    }

    task->next = NULL;
    if (!queue->rear) 
    {
        queue->front = task;
        queue->rear = task;
    }
    else
    {
        queue->rear->next = task;
        queue->rear = task;
    }

    queue->num_task++;

	release_spin_lock(&queue->lock);

    return 1;
}

// 작업 큐에서 꺼내기
struct threadpool_task_t* queue_pop(
    struct threadpool_queue_t* queue
) 
{
    acquire_spin_lock(&queue->lock);

    if (!queue->front) 
    {
        release_spin_lock(&queue->lock);
        return NULL;
    }

    struct threadpool_task_t* task = queue->front;
    queue->front = task->next;
    if (!queue->front) 
    {
        queue->rear = NULL;
    }
    queue->num_task--;

    release_spin_lock(&queue->lock);

	task->next = NULL;

    return task;
}

// 작업 큐 정리
void queue_destroy(
    struct threadpool_queue_t* queue
)
{
	struct threadpool_task_t* task;
    while (NULL != (task = queue_pop(queue))) 
    {
		memset(task, 0, sizeof(struct threadpool_task_t));
		pool_release_task(task);
    }
}

inline void increment_pending_tasks(
    struct threadpool_t* pool
) 
{
	interlock_increment_l(&pool->pending_tasks, MEMORY_ORDER_RELAXED);
    ResetEvent(pool->all_tasks_done_event);
}

inline void decrement_pending_tasks(
    struct threadpool_t* pool
)
{
    LONG count = interlock_decrement_l(&pool->pending_tasks, MEMORY_ORDER_RELAXED);
    if (0 == count) 
    {
        SetEvent(pool->all_tasks_done_event);
    }
}

// Worker 스레드 함수
unsigned __stdcall worker_thread(void* arg)
{
    struct threadpool_t* pool = (struct threadpool_t*)arg;

    HANDLE events[2];
    events[0] = pool->task_available_event;
    events[1] = pool->shutdown_event;

    for(;;)
    {
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE); // 작업 또는 종료 신호 대기

		if (result == WAIT_OBJECT_0 + 1)  // 종료 신호 수신
            break;

        // 작업 처리
        struct threadpool_task_t* task;
        while (NULL != (task = queue_pop(&pool->task_queue))) 
        {
            if (NULL != task->task_func)
                task->task_func();

            if (NULL != task->task_func_arg)
                task->task_func_arg(task->arg);

            memset(task, 0, sizeof(struct threadpool_task_t));
            pool_release_task(task);

            // 작업 완료 후 카운터 감소
            decrement_pending_tasks(pool);
        }
        
		acquire_spin_lock(&pool->task_queue.lock);
        int is_empty = (pool->task_queue.num_task == 0); // 큐가 비었으면 이벤트를 리셋 해야 한다.
		release_spin_lock(&pool->task_queue.lock);

        if (is_empty) 
            ResetEvent(pool->task_available_event);
    }

    return 0;
}

inline BOOL is_valid_threadpool(struct threadpool_t* pool)
{
#if defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_)
    return (pool && memcmp(pool->magic, THREADPOOL_MAGIC, sizeof(pool->magic)) == 0);
#else 
	return (pool != NULL);
#endif 
}

// 스레드 풀 생성
HANDLE threadpool_create(size_t num_threads) 
{
    struct threadpool_t* pool = (struct threadpool_t*)crt_malloc(sizeof(struct threadpool_t));
    if (!pool) 
        return NULL;

#if defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_)
	memcpy(pool->magic, THREADPOOL_MAGIC, sizeof(pool->magic));
#endif // _DEBUG

    pool->num_thread = num_threads;
    pool->threads = (HANDLE*)crt_malloc(sizeof(HANDLE) * num_threads);
    pool->shutdown = 0;
    pool->pending_tasks = 0;

    // 작업 큐 초기화
	pool_init(sizeof(struct threadpool_task_t), MAX_QUEUE_SIZE * 2);
    queue_init(&pool->task_queue);

	init_spin_lock(&pool->pending_lock);

    // 이벤트 생성
    pool->task_available_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    pool->shutdown_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    pool->all_tasks_done_event = CreateEvent(NULL, TRUE, TRUE, NULL);  // 초기 상태: 시그널

    if (!pool->task_available_event ||
        !pool->shutdown_event ||
        !pool->all_tasks_done_event
    )
    {
        crt_free(pool->threads);
        crt_free(pool);
        return NULL;
    }

    // Worker 스레드 생성
    for (int i = 0; i < num_threads; i++) 
    {
        pool->threads[i] = (HANDLE)_beginthreadex(
            NULL,
            0,
            worker_thread,
            pool,
            0,
            NULL
        );
    }
    
    return (HANDLE)pool;
}

int threadpool_add_task(
    HANDLE pool,
    void (*task_func)(void*)
)
{
    struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return -1;

    if (thread_pool->shutdown)
        return 0;

    struct threadpool_task_t* task = pool_alloc_task();
    if (!task)
        return 0;

    task->task_func = task_func;
    task->task_func_arg = NULL;
    task->arg = NULL;
    task->next = NULL;

    increment_pending_tasks(thread_pool); // 작업 추가 전 카운터 증가

    if (!queue_push(&thread_pool->task_queue, task))
    {
        pool_release_task(task);
        decrement_pending_tasks(thread_pool);  // 실패 시 카운터 감소
        return 0;
    }

    // 작업이 추가되었음을 알림
    SetEvent(thread_pool->task_available_event);

    return 1;
}

// 작업 추가
int threadpool_add_task_arg(
    HANDLE pool, 
    void (*task_func)(void*),
    void* arg
)
{
	struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return -1;

    if (thread_pool->shutdown) 
        return 0;

    struct threadpool_task_t* task = pool_alloc_task();
    if (!task)
        return 0;

    task->task_func_arg = task_func;
    task->arg = arg;
	task->task_func = NULL;
	task->next = NULL;

    increment_pending_tasks(thread_pool); // 작업 추가 전 카운터 증가

    if (!queue_push(&thread_pool->task_queue, task))
    {
		pool_release_task(task);
        decrement_pending_tasks(thread_pool);  // 실패 시 카운터 감소
        return 0;
    }

    // 작업이 추가되었음을 알림
    SetEvent(thread_pool->task_available_event);

    return 1;
}

// 모든 작업 완료 대기
void threadpool_wait(HANDLE pool) 
{
    struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return;

    WaitForSingleObject(thread_pool->all_tasks_done_event, INFINITE);
}

// 타임아웃을 지정하여 대기
BOOL threadpool_wait_timeout(HANDLE pool, uint32 milliseconds) 
{
    struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return FALSE;

    DWORD result = WaitForSingleObject(thread_pool->all_tasks_done_event, milliseconds);

    if (result == WAIT_OBJECT_0)
    {
        return TRUE;
    }
    else if (result == WAIT_TIMEOUT) 
    {
		// 타임아웃 발생, 아직 작업이 남아있음
        return FALSE;
    }

    return FALSE;
}

// 현재 대기 중인 작업 수 조회
int threadpool_get_pending_count(HANDLE pool) 
{
    struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return -1;

    return thread_pool->pending_tasks;
}

// 모든 대기 중인 작업 취소 (실행 중인 작업은 완료될 때까지 대기)
int threadpool_cancel_all_tasks(HANDLE pool)
{
    struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return 0;

	acquire_spin_lock(&thread_pool->task_queue.lock);

    // 큐에 있는 모든 작업 삭제
    int cancelled_count = 0;
    struct threadpool_task_t* task;
    while (NULL != thread_pool->task_queue.front)
    {
        task = thread_pool->task_queue.front;
        thread_pool->task_queue.front = task->next;

        memset(task, 0, sizeof(struct threadpool_task_t));
        pool_release_task(task);

        cancelled_count++;
        thread_pool->task_queue.num_task--;

        // pending_tasks 감소
        interlock_decrement_l(&thread_pool->pending_tasks, MEMORY_ORDER_RELAXED);
    }

    thread_pool->task_queue.rear = NULL;
    thread_pool->task_queue.num_task = 0;

	release_spin_lock(&thread_pool->task_queue.lock);

    // 이벤트 리셋
    ResetEvent(thread_pool->task_available_event);

    // 모든 작업이 취소되었으면 완료 이벤트 시그널
    if (0 == thread_pool->pending_tasks) 
    {
        SetEvent(thread_pool->all_tasks_done_event);
    }

    return cancelled_count;
}

// 스레드 풀 종료
void threadpool_destroy(HANDLE pool)
{
    struct threadpool_t* thread_pool = (struct threadpool_t*)pool;

    if (!is_valid_threadpool(thread_pool))
        return;

    thread_pool->shutdown = 1; // 종료 플래그 설정
    SetEvent(thread_pool->shutdown_event); // 모든 스레드에 종료 신호 전송

    // 모든 스레드가 종료될 때까지 대기
    WaitForMultipleObjects(
        (DWORD)thread_pool->num_thread, 
        thread_pool->threads, 
        TRUE, 
        INFINITE
    );

    // 리소스 정리
    for (int i = 0; i < thread_pool->num_thread; i++) 
    {
        CloseHandle(thread_pool->threads[i]);
    }

    CloseHandle(thread_pool->task_available_event);
    CloseHandle(thread_pool->shutdown_event);
    CloseHandle(thread_pool->all_tasks_done_event);
    queue_destroy(&thread_pool->task_queue);
	pool_destroy();

    crt_free(thread_pool->threads);
    crt_free(thread_pool);
}

#endif // __THREAD_POOL_WIN32_C__