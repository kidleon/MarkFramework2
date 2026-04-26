#include "pch.h"
#include "SpinLock.h"

// POSIX 계열 OS에서 sched_yield 사용을 위한 헤더
#if !defined(__TARGET_OS_WINDOWS)
#   include <sched.h>
#endif

namespace mark
{
	/** 잠금 해제 상태: stat 이 이 값이면 아무 스레드도 잠금을 보유하지 않음 */
	constexpr static long SPIN_THREAD_WAIT    = 0;

	/** 잠금 획득 상태: stat 이 이 값이면 어떤 스레드가 잠금을 보유 중 */
	constexpr static long SPIN_THREAD_WORKING = 1;

	/**
	 * OS 스케줄러에 실행권을 넘기기 전에 CPU 힌트를 반복할 최대 라운드 수.
	 * 값을 늘리면 짧은 대기에 유리하고, 줄이면 긴 대기 시 CPU 낭비를 줄인다.
	 */
	constexpr static long MAX_YIELD_ROUND = 4;

	/**
	 * 한 라운드에서 CPU 힌트 명령어를 반복하는 횟수.
	 * 너무 작으면 루프 오버헤드가 커지고, 너무 크면 응답 지연이 생긴다.
	 */
	constexpr static long MAX_YIELD_COUNT = 32;

	/**
	 * @brief YIELD_PROCESSOR
	 *
	 * CPU에게 "지금은 바쁘게 기다리는 중(busy-wait)"임을 알려
	 * 파이프라인 자원을 다른 하이퍼스레드에 양보하도록 유도한다.
	 * 실제 컨텍스트 스위치는 발생하지 않는다.
	 *
	 * 플랫폼별 구현:
	 *  - x86/x86-64 : PAUSE 명령어 – 스핀 루프 감지, 메모리 순서 힌트
	 *  - ARM32       : YIELD 명령어 – 하이퍼스레드 힌트
	 *  - AArch64     : YIELD 명령어 – ARMv8 공식 힌트
	 *  - PowerPC     : or 27,27,27  – "low priority" 힌트 관용구
	 *  - RISC-V      : .insn pause  – Zihintpause 확장 (선택적)
	 *  - 그 외       : 빈 메모리 배리어로 최소한의 순서 보장
	 */
	static inline void YIELD_PROCESSOR() noexcept
	{
#if defined(__COMPILER_MSVC)
		// MSVC 내장 함수: _mm_pause() 와 동일
		YieldProcessor();

#elif defined(__COMPILER_GCC) || defined(__COMPILER_CLANG)
	// ── x86 / x86-64 ──────────────────────────────────────────────────
#   if defined(__x86_64__) || defined(__i386__) \
	|| defined(_M_X64)    || defined(_M_IX86)
		__asm__ volatile("pause" ::: "memory");

	// ── ARM 32비트 ────────────────────────────────────────────────────
#   elif defined(__arm__) || defined(_M_ARM)
		__asm__ volatile("yield" ::: "memory");

	// ── ARM 64비트 (AArch64) ──────────────────────────────────────────
#   elif defined(__aarch64__) || defined(_M_ARM64)
		__asm__ volatile("yield" ::: "memory");

	// ── PowerPC / Power ISA ───────────────────────────────────────────
#   elif defined(__powerpc__) || defined(__powerpc64__) \
	  || defined(__ppc__)     || defined(__ppc64__)
		// "or 27,27,27" 은 PowerPC 에서 "low priority" 힌트로 사용되는 관용구
		__asm__ volatile("or 27,27,27" ::: "memory");

	// ── RISC-V ────────────────────────────────────────────────────────
#   elif defined(__riscv)
		// Zihintpause 확장: pause 힌트 인코딩 (fence.i 기반 인코딩)
		// 확장을 지원하지 않는 구현에서는 NOP으로 처리된다.
		__asm__ volatile(".insn i 0x0F, 0, x0, x0, 0x010" ::: "memory");

	// ── 미지원 아키텍처 폴백 ──────────────────────────────────────────
#   else
		// 최소한의 컴파일러 배리어만 삽입하여 루프 최적화를 억제
#       warning "YIELD_PROCESSOR: 미지원 아키텍처 – 컴파일러 배리어 폴백 사용"
		__asm__ volatile("" ::: "memory");
#   endif

#else
	// 컴파일러 자체를 알 수 없는 경우: 아무것도 하지 않음
#   warning "YIELD_PROCESSOR: 알 수 없는 컴파일러"
#endif
	}

	/**
	 * @brief SWITCH_TO_THREAD
	 *
	 * 현재 스레드의 남은 타임 슬라이스를 OS 스케줄러에 반납하여
	 * 다른 실행 가능한 스레드에 CPU를 양보한다.
	 * 경쟁이 심할 때 CPU 낭비를 막기 위해 호출한다.
	 *
	 * 플랫폼별 구현:
	 *  - Windows : SwitchToThread() – 같은 프로세서의 다른 스레드 우선 양보
	 *  - POSIX   : sched_yield()    – OS 스케줄러에 완전 양보
	 */
	static inline void SWITCH_TO_THREAD() noexcept
	{
#if defined(__COMPILER_MSVC)
		// Windows 전용: 같은 논리 프로세서에서 대기 중인 스레드에 먼저 양보
		SwitchToThread();

#elif defined(__COMPILER_GCC) || defined(__COMPILER_CLANG)
		// POSIX 공통: SCHED_FIFO/SCHED_RR 이 아니면 OS가 알아서 스케줄
		sched_yield();

#else
#   warning "SWITCH_TO_THREAD: 알 수 없는 컴파일러 – 양보 불가"
#endif
	}

	/**
	 * @brief CAS(Compare-And-Swap)로 잠금 획득을 시도한다.
	 *
	 * stat 이 SPIN_THREAD_WAIT(0)이면 SPIN_THREAD_WORKING(1)으로 교체 후 true 반환.
	 * 이미 다른 스레드가 잠금 중이면 stat 을 그대로 두고 false 반환.
	 *
	 * 메모리 순서:
	 *  - 성공: ACQUIRE – 이후 임계 구역 읽기/쓰기가 이 교환 이전으로 올라오지 않음
	 *  - 실패: RELAXED – 단순 값 확인이므로 배리어 불필요
	 *
	 * @param stat spin_lock_t::stat 의 주소
	 * @return 잠금 획득 성공 여부
	 */
	static inline bool TRY_ACQUIRE(volatile LONG* stat) noexcept
	{
#if defined(__COMPILER_MSVC)
		// InterlockedCompareExchange: 반환값은 교환 전 원래 값
		// 원래 값이 WAIT 이었다면 → 내가 WORKING 으로 바꾼 것 → 획득 성공
		return ::InterlockedCompareExchange(
			stat,
			SPIN_THREAD_WORKING, // 교환할 새 값
			SPIN_THREAD_WAIT     // 기대하는 현재 값
		) == SPIN_THREAD_WAIT;

#else
		// GCC / Clang __atomic 내장 함수 (C11 _Atomic 과 동등)
		// strong CAS: spurious failure 없음 (루프 내에서도 안전)
		LONG expected = SPIN_THREAD_WAIT;
		return __atomic_compare_exchange_n(
			stat,
			&expected,           // [in/out] 기대 값; 실패 시 현재 값으로 갱신
			SPIN_THREAD_WORKING, // 교환할 새 값
			false,               // strong CAS (spurious failure 허용 안 함)
			__ATOMIC_ACQUIRE,    // 성공 시 메모리 순서: acquire
			__ATOMIC_RELAXED     // 실패 시 메모리 순서: relaxed
		);
#endif
	}

	/**
	 * @brief 잠금을 원자적으로 해제한다 (stat → SPIN_THREAD_WAIT).
	 *
	 * 메모리 순서 RELEASE:
	 *  임계 구역의 모든 읽기/쓰기가 이 저장 이후로 내려오지 않음을 보장.
	 *  다른 스레드가 ACQUIRE 로 잠금을 획득할 때 변경 내용이 보임.
	 *
	 * @param stat spin_lock_t::stat 의 주소
	 */
	static inline void DO_RELEASE(volatile LONG* stat) noexcept
	{
#if defined(__COMPILER_MSVC)
		// InterlockedExchange: 단순 원자 저장 (RELEASE 의미 포함)
		::InterlockedExchange(stat, SPIN_THREAD_WAIT);

#else
		// __ATOMIC_RELEASE: 임계 구역 쓰기가 이 저장보다 앞에 완료됨을 보장
		__atomic_store_n(stat, SPIN_THREAD_WAIT, __ATOMIC_RELEASE);
#endif
	}

	/**
	 * @brief spin_lock_t 를 초기화한다.
	 *
	 * stat 을 SPIN_THREAD_WAIT(0)으로 설정하여 "아무도 잠금을 보유하지 않음"
	 * 상태로 만든다. 사용 전 반드시 한 번 호출해야 한다.
	 */
	void init_spin_lock(struct spin_lock_t* spin_lock)
	{
		spin_lock->stat = SPIN_THREAD_WAIT;
	}

	/**
	 * @brief spin_lock_t 잠금을 획득한다.
	 *
	 * [백오프 알고리즘]
	 *  round < MAX_YIELD_ROUND  →  CPU 힌트(YIELD_PROCESSOR) × MAX_YIELD_COUNT
	 *  round >= MAX_YIELD_ROUND →  OS 양보(SWITCH_TO_THREAD) 후 round 리셋
	 *
	 * 이 함수는 잠금이 성공할 때까지 블로킹된다.
	 */
	void acquire_spin_lock(struct spin_lock_t* spin_lock)
	{
		long round = 0; // 현재 백오프 라운드 카운터

		// CAS 성공(잠금 획득)할 때까지 루프
		while (!TRY_ACQUIRE(&spin_lock->stat))
		{
			if (round < MAX_YIELD_ROUND)
			{
				// ── 1단계: CPU 파이프라인 힌트 ──────────────────────────
				// 컨텍스트 스위치 없이 잠깐 대기.
				// 잠금 보유 시간이 짧은 경우 이 단계에서 획득되는 경우가 많다.
				for (long i = 0; i < MAX_YIELD_COUNT; ++i)
				{
					YIELD_PROCESSOR();
				}
				++round;
			}
			else
			{
				// ── 2단계: OS 스케줄러 양보 ─────────────────────────────
				// 잠금 경쟁이 심하거나 보유 시간이 길면 CPU 낭비를 막기 위해
				// 실행권을 완전히 넘긴다.
				round = 0; // 다음 획득 시도 전 라운드 카운터 리셋
				SWITCH_TO_THREAD();
			}
		}
		// 루프 탈출 시점: TRY_ACQUIRE 가 true → 잠금 획득 완료
		// ACQUIRE 메모리 배리어로 인해 이후 임계 구역 접근이 안전하게 가시됨
	}

	/**
	 * @brief spin_lock_t 잠금을 해제한다.
	 *
	 * RELEASE 배리어로 임계 구역 변경 내용을 다른 스레드에 가시화한 뒤
	 * stat 을 SPIN_THREAD_WAIT(0)으로 되돌린다.
	 */
	void release_spin_lock(struct spin_lock_t* spin_lock)
	{
		DO_RELEASE(&spin_lock->stat);
	}

} // namespace mark
