# MTL (Mark Template Library) — Fixed 컨테이너 설계 검토 결과

> 검토 일자: 2026-05-19
> 대상: `Engine/CoreService/Public/mtl/` 의 fixed_* 컨테이너 전체
> 검토 범위: 정확성 / 성능 / 사용성 / Release 안전성 / 문서화

---

## 1. 개요

`mtl` 은 게임 엔진(MarkFramework2)용으로 작성된 STL 대체 라이브러리입니다.
동적 할당을 피하고 결정론적 성능을 보장하는 **고정 용량 컨테이너 + 풀 메모리** 모델을 제공합니다.

설계 모델 비교:

| 모델 | 실패 처리 | 예시 |
|---|---|---|
| STL 예외 기반 | `bad_alloc` 던짐 | `std::list`, `std::vector` |
| 자동 확장 (SBO) | 힙으로 확장, 실패시 예외 | `absl::InlinedVector` |
| No-throw + assert | debug=assert, release=UB | EASTL fixed_* |
| **No-throw + overflow allocator** | 풀 가득 차면 sys_malloc fallback | **mtl (기본값)**, EASTL fixed_*_with_overflow |

mtl 은 EASTL 계열의 게임 엔진 친화 모델을 따르며, `EnableOverflow=true` 가 기본값이라 일반 사용에선 자동 fallback 으로 안전합니다.

---

## 2. 라이브러리 구성

### 2.1 메모리 관리
- **`allocator.h`** — `mallocator` / `dummy_allocator` / `default_allocator` (mallocator alias)
- **`allocator_malloc.h`** — `sys_malloc` / `sys_free` 인터페이스 (DLL 경계 통과)
- **`fixed_pool.h`** — `fixed_pool` / `fixed_pool_with_overflow` / `fixed_node_allocator`

### 2.2 시퀀스 컨테이너
- **`fixed_vector.h`** — in-place 버퍼 벡터 (O(1) push_back, erase_unsorted 제공)
- **`fixed_list.h`** — 풀 기반 양방향 연결 리스트
- **`fixed_slist.h`** — 풀 기반 단방향 연결 리스트
- **`fixed_ring_buffer.h`** — FIFO 원형 큐 (push / push_overwrite 두 정책)

### 2.3 연관 컨테이너
- **`fixed_hash_map.h`** — open addressing + linear probing + backward-shift deletion
- **`fixed_hash_set.h`** — 위와 동일 구조, value 부분 제거

### 2.4 문자열 / 비트
- **`fixed_string.h`** — `fixed_basic_string<CharT, N>` (std::format 통합)
- **`fixed_wstring.h`** — `fixed_basic_string<wchar_t, N>` alias
- **`fixed_bitset.h`** — 워드 단위 비트 배열 (popcount, set_indices iteration)

### 2.5 테스트 / 벤치
모든 컨테이너에 1:1 매칭되는 `Test_fixed_*.cpp` / `Bench_fixed_*.cpp` 가 `Samples/ConsoleApp/src/` 에 존재.

---

## 3. 설계 원칙

### 3.1 일관된 정책
- **Debug**: assert 로 즉시 감지
- **Release**: 컨테이너별 contract 기반 처리. `fixed_string`은 truncation, `fixed_hash_*`는 `{end(), false}`/`false`, `fixed_ring_buffer`는 `false` 반환을 제공한다. `fixed_vector`와 일부 sequence 컨테이너의 기존 STL-style API는 capacity contract 위반 시 caller bug이며, 실패 채널이 필요한 경우 `try_*` API를 사용한다.
- **kCapacity / full() / empty()**: 모든 컨테이너에서 통일된 진단 인터페이스
- **has_overflowed()**: 풀 기반 컨테이너의 튜닝 진단

### 3.2 게임 엔진 특화 결정
- `fixed_vector::erase_unsorted` — 순서 무관 컬렉션의 O(1) 삭제
- `fixed_ring_buffer::push_overwrite` — FPS 히스토리/로그 ring 에 적합
- `fixed_bitset::set_indices()` / `for_each_set` — ECS 컴포넌트 마스크 순회
- `fixed_pool::has_overflowed()` — 풀 크기 튜닝 시그널
- splice 미지원 (cross-pool 안전성), `fixed_slist::back/push_back` 미지원 (O(n) 회피) — *의도적* 미구현

### 3.3 STL 호환성
- 표준 STL 인터페이스 시그니처 유지 (begin/end, iterator_category 등)
- `std::format` formatter 특수화 제공
- `view_type` 으로 std::string_view 암묵 변환

### 3.4 예외를 사용하지 않는 이유
1. **성능** — try/catch 정상 경로는 거의 0이지만 throw 시 스택 언와인딩이 매우 무거움
2. **빌드 호환성** — 콘솔/모바일에서 `-fno-exceptions` 빌드 일반
3. **메모리 예산이 정해진 워크로드** — OOM 대응 코드 비용 > 발생 빈도
4. **디버깅 용이성** — assert 가 catch 보다 직관적

---

## 4. 검토 1차 — 첫 평가에서 발견된 항목

### 4.1 강점
- 라인업 커버리지 완전함
- 일관된 설계 어휘
- 게임 엔진 특화 결정 명확
- 현대 C++ 활용 (`std::launder`, `std::bit_ceil`, `std::countr_zero`, `std::popcount`, `std::format`)
- "사용 용도 / 안 쓰는 게 좋은 경우 / 특성 / 사용 예" 4부 구조의 문서화

### 4.2 약점 (1차)
| # | 항목 | 파일 | 심각도 |
|---|---|---|---|
| 1 | `kBufferSize` 슬랙이 `NodeSize-1` (잘못, `NodeAlignment-1` 이어야 함) | fixed_pool.h | 중 |
| 2 | `fixed_vector::resize()` 누락 | fixed_vector.h | 중 |
| 3 | `fixed_vector::emplace` 중간 삽입 self-reference UB 가능 | fixed_vector.h | 낮음 (STL parity) |
| 4 | iterator 비교가 컨테이너 identity 무시 | hash_map/set, ring_buffer | 중 |
| 5 | `fixed_pool::memory_in_use` 가 overflow 누락처럼 보임 | fixed_pool.h | 낮음 |
| 6 | `fixed_string::swap` 구현이 복잡 (동작은 정확) | fixed_string.h | 낮음 |
| 7 | `insert(pos, count, value)` self-reference | list/slist | 낮음 (STL parity) |
| 8 | `<=>` 미적용 (C++20 spaceship) | fixed_vector.h | 낮음 (스타일) |

---

## 5. 검토 2차 — 정밀 검토에서 추가 발견

### 5.1 진짜 버그
| # | 항목 | 파일 | 영향 |
|---|---|---|---|
| 9 | `find_next(npos)` / `find_next_unset(npos)` 에서 `++pos` wrap → first set 비트로 되감김 | fixed_bitset.h | end iterator 증가 시 wrap-around |
| 10 | `fixed_list/slist::emplace` 가 null 노드를 link → release 빌드에서 즉시 크래시 | list/slist | OOM/풀고갈 시 crash |

### 5.2 중간 우려
| # | 항목 | 파일 | 영향 |
|---|---|---|---|
| 11 | `first_occupied()` 가 빈 맵에서도 전체 스캔 | hash_map/set | 빈 컨테이너 begin() O(N_buckets) |
| 12 | `init()` 재호출 시 outstanding 할당 silent loss | fixed_pool.h | 메모리 누수/UB |
| 13 | `set_overflow_allocator` 가 활성 overflow 중 교체 가능 | fixed_pool.h | 누수/UB |

### 5.3 사소한 sharp edge
| # | 항목 | 파일 | 결정 |
|---|---|---|---|
| 14 | `format_append` assert 메시지가 정책과 불일치 ("용량 초과" → 실제는 silent truncation) | fixed_string.h | 메시지 수정 |
| 15 | `operator[]` 가 `i == m_size` 슬롯 쓰기 허용 | fixed_string.h | STL parity 유지 |
| 16 | `erase(iterator)` 반환 iterator semantic (backward-shift 부작용) | fixed_hash_map.h | open-addressing 본질적 한계, 유지 |
| 17 | `emplace_back/front` 의 reference 반환이 nullptr 경로에서 UB | list/slist | STL parity (다음 섹션 참고) |

---

## 6. 적용된 수정 내역

| # | 수정 | 파일 / 위치 |
|---|---|---|
| F1 | `kBufferSize` 슬랙: `NodeSize-1` → `NodeAlignment-1` | `fixed_pool.h` (`fixed_node_allocator::kBufferSize`) |
| F2 | `fixed_vector::resize(n)` / `resize(n, value)` 추가 | `fixed_vector.h` (erase_unsorted 다음) |
| F3 | `find_next(npos)` wrap 가드 — `if (pos >= N) return npos;` 선행 체크 | `fixed_bitset.h::find_next` |
| F4 | `find_next_unset(npos)` 동일 가드 | `fixed_bitset.h::find_next_unset` |
| F5 | `fixed_list::emplace` 에 `if (!n) return iterator(pos.m_node);` 가드 | `fixed_list.h::emplace` |
| F6 | `fixed_slist::emplace_after` 동일 가드 | `fixed_slist.h::emplace_after` |
| F7 | hash_map iterator `==` 에 `assert(a.m_map == b.m_map)` 추가 | `fixed_hash_map.h::iterator_t` |
| F8 | hash_set const_iterator `==` 에 `assert(a.m_set == b.m_set)` 추가 | `fixed_hash_set.h::const_iterator` |
| F9 | ring_buffer iterator `==` 에 `assert(a.m_buf == b.m_buf)` 추가 | `fixed_ring_buffer.h::iterator_t` |
| F10 | hash_map `first_occupied()` 가 `m_size == 0` 일 때 `kBucketCount` 즉시 반환 | `fixed_hash_map.h::first_occupied` |
| F11 | hash_set 동일 단락 | `fixed_hash_set.h::first_occupied` |
| F12 | `fixed_pool_base::init()` 에 `assert(m_current_size == 0)` 추가 | `fixed_pool.h::fixed_pool_base::init` |
| F13 | `pool_memory_in_use()` / `overflow_memory_in_use()` 추가 (풀/overflow 분리 진단) | `fixed_pool.h::fixed_pool_with_overflow` |
| F14 | `set_overflow_allocator` 에 `assert(m_overflow_current == 0)` 가드 추가 | `fixed_pool.h::fixed_pool_with_overflow::set_overflow_allocator` |
| F15 | `format_append` assert 메시지를 "출력이 잘림 (truncated)" 로 명확화 | `fixed_string.h::format_append` |
| F16 | `fixed_hash_map/set::contains` 가 iterator 생성 없이 `probe()` 결과를 직접 반환하도록 최적화 | `fixed_hash_map.h`, `fixed_hash_set.h` |
| F17 | `fixed_pool_with_overflow::init()` 시 overflow counter reset 및 활성 overflow 할당 assert 추가 | `fixed_pool.h::fixed_pool_with_overflow::init` |
| F18 | `fixed_pool_with_overflow::is_in_pool()` 을 포인터 관계 비교 대신 `uintptr_t` 범위 비교로 변경 | `fixed_pool.h::fixed_pool_with_overflow::is_in_pool` |
| F19 | `__MEMORY_TRACKER_ENABLED__` 시 overflow allocation 에 `std::source_location` 전달 | `fixed_pool.h` |
| F20 | 실패 채널용 `try_*` API 추가 (`fixed_vector`, `fixed_list`, `fixed_slist`) | sequence containers |

---

## 7. 남은 Sharp Edge (의도적 유지)

### 7.1 STL Parity 항목
다음 항목들은 std::* 의 동일한 sharp edge 와 일치하므로 유지:

- **fixed_vector::insert(pos, count, value) self-reference** — `v.insert(v.begin(), 3, v[5])` 같은 자기참조는 UB. std::vector 동일.
- **list/slist::insert(pos, count, value) self-reference** — 동일.
- **fixed_string::operator[] 가 i == size() 슬롯 쓰기 허용** — std::string 동일.

### 7.2 emplace_back/emplace_front 의 UB 가능성

```cpp
reference emplace_back(Args&&... args)
{
    return *emplace(end(), std::forward<Args>(args)...);
}
```

`EnableOverflow=false` + 풀 가득 + release 빌드 조합에서:
1. emplace 가 release-safe 가드로 `iterator(end().m_node)` (sentinel) 반환
2. `*` 연산이 sentinel 노드를 `static_cast<node*>` 후 storage 영역 deref → **UB**

reference 를 반환하는 함수는 "실패" 를 표현할 채널이 없어서 깔끔한 수정이 불가:
- **옵션 A** (정적 dummy 반환): 호출자가 알 길 없어 더 위험
- **옵션 B** (현재): STL 과 동일한 contract — caller 책임
- **옵션 C** (API 분리): `bool try_emplace_back(...) noexcept` 같은 별도 메서드

현재는 **옵션 B + 옵션 C 일부 적용**. 기존 STL-style reference 반환 API는 contract 유지, 실패 채널이 필요한 코드는 새 `try_*` API를 사용한다. 호출자가 다음 방법으로 안전 보장:
1. `if (!list.full()) list.emplace_back(...)` 사전 체크
2. `auto it = list.insert(list.end(), val);` 후 `it == end()` 비교
3. `EnableOverflow=true` (기본값) 사용 — sys_malloc fallback 으로 거의 항상 성공
4. `try_emplace_back(...)`, `try_push_back(...)`, `try_emplace_front(...)` 등 bool 반환 API 사용

### 7.4 메모리 트래커 통합 범위

`__MEMORY_TRACKER_ENABLED__` 활성 시 `fixed_pool_with_overflow` 의 overflow fallback 은 `std::source_location` 을 `sys_malloc` 에 전달한다.

주의:
- 고정 풀 내부 버퍼에서 나가는 노드는 시스템 malloc 이 아니므로 개별 메모리 트래킹 대상이 아니다.
- 트래킹 대상은 풀 용량을 초과해 `OverflowAllocator` / `sys_malloc` 으로 fallback 되는 할당이다.
- `fixed_node_allocator` 는 `EnableOverflow=true` 일 때 overflow 경로에 한해 location 이 전달된다.

### 7.3 open-addressing 의 본질적 한계
- `fixed_hash_map::erase(iterator)` 반환값이 backward-shift 결과로 displaced 원소를 가리킬 수 있음 — Boost/abseil 도 동일. 동작은 안전 (skip_empty 로 valid iterator 보장).

---

## 8. UB (Undefined Behavior) 보충 설명

C++ 표준이 "이 상황에서 무슨 일이 일어나야 하는지 정의하지 않는다" 고 명시한 케이스.

**위험 양상**:
- 정상 동작할 수도, 즉시 크래시할 수도, 쓰레기 값 반환할 수도 있음
- 컴파일러는 "UB 는 발생하지 않는다" 고 가정하고 **코드를 통째로 변형/제거** 가능
- **debug 에선 멀쩡한데 release 에서만 다른 동작** — 최적화가 UB 를 전제로 재배치하기 때문
- 디버깅이 매우 어려움

**대표 예시**: null pointer dereference, use-after-free, out-of-bounds 접근, signed integer overflow, uninitialized read

**mtl 에서 발생 가능한 UB**:
- (수정 완료) `emplace` 의 nullptr link
- (STL parity) self-reference insert
- (contract) emplace_back/front 의 sentinel deref
- (contract) `operator[]` 의 i == size() 쓰기

---

## 9. STL 의 실패 처리 비교

### std::list::emplace_back
```cpp
std::list<int> list;
try {
    int& ref = list.emplace_back(99);   // bad_alloc 가능
    // 도달했다면 ref 는 항상 valid
}
catch (const std::bad_alloc&) {
    // 삽입 실패
}
```

STL 은 **예외라는 별도 채널** 로 실패를 표현. reference 반환 함수의 한계를 우회.

### Exception Safety Guarantee
| 등급 | 의미 |
|---|---|
| nothrow | 절대 안 던짐 (swap, clear) |
| strong | 던지면 호출 전 상태로 롤백 |
| basic | 던져도 invariant 유지 |
| none | 보장 없음 |

`std::list::emplace_back` 은 **strong**: 할당 실패해도 리스트 변화 없음.

### std::array
유일하게 진짜 고정 크기 STL 컨테이너 — push_back 자체가 없음. "가변 capacity + bounded" 모델은 STL 에 없음.

### 다른 라이브러리
- **Boost.Container `static_vector`**: 가득 차면 `std::bad_alloc`
- **EASTL `fixed_*`**: assert + release UB (mtl 과 거의 동일)
- **Abseil `InlinedVector`**: SBO 모델, 자동 힙 확장
- **Folly `small_vector`**: SBO 모델

mtl 은 EASTL 의 게임 엔진 친화 모델 + overflow allocator 의 안전성 hybrid.

---

## 10. 최종 평가

| 측면 | 평가 | 비고 |
|---|---|---|
| 기능 완성도 | A | 게임 엔진용 범위 100% 커버 |
| 정확성 | A- | 식별된 critical 이슈는 수정됨. 기존 STL-style API 일부는 contract 기반 sharp edge 유지 |
| 성능 | A | 워드 단위 비트 연산, backward-shift, free-list, in-place 버퍼 |
| 사용성 | A | STL 호환 + 게임 특화 + 명확한 진단 |
| 문서화 | A+ | 각 헤더의 4부 구조가 모범적 |
| Release 안전성 | B+ | 컨테이너별 정책 차이 존재. 실패 채널은 `try_*` API 또는 bool 반환 API 사용 권장 |
| 테스트 커버리지 | B | SampleConsole 미니 테스트/벤치 존재. 공식 unit test/CI는 아직 없음 |

**결론**: **게임엔진 내부 v1 사용에 충분한 수준의 라이브러리**. 알려진 critical 이슈는 수정됨. 다만 일부 기존 API는 STL parity/contract 기반이므로 release-safe 실패 처리가 필요한 경로에서는 `try_*` API 또는 bool 반환 API를 우선 사용한다.

---

## 11. 향후 작업 권장

### 단기 (선택사항)
- [x] 실패 채널용 `try_*` API 추가 (`fixed_vector`, `fixed_list`, `fixed_slist`)
- [ ] 기존 `emplace_back/front` contract 를 헤더 상단 문서에 더 명확히 명시
- [ ] Test/Bench 회귀 검증 (수정 후 빌드 + 실행)
- [ ] 정적 분석 (/W4 또는 clang-tidy) 한 번 돌려보기

### 중기
- [ ] `flat_map` / `flat_set` (정렬된 컨테이너) — 헤더 내 "구현 예정" 으로 명시되어 있음
- [ ] SPSC lock-free ring buffer (멀티스레드 컨텍스트) — ring_buffer 헤더에 "v1 미포함" 명시
- [ ] C++20 `<=>` (spaceship) 적용으로 비교 연산자 단축

### 장기
- [x] `try_emplace_back / try_emplace_front` 명시적 실패 API 일부 추가 (UB 회피)
- [ ] 메모리 트래커 통합 검증 (`__MEMORY_TRACKER_ENABLED__` 경로)
- [ ] DLL 경계 stress 테스트 (`get_default_allocator` 인스턴스 공유)

---

## 부록 A — 적용된 수정 코드 위치 (빠른 참조)

```
fixed_pool.h
  - kBufferSize 슬랙: NodeSize-1 → NodeAlignment-1
  - fixed_pool_base::init() 에 m_current_size == 0 assert
  - fixed_pool_with_overflow::pool_memory_in_use() / overflow_memory_in_use() 추가
  - fixed_pool_with_overflow::set_overflow_allocator() 에 m_overflow_current == 0 assert
  - fixed_pool_with_overflow::init() 에 overflow counter reset / 활성 overflow assert 추가
  - fixed_pool_with_overflow::is_in_pool() 을 uintptr_t 범위 비교로 변경
  - __MEMORY_TRACKER_ENABLED__ 시 overflow allocation 에 source_location 전달

fixed_vector.h
  - resize(size_type) / resize(size_type, const T&) 추가 (erase_unsorted 다음)
  - try_push_back / try_emplace_back 추가

fixed_list.h
  - emplace() 에 if (!n) return iterator(pos.m_node); 가드
  - try_push_front/back, try_emplace_front/back, try_emplace(pos) 추가

fixed_slist.h
  - emplace_after() 에 if (!n) return iterator(pos.m_node); 가드
  - try_push_front, try_emplace_front, try_emplace_after 추가

fixed_hash_map.h
  - iterator_t::operator== 에 m_map identity assert
  - first_occupied() 에 m_size == 0 단락
  - contains() 가 probe() 결과를 직접 사용

fixed_hash_set.h
  - const_iterator::operator== 에 m_set identity assert
  - first_occupied() 에 m_size == 0 단락
  - contains() 가 probe() 결과를 직접 사용

fixed_ring_buffer.h
  - iterator_t::operator== 에 m_buf identity assert

fixed_bitset.h
  - find_next(): pos >= N 선행 체크로 npos wrap 차단
  - find_next_unset(): 동일

fixed_string.h
  - format_append() assert 메시지 명확화
```

---

## 부록 B — 핵심 설계 결정 요약

1. **EnableOverflow=true 기본값** — 풀 가득 차면 sys_malloc fallback. 안전성 + 진단 (`has_overflowed`) 양립.
2. **풀 복사/이동 금지** — 자유 리스트가 버퍼 주소에 묶여 있어 더블 프리/메모리 손상 위험. 컨테이너 swap 은 원소 단위 이동.
3. **kBucketCount = bit_ceil(N*2)** — 부하율 < 0.5 강제 → linear probing 평균 ~2회.
4. **Backward-shift deletion** — tombstone 없음 → 시간 경과에 따른 성능 저하 없음.
5. **splice 미지원** — cross-pool 노드 이동 안전성 불가능, 명시적 제외.
6. **alignas(T) 버퍼 + placement new** — 동적 할당 없는 객체 수명 관리.
7. **std::launder** — strict aliasing 안전 보장 (open-addressing 슬롯 재사용에 필수).

---

*이 문서는 mtl/fixed_* 컨테이너의 v1 잠금 시점 설계 검토 결과입니다. 이후 변경은 별도 changelog 로 관리 권장.*
