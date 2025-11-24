#include "pch.h"
#include "interlock_atomic.h"


#if defined(__TARGET_COMPILER_MSC)

LONG interlock_store_l(
	LONG volatile* target,
	LONG value,
	MEMORY_ORDER order
)
{
	switch (order)
	{
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedExchangeNoFence(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedExchangeAcquire(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedExchange(target, value); // Exchanged는 Release가 없다.

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedExchange(target, value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG)::InterlockedExchangeNoFence((volatile LONG*)target, (LONG)value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG)::InterlockedExchangeAcquire((volatile LONG*)target, (LONG)value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG)::InterlockedExchange((volatile LONG*)target, (LONG)value); // Exchanged는 Release가 없다.

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG)::InterlockedExchange((volatile LONG*)target, (LONG)value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedExchangeNoFence64(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedExchangeAcquire64(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedExchange64(target, value); // Exchanged는 Release가 없다.

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedExchange64(target, value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG64)::InterlockedExchangeNoFence64((volatile LONG64*)target, (LONG64)value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG64)::InterlockedExchangeAcquire64((volatile LONG64*)target, (LONG64)value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG64)::InterlockedExchange64((volatile LONG64*)target, (LONG64)value); // Exchanged는 Release가 없다.

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG64)::InterlockedExchange64((volatile LONG64*)target, (LONG64)value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedIncrementNoFence(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedIncrementAcquire(target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedIncrementRelease(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedIncrement(target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedIncrementNoFence(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedIncrementAcquire(target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedIncrementRelease(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedIncrement(target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedIncrementNoFence64(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedIncrementAcquire64(target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedIncrementRelease64(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedIncrement64(target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG64)::InterlockedIncrementNoFence64((volatile LONG64*)target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG64)::InterlockedIncrementAcquire64((volatile LONG64*)target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG64)::InterlockedIncrementRelease64((volatile LONG64*)target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG64)::InterlockedIncrement64((volatile LONG64*)target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedDecrementNoFence(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedDecrementAcquire(target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedDecrementRelease(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedDecrement(target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedDecrementNoFence(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedDecrementAcquire(target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedDecrementRelease(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedDecrement(target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedDecrementNoFence64(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedDecrementAcquire64(target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedDecrementRelease64(target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedDecrement64(target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG64)::InterlockedDecrementNoFence64((volatile LONG64*)target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG64)::InterlockedDecrementAcquire64((volatile LONG64*)target);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG64)::InterlockedDecrementRelease64((volatile LONG64*)target);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG64)::InterlockedDecrement64((volatile LONG64*)target);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedExchangeAddNoFence(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedExchangeAddAcquire(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedExchangeAddRelease(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedExchangeAdd(target, value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG)::InterlockedExchangeAddNoFence((volatile LONG*)target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG)::InterlockedExchangeAddAcquire((volatile LONG*)target, value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG)::InterlockedExchangeAddRelease((volatile LONG*)target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG)::InterlockedExchangeAdd((volatile LONG*)target, value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedExchangeAddNoFence64(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedExchangeAddAcquire64(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedExchangeAddRelease64(target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedExchangeAdd64(target, value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG64)::InterlockedExchangeAddNoFence64((volatile LONG64*)target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG64)::InterlockedExchangeAddAcquire64((volatile LONG64*)target, value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG64)::InterlockedExchangeAddRelease64((volatile LONG64*)target, value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG64)::InterlockedExchangeAdd64((volatile LONG64*)target, value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedExchangeAddNoFence(target, -value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedExchangeAddAcquire(target, -value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedExchangeAddRelease(target, -value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedExchangeAdd(target, -value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG)::InterlockedExchangeAddNoFence((volatile LONG*)target, -(LONG)value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG)::InterlockedExchangeAddAcquire((volatile LONG*)target, -(LONG)value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG)::InterlockedExchangeAddRelease((volatile LONG*)target, -(LONG)value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG)::InterlockedExchangeAdd((volatile LONG*)target, -(LONG)value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return ::InterlockedExchangeAddNoFence64(target, -value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return ::InterlockedExchangeAddAcquire64(target, -value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return ::InterlockedExchangeAddRelease64(target, -value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return ::InterlockedExchangeAdd64(target, -value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (ULONG64)::InterlockedExchangeAddNoFence64((volatile LONG64*)target, -(LONG64)value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (ULONG64)::InterlockedExchangeAddAcquire64((volatile LONG64*)target, -(LONG64)value);

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (ULONG64)::InterlockedExchangeAddRelease64((volatile LONG64*)target, -(LONG64)value);

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (ULONG64)::InterlockedExchangeAdd64((volatile LONG64*)target, -(LONG64)value);

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (::InterlockedCompareExchangeNoFence(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (::InterlockedCompareExchangeAcquire(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (::InterlockedCompareExchangeRelease(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (::InterlockedCompareExchange(target, exchange, comperand) == comperand) ? TRUE : FALSE;

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (::InterlockedCompareExchangeNoFence(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (::InterlockedCompareExchangeAcquire(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (::InterlockedCompareExchangeRelease(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (::InterlockedCompareExchange(target, exchange, comperand) == comperand) ? TRUE : FALSE;

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (::InterlockedCompareExchangeNoFence64(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (::InterlockedCompareExchangeAcquire64(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (::InterlockedCompareExchangeRelease64(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (::InterlockedCompareExchange64(target, exchange, comperand) == comperand) ? TRUE : FALSE;

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (::InterlockedCompareExchangeNoFence64((LONG64 volatile*)target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (::InterlockedCompareExchangeAcquire64((LONG64 volatile*)target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (::InterlockedCompareExchangeRelease64((LONG64 volatile*)target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (::InterlockedCompareExchange64((LONG64 volatile*)target, exchange, comperand) == comperand) ? TRUE : FALSE;

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
	case MEMORY_ORDER::MEMORY_ORDER_RELAXED:
		return (::InterlockedCompareExchangePointerNoFence(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQUIRE:
		return (::InterlockedCompareExchangePointerAcquire(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_RELEASE:
		return (::InterlockedCompareExchangePointerRelease(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	case MEMORY_ORDER::MEMORY_ORDER_ACQ_REL:
		return (::InterlockedCompareExchangePointer(target, exchange, comperand) == comperand) ? TRUE : FALSE;

	default:
		return FALSE;
	}
}

#endif // __TARGET_COMPILER_MSC