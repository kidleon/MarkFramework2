#ifndef __THASH_H__
#define __THASH_H__

#include <cstdint>

namespace mark
{
    typedef size_t HASH;
    typedef uint32_t HASH32;
    typedef uint64_t HASH64;

    constexpr HASH INVALID_HASH = 0x00000000;

    constexpr uint32_t HASH_FNV1_INITIAL_VALUE = 2166136261U;
    constexpr uint64_t HASH_FNV64_INITIAL_VALUE = UINT64_C(14695981039346656037);

    constexpr uint32_t HASH_SDBM_INITIAL_VALUE = 2166136261U;
    constexpr uint64_t HASH_SDBM64_INITIAL_VALUE = UINT64_C(14695981039346656037);


    inline uint32_t HashFunc_FNV1(const void* data, size_t length, uint32_t initialize = HASH_FNV1_INITIAL_VALUE)
    {
        const uint8_t* data8 = (const uint8_t*)data;
        const uint8_t* const data8end = data8 + length;

        while (data8 < data8end)
            initialize = (initialize * 16777619) ^ *data8++;

        return initialize;
    }

    inline uint64_t HashFunc_FNV64(const void* data, size_t length, uint64_t initialize = HASH_FNV64_INITIAL_VALUE)
    {
        const uint8_t* data8 = (const uint8_t*)data;
        const uint8_t* const data8end = data8 + length;

        while (data8 < data8end)
            initialize = (initialize * UINT64_C(1099511628211)) ^ *data8++;

        return initialize;
    }

    inline uint32_t HashFunc_SDBM(const void* data, size_t length, uint32_t initialize = HASH_SDBM_INITIAL_VALUE)
    {
        uint32_t hash = initialize;
        uint8_t* t = (uint8_t*)data;

        for (size_t i = 0; i < length; ++i)
        {
            hash = (*t++) + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }

    inline uint64_t HashFunc_SDBM64(const void* data, size_t length, uint64_t initialize = HASH_SDBM64_INITIAL_VALUE)
    {
        uint64_t hash = initialize;
        uint8_t* t = (uint8_t*)data;

        for (size_t i = 0; i < length; ++i)
        {
            hash = (*t++) + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }

    enum class HASH_FUNC : unsigned
    {
        FNV1 = 0,
        SDBM,
    };

    template<typename _Tk>
    inline HASH _FNV_Hash_Integal(const _Tk& KeyVal) noexcept
    {
        if constexpr (sizeof(HASH) == sizeof(uint64_t))
        {
            HASH hash = HashFunc_FNV64((const void*)(&KeyVal), sizeof(_Tk));
            return hash;
        }
        else
        {
            HASH hash = HashFunc_FNV1((const void*)(&KeyVal), sizeof(_Tk));
            return hash;
        }
    }

    inline HASH _FNV_Hash_Stream(const void* pStream, size_t StreamSize) noexcept
    {
        if constexpr (sizeof(HASH) == sizeof(uint64_t))
        {
            HASH hash = HashFunc_FNV64((const void*)pStream, StreamSize);
            return hash;
        }
        else
        {
            HASH hash = HashFunc_FNV1((const void*)pStream, StreamSize);
            return hash;
        }
    }

    template<typename _Tk>
    inline HASH _SDBM_Hash_Integal(const _Tk& KeyVal) noexcept
    {
        if constexpr (sizeof(HASH) == sizeof(uint64_t))
        {
            HASH hash = HashFunc_SDBM64((const void*)(&KeyVal), sizeof(_Tk));
            return hash;
        }
        else
        {
            HASH hash = HashFunc_SDBM((const void*)(&KeyVal), sizeof(_Tk));
            return hash;
        }
    }

    inline HASH _SDBM_Hash_Stream(const void* pStream, size_t StreamSize) noexcept
    {
        if constexpr (sizeof(HASH) == sizeof(uint64_t))
        {
            HASH hash = HashFunc_SDBM64((const void*)pStream, StreamSize);
            return hash;
        }
        else
        {
            HASH hash = HashFunc_SDBM((const void*)pStream, StreamSize);
            return hash;
        }
    }

    template <class _Tk>
    struct THash_FNV;

    template<typename _Tk, bool Enabled>
    struct __TConditionally_Enabled_Hash_FNV
    {
        inline size_t operator()(const _Tk& KeyVal) const noexcept
        {
            return THash_FNV<_Tk>::DoHash(KeyVal);
        }
    };

    template<typename _Tk>
    struct __TConditionally_Enabled_Hash_FNV<_Tk, false>
    {
        __TConditionally_Enabled_Hash_FNV() = delete;
        __TConditionally_Enabled_Hash_FNV(const __TConditionally_Enabled_Hash_FNV&) = delete;
        __TConditionally_Enabled_Hash_FNV(__TConditionally_Enabled_Hash_FNV&&) = delete;
        __TConditionally_Enabled_Hash_FNV& operator=(const __TConditionally_Enabled_Hash_FNV&) = delete;
        __TConditionally_Enabled_Hash_FNV& operator=(__TConditionally_Enabled_Hash_FNV&&) = delete;
    };

    template<typename _Tk>
    //struct THash_FNV : _TConditionally_Enabled_Hash_FNV<_Tk, !std::is_const_v<_Tk> && !std::is_volatile_v<_Tk> && (std::is_enum_v<_Tk> || std::is_integral_v<_Tk> || std::is_pointer_v<_Tk>)>
    struct THash_FNV : __TConditionally_Enabled_Hash_FNV<_Tk, true>
    {
        static HASH DoHash(const _Tk& KeyVal) noexcept
        {
            return _FNV_Hash_Integal(KeyVal);
        }
    };

    template<>
    struct THash_FNV<float>
    {
        inline HASH operator()(const float KeyVal) const noexcept
        {
            return _FNV_Hash_Integal(KeyVal == 0.0f ? 0.0f : KeyVal); // -0 to 0
        }
    };

    template<>
    struct THash_FNV<double>
    {
        inline HASH operator()(const double KeyVal) const noexcept
        {
            return _FNV_Hash_Integal(KeyVal == 0.0f ? 0.0f : KeyVal); // -0 to 0
        }
    };

    template<>
    struct THash_FNV<long double>
    {
        inline HASH operator()(const long double KeyVal) const noexcept
        {
            return _FNV_Hash_Integal(KeyVal == 0.0f ? 0.0f : KeyVal); // -0 to 0
        }
    };

    template<typename _Tval>
    using THash = THash_FNV<_Tval>;


    //----------------------------------------------------------------------------------------
    template <class _Tk>
    struct THash_SDBM;

    template<typename _Tk, bool _Enabled>
    struct __TConditionally_Enabled_Hash_SDBM
    {
        inline size_t operator()(const _Tk& KeyVal) const noexcept
        {
            return THash_SDBM<_Tk>::DoHash(KeyVal);
        }
    };

    template<typename _Tk>
    struct __TConditionally_Enabled_Hash_SDBM<_Tk, false>
    {
        __TConditionally_Enabled_Hash_SDBM() = delete;
        __TConditionally_Enabled_Hash_SDBM(const __TConditionally_Enabled_Hash_SDBM&) = delete;
        __TConditionally_Enabled_Hash_SDBM(__TConditionally_Enabled_Hash_SDBM&&) = delete;
        __TConditionally_Enabled_Hash_SDBM& operator=(const __TConditionally_Enabled_Hash_SDBM&) = delete;
        __TConditionally_Enabled_Hash_SDBM& operator=(__TConditionally_Enabled_Hash_SDBM&&) = delete;
    };

    template<typename _Tk>
    //struct THash_SDBM : __TConditionally_Enabled_Hash_SDBM<_Tk, !std::is_const_v<_Tk> && !std::is_volatile_v<_Tk> && (std::is_enum_v<_Tk> || std::is_integral_v<_Tk> || std::is_pointer_v<_Tk>)>
    struct THash_SDBM : __TConditionally_Enabled_Hash_SDBM<_Tk, true>
    {
        static HASH DoHash(const _Tk& KeyVal) noexcept
        {
            return _SDBM_Hash_Integal(KeyVal);
        }
    };

    template<>
    struct THash_SDBM<float>
    {
        HASH operator()(const float KeyVal) const noexcept
        {
            return _SDBM_Hash_Integal(KeyVal == 0.0f ? 0.0f : KeyVal); // -0 to 0
        }
    };

    template<>
    struct THash_SDBM<double>
    {
        HASH operator()(const double KeyVal) const noexcept
        {
            return _SDBM_Hash_Integal(KeyVal == 0.0f ? 0.0f : KeyVal); // -0 to 0
        }
    };

    template<>
    struct THash_SDBM<long double>
    {
        HASH operator()(const long double KeyVal) const noexcept
        {
            return _SDBM_Hash_Integal(KeyVal == 0.0f ? 0.0f : KeyVal); // -0 to 0
        }
    };

    //----------------------------------------------------------------------------------------
    template<typename _Tk, typename _TVal>
    struct TPair
    {
        _Tk key;
        _TVal value;
    };

    enum class HASH_TYPE : unsigned
    {
        FNV = 0,
        SDBM
    };
}

#endif // 