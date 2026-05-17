# MTL — Mark Template Library 설계 문서

> Mark Framework 2 엔진을 위한 게임 친화적 템플릿 라이브러리

---

## 1. 개요

**MTL(Mark Template Library)** 은 Mark Framework 2 엔진 내부에서 사용되는 STL 스타일의 C++ 템플릿 라이브러리이다. 표준 STL이 게임 엔진/실시간 시스템에서 가지는 한계 — 예측 불가능한 할당, 캐시 비친화적 레이아웃, 디버그 빌드에서의 성능 저하 — 를 우회하기 위해 자체적으로 설계한다.

### 1.1 위치
```
Engine/CoreService/Public/mtl/
```
`CoreService`의 공개 헤더로 제공되며, 엔진의 모든 상위 모듈이 이 라이브러리에 의존할 수 있다.

### 1.2 네임스페이스
모든 심볼은 `mtl::` 네임스페이스에 위치한다.

---

## 2. 설계 원칙

### 2.1 핵심 가치
1. **결정적 메모리 (Deterministic memory)** — 런타임 할당을 최소화하고, 컴파일 타임에 크기를 고정할 수 있는 컨테이너를 우선 제공한다.
2. **명시적 할당자 (Explicit allocator)** — 어떤 할당자에서 메모리가 나오는지 코드에 드러나야 한다. STL의 PMR보다 가볍고 직접적인 모델을 사용한다.
3. **STL 친화적 인터페이스** — 학습 비용을 줄이기 위해 STL과 유사한 식별자(`begin/end`, `push_back`, `emplace_back`, ...)와 시그니처를 따른다.
4. **헤더 우선 (Header-first)** — 템플릿 코드는 인라인 가능하도록 헤더에 둔다. 비템플릿 핵심부만 `.cpp`로 분리한다.
5. **DLL 경계 지원** — `MTL_API` 매크로로 export/import를 제어해, 엔진을 정적/동적 어느 방식으로도 빌드할 수 있다.

### 2.2 비목표 (Non-Goals)
- 표준 STL의 완전한 대체 — 일반 목적 컨테이너의 모든 기능을 흉내내지 않는다.
- 예외 안전성에 대한 강한 보장 — 엔진은 예외를 기본적으로 비활성화한다. 오류는 `assert`와 사전 조건으로 처리한다.
- 스레드 안전 컨테이너 — 컨테이너 자체는 단일 스레드 사용을 가정한다. 동기화는 호출자 책임.

---

## 3. 코딩 컨벤션

| 대상 | 규칙 | 예시 |
|---|---|---|
| 타입(클래스/구조체) | `snake_case` | `fixed_vector`, `allocator` |
| 함수/메서드 | `snake_case` | `push_back`, `get_name` |
| 멤버 변수 | `m_` 접두사 | `m_name`, `m_end`, `m_head` |
| 상수 | `kCamelCase` 또는 `UPPER_SNAKE_CASE` | `kCapacity`, `MTL_FIXED_MAX_OBJECT_BYTES` |
| 템플릿 매개변수 | `PascalCase` | `NodeSize`, `OverflowAllocator` |
| 매크로 | `MTL_*` | `MTL_API` |

---

## 4. 아키텍처

### 4.1 모듈 구성

```
mtl/
├── internal.h             # MTL_API, 공통 매크로
├── allocator.h/.cpp       # 기본 allocator 인터페이스
├── allocator_malloc.h/.cpp # 시스템 malloc 래퍼
├── fixed_pool.h           # 고정 크기 메모리 풀
├── fixed_vector.h         # 고정 용량 벡터
├── fixed_list.h           # 고정 용량 이중 연결 리스트 (풀 기반)
├── fixed_hash_map.h       # 고정 용량 해시 맵 (open addressing + linear probing)
├── fixed_hash_set.h       # 고정 용량 해시 셋 (hash_map과 동일 구조)
├── fixed_bitset.h         # 컴파일 타임 크기 비트 배열 (popcount/bit-scan)
├── fixed_ring_buffer.h    # 고정 용량 원형 FIFO 큐 (실패형 / 덮어쓰기형 푸시)
├── fixed_string.h         # fixed_basic_string 템플릿 + fixed_string 별칭
├── fixed_wstring.h        # fixed_wstring 별칭 (wchar_t)
└── string_buffer.h        # 포맷 기반 문자열 빌더
```

### 4.2 의존성 흐름

```
fixed_vector ──┐
fixed_pool ────┼──> allocator_malloc ──> internal
string_buffer ─┘                         (MTL_API)
allocator ──────────────────────────────> internal
```

- `internal.h`는 어떤 외부 헤더에도 의존하지 않는다.
- `allocator.h`는 인터페이스만 정의하고 구현체에 의존하지 않는다.
- 컨테이너는 필요한 할당자 헤더만 포함한다.

---

## 5. 메모리 모델

### 5.1 할당자 컨셉

MTL의 할당자는 STL `Allocator` 컨셉보다 단순한 duck-typed 컨셉이다. 다음 메서드를 갖춘 임의의 타입을 할당자로 사용할 수 있다.

```cpp
class MyAllocator {
public:
    void* allocate(std::size_t n);
    void* allocate(std::size_t n, std::size_t alignment);
    void* allocate(std::size_t n, std::size_t alignment, std::size_t offset);
    void  deallocate(void* p, std::size_t n = 0);

    const char* name() const;
    void        set_name(const char* name);
};
```

이름(`name`)은 **메모리 추적/프로파일링용 라벨**이다. 누수·과다할당 후처리 도구에서 할당자 식별에 사용. 동등성 비교나 동작 자체에는 영향을 주지 않는다.

### 5.2 제공되는 할당자

| 이름 | 위치 | 용도 |
|---|---|---|
| `mallocator` | `allocator.h` | 기본 할당자. `sys_malloc`/`sys_free`(=mi_malloc)에 위임. 사실상 무상태. |
| `dummy_allocator` | `allocator.h` | 테스트/스텁용. 항상 nullptr 반환. |
| `default_overflow_allocator` | `fixed_pool.h` | 고정 풀이 가득 찼을 때 fallback. (`mallocator`와 동일 백엔드) |

- `default_allocator` (= `mallocator`)는 컨테이너의 Allocator 디폴트 인자로 사용한다.
- `mtl::get_default_allocator()`는 DLL 경계 너머에서도 공유되는 단일 `mallocator` 인스턴스를 반환한다. 이름 설정/조회 외에는 굳이 거치지 않아도 무방하다 (`mallocator{}`로 직접 만들어도 됨).

### 5.3 메모리 풀

**`fixed_pool`** 은 동일 크기 노드를 빠르게 할당/해제하기 위한 자유 리스트 기반 풀이다.

- O(1) `allocate` / `deallocate`
- 외부 버퍼를 주입받음 (스택, 정적, 힙 모두 가능)
- `peak_size`, `current_size`로 사용량 추적
- **`fixed_pool_with_overflow<OverflowAllocator>`**: 풀이 가득 차면 보조 할당자로 fallback

상위 어댑터인 **`fixed_node_allocator<NodeSize, NodeCount, ...>`** 는 컴파일 타임에 노드 크기와 개수를 고정하고, 내부 버퍼와 풀을 묶어 제공한다. → 컨테이너의 `Allocator` 파라미터로 직접 사용 가능.

---

## 6. 현재 구현된 컴포넌트

### 6.1 `fixed_vector<T, N, Alignment>`

스택/내장 버퍼에 `N`개의 `T`를 보관하는 STL `std::vector` 호환 컨테이너.

| 특성 | 값 |
|---|---|
| 동적 할당 | 없음 |
| 용량 | 컴파일 타임 고정 |
| 정렬 | `Alignment` (기본: `alignof(T)`) |
| 크기 상한 | `GTL_FIXED_MAX_OBJECT_BYTES` (기본 64KB) |

**제공 연산**: `push_back`, `emplace_back`, `insert`, `emplace`, `erase`, `erase_unsorted`, `swap`, `clear`, `assign`, iterators, comparison

**특기사항**:
- `erase_unsorted` — 순서 보존이 필요 없을 때 O(1) 제거. 게임 코드의 핵심 패턴.
- 용량 초과 시 `assert`로 즉시 실패 (예외 없음).
- `full()` 멤버 제공.

> **명명 이슈**: 매크로 이름이 `GTL_FIXED_MAX_OBJECT_BYTES`로 남아 있다 (GTL → MTL 리네임 잔재로 추정). `MTL_FIXED_MAX_OBJECT_BYTES`로 교체 필요.

### 6.2 `fixed_pool` / `fixed_pool_with_overflow` / `fixed_node_allocator`
[5.3 메모리 풀](#53-메모리-풀) 참조.

### 6.3 `fixed_list<T, N, EnableOverflow, OverflowAllocator>`

`fixed_node_allocator`를 사용한 노드 풀 기반의 이중 연결 리스트. `std::list`와 호환 가능한 인터페이스를 제공한다.

| 특성 | 값 |
|---|---|
| 노드 메모리 | 내부 풀 (`N`개) |
| 오버플로우 | 기본 활성 — 풀 가득 차면 `OverflowAllocator`로 fallback |
| 동적 할당 | 풀 안에서는 없음. overflow 발생 시 `sys_malloc` |
| 이터레이터 | bidirectional |

**제공 연산**: `push_back/front`, `pop_back/front`, `emplace_back/front/<pos>`, `insert`, `erase`, `clear`, `assign`, `resize`, `remove`, `remove_if`, `reverse`, `swap`, iterators, comparison

**특기사항**:
- `has_overflowed()` — 풀이 한 번이라도 가득 찼는지 확인. 풀 크기 튜닝에 사용.
- `full()` — `size() >= N`. 다음 삽입이 overflow를 트리거하는지 알 수 있음.
- `swap`은 풀이 자기 버퍼에 묶여 있어 O(N) (std::list와 다름).
- `splice`는 v1에서 제외. cross-pool 노드 이동이 복잡해서 의도적으로 미구현.

### 6.4 `fixed_hash_map<Key, Value, N, Hash, KeyEqual>`

고정 용량 해시 맵. **Open addressing + linear probing** 구조로 노드 할당 없이 내부 버퍼에 직접 저장한다.

| 특성 | 값 |
|---|---|
| Addressing | Open (closed/chaining 아님) |
| Probing | Linear |
| 버킷 개수 | `next_pow2(N * 2)` — 부하율 < 0.5 |
| Erase | Backward-shift deletion (tombstone 없음) |
| 슬롯 상태 | 분리된 `uint8_t[bucket_count]` 배열 — probe 시 키 접근 없이 occupancy만 스캔 |
| 동적 할당 | 없음 |
| 이터레이터 | forward (occupied 슬롯만 순회) |

**제공 연산**:
- 생성: 기본, initializer_list, iterator 쌍, 복사, 이동
- 조회: `find`, `contains`, `count`, `at`, `operator[]`
- 삽입: `insert`, `try_emplace`, `insert_or_assign`, `emplace`
- 삭제: `erase(key)`, `erase(iterator)`, `clear`
- 용량: `size`, `empty`, `full`, `capacity`, `bucket_count`, `load_factor`
- 기타: `swap`, `hash_function`, `key_eq`, 비교 연산자

**특기사항**:
- `try_emplace`가 가장 효율적 — 임시 객체 없이 키/값을 in-place 구성. `operator[]`, `at` 모두 이를 활용.
- `bucket_count = next_pow2(N * 2)` → 평균 probe 길이가 매우 짧음 (load < 0.5 → 평균 ~2 probe). 모듈로 연산은 `& (bucket_count - 1)` 한 번.
- Backward-shift deletion → 삭제 후에도 cluster 깨끗하게 유지, 시간에 따른 성능 저하 없음.
- 충돌 공격은 게임엔진에서 가정하지 않음. 키가 사용자 입력일 경우 별도 검토.

**Overflow 정책**:
- Debug: 용량 초과 삽입 시 `assert`.
- Release: 용량 초과 삽입은 `{end(), false}` 반환 (no-op).
- `at()` 키 부재는 호출자 버그로 간주 — assert만, 자동 복구 없음.

### 6.5 `fixed_hash_set<Key, N, Hash, KeyEqual>`

고정 용량 해시 셋. `fixed_hash_map`과 **동일한 구조 / 알고리즘** — Value만 제거된 형태.

| 특성 | 값 |
|---|---|
| Addressing | Open + Linear probing |
| 버킷 개수 | `std::bit_ceil(N * 2)` |
| Erase | Backward-shift |
| 이터레이터 | forward, **const-only** (키는 수정 불가, 해시 무결성 유지) |

**전형적 use case**:
- 멤버십 테스트 — "이 ID가 활성인가", "이 태그가 있는가"
- 중복 제거 — visited 집합, 처리 완료 집합
- 작은 집합 연산 — dirty flags, active component type

**제공 연산**: `insert(key)`, `emplace(args...)`, `find`, `contains`, `count`, `erase(key|iter)`, `clear`, `size`, `empty`, `full`, `bucket_count`, `load_factor`, swap, 비교

`fixed_hash_map`과의 주요 차이:
- `value_type == key_type` (= `Key`).
- `iterator == const_iterator` — 셋 원소는 immutable.
- `operator[]`, `at`, `try_emplace`, `insert_or_assign` 없음 (map 전용 개념).

### 6.6 `fixed_bitset<N>`

컴파일 타임 크기의 비트 배열. 내부 저장은 `uint64_t[(N+63)/64]`.

| 특성 | 값 |
|---|---|
| 메모리 | `(N + 7) / 8` 바이트 (예: N=256 → 32 B) |
| `test` / `set` / `reset` / `flip` | O(1) — 단일 비트 연산 |
| `count` | O(N/64) — `std::popcount` per word |
| `find_first` / `find_next` | O(N/64) 최악 — `std::countr_zero` per word, 보통 한 워드 안에서 끝남 |
| AND / OR / XOR / NOT | O(N/64) — 워드 단위 SIMD-친화적 |

**전형적 use case**:
- ECS 컴포넌트 마스크 (어떤 컴포넌트가 활성인지)
- 플래그 시스템 (`IsVisible | CastsShadow | ...`)
- 슬롯 점유 비트맵 (object pool의 free 비트)
- 입력 키 상태 (256개 키코드)
- dirty flag 모음

**제공 연산**:
- 단일 비트: `test`, `set(i,v)`, `reset(i)`, `flip(i)`, `operator[]` (proxy)
- 전체 비트: `set()`, `reset()`, `flip()`
- 집계: `count`, `any`, `none`, `all`
- 비트 스캔: `find_first`, `find_next`, `find_first_unset`, `find_next_unset`
- 순회: `for_each_set(F)`, `for (auto i : bits.set_indices()) { }`
- 비트연산: `&`, `|`, `^`, `~`, `&=`, `|=`, `^=`
- 비교: `==`, `!=`
- 마스크 관계: `contains(mask)`, `intersects(other)`
- 저수준: `data()`

**특기사항**:
- 마지막 워드의 불사용 비트는 항상 0 유지 (invariant) → `count`, `find_first` 등이 항상 정확.
- C++20 `<bit>` 인트린식 활용 (`std::popcount`, `std::countr_zero`) → CPU 인스트럭션으로 직접 컴파일.
- `set_indices()`는 view를 반환해 range-based for로 set 비트만 빠르게 순회 (워드 단위 bit-scan).

### 6.7 `fixed_ring_buffer<T, N>`

고정 용량 원형 FIFO 큐. 단일 스레드 가정.

| 특성 | 값 |
|---|---|
| 메모리 | 내부 버퍼 `N * sizeof(T)` + 인덱스 2개 |
| `push` / `pop` | O(1) |
| 임의 접근 `operator[]` | O(1) — front 기준 offset |
| 순회 | forward iterator (front → back 순서) |
| 동적 할당 | 없음 |

**두 가지 푸시 정책**:
- `push(v)` / `emplace(args...)` — 가득 차면 **false** 반환 (실패). 잡 큐에 적합.
- `push_overwrite(v)` / `emplace_overwrite(args...)` — 가득 차면 **가장 오래된 항목을 덮어씀**. 로그/이력에 적합.

**전형적 use case**:
- 잡 큐 / 커맨드 큐 (메인 → 워커 작업 전달)
- 입력 이벤트 큐 (한 프레임에 누적된 입력 처리)
- 프레임 시간 히스토리 / FPS 그래프 (최근 N개 자동 유지)
- 로그 ring (최근 N개 메시지)
- 슬라이딩 윈도우 통계 (최근 N개 샘플)

**제공 연산**: `push`, `push_overwrite`, `emplace`, `emplace_overwrite`, `pop()`, `pop(out)`, `front`, `back`, `operator[]`, `size`, `empty`, `full`, `available`, `clear`, `swap`, 비교 연산자.

**미포함 (의도적)**:
- 멀티스레드 SPSC lock-free — 별도 타입으로 추가 예정.
- 임의 위치 insert/erase — ring 의미 위반. 필요시 `fixed_list`.

### 6.8 `fixed_basic_string<CharT, N>` (별칭: `fixed_string<N>`, `fixed_wstring<N>`)

고정 용량 문자열 — 내부 버퍼 `CharT[N + 1]`, 항상 null-terminated 유지. overflow 없음 (초과 시 `assert`).

표준의 `basic_string` / `string` / `wstring` 관계와 동일한 구조:
- `fixed_basic_string<CharT, N>` (`fixed_string.h`): 템플릿 본체
- `fixed_string<N>` = `fixed_basic_string<char, N>` (`fixed_string.h`)
- `fixed_wstring<N>` = `fixed_basic_string<wchar_t, N>` (`fixed_wstring.h`)

| 특성 | 값 |
|---|---|
| 동적 할당 | 없음 |
| 용량 | 컴파일 타임 고정 (`N` 문자) |
| Null 종료 | 항상 보장 (`c_str()` 안전) |
| `std::basic_string_view<CharT>` 변환 | 암묵 |

**제공 연산**:
- 생성: `const char*`, `(ptr, count)`, `(count, ch)`, `string_view`, `initializer_list`, iterator 쌍, 복사, 다른 `N`의 `fixed_string`
- 접근: `[]`, `at`, `front`, `back`, `c_str`, `data`
- 수정: `clear`, `push_back`, `pop_back`, `append`, `+=`, `insert`, `erase`, `resize`, `swap`
- 검색: `find`, `rfind`, `contains`, `starts_with`, `ends_with`
- 부분: `substr` — `string_view` 반환 (소유 없음, 효율적)
- 포맷: `format_append("...", args)`, `from_format(...)` — `std::format` 통합
- 비교: `==`, `!=`, `<`, `>`, `<=`, `>=` (다른 `N`, `string_view`, `const char*` 와 자유롭게 비교)
- `std::format`에 직접 인자로 전달 가능 (`std::formatter` 특수화 제공)

**특기사항**:
- `substr`은 `std::string`처럼 새 문자열을 반환하지 않고 `string_view`를 반환. 진정한 소유 사본이 필요하면 `fixed_string<M>(s.substr(...))`로 명시.
- 비교/검색/변환은 모두 `std::string_view`로 위임 → 잘 검증된 표준 구현 활용.
- `format_append`로 누적 포맷, `from_format`으로 새 인스턴스 즉시 생성.

**Overflow 정책** (데이터 변경 메서드: `assign`, `append`, `insert`, `push_back`, `resize`, `format_append`):
- **Debug** (NDEBUG 미정의): `assert` 실패 → 버그를 즉시 노출.
- **Release** (NDEBUG 정의): 자동 truncation → 안전하게 잘림, UB 없음.
- **인덱스 접근** (`at`, `operator[]`, `front`, `back`): assert만 — 호출자 측 버그로 간주, 자동 보정 없음.

### 6.9 `mallocator` / `dummy_allocator`
[5.2 제공되는 할당자](#52-제공되는-할당자) 참조.

### 6.10 `string_buffer<InitialCapacity, Alloc>`

`std::format` 기반 문자열 빌더.
- 초기 용량을 템플릿 인자로 예약
- 정적 헬퍼 `make_string` 제공
- 할당자 주입 가능

> **TODO**: 인터페이스 상세 정리 필요 (현재 일부만 검토됨).

---

## 7. 로드맵

현재 라이브러리는 **컨테이너/할당자 기초**만 갖춰져 있다. 엔진 수요에 따라 다음 컴포넌트를 단계적으로 추가한다.

### Phase 1 — 기초 컨테이너 보강
- [x] `fixed_list<T, N>` — `std::list` 대체, 풀 기반
- [x] `fixed_basic_string<CharT, N>` — `fixed_string<N>` / `fixed_wstring<N>` 별칭
- [x] `fixed_hash_map<K, V, N>` — open addressing + linear probing
- [x] `fixed_hash_set<K, N>` — `fixed_hash_map`과 동일 구조 (value 제거)
- [x] `fixed_bitset<N>` — 비트 배열, popcount/bit-scan, AND/OR/XOR 등
- [x] `fixed_ring_buffer<T, N>` — 원형 FIFO 큐 (실패형 / 덮어쓰기형 푸시)
- [ ] `spsc_ring_buffer<T, N>` — 단일 producer / 단일 consumer lock-free 변종
- [ ] `fixed_array<T, N>` — `std::array` 대체, 정렬 옵션 추가
- [ ] `intrusive_list<T>` — 노드 임베드형 양방향 리스트 (포인터만으로 멤버 임베드)
- [ ] `fixed_list::splice` — 동일 풀 내에서는 O(1) 가능 (cross-pool은 unsupported)
- [ ] `fixed_u8string<N>`, `fixed_u16string<N>`, `fixed_u32string<N>` — 필요 시 추가 별칭 (구현 변경 불필요)
- [ ] `fixed_hash_table_base` 추출 — `hash_map`/`hash_set` 코드 공유 (현재는 의도적 중복)

### Phase 2 — 해시/맵 계열
- [ ] `fixed_hash_map<K, V, N>` — 오픈 어드레싱, robin-hood 또는 SwissTable 스타일
- [ ] `flat_map<K, V>` — 정렬된 vector 기반

### Phase 3 — 할당자 확장
- [ ] `stack_allocator` — LIFO 빠른 할당
- [ ] `linear_allocator` (arena) — 프레임 단위 폐기
- [ ] `tracking_allocator` — 디버그 빌드에서 호출 스택 + 라벨 추적
- [ ] `aligned_allocator<Alignment>`

### Phase 4 — 유틸리티
- [ ] `span<T>` — C++20 `std::span` 미지원 환경 대응
- [ ] `function_ref<Sig>` — 비소유 콜백
- [ ] `optional<T>` / `expected<T, E>` — 표준 사용 가능 여부에 따라
- [ ] `hash<T>` — 결정적/플랫폼 독립 해시

### Phase 5 — 동시성 (선택적)
- [ ] `spsc_queue<T, N>` — 단일 생산자/단일 소비자 큐
- [ ] `mpsc_queue<T>`

---

## 8. 테스트 전략

> **현황**: 테스트 코드 부재. 우선순위 높음.

- 단위 테스트 프레임워크 선정 필요 (Catch2 / doctest / GoogleTest)
- 각 컨테이너에 대해:
  - 생성/소멸 시 destructor 호출 횟수 검증
  - 용량 경계 동작 (`full()`, overflow allocator)
  - move-only 타입 지원
  - 정렬 요구사항이 까다로운 타입 (SIMD 벡터 등) 통과 여부
- 할당자에 대해:
  - 할당/해제 균형
  - `peak_size` 정확성
  - 별칭/alignment 정확성

---

## 9. 빌드 / 통합

- C++ 표준: **C++20** (`std::format`, `std::conditional_t`, `if constexpr` 등 사용)
- 컴파일러: MSVC 우선 지원, Clang/GCC 호환 (`MTL_API` 분기 존재)
- DLL 빌드 시: `USE_DLL` 정의, MTL 빌드 단위에서 `MTL_EXPORT` 추가 정의

---

## 10. 미해결 이슈 / TODO

1. **할당자 컨셉 문서화**: C++20 `concept`로 명문화 검토
2. **공식 테스트 스위트** 추가
3. **벤치마크**: STL 대비 성능 수치 측정 (정량 근거 확보)
4. **문서화**: 컴포넌트별 사용 예제와 함정 (`assert` 정책, alignment 주의점 등)
5. **`fixed_node_allocator` 복사 정책**: `fixed_pool_base`의 복사가 금지되어 자연스레 비복사 타입이 됨. STL 스타일 컨테이너에서 사용하려면 외부 풀에 대한 참조/포인터를 보유하는 형태로 변경 검토.

---

## 부록 A. 참고 사례

MTL의 설계는 다음 라이브러리들로부터 영향을 받는다.

- **EASTL** (Electronic Arts) — 게임 엔진용 STL 변형. `fixed_*` 네이밍과 overflow allocator 컨셉 차용.
- **abseil** (Google) — 플랫 컨테이너 / SwissTable.
- **folly** (Meta) — small_vector, F14 해시맵.
- **PMR (C++17)** — 표준 다형 할당자. MTL은 더 단순한 모델 채택.
