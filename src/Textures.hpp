#pragma once

#include "ConstantTexture.hpp"
#include "FunkyTexture.hpp"
#include "HostTexture.hpp"
#include "Variant.hpp"
// ... more texture types

using TexturesType = variant::variant<ConstantTexture, FunkyTexture, HostTexture>;

struct TextureApplier
{
    Vector3f operator()(const TexturesType& tex, const HitRecord& rec) const
    {
        return variant::visit(TextureVisitor({ rec }), tex);
    }
};
