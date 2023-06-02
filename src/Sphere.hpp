#pragma once

#include "AABB.hpp"
#include "Hitable.hpp"
#include "Materials.hpp"

struct Sphere : Hitable<Sphere>
{
    Vector3f center;
    float radius;
    MaterialsType mat;

    Sphere()
        : radius(0.f)
    {
    }
    Sphere(Vector3f center, float radius, MaterialsType mat)
        : center(center)
        , radius(radius)
        , mat(mat)
    {
    }

    constexpr bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
    {
        Vector3f oc = r.origin() - center;
        const float a = dot(r.direction(), r.direction());
        const float b = dot(oc, r.direction());
        const float c = dot(oc, oc) - radius * radius;
        const float discriminant = b * b - a * c;
        if (discriminant > 0)
        {
            float discSqrt = std::sqrt(discriminant);
            float temp = (-b - discSqrt) / a;
            auto conditionalUpdate = [&, this]()
            {
                if (temp < t_max && temp > t_min)
                {
                    rec.t = temp;
                    rec.p = r.pos(rec.t);
                    rec.normal = (rec.p - center) / radius;
                    rec.mat_ptr = &mat;
                    // https://gamedev.stackexchange.com/questions/114412/how-to-get-uv-coordinates-for-sphere-cylindrical-projection
                    Vector<float, 2> tmp = Vector<float, 2>(0.0f, 1.f) - Vector<float, 2>(-(atan2(rec.normal.x(), rec.normal.z()) / (tau<float>()) + 0.5f), rec.normal.y() * 0.5f + 0.5f);
                    rec.tex_coords = Vector<float, 2>(sin(tmp[0] * pi<float>() * 0.5f) + 0.5f, tmp[1]);
                    return true;
                }
                return false;
            };
            if (conditionalUpdate())
                return true;
            temp = (-b + discSqrt) / a;
            if (conditionalUpdate())
                return true;
        }
        return false;
    }

    constexpr bool bounding_box(float, float, AABB& box) const
    {
        box = AABB(center - Vector3f(radius), center + Vector3f(radius));
        return true;
    }
};