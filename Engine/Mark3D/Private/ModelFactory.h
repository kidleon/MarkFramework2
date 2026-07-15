#pragma once


namespace mark
{
	class RenderSystem;
	class Model;
	class ModelAsset;

	class ModelFactory
	{
	public:
		/**
		 * @brief ModelAsset 로부터 Model 을 생성합니다.
		 * @param pRenderSystem GPU 버퍼 생성에 사용할 RenderSystem 인스턴스입니다.
		 * @param pModelAsset   기하/서브메쉬 정보를 담은 로드된 ModelAsset 입니다.
		 * @param Layout        MERGED(단일 병합 PrimitiveBuffer) 또는 SEPARATE(메쉬별 독립 PrimitiveBuffer) 레이아웃.
		 * @return 성공 시 참조 카운트 1 상태의 Model 포인터, 실패 시 nullptr.
		 */
		static Model* CreateModel(
			RenderSystem* pRenderSystem,
			ModelAsset* pModelAsset,
			MODEL_LAYOUT Layout = MODEL_LAYOUT::MERGED
		);
	};
}
