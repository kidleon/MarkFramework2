#include "pch.h"
#include "D3D11GPUBufferPool.h"
#include "D3D11HardwareGraphicsLayer.h"
#include "D3D11GPUBuffer.h"
#include "D3D11RenderDevice.h"


namespace mark
{
	D3D11GPUBufferPool::~D3D11GPUBufferPool() noexcept
	{
		Shutdown();
	}

	void D3D11GPUBufferPool::Initialize(D3D11HardwareGraphicsLayer* pHardwareGraphicsLayer)
	{
		m_pHardwareGraphicsLayer = pHardwareGraphicsLayer;
	}

	void D3D11GPUBufferPool::Shutdown()
	{
		for (int usage = 0; usage < (int)BUFFER_USAGE::EMAX; ++usage)
		{
			for (int size = 0; size < (int)GPU_BUFFER_SIZE::EMAX; ++size)
			{
				for (auto pBuffer : m_VBPool[usage][size].OriginalBuffers)
				{
					pBuffer->Release();
				}

				m_VBPool[usage][size].OriginalBuffers.clear();
				m_VBPool[usage][size].FreeBuffers.clear();

				for (auto pBuffer : m_IBPool[usage][size].OriginalBuffers)
				{
					pBuffer->Release();
				}

				m_IBPool[usage][size].OriginalBuffers.clear();
				m_IBPool[usage][size].FreeBuffers.clear();

				for (auto pBuffer : m_CBPool[usage][size].OriginalBuffers)
				{
					pBuffer->Release();
				}

				m_CBPool[usage][size].OriginalBuffers.clear();
				m_CBPool[usage][size].FreeBuffers.clear();
			}
		}
	}

	int32_t D3D11GPUBufferPool::MapBufferSize(size_t BufferSize)
	{
		for (int32_t i = 0; i < (int32_t)GPU_BUFFER_SIZE::EMAX; ++i)
		{
			if (BufferSize <= GPU_BUFFER_SIZES[i])
				return i;
		}
		return -1;
	}

	D3D11GPUBuffer* D3D11GPUBufferPool::Acquire(
		BUFFER_TYPE BufferType,
		BUFFER_USAGE BufferUsage,
		size_t BufferSize
	)
	{
		int32_t SizeIndex = MapBufferSize(BufferSize);
		if (-1 == SizeIndex) [[unlikely]]
			return nullptr;

		switch (BufferType)
		{
			case BUFFER_TYPE::VERTEX_BUFFER:
				{
					auto pGroup = &m_VBPool[(int)BufferUsage][SizeIndex];

					AUTO_SPIN_LOCK Guard(&pGroup->SpinLock);
					if (pGroup->FreeBuffers.empty())
					{
						if (!CreateBufferPage(
							GPU_BUFFER_TYPE::VERTEX_BUFFER,
							(GPU_BUFFER_USAGE)BufferUsage,
							(GPU_BUFFER_SIZE)SizeIndex))
						{
							SYS_LOG_ERR_F("GPUBufferPool::Acquire - Failed to create buffer page for vertex buffer (Usage: {}, Size: {})", (int)BufferUsage, GPU_BUFFER_SIZES[SizeIndex]);
							return nullptr;
						}
					}

					D3D11GPUBuffer* pBuffer = pGroup->FreeBuffers.front();
					pGroup->FreeBuffers.pop_front();
					pBuffer->AddRef();

					return pBuffer;

				} break;

			case BUFFER_TYPE::INDEX_BUFFER:
				{
					auto pGroup = &m_IBPool[(int)BufferUsage][SizeIndex];

					AUTO_SPIN_LOCK Guard(&pGroup->SpinLock);
					if (pGroup->FreeBuffers.empty())
					{
						if (!CreateBufferPage(
							GPU_BUFFER_TYPE::INDEX_BUFFER,
							(GPU_BUFFER_USAGE)BufferUsage,
							(GPU_BUFFER_SIZE)SizeIndex))
						{
							SYS_LOG_ERR_F("GPUBufferPool::Acquire - Failed to create buffer page for index buffer (Usage: {}, Size: {})", (int)BufferUsage, GPU_BUFFER_SIZES[SizeIndex]);
							return nullptr;
						}
					}

					D3D11GPUBuffer* pBuffer = pGroup->FreeBuffers.front();
					pGroup->FreeBuffers.pop_front();
					pBuffer->AddRef();

					return pBuffer;

				} break;

			case BUFFER_TYPE::CONSTANT_BUFFER:
				{
					auto pGroup = &m_CBPool[(int)BufferUsage][SizeIndex];

					AUTO_SPIN_LOCK Guard(&pGroup->SpinLock);
					if (pGroup->FreeBuffers.empty())
					{
						if (!CreateBufferPage(
							GPU_BUFFER_TYPE::CONSTANT_BUFFER,
							(GPU_BUFFER_USAGE)BufferUsage,
							(GPU_BUFFER_SIZE)SizeIndex))
						{
							SYS_LOG_ERR_F("GPUBufferPool::Acquire - Failed to create buffer page for constant buffer (Usage: {}, Size: {})", (int)BufferUsage, GPU_BUFFER_SIZES[SizeIndex]);
							return nullptr;
						}
					}

					D3D11GPUBuffer* pBuffer = pGroup->FreeBuffers.front();
					pGroup->FreeBuffers.pop_front();
					pBuffer->AddRef();

					return pBuffer;
				} break;

			default:
				return nullptr;
		}
	}

	void D3D11GPUBufferPool::Release(D3D11GPUBuffer* pBuffer)
	{
		if (!pBuffer)
			return;

		BUFFER_TYPE BufferType = pBuffer->GetBufferType();
		BUFFER_USAGE BufferUsage = pBuffer->GetBufferUsage();
		size_t BufferSize = pBuffer->GetBufferSize();

		int32_t SizeIndex = MapBufferSize(BufferSize);
		if (-1 == SizeIndex) [[unlikely]]
			return;

		switch (BufferType)
		{
			case BUFFER_TYPE::VERTEX_BUFFER:
				{
					auto pGroup = &m_VBPool[(int)BufferUsage][SizeIndex]; // Usage는 버퍼 생성 시 고정이므로 0으로 접근
					AUTO_SPIN_LOCK Guard(&pGroup->SpinLock);
					pGroup->FreeBuffers.push_front(pBuffer);
				} break;

			case BUFFER_TYPE::INDEX_BUFFER:
				{
					auto pGroup = &m_IBPool[(int)BufferUsage][SizeIndex]; // Usage는 버퍼 생성 시 고정이므로 0으로 접근
					AUTO_SPIN_LOCK Guard(&pGroup->SpinLock);
					pGroup->FreeBuffers.push_front(pBuffer);
				} break;

			case BUFFER_TYPE::CONSTANT_BUFFER:
				{
					auto pGroup = &m_CBPool[(int)BufferUsage][SizeIndex]; // Usage는 버퍼 생성 시 고정이므로 0으로 접근
					AUTO_SPIN_LOCK Guard(&pGroup->SpinLock);
					pGroup->FreeBuffers.push_front(pBuffer);
				} break;

			default:
				return;
		}
	}

	bool D3D11GPUBufferPool::CreateBufferPage(
		GPU_BUFFER_TYPE BufferType,
		GPU_BUFFER_USAGE BufferUsage,
		GPU_BUFFER_SIZE BufferSize
	)
	{
		int UsageIndex = (int)BufferUsage;
		int SizeIndex = (int)BufferSize;

		GPUBufferPageGroup* pGroup = nullptr;
		switch (BufferType)
		{
			case GPU_BUFFER_TYPE::VERTEX_BUFFER:
				pGroup = &m_VBPool[UsageIndex][SizeIndex];
				break;

			case GPU_BUFFER_TYPE::INDEX_BUFFER:
				pGroup = &m_IBPool[UsageIndex][SizeIndex];
				break;

			case GPU_BUFFER_TYPE::CONSTANT_BUFFER:
				pGroup = &m_CBPool[UsageIndex][SizeIndex];
				break;

			default:
				return false;
		}

		uint32_t NumBuffers = (uint32_t)GPUBUFFER_COUNT_PER_PAGE[SizeIndex];
		size_t BufferBytes = GPU_BUFFER_SIZES[SizeIndex];

		GPUBufferCreateDesc CreateDesc = {};
		switch (BufferType)
		{
			case GPU_BUFFER_TYPE::VERTEX_BUFFER:
				CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
				break;

			case GPU_BUFFER_TYPE::INDEX_BUFFER:
				CreateDesc.Type = BUFFER_TYPE::INDEX_BUFFER;
				break;

			case GPU_BUFFER_TYPE::CONSTANT_BUFFER:
				CreateDesc.Type = BUFFER_TYPE::CONSTANT_BUFFER;
				break;

			default:
				return false;
		}

		CreateDesc.Usage = (BufferUsage == GPU_BUFFER_USAGE::DYNAMIC) ? BUFFER_USAGE::DYNAMIC : BUFFER_USAGE::DEFAULT;
		CreateDesc.Access = (BufferUsage == GPU_BUFFER_USAGE::DYNAMIC) ? BUFFER_ACCESS::WRITE : BUFFER_ACCESS::NONE;
		CreateDesc.BufferSize = BufferBytes;

		D3D11RenderDevice* pRenderDevice = m_pHardwareGraphicsLayer->INL_GetRenderDevice();

		uint32_t Created = 0;
		for (uint32_t i = 0; i < NumBuffers; ++i)
		{
			ID3D11Buffer * pD3D11Buffer = pRenderDevice->CreateBuffer(CreateDesc);
			D3D11GPUBuffer* pGPUBuffer = CORE_NEW(D3D11GPUBuffer)(pRenderDevice->INL_GetD3D11Context(), pD3D11Buffer);
			if (!pGPUBuffer) [[unlikely]]
				continue;

			pGroup->OriginalBuffers.push_back(pGPUBuffer);
			pGroup->FreeBuffers.push_back(pGPUBuffer);

			++Created;
		}

		if (0 == Created) [[unlikely]]
		{
			SYS_LOG_ERR_F("GPUBufferPool::CreateBufferPage - Failed to create any buffer (Type: {}, Usage: {}, Size: {})",
				(int)BufferType, (int)BufferUsage, BufferBytes);
			return false;
		}

		return true;
	}
}
