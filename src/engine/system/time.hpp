#include <chrono>

class Time
{
private:
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _lastReset;

public:
    void Start()
    {
        _lastReset = std::chrono::high_resolution_clock::now();
        _start = std::chrono::high_resolution_clock::now();
    }

    void Reset()
    {
        _lastReset = std::chrono::high_resolution_clock::now();
    }

    float GetTimeAlived() const
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> duration = currentTime - _start;
        return duration.count();
    }

    float GetResetDelta() const
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> duration = currentTime - _lastReset;
        return duration.count();
    }

    float GetResetDeltaFps() const
    {
        const auto delta = GetResetDelta();
        return 1 / delta;
    }
};