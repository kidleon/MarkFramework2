#ifndef __OS_FILE_DATA_STREAM_H__
#define __OS_FILE_DATA_STREAM_H__


class OSFileDataStream : public IDataStream
{
public:
	OSFileDataStream(HANDLE hFile);

	virtual size_t Read(void* buffer, size_t size) override;

	virtual size_t Write(const void* buffer, size_t size) override;

	virtual bool Seek(size_t position) override;

	virtual size_t Tell() const override;

	virtual size_t GetSize() const override;

protected:
	virtual ~OSFileDataStream() noexcept;
	virtual void OnDestroy() override;

private:
	HANDLE m_hFile;

};


#endif // __OS_FILE_DATA_STREAM_H__
