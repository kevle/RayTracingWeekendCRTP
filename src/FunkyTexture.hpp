#pragma once

#include "HitRecord.hpp"
#include "TextureBase.hpp"

#include <cmath>

struct FunkyTexture : Texture<FunkyTexture>
{
    Vector3f albedo;

    FunkyTexture()
        : albedo(0.f)
    {
    }
    FunkyTexture(Vector3f albedo)
        : albedo(albedo)
    {
    }

    Vector3f value(const HitRecord& rec) const
    {
        return albedo * Vector3f(sin(rec.tex_coords[1] * pi<float>() / 2.0f), sin(rec.tex_coords[0] * pi<float>() / 2.0f), 1.0f);
    }
};
