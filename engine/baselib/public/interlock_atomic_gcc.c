#include "pch.h"
#include "interlock_atomic.h"

#if defined(__TARGET_COMPILER_GCC)
#include <stdio.h>


LONG interlock_store_l(
	LONG volatile* target,
	LONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_exchange_n(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_exchange_n(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG interlock_store_ul(
	ULONG volatile* target,
	ULONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_exchange_n(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_exchange_n(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}


LONG64 interlock_store_l64(
	LONG64 volatile* target,
	LONG64 value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_exchange_n(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_exchange_n(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG64 interlock_store_ul64(
	ULONG64 volatile* target,
	ULONG64 value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_exchange_n(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_exchange_n(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_exchange_n(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}


LONG interlock_increment_l(
	LONG volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG interlock_increment_ul(
	ULONG volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG64 interlock_increment_l64(
	LONG64 volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG64 interlock_increment_ul64(
	ULONG64 volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG interlock_decrement_l(
	LONG volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG interlock_decrement_ul(
	ULONG volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG64 interlock_decrement_l64(
	LONG64 volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG64 interlock_decrement_ul64(
	ULONG64 volatile* target,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, 1, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, 1, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG interlock_fetch_add_l(
	LONG volatile* target,
	LONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG interlock_fetch_add_ul(
	ULONG volatile* target,
	ULONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG64 interlock_fetch_add_l64(
	LONG64 volatile* target,
	LONG64 value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG64 interlock_fetch_add_ul64(
	ULONG64 volatile* target,
	ULONG64 value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_add(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_add(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_add(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG interlock_fetch_sub_l(
	LONG volatile* target,
	LONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG interlock_fetch_sub_ul(
	ULONG volatile* target,
	ULONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

LONG64 interlock_fetch_sub_l64(
	LONG64 volatile* target,
	LONG64 value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

ULONG64 interlock_fetch_sub_ul64(
	ULONG64 volatile* target,
	ULONG64 value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELAXED);

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQUIRE);

	case MEMORY_ORDER_RELEASE:
		return __atomic_fetch_sub(target, value, __ATOMIC_RELEASE);

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_fetch_sub(target, value, __ATOMIC_ACQ_REL);

	default:
		return *target;
	}
}

BOOL interlock_compare_exchange_l(
	LONG volatile* target,
	LONG exchange,
	LONG comperand,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_RELEASE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED) ? TRUE : FALSE;

	default:
		return FALSE;
	}
}

BOOL interlock_compare_exchange_ul(
	ULONG volatile* target,
	ULONG exchange,
	ULONG comperand,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_RELEASE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED) ? TRUE : FALSE;

	default:
		return FALSE;
	}
}

BOOL interlock_compare_exchange_l64(
	LONG64 volatile* target,
	LONG64 exchange,
	LONG64 comperand,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_RELEASE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED) ? TRUE : FALSE;

	default:
		return FALSE;
	}
}

BOOL interlock_compare_exchange_ul64(
	ULONG64 volatile* target,
	ULONG64 exchange,
	ULONG64 comperand,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_RELEASE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED) ? TRUE : FALSE;

	default:
		return FALSE;
	}
}

BOOL interlock_compare_exchange_ptr(
	void* volatile* target,
	void* exchange,
	void* comperand,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER_RELAXED:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQUIRE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_RELEASE:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED) ? TRUE : FALSE;

	case MEMORY_ORDER_ACQ_REL:
		return __atomic_compare_exchange_n(target, &comperand, exchange, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED) ? TRUE : FALSE;

	default:
		return FALSE;
	}
}


#endif // __TARGET_COMPILER_GCC