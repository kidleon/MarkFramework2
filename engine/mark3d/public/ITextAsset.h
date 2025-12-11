#ifndef __ITEXTASSET_H__
#define __ITEXTASSET_H__

#include "IAssetImpl.h"


/**
* @brief 텍스트 자산 인터페이스, 문자열 데이터를 다루기 위한 기능 제공, UTF-8 인코딩 사용
*/
interface ITextAsset : public IASSET_IMPL<ASSET_TYPE::TEXT>
{
	/**
	* @brief 문자열 데이터 포인터 반환 (ITextAsset는 UTF-8 인코딩을 사용함)
	* @return 문자열 데이터 포인터
	* @brief 문자열 크기 반환
	*/
	virtual const char* GetData() const noexcept = 0;

	/**
	* @brief 문자열 크기 반환
	* @return 문자열 크기 (바이트 단위)
	*/
	virtual size_t GetSize() const noexcept = 0;

	/**
	* @brief UTF-8 인코딩 여부 반환
	* @return UTF-8 인코딩일 경우 TRUE, 아닐 경우 FALSE
	*/
	virtual BOOL IsUTF8() const noexcept = 0;

	/**
	* @brief 인코딩 유형 반환
	* @return 인코딩 유형
	* @note ITextAsset는 기본적으로 UTF-8 인코딩을 사용하지만, 파일에 BOM이 포함된 경우 해당 인코딩 유형을 반환할 수 있음
	* @note 반환 값은 ENCODING_TYPE 열거형 값 중 하나임
	*/
	virtual ENCODING_TYPE GetEncodingType() const noexcept = 0;

	/**
	* @brief UTF-32 변환
	* @param pBuffer 변환된 문자열을 저장할 버퍼
	* @param BufferSize pBuffer 버퍼 크기 (바이트 단위)
	* @param pResultSize 변환된 문자열 크기 (문자 단위)
	*/
	virtual BOOL ConvertUTF32(char32_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept = 0;

	/**
	* @brief UTF-16 변환
	* @param pBuffer 변환된 문자열을 저장할 버퍼
	* @param BufferSize pBuffer 버퍼 크기 (바이트 단위)
	* @param pResultSize 변환된 문자열 크기 (문자 단위)
	* @return 변환 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL ConvertUTF16(char16_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept = 0;

	/**
	* @brief WCHAR 변환 (Linux/MacOS 4byte, Windows 2byte)
	* @param pBuffer 변환된 문자열을 저장할 버퍼
	* @param BufferSize pBuffer 버퍼 크기 (바이트 단위)
	* @param pResultSize 변환된 문자열 크기 (문자 단위)
	* @return 변환 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL ConvertWCHAR(wchar_t* pBuffer, size_t BufferSize, size_t* pResultSize) const noexcept = 0;
};

#endif // __ITEXTASSET_H__
