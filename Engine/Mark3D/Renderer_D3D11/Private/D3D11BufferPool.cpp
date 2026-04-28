#include "pch.h"
#include "D3D11BufferPool.h"
#include "D3D11RenderDef.h"
#include "D3D11RenderDevice.h"
#include "D3D11Buffer.h"
#include "SpinLock.h"


namespace mark
{
	inline uint32_t ConvBindFlags(BUFFER_TYPE BufferType)
	{
		switch (BufferType)
		{
		case BUFFER_TYPE::VERTEX:
			return D3D11_BIND_VERTEX_BUFFER;
		case BUFFER_TYPE::INDEX:
			return D3D11_BIND_INDEX_BUFFER;
		case BUFFER_TYPE::CONSTANT:
			return D3D11_BIND_CONSTANT_BUFFER;
		default:
			assert(false && "Invalid buffer type");
			return 0;
		}
	}

	static D3D11Buffer* CreateBuffer(
		BUFFER_TYPE BufferType,
		BUFFER_USAGE Usage,
		size_t BufferSize
	)
	{
		ID3D11Device* pDevice = D3D11RenderDevice::Get().INL_GetDevice();

		D3D11_BUFFER_DESC Desc{};
		Desc.ByteWidth = static_cast<UINT>(BufferSize);
		Desc.Usage = D3D11_IMPL_BUFFER_USAGE[(size_t)Usage];
		Desc.BindFlags = ConvBindFlags(BufferType);
		Desc.CPUAccessFlags = (Usage == BUFFER_USAGE::DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
		Desc.MiscFlags = 0;

		ID3D11Buffer* pD3D11Buffer = nullptr;
		if(FAILED(pDevice->CreateBuffer(&Desc, nullptr, &pD3D11Buffer)))
		{
			assert(false && "Failed to create D3D11 buffer");
			return nullptr;
		}

		D3D11Buffer* pBuffer = (D3D11Buffer*)CORE_SYS_ALLOC(sizeof(D3D11Buffer));
		pBuffer->pBuffer = pD3D11Buffer;
		pBuffer->BufferSize = static_cast<UINT>(BufferSize);
		pBuffer->Usage = Usage;
		pBuffer->Type = BufferType;
		pBuffer->SizeIndex = 0; // SizeIndex는 Acquire에서 결정되므로 초기값은 0으로 설정

		return pBuffer;
	}

	D3D11BufferPool::~D3D11BufferPool() noexcept
	{
		Shutdown();
	}

	bool D3D11BufferPool::Initialize()
	{
		return true;
	}

	void D3D11BufferPool::Shutdown()
	{
		for (size_t usageIdx = 0; usageIdx < (size_t)BUFFER_USAGE::EMAX; ++usageIdx)
		{
			for (size_t sizeIdx = 0; sizeIdx < (size_t)BUFFER_SIZE::SIZE_COUNT; ++sizeIdx)
			{
				for (auto& pool : { m_VBPool, m_IBPool, m_CBPool })
				{
					for (auto buff : pool[usageIdx][sizeIdx].lstBuffers)
					{
						if (buff)
						{
							CHECK_RELEASE(buff->pBuffer);
							CORE_SYS_FREE(buff);
						}
					}
					pool[usageIdx][sizeIdx].lstBuffers.clear();
				}
			}
		}
	}

	D3D11Buffer* D3D11BufferPool::Acquire(
		BUFFER_TYPE BufferType,
		BUFFER_USAGE Usage,
		size_t BufferSize
	) noexcept
	{
		if (BufferSize == 0 || BufferType == BUFFER_TYPE::UNKNOWN || Usage >= BUFFER_USAGE::EMAX) [[unlikely]]
		{
			assert(false && "Invalid buffer acquisition parameters");
			return nullptr;
		}

		size_t sizeIdx = 0;
		size_t size = 1024; // 1KB
		while (size < BufferSize && sizeIdx < (size_t)BUFFER_SIZE::SIZE_COUNT - 1)
		{
			size <<= 1; // 크기를 2배로 증가
			++sizeIdx;
		}

		switch (BufferType)
		{
			case BUFFER_TYPE::VERTEX:
			{
				AUTO_SPIN_LOCK lock(&m_VBPool[(size_t)Usage][sizeIdx].lock);
				auto& lst = m_VBPool[(size_t)Usage][sizeIdx].lstBuffers;
				if (!lst.empty())
				{
					D3D11Buffer* pBuffer = lst.front();
					lst.pop_front();
					return pBuffer;
				}
				break;
			}
			case BUFFER_TYPE::INDEX:
			{
				AUTO_SPIN_LOCK lock(&m_IBPool[(size_t)Usage][sizeIdx].lock);
				auto& lst = m_IBPool[(size_t)Usage][sizeIdx].lstBuffers;
				if (!lst.empty())
				{
					D3D11Buffer* pBuffer = lst.front();
					lst.pop_front();
					return pBuffer;
				}
				break;
			}
			case BUFFER_TYPE::CONSTANT:
			{
				AUTO_SPIN_LOCK lock(&m_CBPool[(size_t)Usage][sizeIdx].lock);
				auto& lst = m_CBPool[(size_t)Usage][sizeIdx].lstBuffers;
				if (!lst.empty())
				{
					D3D11Buffer* pBuffer = lst.front();
					lst.pop_front();
					return pBuffer;
				}
				break;
			}

			default:
				assert(false && "Invalid buffer type");
				return nullptr;
		}

		D3D11Buffer* pBuffer = CreateBuffer(BufferType, Usage, size);
		if (pBuffer)
			pBuffer->SizeIndex = static_cast<uint32_t>(sizeIdx);

		return pBuffer;
	}

	void D3D11BufferPool::Release(D3D11Buffer* pBuffer)
	{
		if (!pBuffer || !pBuffer->pBuffer) [[unlikely]]
			return;

		switch(pBuffer->Type)
		{
			case BUFFER_TYPE::VERTEX:
			{
				AUTO_SPIN_LOCK lock(&m_VBPool[(size_t)pBuffer->Usage][pBuffer->SizeIndex].lock);
				m_VBPool[(size_t)pBuffer->Usage][pBuffer->SizeIndex].lstBuffers.push_back(pBuffer);
				break;
			}
			case BUFFER_TYPE::INDEX:
			{
				AUTO_SPIN_LOCK lock(&m_IBPool[(size_t)pBuffer->Usage][pBuffer->SizeIndex].lock);
				m_IBPool[(size_t)pBuffer->Usage][pBuffer->SizeIndex].lstBuffers.push_back(pBuffer);
				break;
			}
			case BUFFER_TYPE::CONSTANT:
			{
				AUTO_SPIN_LOCK lock(&m_CBPool[(size_t)pBuffer->Usage][pBuffer->SizeIndex].lock);
				m_CBPool[(size_t)pBuffer->Usage][pBuffer->SizeIndex].lstBuffers.push_back(pBuffer);
				break;
			}
			default:
				assert(false && "Invalid buffer type");
				break;
		}
	}

}
