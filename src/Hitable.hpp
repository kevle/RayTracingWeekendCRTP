#pragma once

#include "Base.hpp"
#include "Materials.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

struct AABB;

template <typename Derived>
struct Hitable : ExprBase<Derived>
{
    using ExprBase<Derived>::get_me;

    // Returns true iff ray hits the object
    constexpr bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
    {
        return get_me().hit(r, t_min, t_max, rec);
    }
    // Returns true if it has a bounding box and false otherwise
    constexpr bool bounding_box(float t0, float t1, AABB& box) const
    {
        return get_me().bounding_box(t0, t1, box);
    }
};

struct HitableHitVisitor
{
    const Ray& r;
    float t_min;
    float t_max;
    HitRecord& rec;

    template <typename Derived>
    constexpr bool operator()(const Hitable<Derived>& hitable) const
    {
        return hitable.hit(r, t_min, t_max, rec);
    }
};

struct HitableBBVisitor
{
    float t0;
    float t1;
    AABB& box;

    template <typename Derived>
    constexpr bool operator()(const Hitable<Derived>& hitable) const
    {
        return hitable.bounding_box(t0, t1, box);
    }
};
