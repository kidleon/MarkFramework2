#include "pch.h"
#include "core.h"
#include "TestBenchCommon.h"



int main()
{
#if defined(__MEMORY_TRACKER_ENABLED__) && defined(__TARGET_OS_WINDOWS)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // __MEMORY_TRACKER_ENABLED__ && __TARGET_OS_WINDOWS

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


	mark::shutdown_core_service();

	return 0;
}
