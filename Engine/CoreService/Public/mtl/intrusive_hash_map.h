#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

#include "intrusive_hash_node.h"


// ----------------------------------------------------------------------------
// mtl::intrusive_hash_map<T, BucketCount, Tag, Key, Hash, KeyEqual>
//
// 노드(m_hash_next + m_key + 사용자 value 필드)가 원소 자체에 임베드된 해시
// 맵. 충돌 해결은 single-link chaining. 컨테이너는 버킷 배열만 들고, 노드
// 메모리는 호출자가 소유한다.
//
// intrusive_hash_set과 자료구조/알고리즘이 동일하다. 차이는 의미:
//   - set: T는 그 자체로 "키"의 의미. 키 외 다른 필드를 굳이 갖지 않음.
//   - map: T는 키와 함께 value 필드를 가진다. find()로 얻은 iterator를 통해
//     value 필드를 직접 수정한다. operator[]나 std::pair는 없음 — T가 곧
//     entry 본체이기 때문.
//
// 사용 용도:
//   - 이미 객체 풀에서 살아있는 entry를 키로 빠르게 찾고 싶을 때.
//   - 한 entry가 동시에 여러 자료구조에 등록되는 경우 (LRU 리스트 + 룩업 맵
//     동시 멤버십 등).
//   - 동적 할당 없이 insert/erase 반복이 핵심인 경우.
//
// 안 쓰는 게 좋은 경우:
//   - 컨테이너가 entry의 수명을 관리해주길 원할 때 → fixed_hash_map.
//   - 데이터 locality가 중요한 hot path → fixed_hash_map.
//
// 다중 멤버십:
//   Tag로 link/m_key 쌍을 구분한다. 단일 멤버십이면 Tag=void(기본)와
//   Key=T::key_type 자동 추론으로 신경 쓸 필요 없음. 다중 멤버십이면 T가
//   여러 intrusive_hash_node_key<...> 베이스를 상속하게 되어 T::key_type이
//   모호해지므로 Key를 명시적으로 지정해야 한다.
//
// 사용 예 (단일 멤버십):
//   struct entry : mtl::intrusive_hash_node_key<int> {
//       float weight;
//       entry(int k, float w) : intrusive_hash_node_key(k), weight(w) {}
//   };
//
//   mtl::intrusive_hash_map<entry, 64> map;
//   entry e1{1, 1.0f}, e2{2, 2.0f};
//   map.insert(e1);
//   map.insert(e2);
//   if (auto it = map.find(1); it != map.end()) it->weight = 3.14f;
//   for (auto& e : map) { /* e.m_key, e.weight */ }
//
// 사용 예 (다중 멤버십):
//   struct dirty_tag {};
//   struct name_tag  {};
//   struct entity : mtl::intrusive_hash_node_key<int,         dirty_tag>,
//                   mtl::intrusive_hash_node_key<std::string, name_tag>
//   {
//       float weight;
//       entity(int id, std::string name, float w)
//         : intrusive_hash_node_key<int, dirty_tag>(id),
//           intrusive_hash_node_key<std::string, name_tag>(std::move(name)),
//           weight(w) {}
//   };
//   mtl::intrusive_hash_map<entity, 64, dirty_tag, int>         by_id;
//   mtl::intrusive_hash_map<entity, 64, name_tag,  std::string> by_name;
//
// 주의:
//   - T는 mtl::intrusive_hash_node_key<Key, Tag>를 **public 상속**해야 한다.
//   - 동일 노드를 이미 어떤 컨테이너의 멤버인 상태에서 다시 insert하면 UB.
//     디버그 빌드에서는 is_linked 체크로 즉시 assert.
//   - 노드의 m_key를 컨테이너 멤버인 상태에서 수정하면 UB (해시 무결성 파괴).
//     value 필드(예: weight)는 자유롭게 수정 가능.
// ----------------------------------------------------------------------------

namespace mtl
{
	template <
		typename T,
		std::size_t BucketCount,
		typename Tag      = void,
		typename Key      = typename T::key_type,
		typename Hash     = std::hash<Key>,
		typename KeyEqual = std::equal_to<Key>
	>
	class intrusive_hash_map
	{
		static_assert(detail::is_intrusive_hash_compatible_v<T, Tag>,
			"intrusive_hash_map<T, ..., Tag>: T는 mtl::intrusive_hash_node_key<Key, Tag>를 public 상속해야 합니다");
		static_assert(std::is_base_of_v<intrusive_hash_node_key<Key, Tag>, T>,
			"intrusive_hash_map: T must inherit intrusive_hash_node_key<Key, Tag>");
		static_assert(BucketCount > 0, "intrusive_hash_map: BucketCount > 0");

		using node_type     = intrusive_hash_node<Tag>;
		using key_node_type = intrusive_hash_node_key<Key, Tag>;

	public:
		using value_type      = T;
		using key_type        = Key;
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using hasher          = Hash;
		using key_equal       = KeyEqual;
		using reference       = T&;
		using const_reference = const T&;
		using pointer         = T*;
		using const_pointer   = const T*;

		static constexpr size_type kBucketCount = BucketCount;

		// --------------------------------------------------------------------
		// Iterator (forward)
		// --------------------------------------------------------------------
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
			using map_type = std::conditional_t<IsConst, const intrusive_hash_map, intrusive_hash_map>;
			map_type*  m_map    = nullptr;
			size_type  m_bucket = 0;
			node_type* m_node   = nullptr;   // nullptr ⇔ end()

			friend class intrusive_hash_map;
			template <bool> friend class iterator_t;

		public:
			iterator_t() noexcept = default;
			iterator_t(map_type* m, size_type bi, node_type* n) noexcept
				: m_map(m), m_bucket(bi), m_node(n) {}

			// non-const → const 변환만 허용
			template <bool C = IsConst, std::enable_if_t<C, int> = 0>
			iterator_t(const iterator_t<false>& other) noexcept
				: m_map(other.m_map), m_bucket(other.m_bucket), m_node(other.m_node) {}

			reference operator*()  const noexcept { return *static_cast<T*>(m_node); }
			pointer   operator->() const noexcept { return  static_cast<T*>(m_node); }

			iterator_t& operator++() noexcept
			{
				node_type* next = m_node->m_hash_next;
				if (next != &detail::hash_chain_end<Tag>) {
					m_node = next;
				} else {
					++m_bucket;
					while (m_bucket < BucketCount && m_map->m_buckets[m_bucket] == nullptr)
						++m_bucket;
					m_node = (m_bucket < BucketCount) ? m_map->m_buckets[m_bucket] : nullptr;
				}
				return *this;
			}

			iterator_t operator++(int) noexcept { iterator_t t(*this); ++(*this); return t; }

			friend bool operator==(const iterator_t& a, const iterator_t& b) noexcept
			{
				assert(a.m_map == b.m_map && "intrusive_hash_map: 서로 다른 맵의 iterator 비교");
				return a.m_node == b.m_node;
			}
			friend bool operator!=(const iterator_t& a, const iterator_t& b) noexcept { return !(a == b); }
		};

		using iterator       = iterator_t<false>;
		using const_iterator = iterator_t<true>;

		// --------------------------------------------------------------------
		// 생성/소멸 (복사 없음 / 이동 = swap)
		// --------------------------------------------------------------------
		intrusive_hash_map() noexcept = default;

		explicit intrusive_hash_map(const Hash& h, const KeyEqual& eq = KeyEqual{}) noexcept
			: m_hash(h), m_key_equal(eq) {}

		intrusive_hash_map(const intrusive_hash_map&)            = delete;
		intrusive_hash_map& operator=(const intrusive_hash_map&) = delete;

		intrusive_hash_map(intrusive_hash_map&& other) noexcept
		{
			swap(other);
		}

		intrusive_hash_map& operator=(intrusive_hash_map&& other) noexcept
		{
			if (this != &other) {
				clear();
				swap(other);
			}
			return *this;
		}

		~intrusive_hash_map() noexcept { clear(); }

		// --------------------------------------------------------------------
		// iterators
		// --------------------------------------------------------------------
		iterator       begin()        noexcept { return make_begin<false>(); }
		const_iterator begin()  const noexcept { return make_begin<true>(); }
		const_iterator cbegin() const noexcept { return begin(); }

		iterator       end()          noexcept { return iterator(this, BucketCount, nullptr); }
		const_iterator end()    const noexcept { return const_iterator(this, BucketCount, nullptr); }
		const_iterator cend()   const noexcept { return end(); }

		// --------------------------------------------------------------------
		// capacity
		// --------------------------------------------------------------------
		size_type size()  const noexcept { return m_size; }
		bool      empty() const noexcept { return m_size == 0; }

		static constexpr size_type bucket_count() noexcept { return BucketCount; }

		// --------------------------------------------------------------------
		// lookup
		// --------------------------------------------------------------------
		iterator find(const Key& k) noexcept
		{
			const size_type bi = bucket_index(k);
			for (node_type* cur = m_buckets[bi];
			     cur != nullptr && cur != &detail::hash_chain_end<Tag>;
			     cur = cur->m_hash_next)
			{
				if (m_key_equal(static_cast<key_node_type*>(cur)->m_key, k))
					return iterator(this, bi, cur);
			}
			return end();
		}

		const_iterator find(const Key& k) const noexcept
		{
			const size_type bi = bucket_index(k);
			for (node_type* cur = m_buckets[bi];
			     cur != nullptr && cur != &detail::hash_chain_end<Tag>;
			     cur = cur->m_hash_next)
			{
				if (m_key_equal(static_cast<const key_node_type*>(cur)->m_key, k))
					return const_iterator(this, bi, cur);
			}
			return end();
		}

		bool contains(const Key& k) const noexcept
		{
			return find(k) != end();
		}

		// --------------------------------------------------------------------
		// 노드 ↔ 이터레이터 변환 / 멤버십 질의
		// --------------------------------------------------------------------
		static bool is_linked(const_reference v) noexcept
		{
			return static_cast<const node_type*>(&v)->m_hash_next != nullptr;
		}

		iterator iterator_to(reference v) noexcept
		{
			assert(is_linked(v) && "intrusive_hash_map::iterator_to: 노드가 어떤 맵에도 속해있지 않음");
			const Key& k = static_cast<const key_node_type*>(&v)->m_key;
			return iterator(this, bucket_index(k), static_cast<node_type*>(&v));
		}

		const_iterator iterator_to(const_reference v) const noexcept
		{
			assert(is_linked(v) && "intrusive_hash_map::iterator_to: 노드가 어떤 맵에도 속해있지 않음");
			const Key& k = static_cast<const key_node_type*>(&v)->m_key;
			return const_iterator(this, bucket_index(k),
				const_cast<node_type*>(static_cast<const node_type*>(&v)));
		}

		// --------------------------------------------------------------------
		// modifiers
		// --------------------------------------------------------------------

		// 같은 키의 노드가 이미 있으면 {기존, false}. 없으면 chain 앞에 prepend.
		std::pair<iterator, bool> insert(reference v) noexcept
		{
			assert(!is_linked(v) && "intrusive_hash_map::insert: 노드가 이미 다른 맵의 멤버");

			const Key&      k  = static_cast<const key_node_type*>(&v)->m_key;
			const size_type bi = bucket_index(k);

			for (node_type* cur = m_buckets[bi];
			     cur != nullptr && cur != &detail::hash_chain_end<Tag>;
			     cur = cur->m_hash_next)
			{
				if (m_key_equal(static_cast<key_node_type*>(cur)->m_key, k))
					return { iterator(this, bi, cur), false };
			}

			node_type* node = static_cast<node_type*>(&v);
			node->m_hash_next = (m_buckets[bi] != nullptr) ? m_buckets[bi] : &detail::hash_chain_end<Tag>;
			m_buckets[bi] = node;
			++m_size;
			return { iterator(this, bi, node), true };
		}

		size_type erase(const Key& k) noexcept
		{
			const size_type bi = bucket_index(k);
			node_type* prev = nullptr;
			node_type* cur  = m_buckets[bi];
			while (cur != nullptr && cur != &detail::hash_chain_end<Tag>)
			{
				if (m_key_equal(static_cast<key_node_type*>(cur)->m_key, k))
				{
					unlink_node(bi, prev, cur);
					return 1;
				}
				prev = cur;
				cur  = cur->m_hash_next;
			}
			return 0;
		}

		iterator erase(const_iterator pos) noexcept
		{
			assert(pos.m_node != nullptr && "intrusive_hash_map::erase: end() 삭제 불가");

			const size_type bi  = pos.m_bucket;
			node_type* target = pos.m_node;

			node_type* prev = nullptr;
			node_type* cur  = m_buckets[bi];
			while (cur != target) {
				prev = cur;
				cur  = cur->m_hash_next;
			}
			node_type* next = cur->m_hash_next;
			unlink_node(bi, prev, cur);

			if (next != &detail::hash_chain_end<Tag>)
				return iterator(this, bi, next);

			size_type nbi = bi + 1;
			while (nbi < BucketCount && m_buckets[nbi] == nullptr) ++nbi;
			return iterator(this, nbi, (nbi < BucketCount) ? m_buckets[nbi] : nullptr);
		}

		void unlink(reference v) noexcept
		{
			assert(is_linked(v) && "intrusive_hash_map::unlink: 노드가 어떤 맵에도 속해있지 않음");

			const Key&      k  = static_cast<const key_node_type*>(&v)->m_key;
			const size_type bi = bucket_index(k);
			node_type* target = static_cast<node_type*>(&v);

			node_type* prev = nullptr;
			node_type* cur  = m_buckets[bi];
			while (cur != target) {
				prev = cur;
				cur  = cur->m_hash_next;
			}
			unlink_node(bi, prev, cur);
		}

		void clear() noexcept
		{
			for (size_type bi = 0; bi < BucketCount; ++bi)
			{
				node_type* cur = m_buckets[bi];
				while (cur != nullptr && cur != &detail::hash_chain_end<Tag>)
				{
					node_type* next = cur->m_hash_next;
					cur->m_hash_next = nullptr;
					cur = next;
				}
				m_buckets[bi] = nullptr;
			}
			m_size = 0;
		}

		void swap(intrusive_hash_map& other) noexcept
		{
			if (this == &other) return;
			for (size_type i = 0; i < BucketCount; ++i)
				std::swap(m_buckets[i], other.m_buckets[i]);
			std::swap(m_size,      other.m_size);
			std::swap(m_hash,      other.m_hash);
			std::swap(m_key_equal, other.m_key_equal);
		}

	private:
		// --------------------------------------------------------------------
		// internal helpers
		// --------------------------------------------------------------------
		size_type bucket_index(const Key& k) const noexcept
		{
			return m_hash(k) % BucketCount;
		}

		template <bool IsConst>
		iterator_t<IsConst> make_begin() const noexcept
		{
			using iter_t = iterator_t<IsConst>;
			using map_t  = std::conditional_t<IsConst, const intrusive_hash_map, intrusive_hash_map>;
			if (m_size == 0)
				return iter_t(const_cast<map_t*>(this), BucketCount, nullptr);
			size_type bi = 0;
			while (bi < BucketCount && m_buckets[bi] == nullptr) ++bi;
			return iter_t(const_cast<map_t*>(this), bi, (bi < BucketCount) ? m_buckets[bi] : nullptr);
		}

		void unlink_node(size_type bi, node_type* prev, node_type* cur) noexcept
		{
			node_type* next = cur->m_hash_next;
			if (prev == nullptr) {
				m_buckets[bi] = (next == &detail::hash_chain_end<Tag>) ? nullptr : next;
			} else {
				prev->m_hash_next = next;
			}
			cur->m_hash_next = nullptr;
			--m_size;
		}

		// --------------------------------------------------------------------
		// data
		// --------------------------------------------------------------------
		node_type* m_buckets[BucketCount]{};
		size_type  m_size = 0;
		Hash       m_hash{};
		KeyEqual   m_key_equal{};
	};

	template <typename T, std::size_t BC, typename Tag, typename Key, typename H, typename E>
	void swap(intrusive_hash_map<T, BC, Tag, Key, H, E>& a,
	          intrusive_hash_map<T, BC, Tag, Key, H, E>& b) noexcept
	{
		a.swap(b);
	}

} // namespace mtl
