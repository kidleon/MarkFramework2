#ifndef __GENERIC_COMMON_H__
#define __GENERIC_COMMON_H__

#include <cstddef>
#include <type_traits>


namespace mark
{
    template <typename U, typename... Args>
    inline void construct(U* p, Args&&... args)
    {
        ::new ((void*)p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    inline void destroy(U* p)
    {
        p->~U();
    }

	template <typename T>
    inline std::size_t max_size() noexcept 
    {
        return std::size_t(-1) / sizeof(T);
    }
}

#endif // __GENERIC_COMMON_H__
