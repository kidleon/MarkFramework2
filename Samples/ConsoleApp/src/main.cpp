#include "pch.h"
#include "core.h"
#include "TestBenchCommon.h"

#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif


int main()
{
#if defined(__MEMORY_TRACKER_ENABLED__) && defined(__TARGET_OS_WINDOWS)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // __MEMORY_TRACKER_ENABLED__ && __TARGET_OS_WINDOWS

#if defined(_WIN32)
	// 콘솔 출력 코드페이지를 UTF-8 로 강제하여 멀티바이트 문자 깨짐 방지.
	::SetConsoleOutputCP(CP_UTF8);
#endif

	mark::initialaize_core_service(1024 * 1024 * 10);

	// fixed container tests
	run_test_fixed_vector();
	run_test_fixed_string();
	run_test_fixed_wstring();
	run_test_fixed_list();
	run_test_fixed_slist();
	run_test_fixed_hash_map();
	run_test_fixed_hash_set();
	run_test_fixed_bitset();
	run_test_fixed_ring_buffer();
	run_test_fixed_pool();
	run_test_intrusive_list();
	run_test_intrusive_hash_map();
	run_test_intrusive_hash_set();
	run_test_random();

	mtl_tb::summary();

	// fixed container benchmark
	run_bench_fixed_vector();
	run_bench_fixed_string();
	run_bench_fixed_wstring();
	run_bench_fixed_list();
	run_bench_fixed_slist();
	run_bench_fixed_hash_map();
	run_bench_fixed_hash_set();
	run_bench_fixed_bitset();
	run_bench_fixed_ring_buffer();
	run_bench_fixed_pool();
	run_bench_intrusive_list();
	run_bench_intrusive_hash_map();
	run_bench_intrusive_hash_set();


	mark::shutdown_core_service();

	return 0;
}
