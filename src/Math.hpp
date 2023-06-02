#pragma once

#include <cmath>
#include <random>

class RandomGen
{
    RandomGen() = delete;
    thread_local static std::mt19937_64 gen;

public:
    static std::mt19937_64& get()
    {
        return gen;
    }
};

template <typename T>
constexpr T pi()
{
    return T(3.14159265358979323846);
}

template <typename T>
constexpr T tau()
{
    return T(2 * 3.14159265358979323846);
}

inline int div_up(int a, int b)
{
    std::div_t res = std::div(a, b);
    if (res.rem == 0)
    {
        return res.quot;
    }
    else
    {
        return res.quot + 1;
    }
}

// One less instruction than std::min / std::max
// No proper NaN propagation
template <typename T>
T min(T a, T b)
{
    return (a < b ? a : b);
}
template <typename T>
T max(T a, T b)
{
    return (a > b ? a : b);
}

template <typename T>
T random()
{
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    return dist(RandomGen::get());
}
