#pragma once
#include <cstdint>
#include <atomic>
#include <type_traits>


namespace mark
{
	/**
	* @brief unknown_ptr: _T 인터페이스를 위한 스마트 포인터
	*/
	template<typename _T>
	struct unknown_ptr
	{
		_T* _ptr;

		unknown_ptr() noexcept
			: _ptr(nullptr)
		{
		}

		unknown_ptr(_T* ptr) noexcept
			: _ptr(ptr)
		{
			if (_ptr)
				_ptr->add_ref();
		}

		unknown_ptr(const unknown_ptr& other) noexcept
			: _ptr(other._ptr)
		{
			if (_ptr)
				_ptr->add_ref();
		}

		unknown_ptr(unknown_ptr&& other) noexcept
			: _ptr(other._ptr)
		{
			other._ptr = nullptr;
		}

		~unknown_ptr() noexcept
		{
			if (_ptr)
				_ptr->release();
		}

		inline unknown_ptr& operator=(const unknown_ptr& other) noexcept
		{
			if (this != &other)
			{
				if (other._ptr)
					other._ptr->add_ref();
				if (_ptr)
					_ptr->release();
				_ptr = other._ptr;
			}
			return *this;
		}

		inline unknown_ptr& operator=(unknown_ptr&& other) noexcept
		{
			if (this != &other)
			{
				if (_ptr)
					_ptr->release();
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this;
		}

		inline unknown_ptr& operator=(_T* ptr) noexcept
		{
			if (_ptr != ptr)
			{
				if (ptr)
					ptr->add_ref();
				if (_ptr)
					_ptr->release();
				_ptr = ptr;
			}
			return *this;
		}

		inline explicit operator bool() const noexcept { return _ptr != nullptr; }

		// 명시적 해제 — Release 후 null로 초기화
		inline void reset() noexcept
		{
			if (_ptr)
			{
				_ptr->release();
				_ptr = nullptr;
			}
		}

		inline _T* operator->() noexcept { return _ptr; }
		inline const _T* operator->() const noexcept { return _ptr; }
		inline _T& operator*() noexcept { return *_ptr; }
		inline const _T& operator*() const noexcept { return *_ptr; }

		inline _T* ptr() noexcept { return _ptr; }
		inline const _T* ptr() const noexcept { return _ptr; }
		inline _T& ref() { return *_ptr; }
		inline const _T& ref() const { return *_ptr; }

		template<typename U>
		inline U* cast() noexcept
		{
			static_assert(std::is_base_of<_T, U>::value, "U must inherit from _T");
			return static_cast<U*>(_ptr);
		}

		template<typename U>
		inline const U* cast() const noexcept
		{
			static_assert(std::is_base_of<_T, U>::value, "U must inherit from _T");
			return static_cast<const U*>(_ptr);
		}

		template<typename U>
		U& cast_ref() noexcept
		{
			static_assert(std::is_base_of<_T, U>::value, "U must inherit from _T");
			return static_cast<U&>(*_ptr);
		}

		template<typename U>
		const U& cast_ref() const noexcept
		{
			static_assert(std::is_base_of<_T, U>::value, "U must inherit from _T");
			return static_cast<const U&>(*_ptr);
		}

		bool operator==(const unknown_ptr& o) const noexcept { return _ptr == o._ptr; }
		bool operator!=(const unknown_ptr& o) const noexcept { return _ptr != o._ptr; }
		bool operator< (const unknown_ptr& o) const noexcept { return _ptr < o._ptr; }

		// nullptr 비교 — if (p == nullptr) 형태 지원
		bool operator==(std::nullptr_t) const noexcept { return _ptr == nullptr; }
		bool operator!=(std::nullptr_t) const noexcept { return _ptr != nullptr; }
	};

	// nullptr == p 형태도 지원 (좌우 대칭)
	inline bool operator==(std::nullptr_t, const unknown_ptr& p) noexcept { return p == nullptr; }
	inline bool operator!=(std::nullptr_t, const unknown_ptr& p) noexcept { return p != nullptr; }

};
