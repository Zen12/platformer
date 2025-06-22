#pragma once
#include <iostream>

class Window
{
private:
    uint16_t _width;
    uint16_t _height;

public:
    Window(const uint16_t &width, const uint16_t &height, const std::string &windowName);
    void WinInit();

    [[nodiscard]] bool IsOpen() const noexcept;
    void Destroy() const noexcept;
    void SwapBuffers() const noexcept;
    void PullEvent() const noexcept;


    [[nodiscard]] uint16_t GetWidth() const {
        return _width;
    }

    [[nodiscard]] uint16_t GetHeight() const {
        return _height;
    }

    void OnResize(const uint16_t& width,const uint16_t& height) noexcept
    {
        _width = width;
        _height = height;
    };
};