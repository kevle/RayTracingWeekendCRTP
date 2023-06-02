#pragma once

#include "BVHNode.hpp"
#include "DynamicVector.hpp"
#include "Hitable.hpp"
#include "Sphere.hpp"
#include "Triangles.hpp"
#include "Variant.hpp"

#include <memory>

using HittableObjects = variant::variant<Sphere, Triangles, BVHNode>;

struct HitApplier
{
    bool operator()(const HittableObjects& hitable, const Ray& r, float t_min, float t_max, HitRecord& rec) const
    {
        return variant::visit(HitableHitVisitor({ r, t_min, t_max, rec }), hitable);
    }
};

struct HitableList : Hitable<HitableList>
{
    std::shared_ptr<dynamic_vector<HittableObjects>> objects;

    HitableList() {}
    HitableList(const dynamic_vector<HittableObjects>& objects)
        : objects(std::make_shared<dynamic_vector<HittableObjects>>(objects))
    {
    }
    ~HitableList();

    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
    {
        HitRecord tmp_rec;
        bool hit_sentinel = false;
        float closest = t_max;
        for (const auto& object : *objects)
        {
            if (HitApplier()(object, r, t_min, closest, tmp_rec))
            {
                hit_sentinel = true;
                closest = tmp_rec.t;
                rec = tmp_rec;
            }
        }
        return hit_sentinel;
    }

    bool bounding_box(float t0, float t1, AABB& box) const
    {
        if (objects->size() < 1)
            return false;

        AABB tmp_box;
        // Check if first box hits anything
        if (!variant::visit(HitableBBVisitor({ t0, t1, tmp_box }), (*objects)[0]))
        {
            return false;
        }
        else
        {
            box = tmp_box;
        }
        for (size_t i = 1; i < objects->size(); ++i)
        {
            if (variant::visit(HitableBBVisitor({ t0, t1, tmp_box }), (*objects)[i]))
            {
                box = surrounding_box(box, tmp_box);
            }
            else
            {
                return false;
            }
        }
        return true;
    }
};