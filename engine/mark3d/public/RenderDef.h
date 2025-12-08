#ifndef __RENDER_DEF_H__
#define __RENDER_DEF_H__


/**
* @brief 렌더링 API 열거형
*/
enum class RENDER_API : UINT32
{
	UNKNOWN = 0, // 알 수 없음
	DIRECT3D11 = 1, // Direct3D 11
	DIRECT3D12 = 2, // Direct3D 12
	VULKAN = 3, // Vulkan
	OPENGL = 4, // OpenGL
	METAL = 5, // Metal
};

/**
* @brief 3D 엔진 생성 정보 구조체
* @remark CreateEngine 함수에 전달되는 구조체
* @note Width : 화면 너비
* @note Height : 화면 높이
* @note Fullscreen : 전체 화면 모드 여부
*/
struct ENGINE_CREATE_DESC
{
	UINT32 ScreenWidth; // 화면 너비
	UINT32 ScreenHeight; // 화면 높이
	LOG_LEVEL MinLogLevel; // 최소 로그 레벨
	BOOL Fullscreen; // 전체 화면 모드 여부
};

#endif // __RENDER_DEF_H__