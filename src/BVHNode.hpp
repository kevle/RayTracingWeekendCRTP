#pragma once

#include "AABB.hpp"
#include "DynamicVector.hpp"
#include "Hitable.hpp"
#include "Variant.hpp"

#include <memory>

struct Sphere;
struct Triangles;
struct BVHNode;

using HittableObjects = variant::variant<Sphere, Triangles, BVHNode>;

struct HitableList;

struct BVHNode : public Hitable<BVHNode>
{
    AABB box;

    dynamic_vector<HittableObjects>* objects = nullptr;

    int left = -1;
    int right = -1;

    BVHNode(const BVHNode&) = default;
    BVHNode();

    BVHNode(HitableList& list, int start, int end, float t1, float t2);

    ~BVHNode();

    bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;
    constexpr bool bounding_box(float, float, AABB& out_box) const
    {
        out_box = this->box;
        return true;
    }

    HittableObjects& get_left();
    HittableObjects& get_right();
    const HittableObjects& get_left() const;
    const HittableObjects& get_right() const;
};
