#pragma once
#include "internal.h"


namespace mtl
{
	constexpr const char* DEFAULT_ALLOCATOR_NAME = "default_allocator";

	//----------------------------------------------------------------------------------
	/**
	* @brief 시스템의 기본 할당자. mtl의 기본 할당자.
	*/
	class MTL_API allocator
	{
	public:
		explicit allocator(const char* name = DEFAULT_ALLOCATOR_NAME);
		allocator(const allocator& other);
		allocator(const allocator& other, const char* name);

		allocator& operator=(const allocator& other);

		void* allocate(size_t n);
		void* allocate(size_t n, size_t alignment, size_t offset);
		void deallocate(void* p, size_t n);

		const char* get_name() const;
		void set_name(const char* name);

	protected:
		const char* m_name;

	};

	bool operator==(const allocator& a, const allocator& b);
	bool operator!=(const allocator& a, const allocator& b);


	//----------------------------------------------------------------------------------
	class MTL_API dummy_allocator
	{
	public:
		explicit dummy_allocator(const char* name = nullptr) {}
		dummy_allocator(const dummy_allocator& other) {}
		dummy_allocator(const dummy_allocator& other, const char* name) {}

		dummy_allocator& operator=(const dummy_allocator& other) {}

		void* allocate(size_t n) { return nullptr; }
		void* allocate(size_t n, size_t alignment, size_t offset) { return nullptr; }
		void deallocate(void* p, size_t n) {}

		const char* get_name() const { return ""; }
		void set_name(const char* name) {}

	};

	inline bool operator==(const dummy_allocator&, const dummy_allocator&) { return true; }
	inline bool operator!=(const dummy_allocator&, const dummy_allocator&) { return false; }

	MTL_API allocator* get_default_mtlallocator(const allocator*);
	MTL_API void set_default_allocator(allocator* alloc);

}
