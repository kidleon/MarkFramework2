#ifndef __ASSET_DEF_H__
#define __ASSET_DEF_H__


/**
* @brief 자산 유형 열거형
*/
enum class ASSET_TYPE : unsigned
{
	UNKNOWN = 0,
	TEXT,
	BINARY,
	CBUFFER,
	PRIMITIVE_BUFFER,
	SHADER_PROGRAM,
	SURFACE_MATERIAL,
	TEXTURE1D,
	TEXTURE2D,
	TEXTURE3D,
	RENDER_TARGET,
	RENDER_CAMERA,

	MODEL,
	AUDIO,
};

/**
* @brief 자산 로드 상태 열거형
*/
enum class LOAD_STAT : unsigned
{
	NOT_LOADED = 0,
	LOADING,
	LOADED,
	FAILED,
};

/**
* @brief 자산 인터페이스
*/
struct IAsset : public IUNKNOWN
{
	/**
	* @brief 자산 고유 ID 반환
	* @return 자산 고유 ID
	*/
	virtual uint64 GetID() const noexcept = 0;

	/**
	* @brief 자산 유형 반환
	* @return 자산 유형
	*/
	virtual ASSET_TYPE GetAssetType() const noexcept = 0;

	/**
	* @brief 로드 상태 반환
	* @return 로드 상태
	*/
	virtual LOAD_STAT GetLoadStat() const noexcept = 0;
};

/**
* @brief 바이너리 자산 인터페이스
*/
interface IBinaryAsset : public IAsset
{
	/**
	* @brief 바이너리 데이터 포인터 반환
	* @return 바이너리 데이터 포인터
	*/
	virtual const char* GetData() const noexcept = 0;

/**
* @brief 바이너리 데이터 크기 반환
* @return 바이너리 데이터 크기 (바이트 단위)
*/
virtual size_t GetSize() const noexcept = 0;

/**
* @brief CRC32 계산
* @return CRC32 값
*/
virtual uint32 ComputeCRC32() noexcept = 0;

/**
* @brief CRC64 계산
* @return CRC64 값
*/
virtual uint64 ComputeCRC64() noexcept = 0;
};


/**
* @brief 텍스트 자산 인터페이스, 문자열 데이터를 다루기 위한 기능 제공, UTF-8 인코딩 사용
*/
interface ITextAsset : public IAsset
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

interface ITextAsset;
interface IBinaryAsset;
interface ITexture1D;
interface ITexture2D;
interface IMesh;
interface ISurfaceMaterial;

/**
* @brief 자산 관리자 인터페이스
*/
interface IAssets : public IUNKNOWN
{
	/**
	* @brief 텍스트 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITextAsset 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITextAsset * *ppOut) = 0;

/**
* @brief 비동기 텍스트 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 ITextAsset 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL LoadAsync(const char* szRelativePath, ITextAsset** ppOut) = 0;

/**
* @brief 바이너리 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 IBinaryAsset 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL Load(const char* szRelativePath, IBinaryAsset** ppOut) = 0;

/**
* @brief 비동기 바이너리 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 IBinaryAsset 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut) = 0;

/**
* @brief 1D 텍스처 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL Load(const char* szRelativePath, ITexture1D** ppOut) = 0;

/**
* @brief 비동기 1D 텍스처 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL LoadAsync(const char* szRelativePath, ITexture1D** ppOut) = 0;

/**
* @brief 2D 텍스처 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL Load(const char* szRelativePath, ITexture2D** ppOut) = 0;

/**
* @brief 비동기 2D 텍스처 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL LoadAsync(const char* szRelativePath, ITexture2D** ppOut) = 0;

/**
* @brief 메쉬 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 IMesh 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL Load(const char* szRelativePath, IMesh** ppOut) = 0;

/**
* @brief 비동기 메쉬 자산 로드
* @param szRelativePath 자산의 상대 경로
* @param ppOut 로드된 IMesh 포인터를 받을 변수의 주소
* @return 로드 성공 시 TRUE, 실패 시 FALSE
*/
virtual BOOL LoadAsync(const char* szRelativePath, IMesh** ppOut) = 0;
};



#endif // __ASSET_DEF_H__
