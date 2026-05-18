#include "pch.h"
#include "core.h"



int main()
{
#if defined(__MEMORY_TRACKER_ENABLED__) && defined(__TARGET_OS_WINDOWS)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // __MEMORY_TRACKER_ENABLED__ && __TARGET_OS_WINDOWS

	mark::initialaize_core_service(1024 * 1024 * 10);



	mark::shutdown_core_service();

	return 0;
}
