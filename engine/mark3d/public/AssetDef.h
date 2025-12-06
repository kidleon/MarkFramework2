#ifndef __ASET_DEF_H__
#define __ASET_DEF_H__


enum class ASSET_TYPE : unsigned
{
	UNKNOWN = 0,
	TEXT,
	BINARY,
	TEXTURE1D,
	TEXTURE2D,
	TEXTURE3D,
	MESH,
	AUDIO,
};

enum class LOAD_STAT : unsigned
{
	NOT_LOADED = 0,
	LOADING,
	LOADED,
	FAILED,
};


#endif // __ASET_DEF_H__