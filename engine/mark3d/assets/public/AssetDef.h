#ifndef __ASSET_DEF_H__
#define __ASSET_DEF_H__


#define MAX_FILE_LENGTH 256

/**
* @brief 자산 유형 열거형
*/
enum class ASSET_TYPE : unsigned
{
	UNKNOWN = 0,
	TEXT_ASSET,
	BINARY_ASSET,
	MODEL_ASSET,
	MODEL
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

	/**
	* @brief 자산 상대 경로 반환
	* @param IgnoreFileName 파일 이름을 무시할지 여부, TRUE일 경우 파일 이름을 제외한 경로만 반환, FALSE일 경우 전체 상대 경로 반환
	* @return 자산 상대 경로 문자열 포인터
	*/
	virtual BOOL GetRelativePath(char* szBuffer, size_t BufferLen, BOOL IgnoreFileName) const noexcept = 0;
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


enum class MODEL_ATTRIB : uint32
{
	MESH = 0x00000001,
	MATERIAL = 0x00000002,
	ANIMATION = 0x00000004,
};

/**
* @brief 모델 자산 인터페이스, 3D 모델 데이터를 다루기 위한 기능 제공
* @note 모델 자산은 하나 이상의 메쉬로 구성될 수 있으며, 각 메쉬는 여러 서브메쉬로 나뉠 수 있음
* @note 각 메쉬는 위치, 법선, 텍스처 좌표, 색상, 탄젠트, 바이노멀 등의 버텍스 속성을 가질 수 있으며, 인덱스 버퍼를 통해 서브메쉬를 정의할 수 있음
* @note 모델 자산은 모델링 소프트웨어에서 익스포트된 데이터를 기반으로 하며, 다양한 파일 형식을 지원할 수 있음 (예: OBJ, FBX, GLTF 등)
* @note 모델 자산은 애니메이션 데이터를 포함할 수 있으며, 애니메이션은 스켈레탈 애니메이션과 모프 타겟 애니메이션으로 나뉠 수 있음
* @note 모델 자산은 머티리얼 정보를 포함할 수 있으며, 머티리얼은 텍스처, 셰이더, 렌더링 상태 등의 정보를 가질 수 있음
* @note 모델 자산은 로딩 시점에 필요한 버텍스 속성만 로드할 수 있으며, GetModelAttrib() 함수를 통해 현재 로드된 속성을 확인할 수 있음
*/
struct IModelAsset : public IAsset
{
	/**
	* @brief 모델 속성 반환
	* @return 모델 속성
	* @note 반환 값은 MODEL_ATTRIB 열거형 값의 조합임
	*/
	virtual UINT32 GetModelAttrib() const noexcept = 0;

	/**
	* @brief 메쉬 개수 반환
	* @return 메쉬 개수
	*/
	virtual size_t GetNumMesh() const noexcept = 0;

	/**
	* @brief 메쉬 이름 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 메쉬 이름 문자열 포인터
	*/
	virtual const char* GetMeshName(int32 MeshIndex) const noexcept = 0;

	/**
	* @brief 서브메쉬 개수 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 서브메쉬 개수
	*/
	virtual size_t GetNumSubMesh(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 버텍스 개수 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 위치 개수
	*/
	virtual size_t GetNumVertices(int32 MeshIndex) const noexcept = 0;

	/**
	* @brief 버텍스 위치 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 위치 배열 포인터
	* @note 반환 값은 FLOAT3 배열 포인터임
	* @note GetNumPositions() 함수를 통해 배열 크기를 확인할 수 있음
	*/
	virtual FLOAT3* GetPositions(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 버텍스 법선 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 법선 배열 포인터
	*/
	virtual FLOAT3* GetNormals(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 버텍스 텍스처 좌표 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 텍스처 좌표 배열 포인터
	*/
	virtual FLOAT2* GetTexCoords(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 버텍스 색상 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 색상 배열 포인터
	*/
	virtual FLOAT4* GetColor(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 버텍스 탄젠트 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 탄젠트 배열 포인터
	*/
	virtual FLOAT3* GetTangent(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 버텍스 바이노멀 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 버텍스 바이노멀 배열 포인터
	*/
	virtual FLOAT3* GetBinormal(int32 MeshIndex) noexcept = 0;

	/**
	* @brief 인덱스 배열 반환
	* @param MeshIndex 메쉬 인덱스
	* @param SubMeshIndex 서브메쉬 인덱스
	* @return 인덱스 배열 포인터
	*/
	virtual uint32* GetIndices(int32 MeshIndex, int32 SubMeshIndex) noexcept = 0;

	/**
	* @brief 인덱스 개수 반환
	* @param MeshIndex 메쉬 인덱스
	* @return 인덱스 개수
	*/
	virtual size_t GetNumIndices(int32 MeshIndex) const noexcept = 0;

	/**
	* @brief 인덱스 개수 반환
	* @param MeshIndex 메쉬 인덱스
	* @param SubMeshIndex 서브메쉬 인덱스
	* @return 인덱스 개수
	*/
	virtual size_t GetNumIndices(int32 MeshIndex, int32 SubMeshIndex) const noexcept = 0;

	/**
	* @brief 서브메쉬에 적용된 머티리얼 Index 반환
	* @param MeshIndex 메쉬 인덱스
	* @param SubMeshIndex 서브메쉬 인덱스
	* @return 머티리얼 Index
	*/
	virtual int32 GetMaterialIndex(int32 MeshIndex, int32 SubMeshIndex) const noexcept = 0;

	/**
	* @brief 머티리얼 개수 반환
	* @return 머티리얼 개수
	*/
	virtual size_t GetNumMaterials() const noexcept = 0;

	/**
	* @brief 서브메쉬에 적용된 머티리얼이 디퓨즈 텍스처를 가지고 있는지 여부 반환
	* @param MaterialID 머티리얼 ID
	* @return 디퓨즈 텍스처 존재 여부
	*/
	virtual BOOL HasDiffuseTexture(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 서브메쉬에 적용된 머티리얼이 노멀 텍스처를 가지고 있는지 여부 반환
	* @param MaterialID 머티리얼 ID
	* @return 노멀 텍스처 존재 여부
	*/
	virtual BOOL HasNormalTexture(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 서브메쉬에 적용된 머티리얼이 색상을 가지고 있는지 여부 반환
	* @param MaterialID 머티리얼 ID
	* @return 색상 존재 여부
	*/
	virtual BOOL HasColor(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 서브메쉬에 적용된 머티리얼이 스페큘러 텍스처를 가지고 있는지 여부 반환
	* @param MaterialID 머티리얼 ID
	* @return 스페큘러 텍스처 존재 여부
	*/
	virtual BOOL HasSpecularTexture(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 서브메쉬에 적용된 머티리얼이 발광 텍스처를 가지고 있는지 여부 반환
	* @param MaterialID 머티리얼 ID
	* @return 발광 텍스처 존재 여부
	*/
	virtual BOOL HasEmissiveTexture(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 머티리얼 디퓨즈 텍스처 경로 반환
	* @param MaterialID 머티리얼 ID
	* @return 디퓨즈 텍스처 경로 문자열 포인터
	*/
	virtual const char* GetMaterialDiffuse(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 머티리얼 노멀 텍스처 경로 반환
	* @param MaterialID 머티리얼 ID
	* @return 노멀 텍스처 경로 문자열 포인터
	*/
	virtual const char* GetMaterialNormal(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 머티리얼 스페큘러 텍스처 경로 반환
	* @param MaterialID 머티리얼 ID
	* @return 스페큘러 텍스처 경로 문자열 포인터
	*/
	virtual const char* GetMaterialSpecular(int32 MaterialID) const noexcept = 0;

	/**
	* @brief 머티리얼 발광 텍스처 경로 반환
	* @param MaterialID 머티리얼 ID
	* @return 발광 텍스처 경로 문자열 포인터
	*/
	virtual const char* GetMaterialEmissive(int32 MaterialID) const noexcept = 0;
	
	/**
	* @brief 머티리얼 색상 반환
	* @param MaterialID 머티리얼 ID
	* @return 머티리얼 색상
	*/
	virtual FLOAT4 GetMaterialColor(int32 MaterialID) const noexcept = 0;

};

/**
* @brief 자산 관리자 인터페이스
*/
interface IAssets : public IPRIVATE_UNKNOWN
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
	* @brief 메쉬 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 IMesh 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, IModelAsset** ppOut) = 0;

	/**
	* @brief 비동기 메쉬 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 IMesh 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, IModelAsset** ppOut) = 0;

	/**
	* @brief 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITexture1D** ppOut) = 0;

	/**
	* @brief 비동기 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture1D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture1D** ppOut) = 0;

	/**
	* @brief 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture2D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITexture2D** ppOut) = 0;

	/**
	* @brief 비동기 텍스처 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITexture2D 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture2D** ppOut) = 0;
};


#endif // __ASSET_DEF_H__
