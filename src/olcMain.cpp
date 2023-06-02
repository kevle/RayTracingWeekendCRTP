#include "Renderer.hpp"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define VC_EXTRALEAN

#define OLC_PGE_APPLICATION
#ifndef _WIN32
#define OLC_IMAGE_LIBPNG
#endif
// #define OLC_IMAGE_STB
// #define OLC_IMAGE_STB_WRITE
#include "olc/olcPixelGameEngine.h"

// #include "Hash.hpp"

#include <optional>
#include <queue>
#include <set>
#include <thread>
#include <vector>

using Vec2 = Vector<int, 2>;

struct DoubleBufferData
{
    Vec2 size;

    olc::Sprite front;
    std::vector<RGBPixel> back;

    DoubleBufferData(Vec2 size = Vec2{})
        : size(size)
        , front(size[0], size[1])
        , back(size[0] * size[1])
    {
    }
};

struct DoubleBuffer
{
    std::mutex access;

    DoubleBufferData data;

    DoubleBuffer(Vec2 size = Vec2{})
        : data(size)
    {
    }

    void swap()
    {
        std::lock_guard<std::mutex> guard(access);

        std::transform(data.back.begin(), data.back.end(), data.front.GetData(), [](RGBPixel p)
            { return olc::Pixel(p.x(), p.y(), p.z()); });
    }

    void insert_pixels(const std::vector<RGBPixel>& pixels, Vec2 pos, Vec2 size)
    {
        std::lock_guard<std::mutex> guard(access);

        auto& back = data.back;

        for (int y = 0; y < size[1]; ++y)
        {
            for (int x = 0; x < size[0]; ++x)
            {
                back[(pos[0] + x) + (pos[1] + y) * data.size[0]] = pixels[x + y * size[0]];
            }
        }
    }
};

struct WorkItem
{
    Vec2 start;
    Vec2 size;
};

class WorkQueue
{
private:
    std::mutex access;
    std::queue<WorkItem> items;

public:
    void enqueue(const WorkItem& item)
    {
        std::lock_guard<std::mutex> guard(access);
        items.push(item);
    }

    std::optional<WorkItem> retrieve()
    {
        std::lock_guard<std::mutex> guard(access);
        if (items.empty())
            return std::nullopt;
        WorkItem item = items.front();
        items.pop();
        return item;
    }

    void clear()
    {
        std::lock_guard<std::mutex> guard(access);
        while (!items.empty())
        {
            items.pop();
        }
    }
};

class RendererInstance : public olc::PixelGameEngine
{
    Renderer renderer;
    DoubleBuffer buffer;

    int num_threads;
    std::vector<std::thread> threads;
    WorkQueue work;

    Vec2 num_chunks;
    Vec2 screen_size = Vec2{ 0 };

public:
    RendererInstance(int num_threads = 4, Vec2 num_chunks = Vec2{ 4, 3 })
        : num_threads(num_threads)
        , num_chunks(num_chunks)
    {
        sAppName = "Raytracing Weekend";
    }

    olc::rcode Construct(int32_t screen_w, int32_t screen_h, int32_t pixel_w, int32_t pixel_h, bool full_screen = false, bool vsync = false, bool cohesion = false)
    {
        auto r = olc::PixelGameEngine::Construct(screen_w, screen_h, pixel_w, pixel_h, full_screen, vsync, cohesion);
        buffer.data = DoubleBufferData(Vec2{ screen_w, screen_h });
        screen_size = Vec2{ screen_w, screen_h };

        Vec2 chunk_dimensions = get_chunk_dimensions();
        num_chunks = Vec2{ div_up(screen_size[0], chunk_dimensions[0]), div_up(screen_size[1], chunk_dimensions[1]) };

        return r;
    }

    void RenderSegments()
    {
        Vec2 chunk_dimensions = get_chunk_dimensions();

        std::vector<RGBPixel> local_buffer(chunk_dimensions[0] * chunk_dimensions[1]);

        while (auto w = work.retrieve())
        {
            const WorkItem& i = w.value();
            const Vec2& start = i.start;
            const Vec2& size = i.size;

            int max_y = size[1] - 1;

            renderer([&local_buffer, start, size, max_y](int x, int y, const Vector3f& c) { //
                local_buffer[x + (max_y - y) * size[0]] = RGBPixel{ std::uint8_t(c.x() * 255), std::uint8_t(c.y() * 255), std::uint8_t(c.z() * 255) };
            },
                screen_size[0], screen_size[1], size[0], size[1], start[0], start[1]);

            buffer.insert_pixels(local_buffer, start, size); //
        }
    }

    Vec2 get_chunk_dimensions() const
    {
        return Vec2{ div_up(screen_size[0], num_chunks[0]), div_up(screen_size[1], num_chunks[1]) };
    }

    bool OnUserCreate() override
    {
        Vec2 chunk_dimensions = get_chunk_dimensions();

        // Vector3f lookfrom(10.f, 2.0f, 2.5f);
        // Vector3f lookat(0.f, 0.f, 0.f);
        // Vector3f lookfrom(3.5f, 1.8f, 3.1f);
        // Vector3f lookat(0.f, 1.0f, 0.f);
        // Vector3f lookfrom(5.f, 4.0f, 5.f);
        // Vector3f lookat(0.f, 0.f, 0.f);
        // renderer.camSettings = ;
        renderer.world = get_default_scene();

        for (int y = 0; y < num_chunks[1]; ++y)
        {
            for (int x = 0; x < num_chunks[0]; ++x)
            {
                Vector<int, 2> start = Vector<int, 2>{ x * chunk_dimensions[0], screen_size[1] - (y + 1) * chunk_dimensions[1] };
                start = CVec::max(start, Vec2{ 0, 0 });
                Vec2 size = chunk_dimensions;
                size[0] = min(chunk_dimensions[0], screen_size[0] - start[0]);

                if (size[0] == 0 || size[1] == 0)
                    break;

                work.enqueue({ start, size });
            }
        }

        for (int t = 0; t < num_threads; ++t)
        {
            threads.push_back(std::thread([this]()
                { RenderSegments(); }));
        }

        // Called once at the start, so create things here
        return true;
    }

    bool OnUserUpdate(float /*fElapsedTime*/) override
    {
        buffer.swap();

        DrawSprite(0, 0, &buffer.data.front);

        // // Output each rendered frame if it is unique
        // static std::set<std::vector<std::uint8_t>> output_buffers;
        // static int counter = 0;
        // auto hash = Hash(buffer.data.front);
        // bool is_new_image = output_buffers.insert(hash).second;
        // if (is_new_image)
        //     olc::Sprite::loader->SaveImageResource(&buffer.data.front, "frame_" + std::to_string(counter++) + ".png");

        return true;
    }

    virtual bool OnUserDestroy() override
    {
        work.clear();
        for (auto& thread : threads)
        {
            thread.join();
        }
        buffer.swap();

        olc::Sprite::loader->SaveImageResource(&buffer.data.front, "output.png");

        return olc::rcode::OK;
    }
};

int main()
{
    RendererInstance instance(16, Vec2{ 64, 64 });
    if (instance.Construct(1920, 1080, 1, 1))
        // if (instance.Construct(1280, 720, 1, 1))
        return instance.Start() == olc::rcode::OK ? 0 : 1;
    else
        return 1;
}
