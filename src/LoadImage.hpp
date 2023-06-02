#pragma once

#include "HostImageRGB.hpp"

#include <string>

HostImageRGB LoadImageFromFile(const char* imagePath);
HostImageRGB LoadImageFromFile(const std::string& imagePath);
