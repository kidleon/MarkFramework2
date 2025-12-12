#ifndef __IFILE_SYSTEM_H__
#define __IFILE_SYSTEM_H__

#include "IUnknownImpl.h"


struct directory_t;
interface IDataStream;

interface IFileSystem : public IUNKNOWN_IMPL
{
	/**
	* @brief 파일을 엽니다.
	* @param szFilePath 설정된 ROOT PATH의 상대 경로
	* @param ReadOnly 읽기 전용으로 열지 여부
	* @return 파일 핸들, 실패시 NULL
	*/
	virtual IDataStream* OpenFile(
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

MARKENGINE_C_API BOOL CreateOSFileSystem(
	const char* szRootPath,
	IFileSystem** ppFileSystem
);


#endif // __IFILE_SYSTEM_H__