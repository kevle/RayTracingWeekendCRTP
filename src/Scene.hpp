#pragma once

#include "BVHNode.hpp"
#include "DynamicVector.hpp"
#include "HitableList.hpp"
#include "HostTexture.hpp"
#include "LoadImage.hpp"
#include "LoadPLY.hpp"
#include "Materials.hpp"
#include "Textures.hpp"
#include "World.hpp"

#include <mutex>

static HitableList random_scene()
{
    dynamic_vector<HittableObjects> objects;

    // HostImageRGB blue_marble = LoadImageFromFile("./world.topo.bathy.200401.3x5400x2700.png");
    // HostTexture tex(blue_marble);

    float world_radius = 1000.f;
    Vector3f world_center = Vector3f(0.f, -world_radius, 0.f);
    objects.push_back(Sphere(world_center, world_radius, Lambertian(Vector3f(0.5f, 0.5f, 0.5f))));
    // objects.push_back(Sphere(Vector3f(0.f, -1000.f, 0.f), 1000.0f, Metal(Vector3f(0.5f, 0.5f, 0.5f), 0.01f)));
    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            float choose_mat = random<float>();
            float radius = 0.2f;
            Vector3f center(a + 0.9f * random<float>(), radius, b + 0.9f * random<float>());

            Vector3f direction = (center - world_center).normalized();
            // Corrected center to that spheres rest on surface
            center = world_center + direction * (world_radius + radius);

            if ((center - Vector3f(4.0f, 0.2f, 0.0f)).length() > 0.9)
            {
                // if (choose_mat < 0.8f)
                if (choose_mat < 0.6f)
                {
                    objects.push_back(Sphere(center, radius, Lambertian(Vector3f(random<float>() * random<float>(), random<float>() * random<float>(), random<float>() * random<float>()))));
                    // objects.push_back(Sphere(center, radius, Lambertian(tex)));
                }
                // else if (choose_mat < 0.95)
                else if (choose_mat < 0.8)
                {
                    objects.push_back(Sphere(center, radius,
                        Metal(Vector3f(0.5f * (1 + random<float>()), 0.5f * (1 + random<float>()), 0.5f * (1 + random<float>())), 0.5f * random<float>())));
                }
                else
                {
                    objects.push_back(Sphere(center, radius, Dielectric(1.5)));
                }
            }
        }
    }

    // objects.push_back(Sphere(Vector3f(-4.f, 1.f, 0.f), 1.0f, Lambertian(FunkyTexture(Vector3f(1.f, 1.f, 0.5f)))));
    // objects.push_back(Sphere(Vector3f(-4.f, 1.f, 0.f), 1.0f, Metal(Vector3f(0.7f, 0.6f, 0.5f), 0.0f)));
    // objects.push_back(Sphere(Vector3f(-4.f, 1.f, 0.f), 1.0f, Lambertian(tex)));
    objects.push_back(Sphere(Vector3f(-4.f, 1.f, 0.f), 1.0f, Lambertian(Vector3f(0.4f, 0.2f, 0.1f))));

    objects.push_back(Sphere(Vector3f(0.f, 1.f, 0.f), 1.0f, Dielectric(1.5f)));
    // objects.push_back(Sphere(Vector3f(0.f, 1.f, 0.f), 1.0f, Lambertian(tex)));
    // objects.push_back(Sphere(Vector3f(0.f, 1.f, 0.f), 1.0f, new Lambertian(Vector3f(2.4f, 1.2f, 1.1f))));

    objects.push_back(Sphere(Vector3f(4.f, 1.f, 0.f), 1.0f, Metal(Vector3f(1.0f, 1.0f, 1.0f), 0.0f)));
    // objects.push_back(Sphere(Vector3f(4.f, 1.f, 0.f), 1.0f, Dielectric(1.5f)));
    // objects.push_back(Sphere(Vector3f(4.f, 1.f, 0.f), 1.0f, Lambertian(tex)));

    return HitableList(objects);
}

static World init_scene()
{
    // std::vector<HittableObjects> objects;
    // objects.push_back(Sphere(Vector3f(0.f, 0.f, -1.f), 0.5f, new Lambertian(Vector3f(0.1f, 0.2f, 0.5f))));
    // objects.push_back(Sphere(Vector3f(0.f, -100.5f, -1.f), 100.f, new Lambertian(Vector3f(0.8f, 0.8f, 0.0f))));
    // objects.push_back(Sphere(Vector3f(1.f, 0.f, -1.f), 0.5f, new Metal(Vector3f(0.8f, 0.6f, 0.2f), 0.1f)));
    ////objects.push_back(Sphere(Vector3f(-1.f, 0.f, -1.f), 0.5f, new Metal(Vector3f(0.8f, 0.8f, 0.8f), 1.0f)));
    // objects.push_back(Sphere(Vector3f(-1.f, 0.f, -1.f), 0.5f, new Dielectric(1.5f)));
    // objects.push_back(Sphere(Vector3f(-1.f, 0.f, -1.f), -0.45f, new Dielectric(1.5f)));

    // float R = std::cos(pi<float>() / 4.0f);
    // objects.push_back(Sphere(Vector3f(-R, 0.0f, -1.0f), R, new Lambertian(Vector3f(0.0f, 0.0f, 1.0f))));
    // objects.push_back(Sphere(Vector3f(R, 0.0f, -1.0f), R, new Lambertian(Vector3f(1.0f, 0.0f, 0.0f))));

    // HitableList world(objects);

    HitableList world = random_scene();
    // HitableList world;

    // world.objects.push_back(Sphere(Vector3f(0.f, -2000.f, 0.f), 2000.0f, Lambertian(Vector3f(0.5f, 0.5f, 0.5f))));

    // HitableList world;

    // world.objects.push_back(Sphere(Vector3f(0.0f, 0.1f, 0.0f), 0.5f, Dielectric(1.7)));

    // Matrix<float, 3, 3> matrix(0.f);
    // Vector3f vec{ 4.f,5.f,6.f };
    // matrix[0] = 1;
    // Vector3f res = matrix * vec;

    // static std::string file_root = R"(C:\)";
    // TriangleData dragon = LoadPLY(file_root + "/data/dragon_recon/dragon_vrip_res4.ply");
    // TriangleData dragon = LoadPLY(file_root + "/data/dragon_recon/dragon_vrip.ply");
    // TriangleData dragon = LoadPLY(file_root + "/data/lucy/lucy.ply");
    //  //
    //  // Lucy
    //  dragon.transform(RotationX<float>(-pi<float>() / 2.f));
    //  dragon.transform(Scale(2.0f));
    //  dragon.transform(RotationY<float>(pi<float>()));
    //  dragon.translate(Vector3f(1.8f, -0.6f, 2.8f));

    //// Dragon
    // dragon.transform(RotationY<float>(pi<float>() * 1.2f));
    // dragon.translate(Vector3f(1.8f, -0.6f, 2.8f));

    // dragon.faces.resize(40);

    // std::vector<Vector3i> faceCopy = std::vector<Vector3i>(dragon.faces.cbegin(), dragon.faces.cend());
    // std::vector<Vector3f> vertexCopy = std::vector<Vector3f>(dragon.vertices.cbegin(), dragon.vertices.cbegin() + 2000);

    // Ray r(Vector3f(0.3f, 0.5f, 0.3f), Vector3f(0.0f, -1.0f, 0.0f));
    // HitRecord rec;
    // triangles.hit(r, 0.f, 100.f, rec);

    // TriangleData data;
    // data.vertices.push_back(Vector3f(0.f, 2.0f, 0.f));
    // data.vertices.push_back(Vector3f(3.f, 0.5f, 0.f));
    // data.vertices.push_back(Vector3f(-3.f, 0.5f, 0.f));
    // data.faces.push_back(Vector3i(0, 1, 2));
    // data.mat = Lambertian(Vector3f(0.0f, 1.0f, 0.0f));

    // dragon.mat = Lambertian(Vector3f(1.f, 1.f, 1.f)); // white
    // dragon.mat = Lambertian(Vector3f(0.33f, 0.33f, 0.33f));
    // dragon.mat = Metal(Vector3f(0.7f, 0.6f, 0.5f), 0.0f);

    // world.objects.push_back(Sphere(Vector3f(0.f, -2000.f, 0.f), 2000.0f, Lambertian(Vector3f(0.5f, 0.5f, 0.5f))));
    // world.objects.push_back(Sphere(Vector3f(0.f, 1.0f, 0.0f), 1.f, Lambertian(Vector3f(1.0f, 0.0f, 0.0f))));

    World res{ world };
    // res.append(dragon);
    res.initialize();
    return res;
}

static World get_default_scene()
{
    return init_scene();
}
