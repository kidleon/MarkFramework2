#include "pch.h"
#include "Mark3D.h"
#include "StringBuffer.h"
#include "mathlib.h"
#include "CoreHeap.h"
#include "CoreGeneric.h"
#include "Log.h"

#include "TestMath.h"
#include "TestMath_Utils.h"
#include "Test_FileSystem.h"
#include "Test_Compress.h"
//#include "Test_MemoryTempPool.h"
#include "Test_Unicode.h"
#include "Test_Hash.h"
#include "Test_Timer.h"
#include "Test_SRWLock.h"
#include "Test_SpinLock.h"
//#include "Test_UnknownPtr.h"
#include "Test_PrivateMemory2.h"
#include "Test_CoreHeap.h"
#include "Test_CoreGeneric.h"

#include "Benchmark_CoreHeap.h"
#include "Benchmark_CoreGeneric.h"



void TestGeneric();

void TestStringBuffer();

struct Person
{
	char name[20];
	int age;
	int score;
};

void PrintReport(const char* msg)
{
	printf(msg);
}

void TestAllocator();

int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	mark::Mark3D::Initialize();

	{
		TestStringBuffer();

		//TestGeneric();
		mark::TestMathLib();
		mark::TestMathLib_Util();
		mark::Test_FileSystem();
		mark::Test_Compress();
		//mark::Test_MemoryTempPool();
		mark::Test_Unicode();
		mark::Test_Hash();
		mark::Test_Timer();
		mark::Test_SRWLock();
		mark::Test_SpinLock();	
		//mark::Test_UnknownPtr();

		mark::Test_PrivateMemory2();
		mark::Test_CoreHeap();
		mark::Test_CoreGeneric();
	}

	mark::Mark3D::Shutdown();

	mark::Benchmark_PrivateMemory2();
	mark::Benchmark_CoreGeneric();

	return 0;
}

void TestStringBuffer()
{
	mark::string_buffer<char> str_buf;

	mark::FLOAT2 v{ 1.23f, 4.56f };
	str_buf.format("{}", v);
	str_buf.append_endl();

	LOG_F("FLOAT2 LOG => {}", v);

	str_buf.clear();

	mark::FLOAT3 v3{ 7.89f, 0.12f, 3.45f };
	str_buf.format("{}", v3);
	str_buf.append_endl();
	str_buf.clear();

	mark::FLOAT4 v4{ 6.78f, 9.01f, 2.34f, 5.67f };
	str_buf.format("{}", v4);
	str_buf.append_endl();
	str_buf.clear();

	mark::XFLOAT4 sv = { 9.87f, 6.54f, 3.21f, 0.98f };
	str_buf.format("{}", sv);
	str_buf.append_endl();
	str_buf.clear();

	mark::MATRIX4 m4 = {
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	};

	str_buf.format("{}", m4);
	str_buf.clear();

	mark::XMATRIX4 sm = {
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	};
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);
	str_buf.format("{}", sm);

	char buff[256];
	str_buf.to_buffer(buff, sizeof(buff));

	mark::sys_string s2;
	str_buf.to_sys_string(s2);

	mark::temp_string tstr;
	str_buf.to_temp_string(tstr);

	std::string_view view = str_buf.to_string_view();
	mark::string_buffer<wchar_t> wstr_buf;
	wstr_buf.format(L"HI{}", 1);

	mark::upool_wstring uwstr;
	wstr_buf.to_upool_string(uwstr);

	wchar_t uwstr_cstr[4];
	wstr_buf.to_buffer(uwstr_cstr, sizeof(uwstr_cstr));

	{
		mark::temp_string str = mark::string_buffer<char>::temp_format("Hello, {}!", "world");
		LOG(str);
	}
	

	{
		mark::spool_string sp_str = mark::string_buffer<char>::spool_format("Hello, {}!", "world");
		LOG(sp_str);
	}
}

/*
void TestGeneric()
{
	{
		mark::list<Person, mark::ALLOC::SYS<Person>> lstPerson;
		lstPerson.emplace_back(Person{ "Alice", 30, 85 });
		lstPerson.emplace_back(Person{ "Bob", 25, 90 });
	}

	{
		mark::sys_list<Person> lstPerson;
		lstPerson.emplace_back(Person{ "Alice", 30, 85 });
		lstPerson.emplace_back(Person{ "Bob", 25, 90 });
	}

	{
		mark::list<Person, mark::ALLOC::SPOOL<Person>> lstPerson;
		lstPerson.emplace_back(Person{ "Charlie", 28, 88 });
		lstPerson.emplace_back(Person{ "Diana", 22, 92 });
	}

	{
		mark::list<Person, mark::ALLOC::UPOOL<Person>> lstPerson;
		lstPerson.emplace_back(Person{ "Eve", 35, 80 });
		lstPerson.emplace_back(Person{ "Frank", 27, 87 });
	}

	{
		mark::list<Person, mark::ALLOC::TEMP<Person>> lstPerson;
		lstPerson.emplace_back(Person{ "Grace", 32, 91 });
		lstPerson.emplace_back(Person{ "Heidi", 29, 89 });
	}
}
*/
