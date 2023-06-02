#pragma once

#include "HitRecord.hpp"
#include "HostImageRGB.hpp"
#include "TextureBase.hpp"

struct HostTexture : Texture<HostTexture>
{
    HostImageRGB tex;

    HostTexture() {}
    HostTexture(const HostImageRGB& tex)
        : tex(tex)
    {
    }

    Vector3f value(const HitRecord& rec) const
    {
        return tex.get(rec.tex_coords[0], rec.tex_coords[1]);
    }
};
