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
#include <tuple>
#include <type_traits>
#include <utility>


// ----------------------------------------------------------------------------
// mtl::fixed_hash_map<Key, Value, N, Hash, KeyEqual>
//
// 고정 용량 해시 맵. Open addressing + linear probing 기반.
//
// 사용 용도:
//   - 리소스 ID → 핸들 매핑 (텍스처 ID, 메시 ID, 사운드 ID → 내부 인덱스).
//   - 엔티티/컴포넌트 룩업 (EntityID → 컴포넌트 인덱스).
//   - 이름 → 객체 매핑 (네임드 노드, 본 이름, 머티리얼 이름 등).
//   - 캐시/메모이제이션 (해시된 키 → 계산된 결과).
//   - 빠른 키 조회가 핵심이고 N이 사전에 정해진 경우.
//
// 안 쓰는 게 좋은 경우:
//   - 매우 작은 데이터셋 (N < 8): linear search가 빠를 수 있음
//     → fixed_vector + 선형 탐색 고려.
//   - 키 순서가 중요한 경우 (정렬된 컨테이너 필요) → flat_map (구현 예정).
//   - 키가 신뢰할 수 없는 외부 입력 (충돌 공격 가능성).
//   - 활성 원소 수가 N의 50%를 자주 넘는 경우 → 일반 std::unordered_map 검토.
//
// 구조:
//   - kBucketCount = next_pow2(N * 2)  → 부하율 < 0.5 보장 → 평균 probe ~2회.
//   - 슬롯 상태 배열(uint8_t[])과 데이터 버퍼를 분리 → probe 시 캐시 친화.
//   - Backward-shift deletion → tombstone 없음, 시간에 따른 성능 저하 없음.
//
// 정책 (fixed_string과 동일):
//   - Debug: 용량 초과/키 부재 시 assert.
//   - Release: 용량 초과 삽입은 {end(), false}. at() 키 부재는 UB (호출자 책임).
//
// 사용 예:
//   mtl::fixed_hash_map<int, mtl::fixed_string<32>, 64> map;
//   map.try_emplace(1, "one");          // in-place 구성 — 가장 빠름
//   map[2] = "two";
//   if (auto it = map.find(1); it != map.end()) { ... }
//   for (auto& [k, v] : map) { ... }
// ----------------------------------------------------------------------------

namespace mtl
{
	template <
		typename Key,
		typename Value,
		std::size_t N,
		typename Hash = std::hash<Key>,
		typename KeyEqual = std::equal_to<Key>
	>
	class fixed_hash_map
	{
		static_assert(N > 0, "fixed_hash_map: N must be greater than 0.");

	public:
		using key_type        = Key;
		using mapped_type     = Value;
		using value_type      = std::pair<const Key, Value>;
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using hasher          = Hash;
		using key_equal       = KeyEqual;
		using reference       = value_type&;
		using const_reference = const value_type&;
		using pointer         = value_type*;
		using const_pointer   = const value_type*;

		static constexpr size_type kCapacity    = N;
		static constexpr size_type kBucketCount = std::bit_ceil(N * 2);

	private:
		enum slot_state : std::uint8_t { kEmpty = 0, kOccupied = 1 };

		// ----------------------------------------------------------------
		// 멤버 (선언 순서가 초기화 순서)
		// ----------------------------------------------------------------
		alignas(value_type) std::byte m_storage[kBucketCount * sizeof(value_type)];
		std::uint8_t                  m_state[kBucketCount]{};   // 0(empty) 초기화
		size_type                     m_size = 0;
		Hash                          m_hash{};
		KeyEqual                      m_key_equal{};

		// ----------------------------------------------------------------
		// 내부 헬퍼
		// ----------------------------------------------------------------
		static constexpr size_type mask() noexcept { return kBucketCount - 1; }

		value_type* slot_ptr(size_type i) noexcept
		{
			return std::launder(reinterpret_cast<value_type*>(&m_storage[i * sizeof(value_type)]));
		}
		const value_type* slot_ptr(size_type i) const noexcept
		{
			return std::launder(reinterpret_cast<const value_type*>(&m_storage[i * sizeof(value_type)]));
		}

		void* slot_raw(size_type i) noexcept
		{
			return static_cast<void*>(&m_storage[i * sizeof(value_type)]);
		}

		const Key&   key_at(size_type i)         const noexcept { return slot_ptr(i)->first; }
		Value&       value_at(size_type i)             noexcept { return slot_ptr(i)->second; }
		const Value& value_at(size_type i)       const noexcept { return slot_ptr(i)->second; }

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
			if constexpr (!std::is_trivially_destructible_v<value_type>)
			{
				for (size_type i = 0; i < kBucketCount; ++i)
					if (m_state[i] == kOccupied)
						slot_ptr(i)->~value_type();
			}
		}

		size_type first_occupied() const noexcept
		{
			if (m_size == 0) return kBucketCount;   // 빈 맵에서 전체 스캔 회피
			size_type i = 0;
			while (i < kBucketCount && m_state[i] == kEmpty) ++i;
			return i;
		}

		// Backward-shift deletion (no tombstone).
		void erase_at(size_type pos) noexcept
		{
			slot_ptr(pos)->~value_type();
			m_state[pos] = kEmpty;
			--m_size;

			size_type hole = pos;
			size_type cur  = next_idx(hole);
			while (m_state[cur] == kOccupied)
			{
				const size_type h = hash_index(key_at(cur));
				if (((hole - h) & mask()) < ((cur - h) & mask()))
				{
					::new (slot_raw(hole)) value_type(std::move(*slot_ptr(cur)));
					slot_ptr(cur)->~value_type();
					m_state[hole] = kOccupied;
					m_state[cur]  = kEmpty;
					hole = cur;
				}
				cur = next_idx(cur);
			}
		}

	public:
		// ----------------------------------------------------------------
		// Iterator (forward)
		// ----------------------------------------------------------------
		template <bool IsConst>
		class iterator_t
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type        = fixed_hash_map::value_type;
			using difference_type   = std::ptrdiff_t;
			using reference         = std::conditional_t<IsConst, const value_type&, value_type&>;
			using pointer           = std::conditional_t<IsConst, const value_type*, value_type*>;

		private:
			using map_type = std::conditional_t<IsConst, const fixed_hash_map, fixed_hash_map>;
			map_type* m_map = nullptr;
			size_type m_index = 0;

			void skip_empty() noexcept
			{
				while (m_index < kBucketCount && m_map->m_state[m_index] == kEmpty)
					++m_index;
			}

			friend fixed_hash_map;
			template <bool> friend class iterator_t;

		public:
			iterator_t() noexcept = default;
			iterator_t(map_type* m, size_type i) noexcept : m_map(m), m_index(i) {}

			template <bool C = IsConst, std::enable_if_t<C, int> = 0>
			iterator_t(const iterator_t<false>& other) noexcept
				: m_map(other.m_map), m_index(other.m_index)
			{
			}

			reference operator*()  const noexcept { return *m_map->slot_ptr(m_index); }
			pointer   operator->() const noexcept { return  m_map->slot_ptr(m_index); }

			iterator_t& operator++()    noexcept { ++m_index; skip_empty(); return *this; }
			iterator_t  operator++(int) noexcept { iterator_t t(*this); ++(*this); return t; }

			friend bool operator==(const iterator_t& a, const iterator_t& b) noexcept
			{
				assert(a.m_map == b.m_map && "fixed_hash_map: 서로 다른 맵의 iterator 비교");
				return a.m_index == b.m_index;
			}
			friend bool operator!=(const iterator_t& a, const iterator_t& b) noexcept { return !(a == b); }
		};

		using iterator       = iterator_t<false>;
		using const_iterator = iterator_t<true>;

		// ----------------------------------------------------------------
		// 생성/소멸
		// ----------------------------------------------------------------
		fixed_hash_map() = default;

		explicit fixed_hash_map(const Hash& h, const KeyEqual& eq = KeyEqual{})
			: m_hash(h), m_key_equal(eq)
		{
		}

		fixed_hash_map(std::initializer_list<value_type> il)
		{
			for (const auto& kv : il) insert(kv);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_hash_map(InputIt first, InputIt last)
		{
			for (; first != last; ++first) insert(*first);
		}

		fixed_hash_map(const fixed_hash_map& other)
			: m_size(other.m_size), m_hash(other.m_hash), m_key_equal(other.m_key_equal)
		{
			for (size_type i = 0; i < kBucketCount; ++i)
			{
				m_state[i] = other.m_state[i];
				if (m_state[i] == kOccupied)
					::new (slot_raw(i)) value_type(*other.slot_ptr(i));
			}
		}

		fixed_hash_map(fixed_hash_map&& other)
			: m_size(other.m_size), m_hash(std::move(other.m_hash)), m_key_equal(std::move(other.m_key_equal))
		{
			for (size_type i = 0; i < kBucketCount; ++i)
			{
				m_state[i] = other.m_state[i];
				if (m_state[i] == kOccupied)
					::new (slot_raw(i)) value_type(std::move(*other.slot_ptr(i)));
			}
			other.clear();
		}

		~fixed_hash_map()
		{
			destroy_all();
		}

		fixed_hash_map& operator=(const fixed_hash_map& other)
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
					::new (slot_raw(i)) value_type(*other.slot_ptr(i));
			}
			return *this;
		}

		fixed_hash_map& operator=(fixed_hash_map&& other)
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
					::new (slot_raw(i)) value_type(std::move(*other.slot_ptr(i)));
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

		bool contains(const Key& key) const noexcept
		{
			if (m_size == 0) return false;
			return probe(key).second;
		}

		size_type count(const Key& key) const noexcept { return contains(key) ? 1 : 0; }

		Value& at(const Key& key)
		{
			auto [idx, found] = probe(key);
			assert(found && "fixed_hash_map::at: key not found");
			return value_at(idx);
		}

		const Value& at(const Key& key) const
		{
			auto [idx, found] = probe(key);
			assert(found && "fixed_hash_map::at: key not found");
			return value_at(idx);
		}

		Value& operator[](const Key& key)
		{
			return try_emplace(key).first->second;
		}

		Value& operator[](Key&& key)
		{
			return try_emplace(std::move(key)).first->second;
		}

		// ----------------------------------------------------------------
		// Modifiers
		// ----------------------------------------------------------------
		std::pair<iterator, bool> insert(const value_type& v)
		{
			return try_emplace(v.first, v.second);
		}

		std::pair<iterator, bool> insert(value_type&& v)
		{
			// std::pair<const Key, V>::first 는 const라 key는 사실상 복사.
			// value는 move 가능.
			return try_emplace(v.first, std::move(v.second));
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		void insert(InputIt first, InputIt last)
		{
			for (; first != last; ++first) insert(*first);
		}

		void insert(std::initializer_list<value_type> il)
		{
			for (const auto& kv : il) insert(kv);
		}

		// 키가 없을 때만 in-place 구성. 있으면 기존 값 유지.
		// 가장 효율적인 삽입 — Key/Value 모두 인자에서 직접 구성.
		template <typename K, typename... Args>
		std::pair<iterator, bool> try_emplace(K&& key, Args&&... args)
		{
			if (m_size == 0)
			{
				// 빈 맵: 단순 삽입
				const size_type idx = hash_index(key);
				::new (slot_raw(idx)) value_type(
					std::piecewise_construct,
					std::forward_as_tuple(std::forward<K>(key)),
					std::forward_as_tuple(std::forward<Args>(args)...)
				);
				m_state[idx] = kOccupied;
				++m_size;
				return { iterator(this, idx), true };
			}

			auto [idx, found] = probe(key);
			if (found) return { iterator(this, idx), false };

			assert(m_size < N && "fixed_hash_map: 용량 초과");
			if (m_size >= N) return { end(), false };  // release-safe

			::new (slot_raw(idx)) value_type(
				std::piecewise_construct,
				std::forward_as_tuple(std::forward<K>(key)),
				std::forward_as_tuple(std::forward<Args>(args)...)
			);
			m_state[idx] = kOccupied;
			++m_size;
			return { iterator(this, idx), true };
		}

		// 키가 있으면 값을 덮어씀.
		template <typename K, typename V>
		std::pair<iterator, bool> insert_or_assign(K&& key, V&& value)
		{
			if (m_size == 0)
			{
				const size_type idx = hash_index(key);
				::new (slot_raw(idx)) value_type(
					std::piecewise_construct,
					std::forward_as_tuple(std::forward<K>(key)),
					std::forward_as_tuple(std::forward<V>(value))
				);
				m_state[idx] = kOccupied;
				++m_size;
				return { iterator(this, idx), true };
			}

			auto [idx, found] = probe(key);
			if (found)
			{
				value_at(idx) = std::forward<V>(value);
				return { iterator(this, idx), false };
			}

			assert(m_size < N && "fixed_hash_map: 용량 초과");
			if (m_size >= N) return { end(), false };

			::new (slot_raw(idx)) value_type(
				std::piecewise_construct,
				std::forward_as_tuple(std::forward<K>(key)),
				std::forward_as_tuple(std::forward<V>(value))
			);
			m_state[idx] = kOccupied;
			++m_size;
			return { iterator(this, idx), true };
		}

		// 일반 emplace — value_type을 인자로 구성. try_emplace 대비 손해 (임시 객체).
		template <typename... Args>
		std::pair<iterator, bool> emplace(Args&&... args)
		{
			value_type tmp(std::forward<Args>(args)...);
			return try_emplace(tmp.first, std::move(tmp.second));
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
			assert(pos != end() && "fixed_hash_map::erase: end() 삭제 불가");
			const size_type idx = pos.m_index;
			erase_at(idx);
			// erase_at 이후 idx 슬롯이 비었거나 이동된 원소를 담을 수 있음.
			// 양쪽 케이스 모두 iterator(this, idx)에서 skip_empty()로 정리.
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
		void swap(fixed_hash_map& other)
		{
			if (this == &other) return;
			fixed_hash_map tmp(std::move(*this));
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
	// 비교 — 양쪽이 같은 키-값 쌍을 가지면 동등
	// ----------------------------------------------------------------------------
	template <typename K, typename V, std::size_t N1, std::size_t N2,
		typename H1, typename H2, typename E1, typename E2>
	bool operator==(const fixed_hash_map<K, V, N1, H1, E1>& a,
		const fixed_hash_map<K, V, N2, H2, E2>& b)
	{
		if (a.size() != b.size()) return false;
		for (const auto& kv : a)
		{
			auto it = b.find(kv.first);
			if (it == b.end() || !(it->second == kv.second)) return false;
		}
		return true;
	}

	template <typename K, typename V, std::size_t N1, std::size_t N2,
		typename H1, typename H2, typename E1, typename E2>
	bool operator!=(const fixed_hash_map<K, V, N1, H1, E1>& a,
		const fixed_hash_map<K, V, N2, H2, E2>& b)
	{
		return !(a == b);
	}

	template <typename K, typename V, std::size_t N, typename H, typename E>
	void swap(fixed_hash_map<K, V, N, H, E>& a, fixed_hash_map<K, V, N, H, E>& b)
	{
		a.swap(b);
	}

} // namespace mtl
