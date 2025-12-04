#ifndef __IFILE_SYSTEM_H__
#define __IFILE_SYSTEM_H__


struct IFileSystem : public IUNKNOWN
{
	virtual HANDLE OpenFile(
		const char* szFilePath, 
		BOOL ReadOnly
	) = 0;

	virtual void CloseFile(
		HANDLE hFile
	) = 0;

	virtual size_t ReadFile(
		HANDLE hFile, 
		void* pBuffer, 
		size_t BufferSize
	) = 0;

	virtual size_t WriteFile(
		HANDLE hFile, 
		const void* pBuffer, 
		size_t BufferSize
	) = 0;

	virtual size_t SeekFile(
		HANDLE hFile, 
		size_t Position, 
		uint32 SeekType
	) noexcept = 0;

	virtual size_t GetSeekPos(HANDLE Handle) noexcept = 0;

	virtual size_t GetFileSize(HANDLE Handle) noexcept = 0;
};


#endif // __IFILE_SYSTEM_H__