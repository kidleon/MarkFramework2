#include "pch.h"
#include "Texture1D.h"
#include "GlobalVars.h"

#include "idgen.h"


Texture1D::Texture1D(UINT32 ID)
{
	m_ID = ID;
	m_LoadStat = LOAD_STAT::NOT_LOADED;
}

Texture1D::~Texture1D() noexcept
{
	idgen_release(GLOBAL_VARS::ID_GEN_HANDLE, m_ID);
	m_ID = 0;
}
