#pragma once

#include "Camera.hpp"
#include "DynamicVector.hpp"
#include "Matrix.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "World.hpp"

#include <limits>
#include <mutex>
#include <thread>

template <typename Derived>
Vector3f color(const Ray& r, const Hitable<Derived>& world, int num_jumps)
{
    HitRecord rec;
    Ray currentRay = r;
    Vector3f attenuation(1.f);
    Vector3f attenuationLocal(1.f);
    for (int i = 0; i < num_jumps; ++i)
    {
        if (world.hit(currentRay, 0.001f, std::numeric_limits<float>::max(), rec))
        {
            Ray scattered;

            if (ScatterApplier()(*rec.mat_ptr, currentRay, rec, attenuationLocal, scattered))
            {
                attenuation *= attenuationLocal;
                currentRay = scattered;
            }
            else
            {
                attenuation *= Vector3f(0.f);
            }
        }
        else
        {
            Vector3f dir = currentRay.direction().normalized();
            float t = 0.5f * (dir.y() + 1.0f);
            attenuation *= ((1.0f - t) * Vector3f(1.0f, 1.0f, 1.0f) + t * Vector3f(0.5f, 0.7f, 1.0f));
            break;
        }
    }
    return attenuation;
}

struct Renderer
{
    CameraSettings camSettings;

    World world;

    // High quality
    // int num_samples = 96;
    // int num_jumps = 50;
    //
    // Low quality
    int num_samples = 4;
    int num_jumps = 20;

    template <typename RGBSetter>
    void render(RGBSetter&& set,
        int totalWidth, int totalHeight,
        int width, int height,
        int widthOffset, int heightOffset) const
    {
        Camera cam(camSettings);

        std::vector<Vector3f> image(width * height);

        auto compute = [&](int start, int end)
        {
            for (int i = start; i < end; ++i)
            {
                for (int j = 0; j < height; ++j)
                {
                    Vector3f col(0.0f);
                    for (int s = 0; s < num_samples; ++s)
                    {
                        float u = (i + widthOffset + random<float>()) / float(totalWidth);
                        float v = (j + (totalHeight - heightOffset - 1) + random<float>()) / float(totalHeight);
                        Ray r = cam.get_ray(u, v);
                        col += color(r, world.root_node, num_jumps);
                    }
                    col /= float(num_samples);
                    image[j + i * height] = Vector3f(std::pow(col.r(), 1 / 2.2f), std::pow(col.g(), 1 / 2.2f), std::pow(col.b(), 1 / 2.2f));
                }
            }
        };

        compute(0, width);

        for (int i = 0; i < width; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                set(i, j, image[j + i * height]);
            }
        }
    }

    template <typename RGBSetter>
    void operator()(RGBSetter&& set,
        int totalWidth, int totalHeight,
        int width, int height,
        int widthOffset, int heightOffset) const
    {
        render(set, totalWidth, totalHeight, width, height, widthOffset, heightOffset);
    }
};