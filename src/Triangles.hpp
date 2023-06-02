#pragma once

#include "AABB.hpp"
#include "DynamicVector.hpp"
#include "Hitable.hpp"
#include "Material.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

#include <limits>

struct TriangleData
{
    dynamic_vector<Vector3f> vertices;
    dynamic_vector<Vector3i> faces;
    MaterialsType mat;

    void transform(const Matrix<float, 3, 3>& m)
    {
        alg::transform(vertices.begin(), vertices.end(), vertices.begin(), [&m](const Vector3f& v)
            { return m * v; });
    }

    void translate(const Vector3f& t)
    {
        alg::transform(vertices.begin(), vertices.end(), vertices.begin(), [&t](const Vector3f& v)
            { return v + t; });
    }
};

struct Triangle
{
    const Vector3f& v0;
    const Vector3f& v1;
    const Vector3f& v2;

    Triangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2)
        : v0(v0)
        , v1(v1)
        , v2(v2)
    {
    }

    // Möller-Trumbore check
    constexpr bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec, MaterialsType& mat) const
    {
        // Vector3f min_edge = CVec::min(CVec::min(v0, v1), v2);
        // Vector3f max_edge = CVec::max(CVec::max(v0, v1), v2);
        // if (!AABB::hit(r, t_min, t_max, min_edge, max_edge))
        // {
        //	return false;
        // }

        constexpr float epsilon = 1e-7f;
        Vector3f e1 = v1 - v0;
        Vector3f e2 = v2 - v0;
        Vector3f h = cross(r.direction(), e2);
        float a = dot(e1, h);
        if (a > -epsilon && a < epsilon)
        {
            return false; // ray parallel
        }
        float f = 1.f / a;
        Vector3f s = r.origin() - v0;
        float u = f * dot(s, h);
        if (u < 0.0f || u > 1.0f)
        {
            return false;
        }
        Vector3f q = cross(s, e1);
        float v = f * dot(r.direction(), q);
        if (v < 0.f || (u + v > 1.f))
        {
            return false;
        }
        float t = f * dot(e2, q);
        if (t > epsilon && t > t_min && t < t_max)
        {
            rec.t = t;
            rec.p = r.origin() + r.direction() * t;
            rec.normal = cross(e1, e2).normalized();
            rec.mat_ptr = &mat;
            rec.tex_coords = Vector<float, 2>(0.f);
            return true;
        }
        return false;
    }
};

struct Triangles : Hitable<Triangles>
{
    TriangleData* data;
    int num_triangles;

    int start;
    int end;

    Triangles(TriangleData& data, int start, int end)
        : data(&data)
        , num_triangles(end - start)
        , start(start)
        , end(end)
    {
    }
    constexpr bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
    {
        bool has_hit = false;
        for (int t = start; t < end; ++t)
        {
            Vector3i vertice_ids = data->faces[t];
            if (Triangle({ data->vertices[vertice_ids[0]], data->vertices[vertice_ids[1]], data->vertices[vertice_ids[2]] }).hit(r, t_min, t_max, rec, data->mat))
            {
                has_hit = true;
            }
        }
        return has_hit;
    }

    constexpr bool bounding_box(float, float, AABB& box) const
    {
        Vector3f min_verts(std::numeric_limits<float>::max());
        Vector3f max_verts(-std::numeric_limits<float>::max());
        for (int t = start; t < end; ++t)
        {
            Vector3i vertice_ids = data->faces[t];
            for (int verts = 0; verts < 3; ++verts)
            {
                int vert_id = vertice_ids[verts];
                Vector3f vertice = data->vertices[vert_id];
                min_verts = CVec::min(min_verts, vertice);
                max_verts = CVec::max(max_verts, vertice);
            }
        }
        box = AABB(min_verts, max_verts);
        return true;
    }
};