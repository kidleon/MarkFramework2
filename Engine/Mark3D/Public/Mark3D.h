#pragma once
#include "mathlib.h"
#include "name_hash.h"
#include "RenderDef.h"
#include "AssetDef.h"
#include "GeomDef.h"


namespace mark
{
	/**
	 * @brief Mark3D 엔진을 생성할 때 필요한 설정을 담는 구조체
	 */
	struct MARKENGINE_API EngineCreateDesc
	{
		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

		GRAPHICS_API PreferredGraphicsAPI = GRAPHICS_API::D3D11;

		char szAssetRootPath[_MAX_PATH] = { 0 };

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

		/**
		 * @brief RenderSystem 인터페이스를 출력 인자에 반환합니다.
		 * @param ppOut [Owning out-param] *ppOut은 AddRef된 상태로 반환됩니다.
		 *              호출자가 Release()를 책임지거나 unknown_ptr<IRenderSystem>::attach()로 받으세요.
		 * @return ppOut이 nullptr이면 false, 그 외 true (RenderSystem이 미초기화면 *ppOut에 nullptr).
		 */
		virtual bool GetRenderSystemInterface(IRenderSystem** ppOut) = 0;

		/**
		 * @brief AssetManager 인터페이스를 출력 인자에 반환합니다.
		 * @param ppOut [Owning out-param] *ppOut은 AddRef된 상태로 반환됩니다.
		 *              호출자가 Release()를 책임지거나 unknown_ptr<IAssetManager>::attach()로 받으세요.
		 * @return ppOut이 nullptr이면 false, 그 외 true (AssetManager가 미초기화면 *ppOut에 nullptr).
		 */
		virtual bool GetAssetManagerInterface(IAssetManager** ppOut) = 0;

		/**
		 * @brief GPU 지오메트리를 생성합니다.
		 * @return [Owning] 반환된 포인터는 AddRef된 상태입니다. 호출자가 Release()를 책임지거나
		 *         unknown_ptr<IGPUGeometry>::attach()로 takeover 하세요. 실패 시 nullptr.
		 */
		[[nodiscard]] virtual IGPUGeometry* CreateGPUGeometry(const GPUGeometryCreateDesc& CreateDesc) = 0;

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

	/**
	 * @brief Mark3D 엔진을 파괴하는 함수입니다. pEngine->Release()를 호출하여 삭제하면 안되고 반드시 이 함수를 통해 삭제해야 합니다.
	 * @param pEngine 파괴할 Mark3D 엔진 인터페이스 포인터입니다.
	 * @return 없음
	 */
	MARKENGINE_API void DestroyMark3DEngine(IMark3D* pEngine);

}
