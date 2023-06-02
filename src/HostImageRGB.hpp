#pragma once

#include "Vector.hpp"

#include <cmath>
#include <vector>

template <bool interpolate = false>
struct HostImageRGBI
{
private:
    std::vector<RGBPixel> pixels;

public:
    int width;
    int height;

    HostImageRGBI(int width = 0, int height = 0)
        : pixels(width * height)
        , width(width)
        , height(height)
    {
    }
    HostImageRGBI(const std::vector<RGBPixel>& pixels, int width, int height)
        : pixels(pixels)
        , width(width)
        , height(height)
    {
    }

    void set(int x, int y, RGBPixel c)
    {
        pixels[x + y * width] = c;
    }

    RGBPixel get(int x, int y) const
    {
        return pixels[x + y * width];
    }

    Vector3f get(float x, float y) const
    {
        if (x >= 1.f)
        {
            x = x - floor(x);
        }
        if (x < 0)
        {
            x = ceil(x) - x;
        }
        if (y >= 1.f)
        {
            y = y - floor(y);
        }
        if (y < 0)
        {
            y = ceil(y) - y;
        }
        // x = min(max(0.f, x), 1.0f);
        // y = min(max(0.f, y), 1.0f);
        x = (width - 1) * x;
        y = (height - 1) * y;

        int x_sample = static_cast<int>(floor(x));
        int y_sample = static_cast<int>(floor(y));

        RGBPixel q11 = get(x_sample, y_sample);

        if constexpr (interpolate)
        {
            RGBPixel q12 = get(x_sample, y_sample + 1);
            RGBPixel q21 = get(x_sample + 1, y_sample);
            RGBPixel q22 = get(x_sample + 1, y_sample + 1);
            return (((x_sample + 1) - x) * (q11 * ((y_sample + 1) - y) + q12 * (y - y_sample)) + (x - x_sample) * (q21 * ((y_sample + 1) - y) + q22 * (y - y_sample)))
                * (1.f / 255.f);
        }
        return q11 * (1.f / 255.f);
    }
};

using HostImageRGB = HostImageRGBI<false>;
