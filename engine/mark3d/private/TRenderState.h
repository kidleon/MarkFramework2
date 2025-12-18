#ifndef __T_RENDER_STATE_H__
#define __T_RENDER_STATE_H__

#include "fnv.h"


template<typename _T>
class TRenderState
{
public:
	void OnInvalidate()
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


	_T State;
	uint64 StateHash;
};

#endif // __T_RENDER_STATE_H__
