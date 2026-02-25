#ifndef __MARK3D_H__
#define __MARK3D_H__

#include "mathlib.h"
#include "RenderDef.h"
#include "CoreDef.h"
#include "AssetDef.h"
#include "GeomDef.h"
#include "SceneDef.h"


struct MARK3D_CREATE_DESC
{
	char szRootPath[256]; // 자산의 루트 경로 (최대 255자 + null terminator)

	RENDER_API RenderAPI; // 렌더링 API
	UINT32 ScreenWidth; // 화면 너비
	UINT32 ScreenHeight; // 화면 높이

	BOOL Fullscreen; // 전체 화면 모드 여부

#if defined(__TARGET_OS_WINDOWS)
	HWND hWnd; // 윈도우 핸들
#endif // __TARGET_OS_WINDOWS
};


struct IMark3D : public IUNKNOWN
{
	/**
	* @brief Mark3D를 초기화합니다.
	* @param pCreateDesc 생성 파라미터 구조체 포인터
	*/
	virtual BOOL Initialize(const MARK3D_CREATE_DESC& CreateDesc) = 0;

	/**
	* @brief Mark3D를 종료합니다.
	* @return 없음
	*/
	virtual void Shutdown() = 0;

	/**
	* @brief CPU에서 업데이트가 필요한 작업을 수행합니다. 예를 들어, 씬 그래프 업데이트, 애니메이션 업데이트 등이 있을 수 있습니다. 반드시 메인쓰레드에서 호출되어야 합니다.
	* @return 없음
	*/
	virtual void UpdateCPU() = 0;

	//----------------------------------------------------------------------
	// Asset's APIs

	/**
	* @brief 자산 관리자 인터페이스를 반환합니다.
	* @param ppOut IAssets 인터페이스 포인터를 받을 변수의 주소
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL GetAssetsInterface(IAssets** ppOut) = 0;

	/**
	* @brief 렌더링 시스템 인터페이스를 반환합니다.
	* @param ppOut IRenderSystem 인터페이스 포인터를 받을 변수의 주소
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL GetRenderSystemInterface(IRenderSystem** ppOut) = 0;

	//----------------------------------------------------------------------
	// Scene's APIs

	/**
	* @brief 새로운 World 객체를 생성합니다. 단 한개의 World 만을 생성할 수 있습니다.
	* @param szWorldName 생성할 World 이름
	* @param ppOut 생성된 World 객체의 포인터를 받을 IWorld** 포인터
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL CreateWorld(const char* szWorldName, IWorld** ppOut) = 0;

	/**
	* @brief 새로운 Scene 객체를 생성합니다.
	* @param pWorld Scene이 속할 IWorld 포인터
	* @param szSceneName 생성할 Scene 이름
	* @param ppOut 생성된 Scene 객체의 포인터를 받을 IScene** 포인터
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL CreateScene(IWorld* pWorld, const char* szSceneName, IScene** ppOut) = 0;

	/**
	* @brief 새로운 SceneNode 객체를 생성합니다.
	* @param pScene SceneNode가 속할 IScene 포인터
	* @param szNodeName 생성할 SceneNode 이름
	* @param ppOut 생성된 SceneNode 객체의 포인터를 받을 ISceneNode** 포인터
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL CreateSceneNode(IScene* pScene, const char* szNodeName, ISceneNode** ppOut) = 0;

	/**
	* @brief SceneNode 객체를 해제합니다.
	* @param pNode 해제할 ISceneNode 포인터
	* @return 없음.
	* @note SceneNode Pool에서 관리하는 SceneNode 객체만 해제할 수 있습니다.
	*/
	virtual void ReleaseSceneNode(ISceneNode* pNode) = 0;

	/**
	* @brief 현재 World 객체를 반환합니다. AddRef가 호출 되므로 반환된 IWorld 포인터는 사용 후 Release 해야 합니다.
	* @return IWorld 포인터
	*/
	virtual IWorld* GetWorld() noexcept = 0;


	//----------------------------------------------------------------------
	// SceneObject's APIs

	/**
	* @brief 새로운 Model 객체를 생성합니다.
	* @param pModelAsset 생성할 Model의 자산 포인터
	* @param ppOut 생성된 Model 객체의 포인터를 받을 IModel** 포인터
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL CreateModel(IModelAsset* pModelAsset, IModel** ppOut) = 0;

};


MARKENGINE_C_API BOOL __stdcall CreateAndInitEngineModule(const MARK3D_CREATE_DESC& CreateDesc, IMark3D** ppMark3D);


#endif // __MARK3D_H__
