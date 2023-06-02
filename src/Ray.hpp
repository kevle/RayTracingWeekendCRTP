#pragma once

#include "Vector.hpp"

struct Ray
{
    Vector3f o;
    Vector3f d;

    Ray() {}

    constexpr Ray(const Vector3f& o, const Vector3f& d)
        : o(o)
        , d(d)
    {
    }
    constexpr const Vector3f& origin() const { return o; }
    constexpr const Vector3f& direction() const { return d; }

    constexpr Vector3f pos(float t) const { return o + t * d; }
};
