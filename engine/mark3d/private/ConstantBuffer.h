#ifndef __CONSTANT_BUFFER_H__
#define __CONSTANT_BUFFER_H__

#include "IConstantBuffer.h"


class ConstantBuffer : public IConstantBuffer
{
	friend class ConstantBufferPool; // 오직 ConstantBufferPool 만이 ConstantBuffer 인스턴스를 생성/해제할 수 있음

public:
	virtual void UpdateData(const void* pData, size_t DataSize) override;

	virtual void UpdateDataRef(const void* pData, size_t DataSize) override;

protected:
	ConstantBuffer() = default;
	virtual ~ConstantBuffer() noexcept;
	virtual void OnDestroy() override;

private:
	void* m_pBufferData = nullptr;
	size_t m_BufferSize = 0;
	BOOL m_IsDataRef = false;

};


#endif // __CONSTANT_BUFFER_H__