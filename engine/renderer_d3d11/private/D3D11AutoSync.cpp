#include "pch.h"
#include "D3D11AutoSync.h"


D3D11_AUTO_SYNC::D3D11_AUTO_SYNC(struct spin_lock_t* lock) noexcept
	: m_lock(lock)
{
	// Acquire the lock
	acquire_spin_lock(m_lock);
}

D3D11_AUTO_SYNC::~D3D11_AUTO_SYNC() noexcept
{
	// Release the lock
	release_spin_lock(m_lock);
}