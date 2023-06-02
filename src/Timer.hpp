#pragma once

#include <chrono>
#include <cstdint>

class Timer
{
public:
    Timer() { reset(); }
    void reset() { start = clock::now(); }
    std::int64_t elapsed() const
    {
        return std::chrono::duration_cast<msecs>(clock::now() - start).count();
    }

private:
    using clock = std::chrono::steady_clock;
    using msecs = std::chrono::milliseconds;
    std::chrono::time_point<clock> start;
};
