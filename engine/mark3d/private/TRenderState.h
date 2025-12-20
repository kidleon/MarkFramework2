#ifndef __T_RENDER_STATE_H__
#define __T_RENDER_STATE_H__

#include "fnv.h"


template<typename _T>
class TRenderState
{
public:
	TRenderState(_T& state) noexcept
		: State(state)
	{
		OnInvalidate();
	}

	__INLINE void OnInvalidate()
	{
		StateHash = fnv64_c(&State, sizeof(_T));
	}

	__FORCEINLINE const _T& GetState() const noexcept
	{
		return State;
	}

	__FORCEINLINE uint64 GetStateHash() const noexcept
	{
		return StateHash;
	}

	__FORCEINLINE _T* GetStatePtr() noexcept
	{
		return &State;
	}

	__FORCEINLINE _T& GetState() noexcept
	{
		return State;
	}

private:
	_T State;
	uint64 StateHash;
};

#endif // __T_RENDER_STATE_H__
