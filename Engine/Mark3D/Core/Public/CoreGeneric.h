#pragma once
#include <list>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "CoreHeap.h"


namespace mark
{
	/**
	* @brief sys_alloc을 사용하는 STL allocator. allocate/deallocate가 sys_alloc/sys_free와 직접 연결된 allocator.
	*/
	template<typename _T>
	struct system_allocator
	{
		using value_type = _T;
		system_allocator() noexcept = default;

		template<typename _U>
		system_allocator(const system_allocator<_U>&) noexcept
		{
		}

		template <typename U>
		struct rebind {
			using other = system_allocator<U>;  // ← self-rebind
		};

		value_type* allocate(std::size_t n)
		{
			if (n > static_cast<std::size_t>(-1) / sizeof(value_type))
				return nullptr; // or throw std::bad_alloc

			void* p = sys_alloc(sizeof(value_type) * n);
			if (!p)
				return nullptr; // or throw std::bad_alloc

			return static_cast<value_type*>(p);
		}

		void deallocate(value_type* p, std::size_t) noexcept
		{
			sys_free(p);
		}

		template <typename U>
		bool operator==(const system_allocator<U>&) const noexcept { return true; }

		template <typename U>
		bool operator!=(const system_allocator<U>&) const noexcept { return false; }
	};

	/**
	* @brief temp_alloc을 사용하는 STL allocator. 개별 deallocate는 무시되고,
	* temp_reset() 호출 시 전체 메모리가 회수되는 bump-style arena allocator.
	*/
	template<typename _T>
	struct temp_allocator
	{
		using value_type = _T;

		temp_allocator() noexcept = default;

		template<typename _U>
		temp_allocator(const temp_allocator<_U>&) noexcept
		{
		}

		template <typename U>
		struct rebind {
			using other = temp_allocator<U>;  // ← self-rebind
		};

		value_type* allocate(std::size_t n)
		{
			if (n > static_cast<std::size_t>(-1) / sizeof(value_type))
				return nullptr; // or throw std::bad_alloc

			void* p = temp_alloc(sizeof(value_type) * n);
			if (!p)
				return nullptr; // or throw std::bad_alloc

			return static_cast<value_type*>(p);
		}

		void deallocate(value_type* p, std::size_t) noexcept
		{
			// temp_alloc은 bump-style arena allocator이므로 개별 deallocate는 의도적으로 무시.
		}

		template <typename U>
		bool operator==(const temp_allocator<U>&) const noexcept { return true; }

		template <typename U>
		bool operator!=(const temp_allocator<U>&) const noexcept { return false; }
	};

//	system_allocator* get_default_system_allocator() noexcept;
//	temp_allocator* get_default_temp_allocator() noexcept;

	namespace ALLOC
	{
		template<typename _T>
		using SYS = system_allocator<_T>;

		template<typename _T>
		using TEMP = temp_allocator<_T>;
	}

	// list 컨테이너 재정의
	template<typename _T, typename _Alloc>
	using list = std::list<_T, _Alloc>;

	template<typename _T>
	using sys_list = std::list<_T, ALLOC::SYS<_T>>;

	template<typename _T>
	using temp_list = std::list<_T, ALLOC::TEMP<_T>>;

	// vector 컨테이너 재정의
	template<typename _T, typename _Alloc>
	using vector = std::vector<_T, _Alloc>;

	template<typename _T>
	using sys_vector = std::vector<_T, ALLOC::SYS<_T>>;

	template<typename _T>
	using temp_vector = std::vector<_T, ALLOC::TEMP<_T>>;

	// deque 컨테이너 재정의
	template<typename _T, typename _Alloc>
	using deque = std::deque<_T, _Alloc>;

	template<typename _T>
	using sys_deque = std::deque<_T, ALLOC::SYS<_T>>;

	template<typename _T>
	using temp_deque = std::deque<_T, ALLOC::TEMP<_T>>;

	// unordered_map 컨테이너 재정의
	template<typename _Tk, typename _Tv, typename _Alloc>
	using unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	template<typename _Tk, typename _Tv>
	using sys_unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<std::pair<const _Tk, _Tv>>>;

	template<typename _Tk, typename _Tv>
	using temp_unordered_map = std::unordered_map<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<std::pair<const _Tk, _Tv>>>;

	// unordered_set 컨테이너 재정의
	template<typename _Tk, typename _Alloc>
	using unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	template<typename _Tk>
	using sys_unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<_Tk>>;

	template<typename _Tk>
	using temp_unordered_set = std::unordered_set<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<_Tk>>;

	// unordered_multimap 컨테이너 재정의
	template<typename _Tk, typename _Tv, typename _Alloc>
	using unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	template<typename _Tk, typename _Tv>
	using sys_unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<std::pair<const _Tk, _Tv>>>;

	template<typename _Tk, typename _Tv>
	using temp_unordered_multimap = std::unordered_multimap<_Tk, _Tv, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<std::pair<const _Tk, _Tv>>>;

	// unordered_multiset 컨테이너 재정의
	template<typename _Tk, typename _Alloc>
	using unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, _Alloc>;

	template<typename _Tk>
	using sys_unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::SYS<_Tk>>;

	template<typename _Tk>
	using temp_unordered_multiset = std::unordered_multiset<_Tk, std::hash<_Tk>, std::equal_to<_Tk>, ALLOC::TEMP<_Tk>>;

	// string 컨테이너 재정의
	template<typename _Alloc>
	using string = std::basic_string<char, std::char_traits<char>, _Alloc>;

	template<typename _Alloc>
	using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, _Alloc>;

	using sys_string = std::basic_string<char, std::char_traits<char>, ALLOC::SYS<char>>;

	using sys_wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, ALLOC::SYS<wchar_t>>;

	using temp_string = std::basic_string<char, std::char_traits<char>, ALLOC::TEMP<char>>;
	
	using temp_wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, ALLOC::TEMP<wchar_t>>;
}
