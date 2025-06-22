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

    [[nodiscard]] float GetTimeAlive() const noexcept
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> duration = currentTime - _start;
        return duration.count();
    }

    [[nodiscard]] float GetResetDelta() const noexcept
    {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> duration = currentTime - _lastReset;
        return duration.count();
    }

    [[nodiscard]] float GetResetDeltaFps() const
    {
        const auto delta = GetResetDelta();
        return 1 / delta;
    }
};