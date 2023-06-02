#pragma once

#include "DynamicVector.hpp"
#include "HitableList.hpp"
#include "Triangles.hpp"

struct World
{
    HitableList hittables;
    std::shared_ptr<TriangleData> triangles = std::make_shared<TriangleData>();
    BVHNode root_node = BVHNode{};

    World(HitableList hittables = HitableList{})
        : hittables(hittables)
    {
    }

    void append(const TriangleData& new_triangles)
    {
        auto old_vertices_count = static_cast<int>(triangles->vertices.size());
        auto old_faces_count = static_cast<int>(triangles->faces.size());

        triangles->mat = new_triangles.mat;
        triangles->faces.insert(triangles->faces.end(), new_triangles.faces.begin(), new_triangles.faces.end());
        triangles->vertices.insert(triangles->vertices.end(), new_triangles.vertices.begin(), new_triangles.vertices.end());

        alg::transform(triangles->faces.begin() + old_faces_count, triangles->faces.end(), triangles->faces.begin() + old_faces_count, [old_vertices_count](const auto& face)
            { return (face + Vector3i{ old_vertices_count }).eval(); });

        int chunk_size = 16;
        const int num_triangles = static_cast<int>(triangles->faces.size());
        for (int start = old_faces_count; start < num_triangles; start += chunk_size)
        {
            int end = start + min(chunk_size, num_triangles - start);
            Triangles raw_triangles(*triangles, start, end);
            hittables.objects->push_back(raw_triangles);
        }
    }

    void append(const dynamic_vector<HittableObjects>& objects)
    {
        hittables.objects->insert(hittables.objects->end(), objects.cbegin(), objects.cend());
    }

    void initialize()
    {
        root_node = BVHNode(hittables, 0, static_cast<int>(hittables.objects->size()), 0, 0);
    }
};
