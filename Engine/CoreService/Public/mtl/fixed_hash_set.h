#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>


// ----------------------------------------------------------------------------
// mtl::fixed_hash_set<Key, N, Hash, KeyEqual>
//
// 고정 용량 해시 셋. Open addressing + linear probing 기반.
// fixed_hash_map과 동일한 자료구조 / 정책을 사용한다 (Value만 제거된 형태).
//
// 사용 용도:
//   - 멤버십 테스트 (이 ID가 활성인가, 이 태그를 가졌는가).
//   - 중복 제거 (방문한 노드 set, 처리 완료한 ID set).
//   - 집합 연산 (visited, dirty flags, active component types).
//   - 빠른 contains() 체크가 핵심인 경우.
//
// 안 쓰는 게 좋은 경우:
//   - 매우 작은 데이터셋 (N < 8): linear search가 빠를 수 있음
//     → fixed_vector + std::find 고려.
//   - 정렬된 순서가 필요한 경우 → flat_set (구현 예정).
//   - 키→값 매핑이 필요한 경우 → fixed_hash_map.
//   - 키가 신뢰할 수 없는 외부 입력 (충돌 공격 가능성).
//
// 구조:
//   - kBucketCount = std::bit_ceil(N * 2)  → 부하율 < 0.5 보장 → 평균 probe ~2회.
//   - 슬롯 상태 배열(uint8_t[])과 데이터 버퍼를 분리 → probe 시 캐시 친화.
//   - Backward-shift deletion → tombstone 없음, 시간에 따른 성능 저하 없음.
//
// 정책:
//   - Debug: 용량 초과 시 assert.
//   - Release: 용량 초과 삽입은 {end(), false}.
//
// 사용 예:
//   mtl::fixed_hash_set<int, 64> visited;
//   visited.insert(42);
//   if (visited.contains(42)) { /* ... */ }
//   for (const auto& k : visited) { /* ... */ }
// ----------------------------------------------------------------------------

namespace mtl
{
	template <
		typename Key,
		std::size_t N,
		typename Hash = std::hash<Key>,
		typename KeyEqual = std::equal_to<Key>
	>
	class fixed_hash_set
	{
		static_assert(N > 0, "fixed_hash_set: N must be greater than 0.");

	public:
		using key_type        = Key;
		using value_type      = Key;                 // 셋에서는 value_type == key_type
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using hasher          = Hash;
		using key_equal       = KeyEqual;
		using reference       = const Key&;          // 키는 수정 불가
		using const_reference = const Key&;
		using pointer         = const Key*;
		using const_pointer   = const Key*;

		static constexpr size_type kCapacity    = N;
		static constexpr size_type kBucketCount = std::bit_ceil(N * 2);

	private:
		enum slot_state : std::uint8_t { kEmpty = 0, kOccupied = 1 };

		// ----------------------------------------------------------------
		// 멤버 (선언 순서가 초기화 순서)
		// ----------------------------------------------------------------
		alignas(Key) std::byte m_storage[kBucketCount * sizeof(Key)];
		std::uint8_t           m_state[kBucketCount]{};   // 0(empty) 초기화
		size_type              m_size = 0;
		Hash                   m_hash{};
		KeyEqual               m_key_equal{};

		// ----------------------------------------------------------------
		// 내부 헬퍼
		// ----------------------------------------------------------------
		static constexpr size_type mask() noexcept { return kBucketCount - 1; }

		Key* slot_ptr(size_type i) noexcept
		{
			return std::launder(reinterpret_cast<Key*>(&m_storage[i * sizeof(Key)]));
		}
		const Key* slot_ptr(size_type i) const noexcept
		{
			return std::launder(reinterpret_cast<const Key*>(&m_storage[i * sizeof(Key)]));
		}

		void* slot_raw(size_type i) noexcept
		{
			return static_cast<void*>(&m_storage[i * sizeof(Key)]);
		}

		const Key& key_at(size_type i) const noexcept { return *slot_ptr(i); }

		size_type hash_index(const Key& key) const noexcept
		{
			return m_hash(key) & mask();
		}

		static constexpr size_type next_idx(size_type i) noexcept
		{
			return (i + 1) & mask();
		}

		// 키를 probe해서 { 인덱스, 찾았는지 }를 반환.
		// 키가 없으면 삽입할 빈 슬롯의 인덱스를 반환 (부하율 < 1이므로 항상 존재).
		std::pair<size_type, bool> probe(const Key& key) const noexcept
		{
			size_type i = hash_index(key);
			while (true)
			{
				if (m_state[i] == kEmpty) return { i, false };
				if (m_key_equal(key_at(i), key)) return { i, true };
				i = next_idx(i);
			}
		}

		void destroy_all() noexcept
		{
			if constexpr (!std::is_trivially_destructible_v<Key>)
			{
				for (size_type i = 0; i < kBucketCount; ++i)
					if (m_state[i] == kOccupied)
						slot_ptr(i)->~Key();
			}
		}

		size_type first_occupied() const noexcept
		{
			size_type i = 0;
			while (i < kBucketCount && m_state[i] == kEmpty) ++i;
			return i;
		}

		// Backward-shift deletion (no tombstone).
		void erase_at(size_type pos) noexcept
		{
			slot_ptr(pos)->~Key();
			m_state[pos] = kEmpty;
			--m_size;

			size_type hole = pos;
			size_type cur  = next_idx(hole);
			while (m_state[cur] == kOccupied)
			{
				const size_type h = hash_index(key_at(cur));
				if (((hole - h) & mask()) < ((cur - h) & mask()))
				{
					::new (slot_raw(hole)) Key(std::move(*slot_ptr(cur)));
					slot_ptr(cur)->~Key();
					m_state[hole] = kOccupied;
					m_state[cur]  = kEmpty;
					hole = cur;
				}
				cur = next_idx(cur);
			}
		}

	public:
		// ----------------------------------------------------------------
		// Iterator (forward, const-only — 셋의 원소는 수정 불가)
		// ----------------------------------------------------------------
		class const_iterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type        = Key;
			using difference_type   = std::ptrdiff_t;
			using reference         = const Key&;
			using pointer           = const Key*;

		private:
			const fixed_hash_set* m_set = nullptr;
			size_type             m_index = 0;

			void skip_empty() noexcept
			{
				while (m_index < kBucketCount && m_set->m_state[m_index] == kEmpty)
					++m_index;
			}

			friend fixed_hash_set;

		public:
			const_iterator() noexcept = default;
			const_iterator(const fixed_hash_set* s, size_type i) noexcept : m_set(s), m_index(i) {}

			reference operator*()  const noexcept { return *m_set->slot_ptr(m_index); }
			pointer   operator->() const noexcept { return  m_set->slot_ptr(m_index); }

			const_iterator& operator++()    noexcept { ++m_index; skip_empty(); return *this; }
			const_iterator  operator++(int) noexcept { const_iterator t(*this); ++(*this); return t; }

			friend bool operator==(const const_iterator& a, const const_iterator& b) noexcept { return a.m_index == b.m_index; }
			friend bool operator!=(const const_iterator& a, const const_iterator& b) noexcept { return a.m_index != b.m_index; }
		};

		// 셋에서 iterator == const_iterator (키 수정 불가).
		using iterator = const_iterator;

		// ----------------------------------------------------------------
		// 생성/소멸
		// ----------------------------------------------------------------
		fixed_hash_set() = default;

		explicit fixed_hash_set(const Hash& h, const KeyEqual& eq = KeyEqual{})
			: m_hash(h), m_key_equal(eq)
		{
		}

		fixed_hash_set(std::initializer_list<Key> il)
		{
			for (const auto& k : il) insert(k);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_hash_set(InputIt first, InputIt last)
		{
			for (; first != last; ++first) insert(*first);
		}

		fixed_hash_set(const fixed_hash_set& other)
			: m_size(other.m_size), m_hash(other.m_hash), m_key_equal(other.m_key_equal)
		{
			for (size_type i = 0; i < kBucketCount; ++i)
			{
				m_state[i] = other.m_state[i];
				if (m_state[i] == kOccupied)
					::new (slot_raw(i)) Key(*other.slot_ptr(i));
			}
		}

		fixed_hash_set(fixed_hash_set&& other)
			: m_size(other.m_size), m_hash(std::move(other.m_hash)), m_key_equal(std::move(other.m_key_equal))
		{
			for (size_type i = 0; i < kBucketCount; ++i)
			{
				m_state[i] = other.m_state[i];
				if (m_state[i] == kOccupied)
					::new (slot_raw(i)) Key(std::move(*other.slot_ptr(i)));
			}
			other.clear();
		}

		~fixed_hash_set()
		{
			destroy_all();
		}

		fixed_hash_set& operator=(const fixed_hash_set& other)
		{
			if (this == &other) return *this;
			clear();
			m_hash      = other.m_hash;
			m_key_equal = other.m_key_equal;
			m_size      = other.m_size;
			for (size_type i = 0; i < kBucketCount; ++i)
			{
				m_state[i] = other.m_state[i];
				if (m_state[i] == kOccupied)
					::new (slot_raw(i)) Key(*other.slot_ptr(i));
			}
			return *this;
		}

		fixed_hash_set& operator=(fixed_hash_set&& other)
		{
			if (this == &other) return *this;
			clear();
			m_hash      = std::move(other.m_hash);
			m_key_equal = std::move(other.m_key_equal);
			m_size      = other.m_size;
			for (size_type i = 0; i < kBucketCount; ++i)
			{
				m_state[i] = other.m_state[i];
				if (m_state[i] == kOccupied)
					::new (slot_raw(i)) Key(std::move(*other.slot_ptr(i)));
			}
			other.clear();
			return *this;
		}

		// ----------------------------------------------------------------
		// Capacity
		// ----------------------------------------------------------------
		size_type size()  const noexcept { return m_size; }
		bool      empty() const noexcept { return m_size == 0; }
		bool      full()  const noexcept { return m_size >= N; }

		static constexpr size_type capacity()     noexcept { return N; }
		static constexpr size_type max_size()     noexcept { return N; }
		static constexpr size_type bucket_count() noexcept { return kBucketCount; }

		float load_factor() const noexcept
		{
			return static_cast<float>(m_size) / static_cast<float>(kBucketCount);
		}

		// ----------------------------------------------------------------
		// Iterators
		// ----------------------------------------------------------------
		iterator       begin()        noexcept { return iterator(this, first_occupied()); }
		const_iterator begin()  const noexcept { return const_iterator(this, first_occupied()); }
		const_iterator cbegin() const noexcept { return begin(); }
		iterator       end()          noexcept { return iterator(this, kBucketCount); }
		const_iterator end()    const noexcept { return const_iterator(this, kBucketCount); }
		const_iterator cend()   const noexcept { return end(); }

		// ----------------------------------------------------------------
		// Lookup
		// ----------------------------------------------------------------
		iterator find(const Key& key) noexcept
		{
			if (m_size == 0) return end();
			auto [idx, found] = probe(key);
			return found ? iterator(this, idx) : end();
		}

		const_iterator find(const Key& key) const noexcept
		{
			if (m_size == 0) return end();
			auto [idx, found] = probe(key);
			return found ? const_iterator(this, idx) : end();
		}

		bool contains(const Key& key) const noexcept { return find(key) != end(); }
		size_type count(const Key& key) const noexcept { return contains(key) ? 1 : 0; }

		// ----------------------------------------------------------------
		// Modifiers
		// ----------------------------------------------------------------
		std::pair<iterator, bool> insert(const Key& key)
		{
			if (m_size == 0)
			{
				const size_type idx = hash_index(key);
				::new (slot_raw(idx)) Key(key);
				m_state[idx] = kOccupied;
				++m_size;
				return { iterator(this, idx), true };
			}

			auto [idx, found] = probe(key);
			if (found) return { iterator(this, idx), false };

			assert(m_size < N && "fixed_hash_set: 용량 초과");
			if (m_size >= N) return { end(), false };  // release-safe

			::new (slot_raw(idx)) Key(key);
			m_state[idx] = kOccupied;
			++m_size;
			return { iterator(this, idx), true };
		}

		std::pair<iterator, bool> insert(Key&& key)
		{
			if (m_size == 0)
			{
				const size_type idx = hash_index(key);
				::new (slot_raw(idx)) Key(std::move(key));
				m_state[idx] = kOccupied;
				++m_size;
				return { iterator(this, idx), true };
			}

			auto [idx, found] = probe(key);
			if (found) return { iterator(this, idx), false };

			assert(m_size < N && "fixed_hash_set: 용량 초과");
			if (m_size >= N) return { end(), false };

			::new (slot_raw(idx)) Key(std::move(key));
			m_state[idx] = kOccupied;
			++m_size;
			return { iterator(this, idx), true };
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		void insert(InputIt first, InputIt last)
		{
			for (; first != last; ++first) insert(*first);
		}

		void insert(std::initializer_list<Key> il)
		{
			for (const auto& k : il) insert(k);
		}

		// 키를 인자에서 in-place 구성한 뒤 삽입 시도.
		// 임시 키를 만들어야 probe할 수 있으므로 try_emplace 같은 zero-copy는 불가.
		template <typename... Args>
		std::pair<iterator, bool> emplace(Args&&... args)
		{
			Key tmp(std::forward<Args>(args)...);
			return insert(std::move(tmp));
		}

		size_type erase(const Key& key) noexcept
		{
			if (m_size == 0) return 0;
			auto [idx, found] = probe(key);
			if (!found) return 0;
			erase_at(idx);
			return 1;
		}

		iterator erase(const_iterator pos) noexcept
		{
			assert(pos != end() && "fixed_hash_set::erase: end() 삭제 불가");
			const size_type idx = pos.m_index;
			erase_at(idx);
			iterator it(this, idx);
			it.skip_empty();
			return it;
		}

		void clear() noexcept
		{
			destroy_all();
			std::memset(m_state, 0, kBucketCount);
			m_size = 0;
		}

		// 풀이 인플레이스라서 O(N).
		void swap(fixed_hash_set& other)
		{
			if (this == &other) return;
			fixed_hash_set tmp(std::move(*this));
			*this = std::move(other);
			other = std::move(tmp);
		}

		// ----------------------------------------------------------------
		// Hash policy 접근
		// ----------------------------------------------------------------
		hasher    hash_function() const noexcept { return m_hash; }
		key_equal key_eq()        const noexcept { return m_key_equal; }
	};

	// ----------------------------------------------------------------------------
	// 비교 — 양쪽이 같은 키 집합을 가지면 동등
	// ----------------------------------------------------------------------------
	template <typename K, std::size_t N1, std::size_t N2,
		typename H1, typename H2, typename E1, typename E2>
	bool operator==(const fixed_hash_set<K, N1, H1, E1>& a,
		const fixed_hash_set<K, N2, H2, E2>& b)
	{
		if (a.size() != b.size()) return false;
		for (const auto& k : a)
			if (!b.contains(k)) return false;
		return true;
	}

	template <typename K, std::size_t N1, std::size_t N2,
		typename H1, typename H2, typename E1, typename E2>
	bool operator!=(const fixed_hash_set<K, N1, H1, E1>& a,
		const fixed_hash_set<K, N2, H2, E2>& b)
	{
		return !(a == b);
	}

	template <typename K, std::size_t N, typename H, typename E>
	void swap(fixed_hash_set<K, N, H, E>& a, fixed_hash_set<K, N, H, E>& b)
	{
		a.swap(b);
	}

} // namespace mtl
