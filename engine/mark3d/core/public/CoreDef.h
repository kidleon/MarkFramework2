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