#pragma once


namespace mark
{
	class GPUGeometry;

	class GPUGeometryFactory
	{
	public:
		/**
		 * @brief GPU 지오메트리를 생성합니다.
		 * @param CreateDesc GPU 지오메트리 생성에 필요한 정보를 담은 구조체입니다.
		 * @param pGPUGeometry 이미 생성된 GPUGeometry 객체를 전달받아 초기화합니다. 이 포인터는 유효한 GPUGeometry 객체를 가리켜야 합니다.
		 * @return 성공시 true, 실패시 false
		 */
		static bool CreateGeometry(const GPUGeometryCreateDesc& CreateDesc, unknown_ptr<GPUGeometry> pGPUGeometry);

	};
}
