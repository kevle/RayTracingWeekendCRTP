#pragma once

#include "Math.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <limits>
#include <utility>

// Axis-aligned bounding box
struct AABB
{
    Vector3f b_min{ std::numeric_limits<float>::max() };
    Vector3f b_max{ std::numeric_limits<float>::lowest() };

    AABB() {}
    constexpr AABB(const Vector3f& b_min, const Vector3f& b_max)
        : b_min(b_min)
        , b_max(b_max)
    {
    }

    constexpr bool is_valid() const
    {
        return b_min < b_max;
    }

    constexpr const Vector3f& min() const { return b_min; }
    constexpr const Vector3f& max() const { return b_max; }

    constexpr static inline bool hit(const Ray& r, float t_min, float t_max, const Vector3f& minEdge, const Vector3f& maxEdge)
    {
        for (int d = 0; d < 3; ++d)
        {
            float invD = 1.0f / r.direction()[d];
            float t0 = (minEdge[d] - r.origin()[d]) * invD;
            float t1 = (maxEdge[d] - r.origin()[d]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = ::max(t0, t_min);
            t_max = ::min(t1, t_max);
            if (t_max < t_min)
                return false;
        }
        return true;
    }

    constexpr bool hit(const Ray& r, float t_min, float t_max) const
    {
        return AABB::hit(r, t_min, t_max, min(), max());
    }
};

inline AABB surrounding_box(const AABB& box0, const AABB& box1)
{
    Vector3f small = CVec::min<Vector<float, 3>, Vector<float, 3>>(box0.min(), box1.min());
    Vector3f big = CVec::max<Vector<float, 3>, Vector<float, 3>>(box0.max(), box1.max());
    return AABB(small, big);
}
