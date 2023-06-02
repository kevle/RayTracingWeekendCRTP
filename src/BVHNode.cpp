#include "BVHNode.hpp"

#include "DynamicVector.hpp"
#include "HitableList.hpp"
#include "Variant.hpp"

#include <cassert>
#include <iostream>

BVHNode::BVHNode()
{
}

BVHNode::BVHNode(HitableList& list, int start, int end, float t1, float t2)
    : objects(list.objects.get())
    , left(-1)
    , right(-1)
{
    int axis = 0;
    auto predicate = [&axis](const HittableObjects& lhs, const HittableObjects& rhs)
    {
        AABB box_left, box_right;
        if (!variant::visit(HitableBBVisitor({ 0.f, 0.f, box_left }), lhs) || !variant::visit(HitableBBVisitor({ 0.f, 0.f, box_right }), rhs))
        {
            std::cerr << "No bounding box in bvh_node. Needs special handling\n";
        }
        assert(box_left.is_valid() && box_right.is_valid());
        if (box_left.min()[axis] - box_right.min()[axis] < std::numeric_limits<float>::epsilon())
        {
            return false;
        }
        else
        {
            return true;
        }
    };

    // float maxDist = -1.0f;
    // int worstDir = 0;
    // Figure out direction with largest difference along single axis
    // for (int dir = 0; dir < 3; ++dir)
    //{
    //	axis = dir;
    //	thrust::sort(list.objects.begin() + start, list.objects.begin() + end, predicate);
    //	AABB box_left, box_right;
    //	variant::apply_visitor(HitableBBVisitor({ 0.f, 0.f, box_left }), list.objects[start]);
    //	variant::apply_visitor(HitableBBVisitor({ 0.f, 0.f, box_right }), list.objects[end - 1]);
    //	float dist = box_left.min()[axis] - box_right.min()[axis];
    //	if (dist > maxDist)
    //	{
    //		maxDist = dist;
    //		worstDir = axis;
    //	}
    //}
    // axis = worstDir;
    axis = static_cast<int>(random<float>() * 3);

    std::vector<int> bla(100000, 0);
    std::sort(bla.begin(), bla.end(), [](const int& lhs, const int& rhs)
        { return lhs < rhs; });

    alg::sort(objects->begin() + start, objects->begin() + end, predicate);

    int n = end - start;
    if (n == 1)
    {
        // left = right = &list.objects[start];
        left = right = start;
    }
    else if (n == 2)
    {
        // left = &list.objects[start];
        // right = &list.objects[start + 1];
        left = start;
        right = start + 1;
    }
    else
    {
        BVHNode leftNode(list, start, start + n / 2, t1, t2);
        objects->push_back(leftNode);
        left = static_cast<int>(objects->size() - 1);
        BVHNode rightNode(list, start + n / 2, end, t1, t2);
        objects->push_back(rightNode);
        right = static_cast<int>(objects->size() - 1);
        // left = new HittableObjects;
        //*left = BVHNode(list, start, start + n / 2, t1, t2);
        // right = new HittableObjects;
        //*right = BVHNode(list, start + n / 2, end, t1, t2);
    }

    AABB box_left, box_right;
    if (!variant::visit(HitableBBVisitor({ 0.f, 0.f, box_left }), get_left()) || !variant::visit(HitableBBVisitor({ 0.f, 0.f, box_right }), get_right()))
    {
        std::cerr << "No bounding box in bvh_node. Needs special handling\n";
    }
    box = surrounding_box(box_left, box_right);
}

BVHNode::~BVHNode()
{
}

HittableObjects& BVHNode::get_left()
{
    return (*objects)[left];
}
HittableObjects& BVHNode::get_right()
{
    return (*objects)[right];
}

const HittableObjects& BVHNode::get_left() const
{
    return (*objects)[left];
}
const HittableObjects& BVHNode::get_right() const
{
    return (*objects)[right];
}

bool BVHNode::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
{
    if (box.hit(r, t_min, t_max))
    {
        HitRecord left_rec, right_rec;
        bool hit_left = HitApplier()(get_left(), r, t_min, t_max, left_rec);
        bool hit_right = false;
        if (left != right)
        {
            hit_right = HitApplier()(get_right(), r, t_min, t_max, right_rec);
        }
        if (hit_left && hit_right)
        {
            if (left_rec.t < right_rec.t)
            {
                rec = left_rec;
            }
            else
            {
                rec = right_rec;
            }
            return true;
        }
        else if (hit_left)
        {
            rec = left_rec;
            return true;
        }
        else if (hit_right)
        {
            rec = right_rec;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}