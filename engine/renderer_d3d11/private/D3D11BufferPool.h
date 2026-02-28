#ifndef __D3D11_BUFFER_POOL_H__
#define __D3D11_BUFFER_POOL_H__


// DEFAULT BUFFER만 일단 지원한다. 나중에 필요하면 다른 Usage도 지원하도록 확장할 수 있다.
class D3D11BufferPool
{
public:
	D3D11BufferPool();
	~D3D11BufferPool();

	ID3D11Buffer* AcquireVB(size_t BufferSize);
	ID3D11Buffer* AcquireIB(size_t BufferSize);

	void ReleaseVB(ID3D11Buffer* pBuffer);
	void ReleaseIB(ID3D11Buffer* pBuffer);

};

#endif // __D3D11_BUFFER_POOL_H__
