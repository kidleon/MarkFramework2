#ifndef __MARK3D_H__
#define __MARK3D_H__

#include "RenderDef.h"
#include "GeomDef.h"
#include "SceneDef.h"

struct IMark3D : public IUNKNOWN
{
	/**
	* @brief Mark3D를 초기화합니다.
	* @param pCreateDesc 생성 파라미터 구조체 포인터
	*/
	virtual BOOL Initialize(const void* pCreateDesc) = 0;

	/**
	* @brief Mark3D를 종료합니다.
	* @return 없음
	*/
	virtual void Shutdown() = 0;

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


	//----------------------------------------------------------------------
	// SceneObject's APIs

	/**
	* @brief 새로운 Model 객체를 생성합니다.
	* @param ModelName 생성할 Model 이름 해시
	* @param MaxVertex Model이 가질 수 있는 최대 버텍스 개수
	* @param MaxIndex Model이 가질 수 있는 최대 인덱스 개수
	* @param ppOut 생성된 Model 객체의 포인터를 받을 IModel** 포인터
	* @return 성공시 TRUE, 실패시 FALSE
	*/
	virtual BOOL CreateModel(NameHash ModelName, size_t MaxVertex, size_t MaxIndex, IModel** ppOut) = 0;

};


#endif // __MARK3D_H__
