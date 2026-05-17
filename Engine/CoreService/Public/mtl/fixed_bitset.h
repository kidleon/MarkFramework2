#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <type_traits>


// ----------------------------------------------------------------------------
// mtl::fixed_bitset<N> — 컴파일 타임 크기의 비트 배열.
//
// 사용 용도:
//   - ECS 컴포넌트 마스크 (어떤 컴포넌트가 활성인지).
//   - 플래그 시스템 (IsVisible, CastsShadow, IsCollidable, ...).
//   - 렌더 패스 마스크 (어떤 패스에 포함되는지).
//   - 입력 키/버튼 상태 (256개 키코드 등).
//   - 슬롯 점유 비트맵 (object pool, page allocator의 "free" 비트).
//   - dirty flag 모음 (이번 프레임 변경된 엔티티들).
//
// 안 쓰는 게 좋은 경우:
//   - 키가 sparse하거나 임의 정수 → fixed_hash_set 사용.
//   - 한 두 개 플래그만 → 그냥 bool / uint32_t bit flag로 충분.
//   - N이 매우 큼 + 항상 sparse → sparse 표현 (vector<size_t>) 고려.
//
// 구조:
//   - 내부 저장은 uint64_t[(N+63)/64].
//   - 모든 비트 연산은 워드 단위로 수행 → SIMD에 가까운 효율.
//   - std::popcount / std::countr_zero (<bit>) 활용 → CPU 인트린식으로 컴파일.
//   - 마지막 워드의 불사용 비트는 항상 0으로 유지 (invariant).
//
// 사용 예:
//   mtl::fixed_bitset<64> mask;
//   mask.set(kTransform);
//   mask.set(kRenderable);
//   if (mask.test(kTransform)) { ... }
//   if ((mask & required) == required) { /* 필요한 컴포넌트 다 있음 */ }
//   for (auto i : mask.set_indices()) { /* 셋팅된 인덱스 순회 */ }
// ----------------------------------------------------------------------------

namespace mtl
{
	template <std::size_t N>
	class fixed_bitset
	{
		static_assert(N > 0, "fixed_bitset: N must be greater than 0.");

	public:
		using word_type = std::uint64_t;
		using size_type = std::size_t;

		static constexpr size_type kBitCount    = N;
		static constexpr size_type kBitsPerWord = 64;
		static constexpr size_type kWordCount   = (N + kBitsPerWord - 1) / kBitsPerWord;

		static constexpr size_type npos = static_cast<size_type>(-1);

	private:
		// 마지막 워드의 유효 비트 마스크 (불사용 상위 비트는 항상 0 유지).
		static constexpr word_type kLastWordMask =
			(N % kBitsPerWord == 0)
			? ~word_type(0)
			: ((word_type(1) << (N % kBitsPerWord)) - 1);

		word_type m_words[kWordCount]{};   // 0으로 초기화

		static constexpr size_type word_index(size_type pos) noexcept { return pos / kBitsPerWord; }
		static constexpr size_type bit_index(size_type pos)  noexcept { return pos % kBitsPerWord; }
		static constexpr word_type bit_mask(size_type pos)   noexcept { return word_type(1) << bit_index(pos); }

	public:
		// ----------------------------------------------------------------
		// 생성
		// ----------------------------------------------------------------
		fixed_bitset() noexcept = default;

		// 초기 N개 비트를 value의 하위 비트로 채움 (std::bitset 호환).
		// m_words[1..] 는 default member initializer로 0 유지.
		explicit fixed_bitset(word_type value) noexcept
		{
			if constexpr (kWordCount == 1)
				m_words[0] = value & kLastWordMask;   // N < 64면 상위 잘라냄
			else
				m_words[0] = value;                   // word 0는 64비트 풀 사용
		}

		// ----------------------------------------------------------------
		// std::bitset 스타일 reference 프록시
		// ----------------------------------------------------------------
		class reference
		{
		public:
			reference(fixed_bitset* bs, size_type pos) noexcept : m_bs(bs), m_pos(pos) {}

			operator bool()       const noexcept { return m_bs->test(m_pos); }
			bool     operator~()  const noexcept { return !m_bs->test(m_pos); }

			reference& operator=(bool x) noexcept { m_bs->set(m_pos, x); return *this; }
			reference& operator=(const reference& other) noexcept { return *this = static_cast<bool>(other); }
			reference& flip() noexcept { m_bs->flip(m_pos); return *this; }

		private:
			fixed_bitset* m_bs;
			size_type     m_pos;
		};

		// ----------------------------------------------------------------
		// 단일 비트 접근
		// ----------------------------------------------------------------
		bool test(size_type pos) const noexcept
		{
			assert(pos < N && "fixed_bitset::test: 인덱스 범위 초과");
			return (m_words[word_index(pos)] & bit_mask(pos)) != 0;
		}

		reference operator[](size_type pos) noexcept
		{
			assert(pos < N);
			return reference(this, pos);
		}

		bool operator[](size_type pos) const noexcept
		{
			assert(pos < N);
			return test(pos);
		}

		// ----------------------------------------------------------------
		// 단일 비트 수정
		// ----------------------------------------------------------------
		fixed_bitset& set(size_type pos, bool value = true) noexcept
		{
			assert(pos < N);
			if (value) m_words[word_index(pos)] |=  bit_mask(pos);
			else       m_words[word_index(pos)] &= ~bit_mask(pos);
			return *this;
		}

		fixed_bitset& reset(size_type pos) noexcept
		{
			assert(pos < N);
			m_words[word_index(pos)] &= ~bit_mask(pos);
			return *this;
		}

		fixed_bitset& flip(size_type pos) noexcept
		{
			assert(pos < N);
			m_words[word_index(pos)] ^= bit_mask(pos);
			return *this;
		}

		// ----------------------------------------------------------------
		// 전체 비트 수정
		// ----------------------------------------------------------------
		fixed_bitset& set() noexcept
		{
			for (size_type i = 0; i + 1 < kWordCount; ++i)
				m_words[i] = ~word_type(0);
			m_words[kWordCount - 1] = kLastWordMask;
			return *this;
		}

		fixed_bitset& reset() noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i) m_words[i] = 0;
			return *this;
		}

		fixed_bitset& flip() noexcept
		{
			for (size_type i = 0; i + 1 < kWordCount; ++i)
				m_words[i] = ~m_words[i];
			m_words[kWordCount - 1] = (~m_words[kWordCount - 1]) & kLastWordMask;
			return *this;
		}

		// ----------------------------------------------------------------
		// 집계
		// ----------------------------------------------------------------
		size_type count() const noexcept
		{
			size_type c = 0;
			for (size_type i = 0; i < kWordCount; ++i)
				c += static_cast<size_type>(std::popcount(m_words[i]));
			return c;
		}

		bool any() const noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i)
				if (m_words[i] != 0) return true;
			return false;
		}

		bool none() const noexcept { return !any(); }

		bool all() const noexcept
		{
			for (size_type i = 0; i + 1 < kWordCount; ++i)
				if (m_words[i] != ~word_type(0)) return false;
			return m_words[kWordCount - 1] == kLastWordMask;
		}

		static constexpr size_type size()      noexcept { return N; }
		static constexpr size_type bit_count() noexcept { return N; }
		static constexpr size_type word_count() noexcept { return kWordCount; }

		// ----------------------------------------------------------------
		// 비트 스캔 (게임엔진용 핵심)
		// ----------------------------------------------------------------
		// 첫 set 비트의 인덱스. 없으면 npos.
		size_type find_first() const noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i)
			{
				if (m_words[i] != 0)
					return i * kBitsPerWord + static_cast<size_type>(std::countr_zero(m_words[i]));
			}
			return npos;
		}

		// pos 이후의 첫 set 비트 인덱스. 없으면 npos.
		size_type find_next(size_type pos) const noexcept
		{
			++pos;
			if (pos >= N) return npos;

			size_type wi = word_index(pos);
			const size_type bi = bit_index(pos);

			// 현재 워드의 bi 이상 비트만 검사
			word_type w = m_words[wi] & (~word_type(0) << bi);
			if (w != 0)
				return wi * kBitsPerWord + static_cast<size_type>(std::countr_zero(w));

			for (++wi; wi < kWordCount; ++wi)
			{
				if (m_words[wi] != 0)
					return wi * kBitsPerWord + static_cast<size_type>(std::countr_zero(m_words[wi]));
			}
			return npos;
		}

		// 첫 unset 비트 인덱스. 없으면 npos. (슬롯 할당 등에 사용)
		size_type find_first_unset() const noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i)
			{
				word_type w = ~m_words[i];
				if (i == kWordCount - 1) w &= kLastWordMask;
				if (w != 0)
					return i * kBitsPerWord + static_cast<size_type>(std::countr_zero(w));
			}
			return npos;
		}

		size_type find_next_unset(size_type pos) const noexcept
		{
			++pos;
			if (pos >= N) return npos;

			size_type wi = word_index(pos);
			const size_type bi = bit_index(pos);

			word_type w = ~m_words[wi] & (~word_type(0) << bi);
			if (wi == kWordCount - 1) w &= kLastWordMask;
			if (w != 0)
				return wi * kBitsPerWord + static_cast<size_type>(std::countr_zero(w));

			for (++wi; wi < kWordCount; ++wi)
			{
				word_type ww = ~m_words[wi];
				if (wi == kWordCount - 1) ww &= kLastWordMask;
				if (ww != 0)
					return wi * kBitsPerWord + static_cast<size_type>(std::countr_zero(ww));
			}
			return npos;
		}

		// 모든 set 비트에 대해 콜백 호출. for_each_set([](size_t i){ ... });
		// 워드 단위 bit-scan으로 매우 빠름.
		template <typename F>
		void for_each_set(F&& f) const
		{
			for (size_type i = 0; i < kWordCount; ++i)
			{
				word_type w = m_words[i];
				while (w != 0)
				{
					const size_type b = static_cast<size_type>(std::countr_zero(w));
					f(i * kBitsPerWord + b);
					w &= w - 1;  // 가장 낮은 set 비트 제거
				}
			}
		}

		// ----------------------------------------------------------------
		// set 비트 범위 순회 (range-based for 지원)
		// ----------------------------------------------------------------
		class set_bits_iterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type        = size_type;
			using difference_type   = std::ptrdiff_t;
			using reference         = size_type;
			using pointer           = void;

			set_bits_iterator() noexcept = default;
			set_bits_iterator(const fixed_bitset* bs, size_type pos) noexcept : m_bs(bs), m_pos(pos) {}

			size_type operator*() const noexcept { return m_pos; }

			set_bits_iterator& operator++() noexcept
			{
				m_pos = m_bs->find_next(m_pos);
				return *this;
			}

			set_bits_iterator operator++(int) noexcept { auto t = *this; ++(*this); return t; }

			friend bool operator==(const set_bits_iterator& a, const set_bits_iterator& b) noexcept { return a.m_pos == b.m_pos; }
			friend bool operator!=(const set_bits_iterator& a, const set_bits_iterator& b) noexcept { return a.m_pos != b.m_pos; }

		private:
			const fixed_bitset* m_bs = nullptr;
			size_type           m_pos = npos;
		};

		class set_bits_view
		{
		public:
			explicit set_bits_view(const fixed_bitset* bs) noexcept : m_bs(bs) {}
			set_bits_iterator begin() const noexcept { return set_bits_iterator(m_bs, m_bs->find_first()); }
			set_bits_iterator end()   const noexcept { return set_bits_iterator(m_bs, npos); }

		private:
			const fixed_bitset* m_bs;
		};

		set_bits_view set_indices() const noexcept { return set_bits_view(this); }

		// ----------------------------------------------------------------
		// 비트 연산 (in-place)
		// ----------------------------------------------------------------
		fixed_bitset& operator&=(const fixed_bitset& rhs) noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i) m_words[i] &= rhs.m_words[i];
			return *this;
		}

		fixed_bitset& operator|=(const fixed_bitset& rhs) noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i) m_words[i] |= rhs.m_words[i];
			return *this;
		}

		fixed_bitset& operator^=(const fixed_bitset& rhs) noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i) m_words[i] ^= rhs.m_words[i];
			return *this;
		}

		fixed_bitset operator~() const noexcept
		{
			fixed_bitset result;
			for (size_type i = 0; i + 1 < kWordCount; ++i)
				result.m_words[i] = ~m_words[i];
			result.m_words[kWordCount - 1] = (~m_words[kWordCount - 1]) & kLastWordMask;
			return result;
		}

		// ----------------------------------------------------------------
		// 비교
		// ----------------------------------------------------------------
		bool operator==(const fixed_bitset& rhs) const noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i)
				if (m_words[i] != rhs.m_words[i]) return false;
			return true;
		}

		bool operator!=(const fixed_bitset& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		// ----------------------------------------------------------------
		// 부분집합 / 포함 관계 (게임엔진에서 흔히 씀)
		// ----------------------------------------------------------------
		// *this가 mask의 모든 비트를 포함하는가? "필요 컴포넌트 다 갖췄나" 체크에 사용.
		bool contains(const fixed_bitset& mask) const noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i)
				if ((m_words[i] & mask.m_words[i]) != mask.m_words[i]) return false;
			return true;
		}

		// 두 비트셋이 겹치는 비트가 있는가?
		bool intersects(const fixed_bitset& other) const noexcept
		{
			for (size_type i = 0; i < kWordCount; ++i)
				if ((m_words[i] & other.m_words[i]) != 0) return true;
			return false;
		}

		// ----------------------------------------------------------------
		// 저수준 접근 (직렬화, 디버그용)
		// ----------------------------------------------------------------
		const word_type* data() const noexcept { return m_words; }
		word_type*       data()       noexcept { return m_words; }
	};

	// ----------------------------------------------------------------------------
	// 자유 비트 연산자
	// ----------------------------------------------------------------------------
	template <std::size_t N>
	fixed_bitset<N> operator&(fixed_bitset<N> a, const fixed_bitset<N>& b) noexcept
	{
		a &= b; return a;
	}

	template <std::size_t N>
	fixed_bitset<N> operator|(fixed_bitset<N> a, const fixed_bitset<N>& b) noexcept
	{
		a |= b; return a;
	}

	template <std::size_t N>
	fixed_bitset<N> operator^(fixed_bitset<N> a, const fixed_bitset<N>& b) noexcept
	{
		a ^= b; return a;
	}

} // namespace mtl
