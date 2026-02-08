#ifndef __PREDEFINE_H__
#define __PREDEFINE_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdbool.h>
#include <memory.h>
#include <assert.h>
#include <uchar.h> 
#include <wchar.h>
#include <float.h>


// TARGET OS PREDEFINES
#if defined(_WIN32) || defined(_WIN64)
#	define __TARGET_OS_WINDOWS
#elif defined(__APPLE__)
#	if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MACCATALYST) || defined(TARGET_OS_IPHONE)
#		define __TARGET_OS_IOS
#	elif  defined(TARGET_OS_MAC) 
#		define __TARGET_OS_MAC
#	endif
#elif defined(__ANDROID__)
#	define __TARGET_OS_ANDROID
#elif defined(__linux__)
#	define __TARGET_OS_LINUX
#elif defined(__FreeBSD__)
#	define __TARGET_OS_FREEBSD
#else 
#	error Unsupported target OS
#endif // SETUP TARGET OS


// COMPILER PREDEFINES
#if defined(_MSC_VER)
#	define __TARGET_COMPILER_MSC
#elif defined (__GNUC__)
#	define __TARGET_COMPILER_GCC
#elif defined (__clang__)
#	define __TARGET_COMPILER_CLANG
#else
#	error Unsupported compiler
#endif

// FUNCTION's EXPORT TYPES

#if defined(__TARGET_OS_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <crtdbg.h>
#	include <Windows.h>
#	include <intrin.h>
#	include <basetyps.h>
#	if defined(USE_DLL)
#		if defined(MARK_BASELIB_EXPORTS)
#			ifdef __cplusplus
#				define MARK_BASELIB_API __declspec(dllexport)
#				define MARK_BASELIB_C_API extern "C" __declspec(dllexport)
#			else
#				define MARK_BASELIB_API _declspec(dllexport) 
#				define MARK_BASELIB_C_API __declspec(dllexport)
#			endif // __cplusplus
#		else
#			ifdef __cplusplus
#				define MARK_BASELIB_API __declspec(dllimport)
#				define MARK_BASELIB_C_API extern "C" __declspec(dllimport)
#			else 
#				define MARK_BASELIB_API __declspec(dllimport)
#				define MARK_BASELIB_C_API __declspec(dllimport)
#			endif // __cplusplus
#		endif
#
#		if defined(MARKENGINE_EXPORTS)
#			ifdef __cplusplus
#				define MARKENGINE_API __declspec(dllexport)
#				define MARKENGINE_C_API extern "C" __declspec(dllexport)
#			else
#				define MARKENGINE_API __declspec(dllexport) 
#				define MARKENGINE_C_API __declspec(dllexport)
#			endif // __cplusplus
#		else
#			ifdef __cplusplus
#				define MARKENGINE_API __declspec(dllimport)
#				define MARKENGINE_C_API extern "C" __declspec(dllimport)
#			else 
#				define MARKENGINE_API __declspec(dllimport)
#				define MARKENGINE_C_API __declspec(dllimport)
#			endif // __cplusplus
#		endif
#
#	else
#		ifdef __cplusplus
#			define MARK_BASELIB_API extern "C"
#			define MARK_BASELIB_C_API extern "C"
#			define MARKENGINE_API extern "C"
#			define MARKENGINE_C_API extern "C"
#		else
#			define MARK_BASELIB_API extern
#			define MARK_BASELIB_C_API extern
#			define MARKENGINE_API extern
#			define MARKENGINE_C_API extern
#		endif //__cplusplus
#	endif // USE_DLL

#	ifndef MARK_ENCODING_ANSI
#		define MARK_ENCODING_ANSI "CP949"
#	endif // MARK_ENCODING_ANSI

#	ifndef MARK_ENCODING_UTF8 // Little Endian용만 정의 한다.
#		define MARK_ENCODING_UTF8 "UTF-8"
#	endif // MARK_ENCODING_UTF8

#	ifndef MARK_ENCODING_UTF16 // Little Endian용만 정의 한다.
#		define MARK_ENCODING_UTF16 "UTF-16LE"
#	endif // MARK_ENCODING_UTF16

#	ifndef MARK_ENCODING_UTF32
#		define MARK_ENCODING_UTF32 "UTF-32LE"
#	endif // MARK_ENCODING_UTF32

#	ifndef MARK_ENCODING_WCHAR
#		define MARK_ENCODING_WCHAR MARK_ENCODING_UTF16
#	endif // MARK_ENCODING_UTF32

#elif defined(__TARGET_OS_LINUX)
#   include <malloc.h>
#   include <mm_malloc.h>
#   ifdef __cplusplus
#	define MARKENGINE_API extern "C"
#		define MARKENGINE_C_API extern "C"
#	else
#		define MARKENGINE_API extern
#		define MARKENGINE_C_API extern
#	endif //__cplusplus		

#	ifndef MARK_ENCODING_ANSI
#		define MARK_ENCODING_ANSI "UTF-8"
#	endif // MARK_ENCODING_ANSI

#	ifndef MARK_ENCODING_UTF8 // Little Endian용만 정의 한다.
#		define MARK_ENCODING_UTF8 "UTF-8"
#	endif // MARK_ENCODING_UTF8

#	ifndef MARK_ENCODING_UTF16 // Little Endian용만 정의 한다.
#		define MARK_ENCODING_UTF16 "UTF-16LE"
#	endif // MARK_ENCODING_UTF16

#	ifndef MARK_ENCODING_UTF32
#		define MARK_ENCODING_UTF32 "UTF-32LE"
#	endif // MARK_ENCODING_UTF32

#	ifndef MARK_ENCODING_WCHAR
#		define MARK_ENCODING_WCHAR MARK_ENCODING_UTF32
#	endif // MARK_ENCODING_UTF32

#endif // __TARGET_OS_WINDOWS

// NOT WINDOWS DATA TYPES
#if defined(__TARGET_COMPILER_GCC) || defined(__TARGET_COMPILER_CLANG)
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

#	define __INLINE inline 
#	define __FORCEINLINE inline
#	define __STRUCT__ struct
#	define INTERFACE __STRUCT__

#elif defined(__TARGET_COMPILER_MSC)
#	define __INLINE inline 
#	define __FORCEINLINE __forceinline 
#	define __STRUCT__ struct
#ifndef INTERFACE
#	define INTERFACE __STRUCT__
#endif // INTERFACE
#endif // defined(__TARGET_COMPILER_GCC) || defined(__TARGET_COMPILER_CLANG)

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef int8_t INT8;
typedef uint8_t UINT8;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int32_t INT32;
typedef uint32_t UINT32;
typedef int64_t INT64;
typedef uint64_t UINT64;
typedef float FLOAT;
typedef double DOUBLE;
typedef long double LONG_DOUBLE;


#ifndef BOOL
#	define BOOL UINT32
#endif // BOOL

#ifndef BOOL8
#	define BOOL8 UINT8
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

#ifndef TMIN
#   define T_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif //TMIN

#ifndef TMAX
#   define T_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif //TMAX


#if defined(_DEBUG)
#	define POOL_HEAP_TRACK
#endif // _DEBUG


#if defined(_DEBUG)
#	if defined(__TARGET_OS_WINDOWS)
#		define __ASSERT_EXPR(expr, msg) \
            (void)(                                                                                     \
                (!!(expr)) ||                                                                           \
                (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
                (_CrtDbgBreak(), 0)                                                                     \
            )
#		define __ASSERT(Expr, Msg) __ASSERT_EXPR(Expr, Msg)
#	else
#		define __ASSERT(Expr, Msg) \
		do {\
			assert(Expr && Msg);\
		} while(0)
#	endif // TARGET_OS_WINDOWS  __ASSERT
#else
#	define __ASSERT(Expr, Msg) __noop
#endif //_DEBUG

#define ALIGNED_SIZE(alloc_size, alignment) (((alloc_size) + (alignment) - 1) & ~((alignment) - 1))
#define ALIGNED_PADDING(alloc_size, alignment) ((alignment - (alloc_size & (alignment - 1))) & (alignment - 1))

#if defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
#	define DEFAULT_MEMORY_ALIGNMENT 8
#	define MEM_SIZE unsigned long long
#elif defined(_M_IX86) || defined(__i386__) || defined(__arm__)
#	define DEFAULT_MEMORY_ALIGNMENT 4
#	define MEM_SIZE unsigned int
#endif // DEFAULT_MEMORY_ALIGNMENT

#if defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_IX86) || defined(__i386__) || defined(__arm__)
#	define __LITTLE_ENDIAN__
#elif defined(_PPC_) || defined(__ppc__) || defined(__powerpc__) || defined(__PPC64__) || defined(__powerpc64__)
#	define __BIG_ENDIAN__
#endif // PROCESSOR ARCHITECTURE

#define DEFAULT_MEMORY_BLOCK_ALIGNMENT 16 // 기본 16바이트 정렬
#define MAX_FILE_LENGTH 264 // 최대 파일 경로 길이

#if defined(__cplusplus)
enum class HEAP_TYPE : unsigned
{
	SYSCALL = 0,
	POOL,
	TEMP
};


#define CHECK_DELETE(p) if(p) { delete p; p = nullptr; }
#define CHECK_DELETE_ARRAY(arr) if(arr) { delete[] arr; arr = nullptr; }
#define CHECK_RELEASE(p) if(p) { p->Release(); p = nullptr; }
#define CHECK_ADDREF(p) if(p) { p->AddRef(); }
#define CHECK_FREE(p) if(p) { free(p); p = NULL; }

#include <type_traits>

struct __IUnknown
{
	virtual long AddRef() = 0;
	virtual long Release() = 0;
	virtual long RefCnt() = 0;
};

struct __IPrivateUnknown : public __IUnknown
{
private:
	virtual long AddRef() = 0;
	virtual long Release() = 0;
	virtual long RefCnt() = 0;
};

typedef __IUnknown IUNKNOWN;
typedef __IPrivateUnknown IPRIVATE_UNKNOWN;

enum class ALLOC_TYPE : unsigned
{
	SYSCALL,
	POOL,
	TEMP,
};

struct MemoryStats
{
	size_t sys_alloc_count;
	size_t pool_alloc_count;
	size_t peak_temp_count;

	size_t sys_alloc_size;
	size_t pool_alloc_size;
	size_t peak_temp_size;
};

class MARKENGINE_API NameHash
{
public:
	NameHash() = default;
	NameHash(const char* szName) : m_Hash(0)
	{
		if (szName)
			m_Hash = gen_fnv1(szName);
	}

	explicit NameHash(uint32 Hash)
		: m_Hash(Hash)
	{
	}

	NameHash(const NameHash& Other)
		: m_Hash(Other.m_Hash)
	{
	}

	NameHash(NameHash&& Other) noexcept
	{
		m_Hash = Other.m_Hash;
	}

	inline NameHash& operator=(const char* Other) noexcept
	{
		if (Other)
			m_Hash = gen_fnv1(Other);
		return *this;
	}

	inline NameHash& operator=(const NameHash& Other) noexcept
	{
		m_Hash = Other.m_Hash;
		return *this;
	}

	inline NameHash& operator=(NameHash&& Other) noexcept
	{
		m_Hash = Other.m_Hash;
		return *this;
	}

	inline bool operator ==(const NameHash& Other) const noexcept
	{
		return m_Hash == Other.m_Hash;
	}

	inline bool operator ==(uint32 Other) const noexcept
	{
		return m_Hash == Other;
	}

	inline bool operator ==(const char* Other) noexcept
	{
		uint32 OtherHash = (nullptr != Other) ? gen_fnv1(Other) : 0;
		return m_Hash == OtherHash;
	}

	inline bool operator !=(const NameHash& Other) const noexcept
	{
		return m_Hash != Other.m_Hash;
	}

	inline bool operator !=(uint32 Other) const noexcept
	{
		return m_Hash != Other;
	}

	inline bool operator !=(const char* Other) noexcept
	{
		uint32 OtherHash = (nullptr != Other) ? gen_fnv1(Other) : 0;
		return m_Hash != OtherHash;
	}

	inline bool operator >(const NameHash& Other) const noexcept
	{
		return m_Hash > Other.m_Hash;
	}

	inline bool operator >(uint32 Other) const noexcept
	{
		return m_Hash > Other;
	}

	inline bool operator <(const NameHash& Other) const noexcept
	{
		return m_Hash < Other.m_Hash;
	}

	inline bool operator <(uint32 Other) const noexcept
	{
		return m_Hash < Other;
	}

	inline operator uint32() noexcept
	{
		return m_Hash;
	}

	inline operator const uint32() const noexcept
	{
		return m_Hash;
	}

	inline bool empty() const noexcept
	{
		return !m_Hash ? true : false;
	}

	inline uint32 value() const noexcept
	{
		return m_Hash;
	}

private:
	inline uint32 gen_fnv1(const char* str)
	{
		const uint8* pData = (const uint8*)str;

		uint32 c;

		//FNV1_INITIAL_VALUE 2166136261U
		uint32 initial = 2166136261U;

		while ((c = (uint8)*pData++) != 0)
			initial = (initial * 16777619) ^ c;

		return initial;
	}

private:
	uint32 m_Hash = 0;

};


#endif // __cplusplus






#endif // __PREDEFINE_H__