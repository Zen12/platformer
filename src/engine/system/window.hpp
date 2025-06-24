#pragma once
#include <iostream>

class Window
{
private:
    uint16_t _width;
    uint16_t _height;

    std::vector<std::tuple<int, int>> _keyboardCodes;
    std::vector<std::tuple<int, int>> _mouseCodes;

public:
    Window(const uint16_t &width, const uint16_t &height, const std::string &windowName);
    void WinInit();

    [[nodiscard]] bool IsOpen() const noexcept;
    void Destroy() const noexcept;
    void SwapBuffers() const noexcept;
    void PullEvent() const noexcept;

    void ClearInputState() noexcept {
        _keyboardCodes.clear();
        _mouseCodes.clear();
    }

    [[nodiscard]] uint16_t GetWidth() const noexcept {
        return _width;
    }

    [[nodiscard]] uint16_t GetHeight() const noexcept {
        return _height;
    }

    [[nodiscard]] const std::vector<std::tuple<int, int>> &GetKeyboardCodes() const noexcept {
        return _keyboardCodes;
    }

    [[nodiscard]] const std::vector<std::tuple<int, int>> &GetMouseCodes() const noexcept {
        return _mouseCodes;
    }

    void OnResize(const uint16_t& width,const uint16_t& height) noexcept {
        _width = width;
        _height = height;
    }

    void OnKeyCode(const int &code, const int &mode) noexcept {
        _keyboardCodes.push_back(std::make_tuple(code, mode));
    }

    void OnMouse(const int &code, const int &mode) noexcept {
        _mouseCodes.push_back(std::make_tuple(code, mode));
    }

};