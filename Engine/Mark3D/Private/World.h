#pragma once
#include "Scene.h"


namespace mark
{
	class World final : public IWorld
	{
	public:
		World() noexcept = default;
		~World() noexcept;

		void AddRef() override;
		void Release() override;

		void BigBang(const char* Name) noexcept override;
		void BigRip() noexcept override;

		[[nodiscard]] IScene* CreateScene(const char* Name) noexcept override;
		[[nodiscard]] IScene* LoadScene(const char* FilePath, bool Additive, bool Async) noexcept override;
		void DestroyScene(const char* Name) noexcept override;
		[[nodiscard]] IScene* GetScene(const char* Name) const noexcept override;

		[[nodiscard]] inline const char* INL_GetName() const noexcept { return m_Name; }

	private:
		std::atomic<int64_t> m_RefCount{ 1 };
		char m_Name[64] = { 0 };
		sys_vector<Scene*> m_lstScenes;

	};
}
