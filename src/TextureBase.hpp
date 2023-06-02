#pragma once

#include "Base.hpp"
#include "Vector.hpp"

struct HitRecord;

template <typename Derived>
struct Texture : ExprBase<Derived>
{
    Vector3f value(const HitRecord& rec) const
    {
        return ExprBase<Derived>::get_me().value(rec);
    }
};

struct TextureVisitor
{
    const HitRecord& rec;

    template <typename Derived>
    constexpr Vector3f operator()(const Texture<Derived>& tex)
    {
        return tex.value(rec);
    }
};
