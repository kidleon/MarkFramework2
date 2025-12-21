#ifndef __D3D11_CONSTANT_BUFFER_H__
#define __D3D11_CONSTANT_BUFFER_H__

#include "IConstantBuffer.h"
#include "D3D11ConstantBufferBlock.h"


struct D3D11_CONSTANT_BUFFER_BLOCK;

class D3D11ConstantBufferImpl final : public IConstantBuffer
{
public:
	D3D11ConstantBufferImpl() = default;

	virtual void UpdateData(const void* pData, size_t DataSize) final;

	virtual void UpdateDataRef(const void* pData, size_t DataSize) final;

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
	virtual ~D3D11ConstantBufferImpl() noexcept;
	virtual void OnDestroy() final;

private:
	D3D11_CONSTANT_BUFFER_BLOCK* m_pCBufferBlock = nullptr;

	void* m_pDataRefPtr = nullptr;
	size_t m_DataRefSize = 0;

};


#endif // __D3D11_CONSTANT_BUFFER_H__