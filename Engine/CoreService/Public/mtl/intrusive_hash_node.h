#pragma once

#include <type_traits>
#include <utility>


// ----------------------------------------------------------------------------
// mtl::intrusive_hash_node<Tag> / intrusive_hash_node_key<Key, Tag>
//
// intrusive_hash_set / intrusive_hash_map가 공유하는 노드 타입.
//
// 구조:
//   - intrusive_hash_node<Tag>: chain 연결을 위한 single-link 노드 (m_hash_next).
//   - intrusive_hash_node_key<Key, Tag>: 노드 + 키. 사용자 T는 이걸 public
//     상속하고 자신만의 value 필드를 추가한다. set이면 키만 있고, map이면
//     key + value.
//
// 다중 멤버십 (한 객체가 여러 해시 컨테이너에 동시 등록):
//   Tag로 m_hash_next/m_key 쌍을 구분한다. 사용자 T가 서로 다른 Tag의
//   intrusive_hash_node_key<Key, Tag>를 여러 개 상속하면 각 컨테이너가
//   독립된 link/키를 사용. 단일 멤버십은 Tag=void 기본값으로 그대로.
//
//     struct dirty_tag {};
//     struct name_tag  {};
//     struct entity : mtl::intrusive_hash_node_key<int,         dirty_tag>,
//                     mtl::intrusive_hash_node_key<std::string, name_tag>
//     { ... };
//
//     mtl::intrusive_hash_map<entity, 64, dirty_tag, int>         dirty;
//     mtl::intrusive_hash_map<entity, 64, name_tag,  std::string> by_name;
//
// 링크 invariant (is_linked() O(1) 보장):
//   - 어떤 컨테이너에도 속하지 않음 → m_hash_next == nullptr.
//   - 컨테이너에 속함 → m_hash_next 는 항상 non-null (다른 노드 OR Tag별
//     sentinel detail::hash_chain_end<Tag>).
//   - chain의 마지막 노드는 m_hash_next == &detail::hash_chain_end<Tag>.
//   - 전역 sentinel은 주소만 사용 — 절대 dereference되지 않음.
//
// 복사/이동 의미: 노드는 주소가 곧 정체성이다. 복사 시 새 객체는 어떤 컨테이너
// 에도 속하지 않은 상태로 시작하고, 대입은 자신의 link 상태에 영향을 주지
// 않는다(no-op). T가 기본 복사/이동을 자유롭게 가지도록 하기 위한 의미.
//
// 사용 예 (단일 멤버십, set):
//   struct active_id : mtl::intrusive_hash_node_key<int> {
//       using intrusive_hash_node_key::intrusive_hash_node_key;
//   };
//   mtl::intrusive_hash_set<active_id, 64> set;
//   active_id n{42};
//   set.insert(n);
//
// 사용 예 (단일 멤버십, map):
//   struct entry : mtl::intrusive_hash_node_key<int> {
//       float weight;
//       entry(int k, float w) : intrusive_hash_node_key(k), weight(w) {}
//   };
//   mtl::intrusive_hash_map<entry, 64> map;
//   entry e{42, 3.14f};
//   map.insert(e);
//   if (auto it = map.find(42); it != map.end()) it->weight = 2.71f;
// ----------------------------------------------------------------------------

namespace mtl
{
	template <typename Tag = void>
	struct intrusive_hash_node
	{
		intrusive_hash_node* m_hash_next;

		intrusive_hash_node() noexcept
			: m_hash_next(nullptr) {}

		intrusive_hash_node(const intrusive_hash_node&) noexcept
			: m_hash_next(nullptr) {}

		intrusive_hash_node(intrusive_hash_node&&) noexcept
			: m_hash_next(nullptr) {}

		intrusive_hash_node& operator=(const intrusive_hash_node&) noexcept { return *this; }
		intrusive_hash_node& operator=(intrusive_hash_node&&)      noexcept { return *this; }
	};

	template <typename Key, typename Tag = void>
	struct intrusive_hash_node_key : intrusive_hash_node<Tag>
	{
		using key_type = Key;
		Key m_key;

		intrusive_hash_node_key() = default;

		explicit intrusive_hash_node_key(const Key& k) : m_key(k) {}
		explicit intrusive_hash_node_key(Key&& k)      : m_key(std::move(k)) {}
	};

	namespace detail
	{
		// chain-end sentinel. 주소만 사용 — 절대 dereference되지 않는다.
		// C++17 inline variable template으로 Tag별 단일 인스턴스 보장.
		template <typename Tag>
		inline intrusive_hash_node<Tag> hash_chain_end{};

		template <typename T, typename Tag>
		inline constexpr bool is_intrusive_hash_compatible_v =
			std::is_base_of_v<intrusive_hash_node<Tag>, T>;
	}

} // namespace mtl
