#pragma once

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>


// ----------------------------------------------------------------------------
// mtl::fixed_ring_buffer<T, N> — 고정 용량 원형 FIFO 큐.
//
// 사용 용도:
//   - 잡 큐 / 명령 큐 (메인 → 워커 스레드 작업 전달; 단일 스레드 영역 안에서).
//   - 입력 이벤트 큐 (한 프레임에 누적된 입력을 시퀀스 보존하며 처리).
//   - 오디오 샘플 버퍼 (디버그용 — 실시간은 SPSC lock-free 필요).
//   - 프레임 시간 히스토리 / FPS 그래프 (최근 60~120 프레임 ring으로 유지).
//   - 로그 ring buffer (가장 최근 N개만 유지, 오래된 건 자동 덮어쓰기).
//   - 슬라이딩 윈도우 통계 (최근 N개 샘플 평균/분산).
//
// 안 쓰는 게 좋은 경우:
//   - 임의 순서 삽입/삭제가 필요 → fixed_list.
//   - 순서 무관한 키 조회 → fixed_hash_map / hash_set.
//   - 멀티스레드 동시 접근 → SPSC lock-free ring (v1에는 미포함).
//
// 특성:
//   - 단일 스레드 가정.
//   - push/pop 모두 O(1).
//   - 두 가지 push 정책:
//       * push()            : 가득 차면 실패 (false 반환). 잡 큐에 적합.
//       * push_overwrite()  : 가득 차면 가장 오래된 항목을 덮어씀. 로그/이력에 적합.
//   - 동적 할당 없음. 내부 버퍼에 placement-new로 직접 저장.
//   - 인덱스 [0]은 가장 오래된 (front), [size-1]은 가장 최신 (back).
//
// 사용 예 (FIFO 큐):
//   mtl::fixed_ring_buffer<Job, 64> jobs;
//   if (!jobs.push(j)) { /* full — 처리 후 재시도 */ }
//   while (!jobs.empty()) {
//       Job j;
//       jobs.pop(j);
//       run(j);
//   }
//
// 사용 예 (FPS 히스토리):
//   mtl::fixed_ring_buffer<float, 120> frame_times;
//   frame_times.push_overwrite(dt);  // 항상 성공, 오래된 것 자동 제거
//   for (float t : frame_times) { /* 그래프 그리기 */ }
// ----------------------------------------------------------------------------

namespace mtl
{
	template <typename T, std::size_t N>
	class fixed_ring_buffer
	{
		static_assert(N > 0, "fixed_ring_buffer: N must be greater than 0.");

	public:
		using value_type      = T;
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference       = T&;
		using const_reference = const T&;
		using pointer         = T*;
		using const_pointer   = const T*;

		static constexpr size_type kCapacity = N;

	private:
		alignas(T) std::byte m_storage[N * sizeof(T)];
		size_type            m_head = 0;   // 가장 오래된 (front)
		size_type            m_size = 0;   // 현재 원소 수

		T* slot_ptr(size_type i) noexcept
		{
			return std::launder(reinterpret_cast<T*>(&m_storage[i * sizeof(T)]));
		}
		const T* slot_ptr(size_type i) const noexcept
		{
			return std::launder(reinterpret_cast<const T*>(&m_storage[i * sizeof(T)]));
		}
		void* slot_raw(size_type i) noexcept
		{
			return static_cast<void*>(&m_storage[i * sizeof(T)]);
		}

		// 인덱스 i가 N에 도달했으면 0으로 되감기. modulo 보다 빠름.
		static constexpr size_type wrap(size_type i) noexcept
		{
			return (i >= N) ? (i - N) : i;
		}

		// front로부터 offset만큼 떨어진 실제 슬롯 인덱스.
		size_type slot_at(size_type offset) const noexcept
		{
			return wrap(m_head + offset);
		}

		void destroy_all() noexcept
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				for (size_type i = 0; i < m_size; ++i)
					slot_ptr(slot_at(i))->~T();
			}
		}

	public:
		// ----------------------------------------------------------------
		// Iterator (forward) — front에서 back 방향으로 순회
		// ----------------------------------------------------------------
		template <bool IsConst>
		class iterator_t
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type        = T;
			using difference_type   = std::ptrdiff_t;
			using reference         = std::conditional_t<IsConst, const T&, T&>;
			using pointer           = std::conditional_t<IsConst, const T*, T*>;

		private:
			using buf_type = std::conditional_t<IsConst, const fixed_ring_buffer, fixed_ring_buffer>;
			buf_type* m_buf = nullptr;
			size_type m_offset = 0;   // m_head로부터의 오프셋

			friend fixed_ring_buffer;
			template <bool> friend class iterator_t;

		public:
			iterator_t() noexcept = default;
			iterator_t(buf_type* b, size_type off) noexcept : m_buf(b), m_offset(off) {}

			template <bool C = IsConst, std::enable_if_t<C, int> = 0>
			iterator_t(const iterator_t<false>& other) noexcept
				: m_buf(other.m_buf), m_offset(other.m_offset)
			{
			}

			reference operator*()  const noexcept { return *m_buf->slot_ptr(m_buf->slot_at(m_offset)); }
			pointer   operator->() const noexcept { return  m_buf->slot_ptr(m_buf->slot_at(m_offset)); }

			iterator_t& operator++()    noexcept { ++m_offset; return *this; }
			iterator_t  operator++(int) noexcept { iterator_t t(*this); ++(*this); return t; }

			friend bool operator==(const iterator_t& a, const iterator_t& b) noexcept
			{
				assert(a.m_buf == b.m_buf && "fixed_ring_buffer: 서로 다른 버퍼의 iterator 비교");
				return a.m_offset == b.m_offset;
			}
			friend bool operator!=(const iterator_t& a, const iterator_t& b) noexcept { return !(a == b); }
		};

		using iterator       = iterator_t<false>;
		using const_iterator = iterator_t<true>;

		// ----------------------------------------------------------------
		// 생성/소멸
		// ----------------------------------------------------------------
		fixed_ring_buffer() = default;

		fixed_ring_buffer(std::initializer_list<T> il)
		{
			for (const auto& v : il) push(v);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_ring_buffer(InputIt first, InputIt last)
		{
			for (; first != last; ++first) push(*first);
		}

		fixed_ring_buffer(const fixed_ring_buffer& other)
			: m_head(0), m_size(other.m_size)
		{
			for (size_type i = 0; i < other.m_size; ++i)
				::new (slot_raw(i)) T(*other.slot_ptr(other.slot_at(i)));
		}

		fixed_ring_buffer(fixed_ring_buffer&& other)
			: m_head(0), m_size(other.m_size)
		{
			for (size_type i = 0; i < other.m_size; ++i)
				::new (slot_raw(i)) T(std::move(*other.slot_ptr(other.slot_at(i))));
			other.clear();
		}

		~fixed_ring_buffer()
		{
			destroy_all();
		}

		fixed_ring_buffer& operator=(const fixed_ring_buffer& other)
		{
			if (this == &other) return *this;
			clear();
			for (size_type i = 0; i < other.m_size; ++i)
				::new (slot_raw(i)) T(*other.slot_ptr(other.slot_at(i)));
			m_head = 0;
			m_size = other.m_size;
			return *this;
		}

		fixed_ring_buffer& operator=(fixed_ring_buffer&& other)
		{
			if (this == &other) return *this;
			clear();
			for (size_type i = 0; i < other.m_size; ++i)
				::new (slot_raw(i)) T(std::move(*other.slot_ptr(other.slot_at(i))));
			m_head = 0;
			m_size = other.m_size;
			other.clear();
			return *this;
		}

		// ----------------------------------------------------------------
		// Capacity
		// ----------------------------------------------------------------
		size_type size()  const noexcept { return m_size; }
		bool      empty() const noexcept { return m_size == 0; }
		bool      full()  const noexcept { return m_size == N; }

		static constexpr size_type capacity() noexcept { return N; }
		static constexpr size_type max_size() noexcept { return N; }

		// 비어 있는 공간 수
		size_type available() const noexcept { return N - m_size; }

		// ----------------------------------------------------------------
		// 요소 접근
		// ----------------------------------------------------------------
		reference       front()       noexcept { assert(!empty()); return *slot_ptr(m_head); }
		const_reference front() const noexcept { assert(!empty()); return *slot_ptr(m_head); }

		reference       back()        noexcept { assert(!empty()); return *slot_ptr(slot_at(m_size - 1)); }
		const_reference back()  const noexcept { assert(!empty()); return *slot_ptr(slot_at(m_size - 1)); }

		// front 기준 offset (0 = front, size-1 = back)
		reference       operator[](size_type i)       noexcept { assert(i < m_size); return *slot_ptr(slot_at(i)); }
		const_reference operator[](size_type i) const noexcept { assert(i < m_size); return *slot_ptr(slot_at(i)); }

		// ----------------------------------------------------------------
		// Iterators
		// ----------------------------------------------------------------
		iterator       begin()        noexcept { return iterator(this, 0); }
		const_iterator begin()  const noexcept { return const_iterator(this, 0); }
		const_iterator cbegin() const noexcept { return begin(); }
		iterator       end()          noexcept { return iterator(this, m_size); }
		const_iterator end()    const noexcept { return const_iterator(this, m_size); }
		const_iterator cend()   const noexcept { return end(); }

		// ----------------------------------------------------------------
		// 푸시 — 실패 정책
		// ----------------------------------------------------------------
		// 가득 차면 아무것도 하지 않고 false 반환.
		bool push(const T& value)
		{
			if (m_size == N) return false;
			::new (slot_raw(slot_at(m_size))) T(value);
			++m_size;
			return true;
		}

		bool push(T&& value)
		{
			if (m_size == N) return false;
			::new (slot_raw(slot_at(m_size))) T(std::move(value));
			++m_size;
			return true;
		}

		template <typename... Args>
		bool emplace(Args&&... args)
		{
			if (m_size == N) return false;
			::new (slot_raw(slot_at(m_size))) T(std::forward<Args>(args)...);
			++m_size;
			return true;
		}

		// ----------------------------------------------------------------
		// 푸시 — 덮어쓰기 정책 (로그/이력에 적합)
		// ----------------------------------------------------------------
		// 가득 차면 가장 오래된 항목을 파괴하고 새 항목으로 교체. 항상 성공.
		void push_overwrite(const T& value)
		{
			if (m_size < N)
			{
				::new (slot_raw(slot_at(m_size))) T(value);
				++m_size;
			}
			else
			{
				// m_head 위치를 파괴 후 재구성, head 한 칸 전진.
				// 결과적으로 새 값은 ring의 "끝"에 위치.
				slot_ptr(m_head)->~T();
				::new (slot_raw(m_head)) T(value);
				m_head = wrap(m_head + 1);
			}
		}

		void push_overwrite(T&& value)
		{
			if (m_size < N)
			{
				::new (slot_raw(slot_at(m_size))) T(std::move(value));
				++m_size;
			}
			else
			{
				slot_ptr(m_head)->~T();
				::new (slot_raw(m_head)) T(std::move(value));
				m_head = wrap(m_head + 1);
			}
		}

		template <typename... Args>
		void emplace_overwrite(Args&&... args)
		{
			if (m_size < N)
			{
				::new (slot_raw(slot_at(m_size))) T(std::forward<Args>(args)...);
				++m_size;
			}
			else
			{
				slot_ptr(m_head)->~T();
				::new (slot_raw(m_head)) T(std::forward<Args>(args)...);
				m_head = wrap(m_head + 1);
			}
		}

		// ----------------------------------------------------------------
		// 팝
		// ----------------------------------------------------------------
		// front 제거. 비어 있으면 아무것도 안 하고 false.
		bool pop() noexcept
		{
			if (m_size == 0) return false;
			slot_ptr(m_head)->~T();
			m_head = wrap(m_head + 1);
			--m_size;
			return true;
		}

		// front를 out으로 옮기고 제거. 비어 있으면 false.
		bool pop(T& out)
		{
			if (m_size == 0) return false;
			out = std::move(*slot_ptr(m_head));
			slot_ptr(m_head)->~T();
			m_head = wrap(m_head + 1);
			--m_size;
			return true;
		}

		// ----------------------------------------------------------------
		// 기타
		// ----------------------------------------------------------------
		void clear() noexcept
		{
			destroy_all();
			m_head = 0;
			m_size = 0;
		}

		void swap(fixed_ring_buffer& other)
		{
			if (this == &other) return;
			fixed_ring_buffer tmp(std::move(*this));
			*this = std::move(other);
			other = std::move(tmp);
		}
	};

	// ----------------------------------------------------------------------------
	// 비교 — 길이와 순서 모두 같아야 동등
	// ----------------------------------------------------------------------------
	template <typename T, std::size_t N1, std::size_t N2>
	bool operator==(const fixed_ring_buffer<T, N1>& a, const fixed_ring_buffer<T, N2>& b)
	{
		if (a.size() != b.size()) return false;
		auto ai = a.begin();
		auto bi = b.begin();
		for (; ai != a.end(); ++ai, ++bi)
			if (!(*ai == *bi)) return false;
		return true;
	}

	template <typename T, std::size_t N1, std::size_t N2>
	bool operator!=(const fixed_ring_buffer<T, N1>& a, const fixed_ring_buffer<T, N2>& b)
	{
		return !(a == b);
	}

	template <typename T, std::size_t N>
	void swap(fixed_ring_buffer<T, N>& a, fixed_ring_buffer<T, N>& b)
	{
		a.swap(b);
	}

} // namespace mtl
