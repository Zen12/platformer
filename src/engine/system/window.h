#pragma once
#include <iostream> 


class Window
{
    private:
        uint16_t _width;
        uint16_t _height;
    public:
        Window(const uint16_t& width, const uint16_t& height, const std::string& windowName);
        void WinInit();

        bool IsOpen();
        void Destroy();
        void SwapBuffers();
        const uint16_t GetWidth() {return _width; }
        const uint16_t GetHeight() {return _height; }
        void OnResize(uint16_t width, uint16_t height) {_width = width; _height = height;};
};