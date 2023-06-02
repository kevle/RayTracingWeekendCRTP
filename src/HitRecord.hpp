#pragma once

#include "Variant.hpp"
#include "Vector.hpp"

// Forward declarations for materials
// namespace variant
// {
// 	template<typename ...Args>
// 	class variant;
// }

struct Lambertian;
struct Funky;
struct Metal;
struct Dielectric;

using MaterialsType = variant::variant<Lambertian, Funky, Metal, Dielectric>;

struct HitRecord
{
    float t = 0.f;
    Vector3f p;
    Vector3f normal;
    const MaterialsType* mat_ptr = nullptr;
    Vector<float, 2> tex_coords;
};
