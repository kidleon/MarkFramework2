#pragma once

#include <cstddef>
#include "internal.h"
#include "allocator_malloc.h"


namespace mtl
{
	// ------------------------------------------------------------------------
	// mallocator — sys_malloc / sys_free 기반의 기본 할당자.
	//
	// - 사실상 무상태(name만 보유). 복사·이동 자유.
	// - 모든 인스턴스가 같은 시스템 힙(mi_malloc)을 공유하므로 operator== 는 항상 true.
	// - 이름(name)은 추적/디버깅용 라벨이며 동등성 비교나 동작에 영향을 주지 않음.
	//
	// 사용:
	//   mtl::mallocator a;                       // 이름 "mallocator"
	//   mtl::mallocator b{"Renderer"};            // 이름 "Renderer"
	//   void* p = a.allocate(1024);
	//   a.deallocate(p);
	// ------------------------------------------------------------------------
	class mallocator final
	{
	public:
		constexpr mallocator() noexcept = default;
		constexpr explicit mallocator(const char* name) noexcept
			: m_name(name ? name : "mallocator")
		{
		}

		void* allocate(std::size_t n) const
		{
			return sys_malloc(n);
		}

		void* allocate(std::size_t n, std::size_t alignment) const
		{
			return sys_malloc(n, alignment);
		}

		void* allocate(std::size_t n, std::size_t alignment, std::size_t offset) const
		{
			return sys_malloc(n, alignment, offset);
		}

		void deallocate(void* p, std::size_t /*n*/ = 0) const noexcept
		{
			sys_free(p);
		}

		constexpr const char* name() const noexcept { return m_name; }
		void                  set_name(const char* name) noexcept { m_name = name ? name : "mallocator"; }

	private:
		const char* m_name = "mallocator";
	};

	inline bool operator==(const mallocator&, const mallocator&) noexcept { return true; }
	inline bool operator!=(const mallocator&, const mallocator&) noexcept { return false; }


	// ------------------------------------------------------------------------
	// dummy_allocator — 항상 nullptr 반환 / 해제는 no-op. 테스트/스텁용.
	// ------------------------------------------------------------------------
	class dummy_allocator final
	{
	public:
		constexpr dummy_allocator() noexcept = default;
		constexpr explicit dummy_allocator(const char*) noexcept {}

		void* allocate(std::size_t)                           const noexcept { return nullptr; }
		void* allocate(std::size_t, std::size_t)              const noexcept { return nullptr; }
		void* allocate(std::size_t, std::size_t, std::size_t) const noexcept { return nullptr; }
		void  deallocate(void*, std::size_t = 0)              const noexcept {}

		constexpr const char* name() const noexcept { return "dummy"; }
		void                  set_name(const char*) noexcept {}
	};

	inline bool operator==(const dummy_allocator&, const dummy_allocator&) noexcept { return true; }
	inline bool operator!=(const dummy_allocator&, const dummy_allocator&) noexcept { return false; }


	// ------------------------------------------------------------------------
	// default_allocator — 컨테이너의 Allocator 템플릿 디폴트 인자로 사용.
	// 새 할당자(linear, stack, pool, …)가 추가돼도 컨테이너 디폴트는 여기로 통일.
	// ------------------------------------------------------------------------
	using default_allocator = mallocator;


	// ------------------------------------------------------------------------
	// 시스템 전역 mallocator 인스턴스. DLL 경계 너머에서도 동일 인스턴스 공유.
	// 이름 설정·조회 외에는 굳이 이걸 거치지 않아도 됨 (mallocator{}로 직접 생성해도 OK).
	// ------------------------------------------------------------------------
	MTL_API mallocator& get_default_allocator() noexcept;
}
