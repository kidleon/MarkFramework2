#include "pch.h"
#include "TestBenchCommon.h"

#include <core.h>
#include <memory_block_pool.h>

#include <cstdint>
#include <cstring>
#include <vector>

void run_test_memory_block_pool()
{
	mtl_tb::section("memory_block_pool");

	// 1) 기본 생성/파괴 + invalid 핸들 동작
	{
		HANDLE pool = mark::create_memory_block_pool(sizeof(std::uint64_t), 16);
		MTL_CHECK(pool != nullptr);

		mark::memory_block_id invalid{ 0, 0, 0 };
		MTL_CHECK(mark::is_memory_block_alive(pool, invalid) == false);
		MTL_CHECK(mark::get_memory_block_memory(pool, invalid) == nullptr);

		// generation==0 핸들 release는 no-op이어야 함 (크래시 없음).
		mark::release_memory_block(pool, invalid);

		mark::destroy_memory_block_pool(pool);
	}

	// 2) acquire/release 라이프사이클 + 메모리 쓰기/읽기 검증
	{
		constexpr std::size_t kBlocks = 8;
		HANDLE pool = mark::create_memory_block_pool(sizeof(std::uint64_t), kBlocks);
		MTL_CHECK(pool != nullptr);

		mark::memory_block_id ids[kBlocks];
		for (std::size_t i = 0; i < kBlocks; ++i)
		{
			ids[i] = mark::acquire_memory_block(pool);
			MTL_CHECK(ids[i].generation != 0);
			MTL_CHECK(mark::is_memory_block_alive(pool, ids[i]));

			void* mem = mark::get_memory_block_memory(pool, ids[i]);
			MTL_CHECK(mem != nullptr);
			*static_cast<std::uint64_t*>(mem) = static_cast<std::uint64_t>(i + 1);
		}

		// 모든 슬롯이 고유한지 확인.
		for (std::size_t i = 0; i < kBlocks; ++i)
		{
			void* mi = mark::get_memory_block_memory(pool, ids[i]);
			MTL_CHECK(*static_cast<std::uint64_t*>(mi) == static_cast<std::uint64_t>(i + 1));
			for (std::size_t j = i + 1; j < kBlocks; ++j)
			{
				MTL_CHECK(mi != mark::get_memory_block_memory(pool, ids[j]));
			}
		}

		// 해제 후 stale 핸들이 invalid로 인식되어야 함.
		mark::memory_block_id stale = ids[3];
		mark::release_memory_block(pool, ids[3]);
		MTL_CHECK(mark::is_memory_block_alive(pool, stale) == false);
		MTL_CHECK(mark::get_memory_block_memory(pool, stale) == nullptr);

		// 이중 해제 — no-op 이어야 함.
		mark::release_memory_block(pool, stale);

		// 재획득 시 generation이 달라야 함.
		mark::memory_block_id reacquired = mark::acquire_memory_block(pool);
		MTL_CHECK(reacquired.generation != 0);
		MTL_CHECK(reacquired.generation != stale.generation);
		MTL_CHECK(mark::is_memory_block_alive(pool, reacquired));

		// 정리.
		mark::release_memory_block(pool, reacquired);
		for (std::size_t i = 0; i < kBlocks; ++i)
		{
			if (i == 3) continue;
			mark::release_memory_block(pool, ids[i]);
		}

		mark::destroy_memory_block_pool(pool);
	}

	// 3) 페이지 자동 확장 — blocks_per_page 보다 많이 요청해도 성공.
	{
		constexpr std::size_t kBlocksPerPage = 4;
		constexpr std::size_t kTotal         = kBlocksPerPage * 6; // 6 pages.
		HANDLE pool = mark::create_memory_block_pool(sizeof(std::uint32_t), kBlocksPerPage);
		MTL_CHECK(pool != nullptr);

		std::vector<mark::memory_block_id> ids;
		ids.reserve(kTotal);

		for (std::size_t i = 0; i < kTotal; ++i)
		{
			mark::memory_block_id id = mark::acquire_memory_block(pool);
			MTL_CHECK(id.generation != 0);
			MTL_CHECK(mark::is_memory_block_alive(pool, id));
			ids.push_back(id);
		}

		for (mark::memory_block_id id : ids)
		{
			mark::release_memory_block(pool, id);
		}

		mark::destroy_memory_block_pool(pool);
	}

	// 4) invalid 파라미터 / null handle.
	{
		MTL_CHECK(mark::create_memory_block_pool(0, 8) == nullptr);
		MTL_CHECK(mark::create_memory_block_pool(8, 0) == nullptr);
		MTL_CHECK(mark::create_memory_block_pool(8, 1024) == nullptr); // > MAX_BLOCKS_PER_PAGE

		mark::memory_block_id any = mark::acquire_memory_block(nullptr);
		MTL_CHECK(any.generation == 0);
		MTL_CHECK(mark::is_memory_block_alive(nullptr, any) == false);
		MTL_CHECK(mark::get_memory_block_memory(nullptr, any) == nullptr);

		// null handle / null pool destroy — no-op.
		mark::destroy_memory_block_pool(nullptr);
	}
}
