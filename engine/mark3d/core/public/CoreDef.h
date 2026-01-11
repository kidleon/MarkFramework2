#ifndef __CORE_DEF_H__
#define __CORE_DEF_H__

#include "predefine.h"
#include "linked_list.h"
#include "hash_table.h"
#include "unicode.h"


struct IUNKNOWN_IMPL : public IUNKNOWN
{
	IUNKNOWN_IMPL() = default;

	virtual long AddRef() final
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() final
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				OnDestroy();
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

protected:
	virtual ~IUNKNOWN_IMPL() noexcept = default;
	IUNKNOWN_IMPL(const IUNKNOWN_IMPL&) = delete;
	IUNKNOWN_IMPL(IUNKNOWN_IMPL&&) = delete;
	IUNKNOWN_IMPL& operator=(const IUNKNOWN_IMPL&) = delete;
	IUNKNOWN_IMPL& operator=(IUNKNOWN_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

private:
	volatile long m_RefCnt = 1;
#ifdef __TARGET_OS_WINDOWS
	unsigned PADDING_OR_RESERVED = 0;
#endif // __TARGET_OS_WINDOWS

};

//----------------------------------------------------------------
struct IUNKNOWN_HASH_IMPL : public IUNKNOWN
{
	IUNKNOWN_HASH_IMPL() = default;

	virtual long AddRef() final
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() final
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				OnDestroy();
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept { return &m_HashNode; }

protected:
	virtual ~IUNKNOWN_HASH_IMPL() noexcept = default;
	IUNKNOWN_HASH_IMPL(const IUNKNOWN_HASH_IMPL&) = delete;
	IUNKNOWN_HASH_IMPL(IUNKNOWN_HASH_IMPL&&) = delete;
	IUNKNOWN_HASH_IMPL& operator=(const IUNKNOWN_HASH_IMPL&) = delete;
	IUNKNOWN_HASH_IMPL& operator=(IUNKNOWN_HASH_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

private:
	volatile long m_RefCnt = 1;
#ifdef __TARGET_OS_WINDOWS
	unsigned PADDING_OR_RESERVED = 0;
#endif // __TARGET_OS_WINDOWS

	HASH_NODE m_HashNode;

};

//----------------------------------------------------------------
struct IUNKNOWN_LIST_IMPL : public IUNKNOWN
{
	IUNKNOWN_LIST_IMPL() = default;

	virtual long AddRef() final
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() final
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				OnDestroy();
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }

protected:
	virtual ~IUNKNOWN_LIST_IMPL() noexcept = default;
	IUNKNOWN_LIST_IMPL(const IUNKNOWN_LIST_IMPL&) = delete;
	IUNKNOWN_LIST_IMPL(IUNKNOWN_LIST_IMPL&&) = delete;
	IUNKNOWN_LIST_IMPL& operator=(const IUNKNOWN_LIST_IMPL&) = delete;
	IUNKNOWN_LIST_IMPL& operator=(IUNKNOWN_LIST_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

private:
	volatile long m_RefCnt = 1;
#ifdef __TARGET_OS_WINDOWS
	unsigned PADDING_OR_RESERVED = 0;
#endif // __TARGET_OS_WINDOWS

	LINK_NODE m_LinkNode;

};

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
	MESH,
	RENDER_TARGET,
	RENDER_CAMERA,
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
	* @brief 메쉬 데이터 생성
	* @param MaxVertexCount 최대 버텍스 수
	* @param MaxIndexCount 최대 인덱스 수
	* @param ppOut 생성된 IMesh 포인터를 받을 변수의 주소
	* @return 생성 성공 시 TRUE, 실패 시 FALSE
	*/
	virtual BOOL CreateMesh(
		UINT32 m_BufferFormat,
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
		UINT32 m_BufferFormat,
		size_t MaxVertexCount,
		size_t MaxIndexCount,
		IMesh** ppOut
	) = 0;

	virtual BOOL CreateSurfaceMaterial(ISurfaceMaterial** ppOut) = 0;
};



/**
* @brief 데이터 스트림 인터페이스
*/
interface IDataStream : public IUNKNOWN_IMPL
{
public:
	virtual size_t Read(void* pBuffer, size_t BufferSize) = 0;

	virtual size_t Write(const void* pBuffer, size_t BufferSize) = 0;

	virtual bool Seek(size_t Position) = 0;

	virtual size_t Tell() const = 0;

	virtual size_t GetSize() const = 0;

};


struct directory_t;
interface IDataStream;

/**
* @brief 파일 시스템 인터페이스
*/
interface IFileSystem : public IUNKNOWN_IMPL
{
	/**
	* @brief 파일을 엽니다.
	* @param szFilePath 설정된 ROOT PATH의 상대 경로
	* @param ReadOnly 읽기 전용으로 열지 여부
	* @return 파일 핸들, 실패시 NULL
	*/
	virtual IDataStream * OpenFile(
		const char* szRelativePath,
		BOOL ReadOnly
	) = 0;

#ifdef CreateFile
#undef CreateFile
#endif // CreateFile
	/**
	* @brief 파일을 생성합니다. 생성과 동시에 OpenFile과 동일한 상태로 엽니다.
	* @param szRelativePath 설정된 ROOT PATH의 상대 경로
	* @param OverwriteIfExists 파일이 이미 존재할 경우 덮어쓸지 여부
	* @param HeapType 파일 핸들 할당에 사용할 힙 타입
	* @return 생성된 파일 핸들, 실패시 NULL
	*/
	virtual IDataStream* CreateFile(
		const char* szRelativePath,
		BOOL OverwriteIfExists
	) = 0;

#ifdef DeleteFile
#undef DeleteFile
#endif // DeleteFile
	/**
	* @brief 파일을 삭제합니다.
	* @param szRelativePath 설정된 ROOT PATH의 상대 경로
	* @return 없음
	*/
	virtual void DeleteFile(const char* szRelativePath) = 0;

	/**
	* @brief 파일의 전체 크기를 가져옵니다.
	* @param Handle 파일 핸들
	* @return 파일 크기
	*/
	virtual size_t GetFileSize(const char* szRelativePath) noexcept = 0;

	/**
	* @brief 파일의 존재 여부를 확인합니다.
	* @param szRelativePath 설정된 ROOT PATH의 상대 경로
	* @return 존재하면 TRUE, 존재하지 않으면 FALSE
	*/
	virtual BOOL ExistFile(const char* szRelativePath) noexcept = 0;
};

/**
* @brief OS 파일 시스템 생성 함수
*/
MARKENGINE_C_API BOOL CreateOSFileSystem(
	const char* szRootPath,
	IFileSystem** ppFileSystem
);



#endif // __CORE_DEF_H__