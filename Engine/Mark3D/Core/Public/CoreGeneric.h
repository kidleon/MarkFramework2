#pragma once
#include <memory_resource>
#include <list>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>


namespace mark
{
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_core_system_memory_resource_ptr() noexcept;
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_core_sync_pool_memory_resource_ptr() noexcept;
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_core_usync_pool_memory_resource_ptr() noexcept;
	[[nodiscard]] MARKENGINE_API std::pmr::memory_resource* get_core_temp_memory_resource_ptr() noexcept;

	namespace ALLOC
	{
		template<typename _T>
		struct default_system_memory_allocator : public std::pmr::polymorphic_allocator<_T>
		{
			default_system_memory_allocator() noexcept
				: std::pmr::polymorphic_allocator<_T>(mark::get_core_system_memory_resource_ptr())
			{
			}
			default_system_memory_allocator(std::pmr::memory_resource* res) noexcept
				: std::pmr::polymorphic_allocator<_T>(res)
			{
			}

			template<typename _U>
			default_system_memory_allocator(const default_system_memory_allocator<_U>& other) noexcept
				: std::pmr::polymorphic_allocator<_T>(other)
			{
			}

			template <typename U>
			struct rebind {
				using other = default_system_memory_allocator<U>;  // ← self-rebind
			};
		};

		template<typename _T>
		struct default_spool_memory_allocator : public std::pmr::polymorphic_allocator<_T>
		{
			default_spool_memory_allocator() noexcept
				: std::pmr::polymorphic_allocator<_T>(mark::get_core_sync_pool_memory_resource_ptr())
			{
			}
			default_spool_memory_allocator(std::pmr::memory_resource* res) noexcept
				: std::pmr::polymorphic_allocator<_T>(res)
			{
			}

			template<typename _U>
			default_spool_memory_allocator(const default_spool_memory_allocator<_U>& other) noexcept
				: std::pmr::polymorphic_allocator<_T>(other)
			{
			}

			template <typename U>
			struct rebind {
				using other = default_spool_memory_allocator<U>;  // ← self-rebind
			};
		};

		template<typename _T>
		struct default_upool_memory_allocator : public std::pmr::polymorphic_allocator<_T>
		{
			default_upool_memory_allocator() noexcept
				: std::pmr::polymorphic_allocator<_T>(mark::get_core_usync_pool_memory_resource_ptr())
			{
			}
			default_upool_memory_allocator(std::pmr::memory_resource* res) noexcept
				: std::pmr::polymorphic_allocator<_T>(res)
			{
			}

			template<typename _U>
			default_upool_memory_allocator(const default_upool_memory_allocator<_U>& other) noexcept
				: std::pmr::polymorphic_allocator<_T>(other)
			{
			}

			template <typename U>
			struct rebind {
				using other = default_upool_memory_allocator<U>;  // ← self-rebind
			};
		};

		template<typename _T>
		struct default_temp_memory_allocator : public std::pmr::polymorphic_allocator<_T>
		{
			default_temp_memory_allocator() noexcept
				: std::pmr::polymorphic_allocator<_T>(mark::get_core_temp_memory_resource_ptr())
			{
			}
			default_temp_memory_allocator(std::pmr::memory_resource* res) noexcept
				: std::pmr::polymorphic_allocator<_T>(res)
			{
			}

			template<typename _U>
			default_temp_memory_allocator(const default_temp_memory_allocator<_U>& other) noexcept
				: std::pmr::polymorphic_allocator<_T>(other)
			{
			}

			template <typename U>
			struct rebind {
				using other = default_temp_memory_allocator<U>;  // ← self-rebind
			};
		};

		template<typename _T>
		using SYS = default_system_memory_allocator<_T>;

		template<typename _T>
		using SPOOL = default_spool_memory_allocator<_T>;

		template<typename _T>
		using UPOOL = default_upool_memory_allocator<_T>;

		template<typename _T>
		using TEMP = default_temp_memory_allocator<_T>;
	}

	// list 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _T, typename _Alloc>
	using list = std::list<_T, _Alloc>;

	// list 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using sys_list = std::list<_T, ALLOC::SYS<_T>>;

	// list 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using spool_list = std::list<_T, ALLOC::SPOOL<_T>>;

	// list 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using upool_list = std::list<_T, ALLOC::UPOOL<_T>>;

	// list 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using temp_list = std::list<_T, ALLOC::TEMP<_T>>;


	// vector 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _T, typename _Alloc>
	using vector = std::vector<_T, _Alloc>;

	// vector 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using sys_vector = std::vector<_T, ALLOC::SYS<_T>>;

	// vector 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using spool_vector = std::vector<_T, ALLOC::SPOOL<_T>>;

	// vector 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using upool_vector = std::vector<_T, ALLOC::UPOOL<_T>>;

	// vector 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using temp_vector = std::vector<_T, ALLOC::TEMP<_T>>;


	// deque 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _T, typename _Alloc>
	using deque = std::deque<_T, _Alloc>;

	// deque 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using sys_deque = std::deque<_T, ALLOC::SYS<_T>>;

	// deque 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using spool_deque = std::deque<_T, ALLOC::SPOOL<_T>>;

	// deque 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using upool_deque = std::deque<_T, ALLOC::UPOOL<_T>>;

	// deque 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _T>
	using temp_deque = std::deque<_T, ALLOC::TEMP<_T>>;


	// unordered_map 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv, typename _Alloc>
	using unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	// unordered_map 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using sys_unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<std::pair<const _Tk, _Tv>>>;

	// unordered_map 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using spool_unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SPOOL<std::pair<const _Tk, _Tv>>>;

	// unordered_map 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using upool_unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::UPOOL<std::pair<const _Tk, _Tv>>>;

	// unordered_map 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using temp_unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<std::pair<const _Tk, _Tv>>>;


	// unordered_set 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Alloc>
	using unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	// unordered_set 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using sys_unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<_Tk>>;

	// unordered_set 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using spool_unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SPOOL<_Tk>>;

	// unordered_set 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using upool_unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::UPOOL<_Tk>>;

	// unordered_set 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using temp_unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<_Tk>>;


	// unordered_multimap 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv, typename _Alloc>
	using unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	// unordered_multimap 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using sys_unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<std::pair<const _Tk, _Tv>>>;

	// unordered_multimap 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using spool_unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SPOOL<std::pair<const _Tk, _Tv>>>;

	// unordered_multimap 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using upool_unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::UPOOL<std::pair<const _Tk, _Tv>>>;

	// unordered_multimap 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Tv>
	using temp_unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<std::pair<const _Tk, _Tv>>>;


	// unordered_multiset 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk, typename _Alloc>
	using unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	// unordered_multiset 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using sys_unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<_Tk>>;

	// unordered_multiset 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using spool_unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SPOOL<_Tk>>;

	// unordered_multiset 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using upool_unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::UPOOL<_Tk>>;

	// unordered_multiset 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	template<typename _Tk>
	using temp_unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<_Tk>>;


	// std::string 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Alloc>
	using string = std::basic_string<char, std::char_traits<char>, _Alloc>;

	// std::string 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	using sys_string = std::basic_string<char, std::char_traits<char>, ALLOC::SYS<char>>;

	// std::string 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	using spool_string = std::basic_string<char, std::char_traits<char>, ALLOC::SPOOL<char>>;

	// std::string 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	using upool_string = std::basic_string<char, std::char_traits<char>, ALLOC::UPOOL<char>>;

	// std::string 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	using temp_string = std::basic_string<char, std::char_traits<char>, ALLOC::TEMP<char>>;


	// std::wstring 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Alloc>
	using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, _Alloc>;

	// std::wstring 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	using sys_wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, ALLOC::SYS<wchar_t>>;

	// std::wstring 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	using spool_wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, ALLOC::SPOOL<wchar_t>>;

	// std::wstring 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	using upool_wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, ALLOC::UPOOL<wchar_t>>;

	// std::wstring 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	using temp_wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, ALLOC::TEMP<wchar_t>>;


	// std::u16string 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Alloc>
	using u16string = std::basic_string<char16_t, std::char_traits<char16_t>, _Alloc>;

	// std::u16string 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	using sys_u16string = std::basic_string<char16_t, std::char_traits<char16_t>, ALLOC::SYS<char16_t>>;

	// std::u16string 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	using spool_u16string = std::basic_string<char16_t, std::char_traits<char16_t>, ALLOC::SPOOL<char16_t>>;

	// std::u16string 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	using upool_u16string = std::basic_string<char16_t, std::char_traits<char16_t>, ALLOC::UPOOL<char16_t>>;

	// std::u16string 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	using temp_u16string = std::basic_string<char16_t, std::char_traits<char16_t>, ALLOC::TEMP<char16_t>>;


	// std::u32string 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	template<typename _Alloc>
	using u32string = std::basic_string<char32_t, std::char_traits<char32_t>, _Alloc>;

	// std::u32string 컨테이너 재정의 (기본적으로 시스템 메모리 할당자를 사용하도록 설정)
	using sys_u32string = std::basic_string<char32_t, std::char_traits<char32_t>, ALLOC::SYS<char32_t>>;

	// std::u32string 컨테이너 재정의 (기본적으로 동기화된 풀 메모리 할당자를 사용하도록 설정)
	using spool_u32string = std::basic_string<char32_t, std::char_traits<char32_t>, ALLOC::SPOOL<char32_t>>;

	// std::u32string 컨테이너 재정의 (기본적으로 동기화되지 않은 풀 메모리 할당자를 사용하도록 설정)
	using upool_u32string = std::basic_string<char32_t, std::char_traits<char32_t>, ALLOC::UPOOL<char32_t>>;

	// std::u32string 컨테이너 재정의 (기본적으로 임시 버퍼 메모리 할당자를 사용하도록 설정)
	using temp_u32string = std::basic_string<char32_t, std::char_traits<char32_t>, ALLOC::TEMP<char32_t>>;

}
