#include "pch.h"
#include <MemoryManager.h>
#include <TList.h>
#include <TQueue.h>
#include <TStack.h>
#include <TArray.h>
#include <THashMap.h>
#include <THashSet.h>



static void ReportMemoryLeaks(
	const char* type,
	const char* file,
	int line,
	const char* func,
	size_t size
)
{
	printf("Memory Leak Detected: Type=%s, File=%s, Line=%d, Function=%s, Size=%zu bytes\n",
		type, file, line, func, size);
}


void TestList();
void TestQueue();
void TestStack();
void TestArray();
void TestHashMap();
void TestHashSet();


int main()
{
#if defined(__TARGET_OS_WINDOWS)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // __TARGET_OS_WINDOWS

	mark::MemoryManager::Initialize(
		MEM_SIZE(10 * 1024 * 1024),
		TRUE,
		ReportMemoryLeaks
	); // 10MB 임시 메모리 풀 초기화

	// TList 테스트
	TestList();
	TestQueue();
	TestStack();
	TestArray();
	TestHashMap();
	TestHashSet();

	mark::MemoryManager::Shutdown(); // 메모리 매니저 종료
	
    return 0;
}

void TestList()
{
	mark::TList<int, mark::ALLOC_TYPE::SYSCALL> lstInts;
	lstInts.push_back(10);
	lstInts.push_back(20);
	lstInts.push_back(30);
	lstInts.push_front(0);

	auto it = lstInts.find(20);
	if (it != lstInts.end())
	{
		printf("Found value: %d\n", *it);
	}
	else
	{
		printf("Value not found.\n");
	}

	lstInts.pop_front();
	lstInts.pop_front();
	lstInts.pop_front();
	lstInts.pop_back();
}

void TestQueue()
{
	mark::TQueue<int, mark::ALLOC_TYPE::SYSCALL> queInt;

	queInt.push_back(10);
	queInt.push_back(20);
	queInt.push_back(30);

	while (!queInt.empty())
	{
		auto val = queInt.front();
		queInt.pop_front();

		printf("val : %d\n", val);
	}
}

void TestStack()
{
	mark::TStack<int, mark::ALLOC_TYPE::SYSCALL> stkInt;

	stkInt.push(10);
	stkInt.push(20);
	stkInt.push(30);

	while (!stkInt.empty())
	{
		auto val = stkInt.front();
		stkInt.pop();

		printf("val : %d\n", val);
	}
}

void TestArray()
{
	// 배열 테스트 코드 작성
	mark::TArray<int, mark::ALLOC_TYPE::POOL> arrInts;
	arrInts.reserve(64);
	arrInts.push_back(1);
	arrInts.push_back(2);
	arrInts.push_back(3);
	
}

void TestHashMap()
{
	// 해시맵 테스트 코드 작성
	mark::THashMap<int, float, mark::ALLOC_TYPE::SYSCALL> hashMap;
	hashMap.insert(1, 1.1f);
	hashMap.insert(2, 2.2f);
	hashMap.insert(3, 3.3f);

	auto it = hashMap.find(2);
	if (it != hashMap.end())
	{
		printf("Found key 2 with value: %f\n", it.node->pair.value);
	}
	else
	{
		printf("Key 2 not found.\n");
	}

	float value;
	if (hashMap.try_find(3, value))
	{
		printf("Found key 3 with value: %f\n", value);
	}
	else
	{
		printf("Key 3 not found.\n");
	}
}

void TestHashSet()
{
	// 해시셋 테스트 코드 작성
	mark::THashSet<int, mark::ALLOC_TYPE::SYSCALL> hashSet;
	hashSet.insert(1);
	hashSet.insert(1);
	hashSet.insert(2);
	hashSet.insert(3);

	auto it = hashSet.find(2);
	if (it != hashSet.end())
	{
		printf("Found value 2 in hash set.\n");
	}
	else
	{
		printf("Value 2 not found in hash set.\n");
	}

	if (hashSet.contains(3))
	{
		printf("Hash set contains value 3.\n");
	}
	else
	{
		printf("Hash set does not contain value 3.\n");
	}
}