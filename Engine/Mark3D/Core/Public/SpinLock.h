#pragma once

/**
 * @file SpinLock.h
 * @brief 멀티 플랫폼 스핀락 구현 (Windows / Linux / macOS / ARM / x86 / RISC-V 등)
 *
 * [설계 원칙]
 *  - API(함수 시그니처, 구조체 레이아웃)는 변경하지 않는다.
 *  - volatile LONG stat 은 Windows의 Interlocked API와 GCC __atomic 양쪽에서
 *    동일하게 사용 가능한 공통 타입이므로 그대로 유지한다.
 *    (Windows: LONG == long / POSIX: long 직접 사용)
 *  - 재귀 잠금(Recursive Lock)은 지원하지 않는다.
 */

namespace mark
{
	/**
	 * @brief spin_lock_t
	 *
	 * atomic CAS(Compare-And-Swap) 연산으로 스레드 잠금을 구현한다.
	 * 잠금 실패 시 일정 횟수 CPU 힌트(pause/yield)를 준 뒤,
	 * 그래도 실패하면 OS 스케줄러에 실행권을 양보(sched_yield / SwitchToThread)한다.
	 *
	 * @warning 재귀 잠금 불가 – 같은 스레드에서 이중 acquire 시 데드락 발생
	 */
	struct MARKENGINE_API spin_lock_t
	{
		volatile LONG stat; ///< 0 = 비잠금(WAIT), 1 = 잠금(WORKING)
	};

	/**
	 * @brief spin_lock을 초기화한다.
	 * @param spin_lock 초기화할 spin_lock 객체 포인터 (non-null)
	 */
	MARKENGINE_API void init_spin_lock(
		struct spin_lock_t* spin_lock
	);

	/**
	 * @brief spin_lock 잠금을 획득한다.
	 *
	 * 잠금 획득에 실패하면 아래 순서로 백오프(back-off)를 수행한다.
	 *  1) CPU 파이프라인 힌트(pause / yield ISA) 반복 → 전력/열 절감
	 *  2) MAX_YIELD_ROUND 초과 시 OS 스케줄러에 실행권 양보
	 *
	 * @param spin_lock 잠금을 시도할 spin_lock 객체 포인터 (non-null)
	 */
	MARKENGINE_API void acquire_spin_lock(
		struct spin_lock_t* spin_lock
	);

	/**
	 * @brief spin_lock 잠금을 해제한다.
	 * @param spin_lock 잠금을 해제할 spin_lock 객체 포인터 (non-null)
	 */
	MARKENGINE_API void release_spin_lock(
		struct spin_lock_t* spin_lock
	);

	/**
	 * @brief RAII 패턴 스핀락 – 소멸자에서 자동으로 잠금을 해제한다.
	 *
	 * 일반적인 사용 시 이 객체를 권장한다. 예외나 조기 반환 시에도
	 * 스택 해제(stack unwinding) 과정에서 잠금이 안전하게 풀린다.
	 *
	 * @warning 복사/이동 불가 – 이중 해제(double-release)를 방지하기 위해
	 *          복사 생성자·대입 연산자·이동 생성자·이동 대입 연산자를 삭제한다.
	 *
	 * @code
	 *   spin_lock_t lock;
	 *   init_spin_lock(&lock);
	 *   {
	 *       AUTO_SPIN_LOCK guard(&lock); // acquire
	 *       // ... 임계 구역 ...
	 *   }                               // 소멸 시 자동 release
	 * @endcode
	 */
	struct MARKENGINE_API AUTO_SPIN_LOCK
	{
		/**
		 * @brief 생성자 – 즉시 잠금을 획득한다.
		 * @param spin_lock 대상 spin_lock 포인터 (non-null)
		 */
		explicit AUTO_SPIN_LOCK(struct spin_lock_t* spin_lock) noexcept
			: _sp(spin_lock)
		{
			acquire_spin_lock(_sp);
		}

		/** @brief 소멸자 – 잠금을 해제한다. */
		~AUTO_SPIN_LOCK() noexcept
		{
			if (_sp)
			{
				release_spin_lock(_sp);
				_sp = nullptr; // dangling 포인터 방지
			}
		}

		// ---------------------------------------------------------------
		// 복사/이동 금지: 같은 spin_lock_t를 두 객체가 소유하면
		// 소멸 시 이중 해제(double-release)가 발생한다.
		// ---------------------------------------------------------------
		AUTO_SPIN_LOCK(const AUTO_SPIN_LOCK&)            = delete;
		AUTO_SPIN_LOCK& operator=(const AUTO_SPIN_LOCK&) = delete;
		AUTO_SPIN_LOCK(AUTO_SPIN_LOCK&&)                 = delete;
		AUTO_SPIN_LOCK& operator=(AUTO_SPIN_LOCK&&)      = delete;

		struct spin_lock_t* _sp; ///< 관리 중인 spin_lock 포인터
	};

} // namespace mark
