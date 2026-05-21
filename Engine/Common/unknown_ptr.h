#pragma once
#include <cstdint>
#include <atomic>
#include <type_traits>
#include "unknown.h"


namespace mark
{
	
	/**
	* @brief unknown_ptr: Unknown 기반 인터페이스를 위한 RAII 스마트 포인터.
	*
	* 두 가지 인계 방식이 있습니다:
	*
	*   1) 생성자 `unknown_ptr<T>(rawPtr)` / `operator=(rawPtr)`:
	*      AddRef를 호출하여 *새 owning ref*를 만듭니다.
	*      이미 borrowed 포인터(예: 인자로 받은 raw 포인터)를 안전히 보유할 때 사용.
	*
	*   2) `attach(rawPtr)` / `make_unknown(rawPtr)`:
	*      AddRef를 호출하지 *않습니다*. 이미 AddRef된 owning ref를 takeover할 때 사용.
	*      예: ShaderProgramCache::Query(), GPUBufferPool::Acquire(),
	*          IRenderSystem::CreateGPUBuffer() 등 [Owning] 컨벤션 함수의 반환값.
	*
	*      auto buffer = make_unknown(pRenderSystem->CreateGPUBuffer(desc));
	*      // buffer가 스코프를 벗어나면 자동으로 Release 호출.
	*
	* 인터페이스 함수의 ownership 컨벤션은 헤더 주석의 `[Owning]` / `[Borrowed]` /
	* `[Pool-owned]` 태그를 참고하세요.
	*/
	template<typename T>
	struct unknown_ptr
	{
		static_assert(std::is_base_of_v<Unknown, T>,
			"unknown_ptr can only be used with types derived from IUnknown");
		T* _ptr;

		unknown_ptr() noexcept
			: _ptr(nullptr)
		{
		}

		unknown_ptr(T* ptr) noexcept
			: _ptr(ptr)
		{
			if (_ptr)
				_ptr->AddRef();
		}

		unknown_ptr(const unknown_ptr& other) noexcept
			: _ptr(other._ptr)
		{
			if (_ptr)
				_ptr->AddRef();
		}

		unknown_ptr(unknown_ptr&& other) noexcept
			: _ptr(other._ptr)
		{
			other._ptr = nullptr;
		}

		~unknown_ptr() noexcept
		{
			if (_ptr)
				_ptr->Release();
		}

		inline unknown_ptr& operator=(const unknown_ptr& other) noexcept
		{
			if (this != &other)
			{
				if (other._ptr)
					other._ptr->AddRef();
				if (_ptr)
					_ptr->Release();
				_ptr = other._ptr;
			}
			return *this;
		}

		inline unknown_ptr& operator=(unknown_ptr&& other) noexcept
		{
			if (this != &other)
			{
				if (_ptr)
					_ptr->Release();
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this;
		}

		inline unknown_ptr& operator=(T* ptr) noexcept
		{
			if (_ptr != ptr)
			{
				if (ptr)
					ptr->AddRef();
				if (_ptr)
					_ptr->Release();
				_ptr = ptr;
			}
			return *this;
		}

		inline explicit operator bool() const noexcept { return _ptr != nullptr; }

		inline void attach(T* ptr) noexcept
		{
			if (_ptr)
				_ptr->Release();
			_ptr = ptr;
		}

		// 명시적 해제 — Release 후 null로 초기화
		inline void reset() noexcept
		{
			if (_ptr)
			{
				_ptr->Release();
				_ptr = nullptr;
			}
		}

		inline T* operator->() noexcept { return _ptr; }
		inline const T* operator->() const noexcept { return _ptr; }
		inline T& operator*() noexcept { return *_ptr; }
		inline const T& operator*() const noexcept { return *_ptr; }

		inline T* ptr() noexcept { return _ptr; }
		inline const T* ptr() const noexcept { return _ptr; }
		inline T& ref() { return *_ptr; }
		inline const T& ref() const { return *_ptr; }

		template<typename U>
		inline U* cast() noexcept
		{
			static_assert(std::is_base_of<T, U>::value, "U must inherit from T");
			return static_cast<U*>(_ptr);
		}

		template<typename U>
		inline const U* cast() const noexcept
		{
			static_assert(std::is_base_of<T, U>::value, "U must inherit from T");
			return static_cast<const U*>(_ptr);
		}

		template<typename U>
		U& cast_ref() noexcept
		{
			static_assert(std::is_base_of<T, U>::value, "U must inherit from T");
			return static_cast<U&>(*_ptr);
		}

		template<typename U>
		const U& cast_ref() const noexcept
		{
			static_assert(std::is_base_of<T, U>::value, "U must inherit from T");
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
	template<typename T>
	inline bool operator==(std::nullptr_t, const unknown_ptr<T>& p) noexcept { return p == nullptr; }

	template<typename T>
	inline bool operator!=(std::nullptr_t, const unknown_ptr<T>& p) noexcept { return p != nullptr; }

	template<typename T>
	inline unknown_ptr<T> make_unknown(T* ptr) noexcept
	{
		unknown_ptr<T> uptr;
		uptr.attach(ptr);
		return uptr;
	}


};
