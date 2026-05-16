#pragma once

#if defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	if defined(USE_DLL)
#		if defined(MTL_EXPORT)
#			define MTL_API __declspec(dllexport)
#		else
#			define MTL_API __declspec(dllimport)
#		endif // #if defined(MTL_EXPORT)
#	else
#		define MTL_API
#	endif // #if defined(USE_DLL)
#elif defined(__GNUC__) || defined(__clang__)
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
