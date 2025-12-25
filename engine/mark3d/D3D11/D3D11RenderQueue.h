#ifndef __D3D11_RENDER_QUEUE_H__
#define __D3D11_RENDER_QUEUE_H__


struct BASE_PASS_SORT_KEY
{
	union
	{
		UINT64 Key;
		struct
		{
			UINT64 VertexShaderIndex : 16; // 상위 16비트: 버텍스 셰이더 해시 (65536개)
			UINT64 PixelShaderIndex : 16;  // 다음 16비트: 픽셀 셰이더 해시 (65536개)
			UINT64 BlendMode : 4; // 다음 4비트: 블렌드 모드 (16개)
			UINT64 Distance : 28; // 다음 28비트: 거리 정렬 값 (268435456단계)
		};
	};
};




#endif // __D3D11_RENDER_QUEUE_H__
