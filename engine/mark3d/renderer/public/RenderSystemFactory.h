#ifndef __RENDER_SYSTEM_FACTORY_H__
#define __RENDER_SYSTEM_FACTORY_H__


/**
* @brief 렌더링 시스템 생성 함수
* @param CreateDesc 렌더링 시스템 생성 정보
* @param ppRenderSystem 생성된 렌더링 시스템 인터페이스 포인터
* @return 생성 성공 시 TRUE, 실패 시 FALSE 반환
*/
MARKENGINE_API BOOL __stdcall CreateRenderSystem(
	const ENGINE_CREATE_DESC& CreateDesc,
	IRenderSystem** ppRenderSystem
);


#endif // __RENDER_SYSTEM_FACTORY_H__
