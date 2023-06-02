#pragma once

#include <type_traits>

using SizeType = int;

template <typename Derived>
struct ExprBase
{
    constexpr Derived& get_me()
    {
        return *static_cast<Derived*>(this);
    }

    constexpr const Derived& get_me() const
    {
        return *static_cast<const Derived*>(this);
    }
};

// Forward declarations
template <typename Derived>
struct Traits;

// Meta programming helpers
template <typename T>
struct ExpressionStorage
{
    using type = typename std::conditional<Traits<T>::PrimalStorage,
        const T&,
        const T>::type;
};
