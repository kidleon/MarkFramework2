#ifndef __I_PRIVATE_HEAP_H__
#define __I_PRIVATE_HEAP_H__


interface IPrivateHeap : public IUNKNOWN
{
public:
	virtual void* SysAlloc(
		size_t size,
		const char* file = NULL,
		int line = 0,
		const char* func = NULL
	) = 0;

	virtual void* SysAllocAlign(
		size_t size,
		size_t align,
		const char* file = NULL,
		int line = 0,
		const char* func = NULL
	) = 0;

	virtual void SysFree(void* ptr) = 0;
	virtual void SysFreeAlign(void* ptr) = 0;

	virtual void* PoolAlloc(
		size_t size,
		const char* file = NULL,
		int line = 0,
		const char* func = NULL
	) = 0;

	virtual void PoolFree(void* ptr) = 0;

	virtual int FramePoolGetID() = 0;
	virtual void FramePoolRelease(int PoolID) = 0;
	virtual void* FramePoolAlloc(int PoolID, size_t size) = 0;

	virtual void* StackPoolAlloc(size_t size) = 0;
	virtual void* StackPoolGetMarker() = 0;
	virtual void StackPoolReleaseMarker(void* marker) = 0;
};

#define HEAP_SYS_ALLOC(iprvheap, size, alignment) iprvheap->SysAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#define HEAP_FREE_ALIGN(iprvheap, ptr) iprvheap->SysFree(ptr)

#define MARK_ALLOC(size) SysHeapAlloc_(size, __FILE__, __LINE__, __FUNCTION__)
#define MARK_REALLOC(ptr, size) SysHeapRealloc(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#define MARK_FREE(ptr) SysHeapFree(ptr)

#define MARK_POOL_ALLOC(size) PoolAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#define MARK_POOL_FREE(ptr) PoolFree(ptr)

#	define MARK_TEMP_ALLOC(size) TempAlloc(size)
#	define MARK_TEMP_RESET() TempReset()

#	define MARK_NEW(type) new (MARK_POOL_ALLOC(sizeof(type))) type
#	define MARK_NEW_ARRAY(type, count) new (MARK_POOL_ALLOC(sizeof(type) * (count))) type[count]
#	define MARK_DELETE(ptr, type) { if(ptr) { (ptr)->~type(); MARK_POOL_FREE(ptr); ptr = nullptr; } }

#	define HEAP_ALLOC_AUTO(size, heap_type) HeapAlloc_Auto(size, __FILE__, __LINE__, __FUNCTION__)
#	define HEAP_FREE_AUTO(ptr, heap_type) HeapFree_Auto(ptr, heap_type)

#	define MARK_NEW_AUTO(type, heap_type) new (HEAP_ALLOC_AUTO(sizeof(type), heap_type)) type
#	define MARK_NEW_ARRAY_AUTO(type, count, heap_type) new (HEAP_ALLOC_AUTO(sizeof(type) * (count), heap_type)) type[count]
#	define MARK_DELETE_AUTO(ptr, type, heap_type) { if(ptr) { (ptr)->~type(); HEAP_FREE_AUTO(ptr, heap_type); ptr = nullptr; } }


#endif // __I_PRIVATE_HEAP_H__
