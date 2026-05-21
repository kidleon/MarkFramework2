#include "pch.h"
#include "GPUBufferPool.h"


namespace mark
{
	GPUBufferPool::~GPUBufferPool() noexcept
	{
		Shutdown();
	}

	void GPUBufferPool::Initialize(IHardwareGraphicsLayer* pHardwareGraphicsLayer)
	{
		m_pHardwareGraphicsLayer = pHardwareGraphicsLayer;
	}

	void GPUBufferPool::Shutdown()
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

	int32_t GPUBufferPool::MapBufferSize(size_t BufferSize)
	{
		for (int32_t i = 0; i < (int32_t)GPU_BUFFER_SIZE::EMAX; ++i)
		{
			if (BufferSize <= GPU_BUFFER_SIZES[i])
				return i;
		}
		return -1;
	}

	IGPUBuffer* GPUBufferPool::Acquire(
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

				IGPUBuffer* pBuffer = pGroup->FreeBuffers.front();
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

				IGPUBuffer* pBuffer = pGroup->FreeBuffers.front();
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

				IGPUBuffer* pBuffer = pGroup->FreeBuffers.front();
				pGroup->FreeBuffers.pop_front();
				pBuffer->AddRef();

				return pBuffer;
			} break;

			default:
				return nullptr;
		}
	}

	void GPUBufferPool::Release(IGPUBuffer* pBuffer)
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

	bool GPUBufferPool::CreateBufferPage(
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

		uint32_t Created = 0;
		for (uint32_t i = 0; i < NumBuffers; ++i)
		{
			IGPUBuffer* pBuffer = m_pHardwareGraphicsLayer->CreateGPUBuffer(CreateDesc);
			if (!pBuffer) [[unlikely]]
				continue;

			pGroup->OriginalBuffers.push_back(pBuffer);
			pGroup->FreeBuffers.push_back(pBuffer);
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
