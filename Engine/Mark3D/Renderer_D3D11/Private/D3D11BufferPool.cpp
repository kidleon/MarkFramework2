#include "pch.h"
#include "D3D11BufferPool.h"


namespace mark
{
	enum class BUFFER_SIZE : uint8_t
	{
		SIZE_1K = 0,
		SIZE_2K,
		SIZE_4K,
		SIZE_8K,
		SIZE_16K,
		SIZE_64K,
		SIZE_128K,
		SIZE_256K,
		SIZE_512K,
		SIZE_1M,
		SIZE_2M,

		SIZE_COUNT
	};

	struct BufferPool
	{
		sys_list<D3D11Buffer*> m_lstBuffers[(size_t)BUFFER_USAGE::EMAX][(size_t)BUFFER_SIZE::SIZE_COUNT];
	};

	static BufferPool s_BufferPool[(size_t)BUFFER_TYPE::EMAX];

	struct BufferPoolConfig
	{
		size_t InitialCount;
		size_t MaxPoolCount[(size_t)BUFFER_SIZE::SIZE_COUNT];
	};



}
