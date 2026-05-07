#pragma once
#include "GeomDef.h"
#include "object_pool.h"


namespace mark
{
	struct SurfaceMaterialBlock;

	/*
	class SurfaceMaterial final : public ISurfaceMaterial
	{
		static constexpr size_t MAX_PASS = 8;

	public:
		virtual ~SurfaceMaterial() noexcept;

		// IUnknown 인터페이스 구현
		void AddRef();
		void Release();

		// ISurfaceMaterial 인터페이스 구현
		int32_t AddPass(const char* szPassName);
		uint32_t GetNumPasses() const noexcept;
		void SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader);
		void SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader);

	private:
		friend struct object_pool<SurfaceMaterial>;
		friend class SurfaceMaterialFactory;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };

		uint32_t m_NumPasses = 0;
		SurfaceMaterialBlock* m_pBlocks[MAX_PASS] = { nullptr };
	};
	*/
}
