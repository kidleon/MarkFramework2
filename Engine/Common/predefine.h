#pragma once

// OS PLATFORM DETECTION
#if defined(_WIN32) || defined(_WIN64)
#   define __TARGET_OS_WINDOWS
#   define __PLATFORM_WINDOWS
#elif defined(__linux__)
#   define __TARGET_OS_LINUX
#   define __PLATFORM_LINUX
#elif defined(__APPLE__)
#   include <TargetConditionals.h>   // TARGET_OS_* 매크로 정의 헤더
#   if TARGET_OS_SIMULATOR || TARGET_OS_IPHONE
#       define __TARGET_OS_IOS
#       define __PLATFORM_IOS
#   elif TARGET_OS_MAC
#       define __TARGET_OS_MAC
#       define __PLATFORM_MAC
#   endif
#elif defined(__FreeBSD__)
#   define __TARGET_OS_FREEBSD
#   define __PLATFORM_FREEBSD
#endif

// PROCESSOR ARCHITECTURE DETECTION
#if defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_IX86) || defined(__i386__) || defined(__arm__)
#	ifndef __LITTLE_ENDIAN__
#		define __LITTLE_ENDIAN__
#	endif // __LITTLE_ENDIAN__
#elif defined(_PPC_) || defined(__ppc__) || defined(__powerpc__) || defined(__PPC64__) || defined(__powerpc64__)
#	define __BIG_ENDIAN__
#endif // PROCESSOR ARCHITECTURE

#if defined(_M_X64) || defined(_M_IX86)
#	ifndef __SSE__
#		define __SSE__
#	endif // #ifndef __SSE__
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__)
#	ifndef __NEON__
#		define __NEON__
#	endif // #ifndef __NEON__
#endif // #if defined(_M_X64) || defined(_M_IX86)

// COMPILER DETECTION
#if defined(_MSC_VER)
#	define __COMPILER_MSVC
#elif defined(__GNUC__)
#	define __COMPILER_GCC
#elif defined(__clang__)
#	define __COMPILER_CLANG
#endif // #if defined(_MSC_VER)

// FUNCTION's EXPORT TYPES
#if defined(__TARGET_OS_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#	define NOMINMAX
#endif // NOMINMAX

#	include <crtdbg.h>
#	include <Windows.h>
#	include <intrin.h>
#	include <basetyps.h>


#	if defined(USE_DLL)
#		if defined(MARKENGINE_EXPORTS)
#			define MARKENGINE_API __declspec(dllexport)
#			define MARKENGINE_C_API extern "C" __declspec(dllexport)
#		else
#			define MARKENGINE_API __declspec(dllimport)
#			define MARKENGINE_C_API extern "C" __declspec(dllimport)
#		endif // #if defined(MARKENGINE_EXPORTS)
#	else
#		define MARKENGINE_API
#		define MARKENGINE_C_API
#	endif // #if defined(USE_DLL)
#elif defined(__TARGET_OS_LINUX) || defined(__TARGET_OS_MAC) || defined(__TARGET_OS_IOS) || defined(__TARGET_OS_FREEBSD)
#	if defined(USE_DLL)
#		if defined(MARKENGINE_EXPORTS)
#			define MARKENGINE_API __attribute__((visibility("default")))
#			define MARKENGINE_C_API extern "C" __attribute__((visibility("default")))
#		else
#			define MARKENGINE_API
#			define MARKENGINE_C_API extern "C"
#		endif // #if defined(MARKENGINE_EXPORTS)
#	else
#		define MARKENGINE_API
#	endif // #if defined(USE_DLL)
#endif // #if defined(__TARGET_OS_WINDOWS)


// INLINE DEFINES
#if defined(__COMPILER_MSVC)
#	define __INLINE __inline
#	define __FORCEINLINE __forceinline
#	define __STRUCT__ struct
#ifndef INTERFACE
#	define INTERFACE struct
#endif // INTERFACE
#elif defined(__COMPILER_GCC) || defined(__COMPILER_CLANG)
#	define __INLINE inline
#	define __FORCEINLINE __attribute__((always_inline)) inline
#	define __STRUCT__ struct
#	define INTERFACE struct
#	define interface struct
#	define __forceinline __attribute__((always_inline)) inline
#else
#	define __INLINE inline
#	define __FORCEINLINE inline
#	define __STRUCT__ struct
#	define INTERFACE struct
#	define interface struct
#endif // #if defined(__COMPILER_MSVC)

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
	#include <arm_neon.h>
#endif

// BASIC TYPES DEFINES
#if defined(__APPLE__)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long long QWORD;
typedef long HRESULT;
typedef void* HANDLE;
typedef int LONG;
typedef unsigned int ULONG;
typedef long long LONG64;
typedef unsigned long long ULONG64;
#define __vectorcall
#endif // defined(__TARGET_COMPILER_GCC) || defined(__TARGET_COMPILER_CLANG)

typedef float FLOAT;

// CHARCTER UNICODE DEFINES
#ifndef MARK_ENCODING_ANSI
#	define MARK_ENCODING_ANSI "CP949"
#endif // MARK_ENCODING_ANSI

#ifndef MARK_ENCODING_UTF8 // Little Endian용만 정의 한다.
#	define MARK_ENCODING_UTF8 "UTF-8"
#endif // MARK_ENCODING_UTF8

#ifndef MARK_ENCODING_UTF16 
#	define MARK_ENCODING_UTF16 "UTF-16LE"
#endif // MARK_ENCODING_UTF16

#ifndef MARK_ENCODING_UTF32
#	define MARK_ENCODING_UTF32 "UTF-32LE"
#endif // MARK_ENCODING_UTF32

#ifndef MARK_ENCODING_WCHAR
#	if defined(__TARGET_OS_WINDOWS)
#		define MARK_ENCODING_WCHAR MARK_ENCODING_UTF16
#	else
#		define MARK_ENCODING_WCHAR MARK_ENCODING_UTF32
#	endif // #if defined(__TARGET_OS_WINDOWS)
#endif // MARK_ENCODING_WCHAR

#ifndef BOOL
#	define BOOL uint32_t
#endif // BOOL

#ifndef BOOL8
#	define BOOL8 uint8_t
#endif // BOOL8

#ifndef BOOL16
#	define BOOL16 uint16_t
#endif // BOOL8

#ifndef S_OK
#	define S_OK ((HRESULT)0L)
#endif // S_OK

#ifndef S_FALSE
#	define S_FALSE ((HRESULT)1L)
#endif // S_FALSE

#ifndef FALSE
#	define FALSE 0
#endif // FLASE

#ifndef TRUE
#	define TRUE 1
#endif // TRUE

#ifndef _MIN
#   define _MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif //_MIN

#ifndef _MAX
#   define _MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif //_MAX

#ifndef MAX_PATH
#	define MAX_PATH 256
#endif // MAX_PATH


#ifndef _MAX_PATH
# 	define _MAX_PATH 256
#endif // _MAX_PATH

using resource_handle = uint64_t;

static constexpr size_t DEFAULT_ALIGNMENT = sizeof(size_t);

// ALIGNMENT DEFINES
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

#define CHECK_DELETE(ptr) if (ptr) { delete (ptr); (ptr) = nullptr; }
#define CHECK_DELETE_ARRAY(ptr) if (ptr) { delete[] (ptr); (ptr) = nullptr; }
#define CHECK_RELEASE(ptr) if (ptr) { (ptr)->Release(); (ptr) = nullptr; }

namespace mark
{
	template <typename>
	inline constexpr bool always_false = false;
}



/* strcpy_s가 없는 플랫폼을 위한 크로스플랫폼 래퍼 */
#ifndef __TARGET_OS_WINDOWS  /* Windows가 아닌 경우에만 정의 */

typedef int errno_t;  /* Windows의 errno_t 타입 호환 */

static inline errno_t strcpy_s(char* dest, size_t dest_size, const char* src)
{
	if (!dest || !src) return 1;  // EINVAL
	if (0 == dest_size) return 1; // 

	size_t src_len = strlen(src);

	// 버퍼 오버플로우 방지: src가 dest보다 크면 실패 
	if (src_len >= dest_size)
	{
		dest[0] = '\0';  // Windows 동작과 동일하게 dest를 빈 문자열로
		return 1;        /* ERANGE */
	}

	memcpy(dest, src, src_len + 1);  // +1: 널 종단 문자 포함

	return 0; 
}

#endif  //__TARGET_OS_WINDOWS

static inline size_t safe_strcpy(char* dest, size_t dest_size, const char* src)
{
	if (!dest || !src || !dest_size)
		return SIZE_MAX;

	size_t src_len = strnlen(src, dest_size);
	size_t copy_len = (src_len < dest_size) ? src_len : dest_size - 1;
	
	memmove(dest, src, copy_len);
	dest[copy_len] = '\0';

	return src_len;
}
