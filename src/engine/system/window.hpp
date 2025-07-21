#pragma once
#include <iostream>

#define DEBUG_ENGINE_WINDOW 0

class Window
{
private:
    uint16_t _width;
    uint16_t _height;

    std::unordered_map<int, int> _keyboardCodes;
    std::unordered_map<int, int> _mouseCodes;

public:
    Window(const uint16_t &width, const uint16_t &height, const std::string &windowName);
    void WinInit();

    [[nodiscard]] bool IsOpen() const noexcept;
    void Destroy() const noexcept;
    void SwapBuffers() const noexcept;
    void PullEvent() const noexcept;

    void ClearWithValue(const int &value) noexcept {

        for (auto it = _keyboardCodes.begin(); it != _keyboardCodes.end(); ) {
            if (it->second == value) {
                it = _keyboardCodes.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = _mouseCodes.begin(); it != _mouseCodes.end(); ) {
            if (it->second == value) {
                it = _mouseCodes.erase(it);
            } else {
                ++it;
            }
        }
    }

    [[nodiscard]] uint16_t GetWidth() const noexcept {
        return _width;
    }

    [[nodiscard]] uint16_t GetHeight() const noexcept {
        return _height;
    }

    [[nodiscard]] const std::unordered_map<int, int> &GetKeyboardCodes() const noexcept {
        return _keyboardCodes;
    }

    [[nodiscard]] const std::unordered_map<int, int> &GetMouseCodes() const noexcept {
        return _mouseCodes;
    }

    void OnResize(const uint16_t& width,const uint16_t& height) noexcept {
        _width = width;
        _height = height;
    }

    void OnKeyCode(const int &code, const int &mode) noexcept {
#ifndef NDEBUG
#if DEBUG_ENGINE_WINDOW
        std::cout << code << " " << mode << std::endl;
#endif
#endif
        _keyboardCodes[code] = mode;
    }

    void OnMouse(const int &code, const int &mode) noexcept {
        _mouseCodes[code] =  mode;
    }

};