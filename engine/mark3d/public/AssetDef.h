#ifndef __ASSET_DEF_H__
#define __ASSET_DEF_H__


/**
* @brief 자산 유형 열거형
*/
enum class ASSET_TYPE : unsigned
{
	UNKNOWN = 0,
	TEXT,
	BINARY,
	SHADER_PROGRAM,
	SURFACE_MATERIAL,
	TEXTURE1D,
	TEXTURE2D,
	TEXTURE3D,
	MESH,
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


#endif // __ASSET_DEF_H__