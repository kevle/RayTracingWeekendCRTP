#pragma once

#include "Base.hpp"
#include "Ray.hpp"

struct HitRecord;

template <typename Derived>
struct Material : ExprBase<Derived>
{
    using ExprBase<Derived>::get_me;

    bool scatter(const Ray& r_in, const HitRecord& rec, Vector3f& attenuation, Ray& scattered) const
    {
        return get_me().scatter(r_in, rec, attenuation, scattered);
    }
};

struct MaterialVisitor
{
    const Ray& r_in;
    const HitRecord& rec;
    Vector3f& attenuation;
    Ray& scattered;

    template <typename Derived>
    constexpr bool operator()(const Material<Derived>& mat)
    {
        return mat.scatter(r_in, rec, attenuation, scattered);
    }
};

static bool refract(const Vector3f& v, const Vector3f& n, float ni_over_nt, Vector3f& refracted)
{
    Vector3f uv = v.normalized();
    float dt = dot(uv, n);
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminant > 0.f)
    {
        refracted = ni_over_nt * (uv - n * dt) - n * std::sqrt(discriminant);
        return true;
    }
    else
    {
        return false;
    }
}
