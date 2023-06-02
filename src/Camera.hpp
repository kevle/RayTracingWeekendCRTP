#pragma once

#include "Math.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

struct CameraSettings
{
    Vector3f lookfrom = Vector3f(10.f, 2.0f, 2.5f);
    Vector3f lookat = Vector3f(0.f, 0.f, 0.f);
    Vector3f up = Vector3f(0.f, 1.f, 0.f);

    float vertical_fov_deg = 25.f;
    float aspect = 16.f / 9;

    float focus_dist = (lookfrom - lookat).length() * 0.66f;
    float aperture = 0.0f;
};

struct Camera
{
    Vector3f origin;
    Vector3f lower_left_corner;
    Vector3f horizontal;
    Vector3f vertical;

    Vector3f u;
    Vector3f v;

    float lens_radius;

    Camera()
    {
        lower_left_corner = Vector3f(-2.0f, -1.0f, -1.0f);
        horizontal = Vector3f(4.0f, 0.0f, 0.0f);
        vertical = Vector3f(0.0f, 2.0f, 0.0f);
        origin = Vector3f(0.0f, 0.0f, 0.0f);
        u = Vector3f(1.0f, 0.0f, 0.0f);
        v = Vector3f(0.0f, 1.0f, 0.0f);
        lens_radius = 0.0f;
    }

    Camera(Vector3f lookfrom, Vector3f lookat, Vector3f vup, float vertical_fov_deg, float aspect, float aperture, float focus_dist)
        : Camera()
    {
        lens_radius = aperture / 2.0f;
        double theta = vertical_fov_deg * pi<double>() / 180.;
        double half_height = std::tan(theta * 0.5);
        double half_width = aspect * half_height;
        origin = lookfrom;
        Vector3f w = (lookfrom - lookat).normalized();
        u = cross(vup, w).normalize();
        v = cross(w, u);
        lower_left_corner = origin - focus_dist * (static_cast<float>(half_width) * u + static_cast<float>(half_height) * v + w);
        horizontal = 2 * static_cast<float>(half_width) * focus_dist * u;
        vertical = 2 * static_cast<float>(half_height) * focus_dist * v;
    }

    Camera(const CameraSettings& settings)
        : Camera(settings.lookfrom, settings.lookat, settings.up, settings.vertical_fov_deg, settings.aspect, settings.aperture, settings.focus_dist)
    {
    }

    Ray get_ray(float s, float t) const
    {
        Vector3f rd = lens_radius * random_in_unit_disk();
        Vector3f offset = u * rd.x() + v * rd.y();
        return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }
};