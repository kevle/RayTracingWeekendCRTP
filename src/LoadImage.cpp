#include "LoadImage.hpp"
#include "Vector.hpp"

#include "olc/olcPixelGameEngine.h"

HostImageRGB LoadImageFromFile(const std::string& imagePath)
{
    olc::Sprite s;
    s.LoadFromFile(imagePath);

    HostImageRGB image(s.width, s.height);
    for (int i = 0; i < s.width; ++i)
    {
        for (int j = 0; j < s.height; ++j)
        {
            auto pixel = s.GetPixel(i, j);
            image.set(i, j, RGBPixel(static_cast<unsigned char>(pixel.r), static_cast<unsigned char>(pixel.g), static_cast<unsigned char>(pixel.b)));
        }
    }
    return image;
}

HostImageRGB LoadImageFromFile(const char* imagePath)
{
    return LoadImageFromFile(std::string(imagePath));
}
