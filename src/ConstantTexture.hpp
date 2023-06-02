#pragma once

#include "TextureBase.hpp"

struct ConstantTexture : Texture<ConstantTexture>
{
    Vector3f albedo;

    ConstantTexture()
        : albedo(0.f)
    {
    }
    ConstantTexture(Vector3f albedo)
        : albedo(albedo)
    {
    }

    Vector3f value(const HitRecord&) const
    {
        return albedo;
    }
};
