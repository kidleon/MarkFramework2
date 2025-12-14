#ifndef __I_CONSTANT_BUFFER_H__
#define __I_CONSTANT_BUFFER_H__


interface IConstantBuffer : public IUNKNOWN_IMPL
{
	/**
	* @brief 데이터 복사 후 업데이트. UpdateData 함수는 데이터를 복사하므로, 호출 후 pData의 메모리는 해제해도 됩니다.
	* @param pData 데이터 포인터
	* @param DataSize 데이터 크기 (바이트 단위)
	* @remark 내부 버퍼에 데이터를 복사합니다.
	* @note pData가 nullptr이거나 DataSize가 0이면 동작하지 않습니다.
	* @note DataSize가 내부 버퍼 크기보다 크면 동작하지 않습니다.
	* @return 없음
	*/
	virtual void UpdateData(const void* pData, size_t DataSize) = 0;

	/**
	* @brief 데이터 참조 후 업데이트. UpdateDataRef 함수는 데이터를 복사하지 않으므로, pData의 메모리는 유효해야 합니다.
	* @param pData 데이터 포인터
	* @param DataSize 데이터 크기 (바이트 단위)
	* @return 없음
	*/
	virtual void UpdateDataRef(const void* pData, size_t DataSize) = 0;

};


#endif // __I_CONSTANT_BUFFER_H__
