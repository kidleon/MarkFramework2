#pragma once
#include "mathlib.h"
#include "RenderDef.h"


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

	struct IModel : public Unknown
	{
		// 모델 관련 인터페이스 메서드 선언
		virtual int32_t AddPrimitive(uint32_t VertexCount, uint32_t IndexCount) = 0;
		virtual int32_t AddPrimitive(uint32_t VertexCount, uint32_t* IndexCountArray, size_t NumIndexCountArray) = 0;

		virtual bool UpdateVertex(VERTEX_FORMAT VertexFormat, const void* pVertexData, size_t DataSize) = 0;
		virtual bool UpdateIndex(const void* pIndexData, size_t DataSize) = 0;

	};

	struct ISurfaceMaterial : public Unknown
	{
		// 표면 재질 관련 인터페이스 메서드 선언
		virtual bool SetDiffuseColor(float r, float g, float b, float a) = 0;
		virtual bool SetSpecularColor(float r, float g, float b, float a) = 0;
		virtual bool SetDiffuseColor(const FLOAT4& Color) = 0;
		virtual bool SetSpecularColor(const FLOAT4& Color) = 0;

	};

	/**
	 * @brief Mark3D 엔진의 주요 인터페이스
	 */
	struct IMark3D : public Unknown
	{
		virtual bool Initialize(const EngineCreateDesc& CreateDesc) = 0;
		virtual void Shutdown() = 0;

		virtual bool GetRenderSystemInterface(IRenderSystem** ppOut) = 0;

		virtual IModel* CreateModel(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount) = 0;

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
