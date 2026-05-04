#pragma once
#include "mathlib.h"
#include "RenderDef.h"
#include "GeomDef.h"


namespace mark
{
	/**
	 * @brief Mark3D 엔진을 생성할 때 필요한 설정을 담는 구조체
	 */
	struct EngineCreateDesc
	{
		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

		GraphicsAPI PreferredGraphicsAPI = GraphicsAPI::D3D11;

#if defined(__TARGET_OS_WINDOWS)
		HWND WindowHandle = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)
	};

	/**
	 * @brief Mark3D 엔진의 주요 인터페이스
	 */
	struct IMark3D : public Unknown
	{
		virtual bool Initialize(const EngineCreateDesc& CreateDesc) = 0;
		virtual void Shutdown() = 0;

		virtual bool GetRenderSystemInterface(IRenderSystem** ppOut) = 0;

		[[nodiscard]] virtual IMesh* CreateMesh(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount) = 0;
		[[nodiscard]] virtual IMesh* LoadMesh(const char* szFilePath) = 0;
		[[nodiscard]] virtual IMesh* LoadMeshAsync(const char* szFilePath) = 0;

	};

	/**
	* @brief Mark3D 엔진을 생성하는 함수입니다.
	* @param CreateDesc 엔진 생성에 필요한 설정을 담은 구조체입니다.
	* @param ppOut 생성된 IMark3D 인터페이스 포인터를 반환할 이중 포인터.
	* @return 성공시 true, 실패시 false
	*/
	[[nodiscard]] MARKENGINE_API bool CreateMark3DEngine(
		const EngineCreateDesc& CreateDesc,
		IMark3D** ppOut
	);

}
