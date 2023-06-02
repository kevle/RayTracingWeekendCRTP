#pragma once

#include "HitRecord.hpp"
#include "Material.hpp"
#include "Textures.hpp"
#include "Variant.hpp"

#include <cmath>

struct Lambertian : public Material<Lambertian>
{
    TexturesType tex;

    Lambertian(const Vector3f& albedo)
        : tex(ConstantTexture(albedo))
    {
    }
    Lambertian(const TexturesType& tex = ConstantTexture())
        : tex(tex)
    {
    }
    bool scatter(const Ray&, const HitRecord& rec, Vector3f& attenuation, Ray& scattered) const
    {
        Vector3f target = rec.p + rec.normal + random_on_unit_sphere();
        scattered = Ray(rec.p, target - rec.p);
        attenuation = TextureApplier()(tex, rec);
        return true;
    }
};

struct Funky : public Material<Funky>
{
    bool scatter(const Ray&, const HitRecord& rec, Vector3f& attenuation, Ray& scattered) const
    {
        Vector3f target = rec.p + rec.normal + random_on_unit_sphere();
        scattered = Ray(rec.p, target - rec.p);
        attenuation = Vector3f(random<float>(), random<float>(), random<float>());
        return true;
    }
};

struct Metal : public Material<Metal>
{
    TexturesType tex;

    float fuzz;

    Metal(const TexturesType& tex, float fuzz = 0.f)
        : tex(tex)
        , fuzz(fuzz)
    {
        if (fuzz >= 1)
            fuzz = 1.f;
    }

    Metal(const Vector3f& albedo, float fuzz = 0.f)
        : Metal(ConstantTexture(albedo), fuzz)
    {
    }

    bool scatter(const Ray& r_in, const HitRecord& rec, Vector3f& attenuation, Ray& scattered) const
    {
        Vector3f reflected = reflect(r_in.direction().normalized(), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_on_unit_sphere());
        attenuation = TextureApplier()(tex, rec);
        return dot(scattered.direction(), rec.normal) > 0;
    }
};

static float schlick(float cosine, float ref_idx)
{
    float r0 = (1.f - ref_idx) / (1.f + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.f - r0) * static_cast<float>(std::pow(1.f - cosine, 5));
}

struct Dielectric : public Material<Dielectric>
{
    float ref_idx;

    Dielectric(float ref_idx)
        : ref_idx(ref_idx)
    {
    }

    bool scatter(const Ray& r_in, const HitRecord& rec, Vector3f& attenuation, Ray& scattered) const
    {
        Vector3f outward_normal;
        Vector3f reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = Vector3f(1.0f, 1.0f, 1.0f);
        Vector3f refracted;
        float reflect_prob;
        float cosine;

        if (dot(r_in.direction(), rec.normal) > 0.f)
        {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else
        {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else
        {
            scattered = Ray(rec.p, reflected);
            reflect_prob = 1.0;
        }
        if (random<float>() < reflect_prob)
        {
            scattered = Ray(rec.p, reflected);
        }
        else
        {
            scattered = Ray(rec.p, refracted);
        }
        return true;
    }
};

using MaterialsType = variant::variant<Lambertian, Funky, Metal, Dielectric>;

struct ScatterApplier
{
    bool operator()(const MaterialsType& mat, const Ray& r_in, const HitRecord& rec, Vector3f& attenuation, Ray& scattered) const
    {
        return variant::visit(MaterialVisitor({ r_in, rec, attenuation, scattered }), mat);
    }
};
