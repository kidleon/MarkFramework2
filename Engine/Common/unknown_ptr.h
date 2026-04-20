#pragma once
#include <cstdint>
#include <atomic>
#include <type_traits>


namespace mark
{
	/**
	* @brief unknown: u_ptr와 uweak_ptr의 기반 클래스
	*/
	class unknown
	{
		template<typename U> friend class u_ptr;
		template<typename U> friend class uweak_ptr;

	protected:
		unknown() : _count(0), _weak_count(1) {}
		unknown(long count) : _count(count), _weak_count(1) {}
		virtual ~unknown() noexcept {}

		// strong ref 증가
		inline unknown* inc_ref() noexcept
		{
			_count.fetch_add(1, std::memory_order_relaxed);
			return this;
		}

		// strong ref 감소
		// → 0이 되면 on_delete() 호출 후 weak_count도 1 감소
		inline void dec_ref() noexcept
		{
			if (_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				dec_weak();
			}
		}

	protected:
		virtual void on_delete() noexcept = 0;

	private:
		// weak ref 증가 (uweak_ptr 생성 시 호출)
		inline unknown* inc_weak() noexcept
		{
			_weak_count.fetch_add(1, std::memory_order_relaxed);
			return this;
		}

		// weak ref 감소
		// → 0이 되면 _on_free()로 실제 메모리 해제
		inline void dec_weak() noexcept
		{
			if (_weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				on_delete();
			}
		}

		// lock()의 핵심: strong count가 0이 아닌 경우에만 원자적으로 증가
		// CAS 루프로 "체크 → 증가" 사이에 다른 스레드가 끼어드는 것을 방지
		bool try_inc_ref() noexcept
		{
			long expected = _count.load(std::memory_order_relaxed);

			while (expected > 0)
			{
				if (_count.compare_exchange_weak(
					expected,
					expected + 1,
					std::memory_order_acq_rel,   // 성공 시 ordering
					std::memory_order_relaxed
				))  // 실패 시 ordering
				{
					return true;
				}
				// CAS 실패 시 expected는 현재 값으로 자동 갱신됨
			}

			return false; // strong count가 이미 0 → 객체 소멸됨
		}


		std::atomic<long> _count;       // strong ref count
		std::atomic<long> _weak_count;  // weak ref count (strong 존재 시 최소 1)
	};

	/**
	* @brief u_ptr: unknown을 상속받는 객체에 대한 스마트 포인터
	*/
	template <typename T>
	class u_ptr
	{
		static_assert(std::is_base_of<unknown, T>::value, "T must inherit from unknown");

		template<typename U> friend class uweak_ptr;

		T* _ptr;

		// inc_ref 없이 소유권만 이전받는 내부 생성자
		// lock()에서 try_inc_ref()가 이미 inc_ref 했으므로 중복 호출 방지용
		struct adopt_tag {};
		u_ptr(T* ptr, adopt_tag) noexcept : _ptr(ptr) {}

	public:
		u_ptr() noexcept : _ptr(nullptr) {}

		u_ptr(T* ptr) : _ptr(ptr)
		{
			if (_ptr) _ptr->inc_ref();
		}

		u_ptr(const u_ptr& other) : _ptr(other._ptr)
		{
			if (_ptr) _ptr->inc_ref();
		}

		u_ptr(u_ptr&& other) noexcept : _ptr(other._ptr)
		{
			other._ptr = nullptr;
		}

		~u_ptr()
		{
			if (_ptr) _ptr->dec_ref();
		}

		u_ptr& operator=(const u_ptr& other)
		{
			if (this != &other)
			{
				if (other._ptr) other._ptr->inc_ref();
				if (_ptr)       _ptr->dec_ref();
				_ptr = other._ptr;
			}
			return *this;
		}

		inline u_ptr& operator=(u_ptr&& other) noexcept
		{
			if (this != &other)
			{
				if (_ptr) _ptr->dec_ref();
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this;
		}

		inline u_ptr& operator=(T* other_ptr)
		{
			if (_ptr != other_ptr)
			{
				if (other_ptr) other_ptr->inc_ref();
				if (_ptr)      _ptr->dec_ref();
				_ptr = other_ptr;
			}
			return *this;
		}

		inline explicit operator bool() const noexcept { return _ptr != nullptr; }
		inline bool is_valid()          const noexcept { return _ptr != nullptr; }

		inline void reset() noexcept
		{
			if (_ptr)
			{
				_ptr->dec_ref();
				_ptr = nullptr;
			}
		}

		inline const T& operator*()  const { return *_ptr; }
		inline T& operator*() { return *_ptr; }
		inline const T* operator->() const { return _ptr; }
		inline T* operator->() { return _ptr; }

		inline T* get_ptr()       noexcept { return _ptr; }
		inline const T* get_ptr() const noexcept { return _ptr; }

		template <typename U>
		inline u_ptr<U> cast() const
		{
			return u_ptr<U>(static_cast<U*>(_ptr));
		}

		// ── 비교 연산자: friend free function으로 정의 ──────────────────
		// 원본 코드의 버그: 멤버 함수 안에서 파라미터 2개짜리로 선언하면
		// 실제로는 파라미터가 3개(this + a + b)가 되어 컴파일 에러 발생
		template<typename U>
		friend bool operator==(const u_ptr<U>& a, const u_ptr<U>& b) noexcept;

		template<typename U>
		friend bool operator!=(const u_ptr<U>& a, const u_ptr<U>& b) noexcept;

		template<typename U>
		friend bool operator< (const u_ptr<U>& a, const u_ptr<U>& b) noexcept;
	};

	template<typename T>
	inline bool operator==(const u_ptr<T>& a, const u_ptr<T>& b) noexcept
	{
		return a._ptr == b._ptr;
	}

	template<typename T>
	inline bool operator!=(const u_ptr<T>& a, const u_ptr<T>& b) noexcept {
		return a._ptr != b._ptr;
	}

	template<typename T>
	inline bool operator< (const u_ptr<T>& a, const u_ptr<T>& b) noexcept {
		return a._ptr < b._ptr;
	}

	/**
	* @brief uweak_ptr: unknown을 상속받는 객체에 대한 약한 참조 스마트 포인터
	*/
	template <typename T>
	class uweak_ptr
	{
		static_assert(std::is_base_of<unknown, T>::value,
			"T must inherit from unknown");

		T* _ptr;

	public:
		uweak_ptr() noexcept : _ptr(nullptr) {}

		// strong ptr로부터 weak ptr 생성
		explicit uweak_ptr(const u_ptr<T>& strong) noexcept
			: _ptr(strong._ptr)
		{
			if (_ptr)
				_ptr->inc_weak();
		}

		uweak_ptr(const uweak_ptr& other) noexcept
			: _ptr(other._ptr)
		{
			if (_ptr)
				_ptr->inc_weak();
		}

		uweak_ptr(uweak_ptr&& other) noexcept
			: _ptr(other._ptr)
		{
			other._ptr = nullptr;
		}

		~uweak_ptr() noexcept
		{
			if (_ptr)
				_ptr->dec_weak();
		}

		inline uweak_ptr& operator=(const uweak_ptr& other) noexcept
		{
			if (this != &other)
			{
				if (other._ptr)
					other._ptr->inc_weak();
				if (_ptr)
					_ptr->dec_weak();
				_ptr = other._ptr;
			}
			return *this;
		}

		inline uweak_ptr& operator=(uweak_ptr&& other) noexcept
		{
			if (this != &other)
			{
				if (_ptr)
					_ptr->dec_weak();
				_ptr = other._ptr;
				other._ptr = nullptr;
			}
			return *this;
		}

		// strong ptr로 재할당
		inline uweak_ptr& operator=(const u_ptr<T>& strong) noexcept
		{
			T* new_ptr = strong._ptr;
			if (_ptr != new_ptr)
			{
				if (new_ptr)
					new_ptr->inc_weak();
				if (_ptr)
					_ptr->dec_weak();
				_ptr = new_ptr;
			}
			return *this;
		}

		/**
		* @brief weak_ptr에서 strong_ptr로 승격 시도
		* @details lock()은 weak_ptr이 가리키는 객체가 아직 존재하는 경우에만 strong_ptr을 반환합니다.
		*/
		inline u_ptr<T> lock() const noexcept
		{
			if (_ptr && _ptr->try_inc_ref())
			{
				// try_inc_ref()가 이미 inc_ref 했으므로
				// adopt_tag 생성자로 중복 inc_ref 방지
				return u_ptr<T>(_ptr, typename u_ptr<T>::adopt_tag{});
			}

			return u_ptr<T>(); // 객체 이미 소멸
		}

		/**
		* @brief weak_ptr이 가리키는 객체가 소멸되었는지 여부를 확인
		* @details expired()는 쓰레드 안전하지 않음
		* 멀티스레드 환경에서는 lock()의 결과로 객체 존재 여부를 판단하는 것이 안전함.
		*/
		inline bool expired() const noexcept
		{
			if (!_ptr)
				return true;

			return _ptr->_count.load(std::memory_order_acquire) == 0;
		}

		/**
		* @brief weak_ptr이 가리키는 객체에 대한 참조를 해제
		* @details reset()은 weak_ptr이 가리키는 객체에 대한 참조를 해제하여, 이후 lock()이 항상 빈 u_ptr을 반환하도록 함
		*/
		inline void reset() noexcept
		{
			if (_ptr)
			{
				_ptr->dec_weak();
				_ptr = nullptr;
			}
		}

		// expired()가 false일 때만 true
		inline explicit operator bool() const noexcept { return !expired(); }

		template<typename U>
		friend bool operator==(const uweak_ptr<U>& a, const uweak_ptr<U>& b) noexcept;

		template<typename U>
		friend bool operator!=(const uweak_ptr<U>& a, const uweak_ptr<U>& b) noexcept;
	};

	template<typename T>
	inline bool operator==(const uweak_ptr<T>& a, const uweak_ptr<T>& b) noexcept
	{
		return a._ptr == b._ptr;
	}

	template<typename T>
	inline bool operator!=(const uweak_ptr<T>& a, const uweak_ptr<T>& b) noexcept
	{
		return a._ptr != b._ptr;
	}

	/**
	* @brief IUNKNOWN: COM 스타일 레퍼런스 카운팅 인터페이스
	*/
	struct IUNKNOWN
	{
		virtual uint32_t AddRef() = 0;
		virtual uint32_t Release() = 0;
	};


	/**
	* @brief iunknown_ptr: IUNKNOWN 인터페이스를 위한 스마트 포인터
	*/
	struct iunknown_ptr
	{
		IUNKNOWN* _ptr;

		iunknown_ptr() noexcept
			: _ptr(nullptr)
		{
		}

		iunknown_ptr(IUNKNOWN* ptr) noexcept
			: _ptr(ptr)
		{
			if (_ptr)
				_ptr->AddRef();
		}

		iunknown_ptr(const iunknown_ptr& other) noexcept
			: _ptr(other._ptr)
		{
			if (_ptr)
				_ptr->AddRef();
		}

		iunknown_ptr(iunknown_ptr&& other) noexcept
			: _ptr(other._ptr)
		{
			other._ptr = nullptr;
		}

		~iunknown_ptr() noexcept
		{
			if (_ptr)
				_ptr->Release();
		}

		inline iunknown_ptr& operator=(const iunknown_ptr& other) noexcept
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

		inline iunknown_ptr& operator=(iunknown_ptr&& other) noexcept
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

		inline iunknown_ptr& operator=(IUNKNOWN* ptr) noexcept
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

		// 명시적 해제 — Release 후 null로 초기화
		inline void reset() noexcept
		{
			if (_ptr)
			{
				_ptr->Release();
				_ptr = nullptr;
			}
		}

		inline IUNKNOWN* operator->()              noexcept { return _ptr; }
		inline const IUNKNOWN* operator->()  const noexcept { return _ptr; }
		inline IUNKNOWN& operator*()               noexcept { return *_ptr; }
		inline const IUNKNOWN& operator*()   const noexcept { return *_ptr; }

		inline IUNKNOWN* ptr()       noexcept { return _ptr; }
		inline const IUNKNOWN* ptr() const noexcept { return _ptr; }
		inline IUNKNOWN& ref() { return *_ptr; }
		inline const IUNKNOWN& ref()         const { return *_ptr; }

		// IUNKNOWN** 형태로 주소 반환 — COM API 호출 시 유용
		// 생성할때만 받고 넘겨줄떼는 ptr()로 넘기는 형태로 사용해야 한다.
		inline IUNKNOWN** address_of() noexcept
		{
			reset(); // 기존 _ptr Release 후 nullptr로 초기화
			return &_ptr;
		}

		template<typename U>
		inline U* cast() noexcept
		{
			static_assert(std::is_base_of<IUNKNOWN, U>::value, "U must inherit from IUNKNOWN");
			return static_cast<U*>(_ptr);
		}

		template<typename U>
		inline const U* cast() const noexcept
		{
			static_assert(std::is_base_of<IUNKNOWN, U>::value, "U must inherit from IUNKNOWN");
			return static_cast<const U*>(_ptr);
		}

		template<typename U>
		U& cast_ref() noexcept
		{
			static_assert(std::is_base_of<IUNKNOWN, U>::value, "U must inherit from IUNKNOWN");
			return static_cast<U&>(*_ptr);
		}

		template<typename U>
		const U& cast_ref() const noexcept
		{
			static_assert(std::is_base_of<IUNKNOWN, U>::value, "U must inherit from IUNKNOWN");
			return static_cast<const U&>(*_ptr);
		}

		bool operator==(const iunknown_ptr& o) const noexcept { return _ptr == o._ptr; }
		bool operator!=(const iunknown_ptr& o) const noexcept { return _ptr != o._ptr; }
		bool operator< (const iunknown_ptr& o) const noexcept { return _ptr < o._ptr; }

		// nullptr 비교 — if (p == nullptr) 형태 지원
		bool operator==(std::nullptr_t) const noexcept { return _ptr == nullptr; }
		bool operator!=(std::nullptr_t) const noexcept { return _ptr != nullptr; }
	};

	// nullptr == p 형태도 지원 (좌우 대칭)
	inline bool operator==(std::nullptr_t, const iunknown_ptr& p) noexcept { return p == nullptr; }
	inline bool operator!=(std::nullptr_t, const iunknown_ptr& p) noexcept { return p != nullptr; }

};
