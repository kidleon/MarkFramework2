#ifndef __IASSETS_H__
#define __IASSETS_H__


interface ITextAsset;
interface IBinaryAsset;
interface ITexture1D;
interface ITexture2D;
interface IMesh;

interface IAssets : public IUNKNOWN_IMPL
{
	/**
	* @brief 텍스트 자산 로드
	* @param szRelativePath 자산의 상대 경로
	* @param ppOut 로드된 ITextAsset 포인터를 받을 변수의 주소
	* @return 로드 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL Load(const char* szRelativePath, ITextAsset** ppOut) = 0;

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
	* @brief 메쉬 데이터 생성
	* @param MaxVertexCount 최대 버텍스 수
	* @param MaxIndexCount 최대 인덱스 수
	* @param ppOut 생성된 IMesh 포인터를 받을 변수의 주소
	* @return 생성 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL CreateMesh(
		UINT32 VertexFormat,
		size_t MaxVertexCount, 
		size_t MaxIndexCount, 
		IMesh** ppOut
	) = 0;

	/**
	* @brief 이름 해시를 사용한 메쉬 데이터 생성 (캐싱됨)
	* @param Name 메쉬 데이터의 이름 해시
	* @param MaxVertexCount 최대 버텍스 수
	* @param MaxIndexCount 최대 인덱스 수
	* @param ppOut 생성된 IMesh 포인터를 받을 변수의 주소
	* @return 생성 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL CreateMesh(
		const NameHash& Name,
		UINT32 VertexFormat,
		size_t MaxVertexCount,
		size_t MaxIndexCount,
		IMesh** ppOut
	) = 0;
};


#endif // __IASSETS_H__
