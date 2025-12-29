#ifndef __D3D11_AUTO_SYNC_H__
#define __D3D11_AUTO_SYNC_H__


struct spin_lock_t;

class D3D11_AUTO_SYNC
{
public:
	D3D11_AUTO_SYNC(struct spin_lock_t* lock) noexcept;
	~D3D11_AUTO_SYNC() noexcept;

private:
	D3D11_AUTO_SYNC() = delete;
	D3D11_AUTO_SYNC(const D3D11_AUTO_SYNC&) = delete;
	D3D11_AUTO_SYNC& operator=(const D3D11_AUTO_SYNC&) = delete;

private:
	struct spin_lock_t* m_lock;

};


#endif // __D3D11_AUTO_SYNC_H__
