#pragma once

#include "Matrix.hpp"
#include "Triangles.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace PLY
{
enum class FileType
{
    ASCII = 0,
    BINARY_BIG_ENDIAN = 1,
    UNKNOWN
};

static FileType string_to_filetype(const std::string& s)
{
    if (s == "ascii")
    {
        return FileType::ASCII;
    }
    else if (s == "binary_big_endian")
    {
        return FileType::BINARY_BIG_ENDIAN;
    }
    else
    {
        return FileType::UNKNOWN;
    }
}

static std::vector<std::string> split_string(const std::string& s, const char delimiter = ' ')
{
    auto it = s.begin();
    std::vector<std::string> output;
    std::string tmp;
    while (it != s.end())
    {
        if (*it != delimiter)
        {
            tmp += *it;
        }
        else
        {
            output.push_back(tmp);
            tmp = "";
        }
        ++it;
    }
    output.push_back(tmp);
    return output;
}

static void load_ascii(std::istream& in, TriangleData& output, int num_vertices, int num_faces)
{
    // read vertices
    for (int v_id = 0; v_id < num_vertices; ++v_id)
    {
        Vector3f vertex;
        for (int d = 0; d < 3; ++d)
        {
            in >> vertex[d];
            vertex[d] *= 10;
        }
        output.vertices.push_back(vertex);
    }
    // read faces
    for (int f_id = 0; f_id < num_faces; ++f_id)
    {
        Vector3i face;
        int dummy;
        in >> dummy;
        if (dummy != 3)
        {
            // TODO: Failure...
            return;
        }
        for (int d = 0; d < 3; ++d)
        {
            in >> face[d];
        }
        output.faces.push_back(face);
    }
}

static constexpr int pow2(int power)
{
    return power < 0 ? 0 : (power > 0 ? 2 * pow2(power - 1) : 1);
}

static std::uint32_t from_big_endian(std::uint32_t in)
{
    std::uint32_t out = { 0 };
    std::uint8_t c[4];
    std::memcpy(c, &in, sizeof(std::uint32_t));

    // https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html
    for (int i = 0; i < static_cast<int>(sizeof(std::uint32_t)); ++i)
    {
        out |= (std::uint32_t(c[sizeof(std::uint32_t) - 1 - i]) << (i * 8));
    }
    return out;
}

template <typename T>
T from_big_endian(T input)
{
    static_assert(sizeof(T) == sizeof(std::uint32_t), "Can only convert 4 byte wide types.");

    std::uint32_t i = 0;
    std::memcpy(&i, &input, sizeof(std::uint32_t));
    std::uint32_t o = from_big_endian(i);
    T output = T(0);
    std::memcpy(&output, &o, sizeof(std::uint32_t));
    return output;
}

static void load_binary(std::istream& in, TriangleData& output, int num_vertices, int num_faces)
{
    if (num_vertices <= 0)
        return;
    // read vertices
    output.vertices.resize(num_vertices);
    in.read(reinterpret_cast<char*>(&output.vertices[0]), num_vertices * sizeof(float) * 3);
    if (!in.good())
    {
        std::cerr << "Error reading file.\n";
        return;
    }

    Vector3f minPos = Vector3f(std::numeric_limits<float>::max());
    Vector3f maxPos = Vector3f(std::numeric_limits<float>::min());
    for (int v_id = 0; v_id < num_vertices; ++v_id)
    {
        Vector3f& vertex = output.vertices[v_id];
        for (int d = 0; d < 3; ++d)
        {
            vertex[d] = from_big_endian(vertex[d]);
            minPos[d] = std::min(vertex[d], minPos[d]);
            maxPos[d] = std::max(vertex[d], maxPos[d]);
        }
    }

    float maxDim = max((maxPos - minPos).eval());

    for (int v_id = 0; v_id < num_vertices; ++v_id)
    {
        Vector3f& vertex = output.vertices[v_id];
        vertex -= minPos;
        vertex /= maxDim;
    }

    // read faces
    output.faces.resize(num_faces);
    int line_size = (sizeof(std::int32_t) * 3 + 1);
    std::vector<char> buffer(num_faces * line_size);
    in.read(&buffer[0], buffer.size());
    // Convert from buffer
    for (int f_id = 0; f_id < num_faces; ++f_id)
    {
        int cur_offset = 1 + line_size * f_id;
        Vector3i& face = output.faces[f_id];
        for (int d = 0; d < 3; ++d)
        {
            std::uint8_t bytes[4];
            for (int i = 0; i < 4; ++i)
            {
                bytes[i] = buffer[cur_offset + 4 * d + i];
            }
            std::int32_t i = 0;
            std::memcpy(&i, bytes, sizeof(std::int32_t));
            face[d] = i;
        }
    }
    // Swap endianness
    for (int f_id = 0; f_id < num_faces; ++f_id)
    {
        Vector3i& face = output.faces[f_id];
        for (int d = 0; d < 3; ++d)
        {
            face[d] = from_big_endian(face[d]);
        }
    }
}
} // namespace PLY

static TriangleData LoadPLY(const std::string& imagePath)
{
    std::ifstream in(imagePath, std::ios::binary | std::ios::in);
    std::string line;

    TriangleData output;

    int num_vertices = 0;
    int num_faces = 0;

    PLY::FileType type = PLY::FileType::UNKNOWN;

    // Parse header
    int line_index = 0;
    while (std::getline(in, line))
    {
        if (line_index == 0)
        {
            // check ply tag
        }
        line_index++;

        std::vector<std::string> tokens = PLY::split_string(line, ' ');

        if (line.substr(0, 15) == "element vertex ")
        {
            num_vertices = std::stoi(line.substr(15));
            output.vertices.reserve(num_vertices);
        }
        else if (line.substr(0, 13) == "element face ")
        {
            num_faces = std::stoi(line.substr(13));
            output.faces.reserve(num_faces);
        }
        else if (tokens.size() == 3 && tokens[0] == "format")
        {
            type = PLY::string_to_filetype(tokens[1]);
        }
        else if (tokens.size() == 5 && tokens[0] == "property" && tokens[1] == "list")
        {
            if (tokens[2] != "uchar" || tokens[3] != "int" || tokens[4] != "vertex_indices")
            {
                // TODO: Indicate failure
                return output;
            }
        }
        else if (line.substr(0, 10) == "end_header")
        {
            break;
        }
    }

    switch (type)
    {
    case PLY::FileType::ASCII:
        PLY::load_ascii(in, output, num_vertices, num_faces);
        break;
    case PLY::FileType::BINARY_BIG_ENDIAN:
        PLY::load_binary(in, output, num_vertices, num_faces);
        break;
    default:
        break;
    }

    return output;
}