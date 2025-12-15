#ifndef __CONSTANT_BUFFER_H__
#define __CONSTANT_BUFFER_H__

#include "IConstantBuffer.h"
#include "ConstantBufferBlock.h"


struct CONSTANT_BUFFER_BLOCK;

class ConstantBuffer : public IConstantBuffer
{
public:
	ConstantBuffer() = default;

	virtual void UpdateData(const void* pData, size_t DataSize) override;

	virtual void UpdateDataRef(const void* pData, size_t DataSize) override;

	__FORCEINLINE BOOL INL_IsDataRef() const noexcept
	{
		return (m_pDataRefPtr != nullptr) && (m_DataRefSize > 0);
	}

	__FORCEINLINE const void* INL_GetDataPtr() const noexcept
	{
		if (INL_IsDataRef())
		{
			return m_pDataRefPtr;
		}
		else if (m_pCBufferBlock)
		{
			return m_pCBufferBlock->pData;
		}

		return nullptr;
	}

	__FORCEINLINE size_t INL_GetDataSize() const noexcept
	{
		if (INL_IsDataRef())
		{
			return m_DataRefSize;
		}
		else if (m_pCBufferBlock)
		{
			return m_pCBufferBlock->BufferSize;
		}

		return 0;
	}

protected:
	virtual ~ConstantBuffer() noexcept;
	virtual void OnDestroy() override;

private:
	CONSTANT_BUFFER_BLOCK* m_pCBufferBlock = nullptr;

	void* m_pDataRefPtr = nullptr;
	size_t m_DataRefSize = 0;

};


#endif // __CONSTANT_BUFFER_H__