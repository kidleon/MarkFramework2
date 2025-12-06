#include "pch.h"
#include "stack_pool.h"
#include "spin_lock.h"
#include "crt_memory.h"


#define STACKPOOL_MAGIC 0x5354434B  // 'STCK
#define STACKPOOL_ALIGNMENT 16

#define VALIDATE_POOL(pool) \
    do { \
        assert((pool) != NULL && "Invalid pool handle!"); \
        assert((pool)->magic == STACKPOOL_MAGIC && "Corrupted pool handle!"); \
    } while(0)

struct stackpool_header_t 
{
	MEM_SIZE size;           // 할당된 크기
	MEM_SIZE aligned_size;   // 정렬된 크기
};

struct stackpool_t
{
	uint32 magic; // 매직 넘버
	struct spin_lock_t lock;
	void* pool; // 메모리 풀
	MEM_SIZE capacity; // 전체 용량
	MEM_SIZE used; // 사용 중인 크기
	void* stack_ptr; // 현재 스택 포인터 (top)
	void* base_ptr; // 베이스 포인터 (bottom)
	BOOL thread_safe; // 스레드 세이프 모드 여부
	uint32 PADDING; // 패딩
};

HANDLE stackpool_create(
	MEM_SIZE pool_size,
	BOOL thread_safe
)
{
    if (!pool_size)
        return NULL;

    struct stackpool_t* pool = (struct stackpool_t*)crt_malloc(sizeof(struct stackpool_t));
    if (!pool)
        return NULL;

	pool_size = ALIGNED_SIZE(pool_size, STACKPOOL_ALIGNMENT);

    pool->pool = crt_malloc_align(pool_size, STACKPOOL_ALIGNMENT);
    if (!pool->pool)
	{
        free(pool);
        return NULL;
    }

	memset(pool->pool, 0, pool_size);

    // 초기화
    pool->magic = STACKPOOL_MAGIC;
    pool->capacity = pool_size;
    pool->used = 0;
    pool->base_ptr = pool->pool;
    pool->stack_ptr = pool->pool;
	pool->thread_safe = thread_safe;
	init_spin_lock(&pool->lock);

    return (HANDLE)pool;
}

void stackpool_destroy(
	HANDLE handle
)
{
	if (!handle)
		return;

	struct stackpool_t* pool = (struct stackpool_t*)handle;

	VALIDATE_POOL(pool);

	if (pool->used > 0) 
	{
		assert(FALSE && "Memory leak detected in stack pool!");
	}

	pool->magic = 0;

	crt_free_align(pool->pool);
	crt_free(pool);
}

STACK_MARKER stackpool_getmarker(
	HANDLE handle
)
{
	struct stackpool_t* pool = (struct stackpool_t*)handle;
	VALIDATE_POOL(pool);

    if(pool->thread_safe)
		acquire_spin_lock(&pool->lock);

	STACK_MARKER marker = pool->stack_ptr;
	return marker;
}

void stackpool_releasemarker(
	HANDLE handle,
	STACK_MARKER marker
)
{
	if (!handle || !marker)
		return;

	struct stackpool_t* pool = (struct stackpool_t*)handle;
    VALIDATE_POOL(pool);

    // 마커 유효성 검사
    assert(marker >= pool->base_ptr && "Marker is before pool base!");
    assert(marker <= pool->stack_ptr && "Marker is after current stack pointer!");

    // 해제된 크기 계산
    MEM_SIZE freed = (char*)pool->stack_ptr - (char*)marker;

    // 스택 포인터를 마커로 되돌리기
    pool->stack_ptr = marker;
    pool->used -= freed;

	if (pool->thread_safe)
		release_spin_lock(&pool->lock);
}

void* stackpool_alloc(
	HANDLE handle,
	MEM_SIZE size
)
{
    if (!handle)
        return;

    struct stackpool_t* pool = (struct stackpool_t*)handle;
    VALIDATE_POOL(pool);

    if (!size)
        return NULL;

    // 헤더 포함 정렬된 크기 계산
    MEM_SIZE aligned_size = ALIGNED_SIZE(size, STACKPOOL_ALIGNMENT);
    MEM_SIZE total_size = sizeof(struct stackpool_header_t) + aligned_size;

    // 용량 체크
    if (pool->used + total_size > pool->capacity) 
    {
		char buffer[256];
        sprintf(buffer, "Stack pool out of memory! Requested: %zu, Available: %zu", total_size, pool->capacity - pool->used);
        assert(0 && buffer);

        return NULL;
    }

    // 헤더 작성
    struct stackpool_header_t* header = (struct stackpool_header_t*)pool->stack_ptr;
    header->size = size;
    header->aligned_size = aligned_size;

    // 사용자 포인터 계산
    void* user_ptr = (char*)header + sizeof(struct stackpool_header_t);

    // 스택 포인터 이동
    pool->stack_ptr = (char*)pool->stack_ptr + total_size;
    pool->used += total_size;

    return user_ptr;
}

MEM_SIZE stackpool_get_used(HANDLE handle) 
{
    if (!handle)
        return 0;

    struct stackpool_t* pool = (struct stackpool_t*)handle;
    VALIDATE_POOL(pool);

    if(pool->thread_safe)
		acquire_spin_lock(&pool->lock);

	MEM_SIZE size = pool->used;

	if (pool->thread_safe)
		release_spin_lock(&pool->lock);

    return size;
}

MEM_SIZE stackpool_get_capacity(HANDLE handle) 
{
    if (!handle)
        return 0;

    struct stackpool_t* pool = (struct stackpool_t*)handle;
    VALIDATE_POOL(pool);

	if (pool->thread_safe)
		acquire_spin_lock(&pool->lock);

	MEM_SIZE size = pool->capacity;

	if (pool->thread_safe)
		release_spin_lock(&pool->lock);

    return size;
}

void stackpool_reset(HANDLE handle) 
{
    if (!handle)
        return;

    struct stackpool_t* pool = (struct stackpool_t*)handle;
    VALIDATE_POOL(pool);

    if (pool->thread_safe)
        acquire_spin_lock(&pool->lock);

    pool->stack_ptr = pool->base_ptr;
    pool->used = 0;

    if (pool->thread_safe)
		release_spin_lock(&pool->lock);
}