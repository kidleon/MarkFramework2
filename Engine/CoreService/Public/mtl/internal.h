#pragma once

#if defined(__TARGET_OS_WINDOWS)
#	if defined(USE_DLL)
#		if defined(MTL_EXPORT)
#			define MTL_API __declspec(dllexport)
#		else
#			define MTL_API __declspec(dllimport)
#		endif // #if defined(MTL_EXPORT)
#	else
#		define MTL_API
#	endif // #if defined(USE_DLL)
#elif defined(__TARGET_OS_LINUX) || defined(__TARGET_OS_MAC) || defined(__TARGET_OS_IOS) || defined(__TARGET_OS_FREEBSD)
#	if defined(USE_DLL)
#		if defined(MTL_EXPORT)
#			define MTL_API __attribute__((visibility("default")))
#		else
#			define MTL_API
#		endif // #if defined(MTL_EXPORT)
#	else
#		define MTL_API
#	endif // #if defined(USE_DLL)
#endif // #if defined(__TARGET_OS_WINDOWS)

namespace mtl
{
}
