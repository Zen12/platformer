#pragma once
#include <functional>
#include <iostream>
#include <vector>

#define DEBUG_ENGINE_WINDOW 0

class Window
{
private:
    uint16_t _width;
    uint16_t _height;

    std::unordered_map<int, int> _keyboardCodes;
    std::unordered_map<int, int> _mouseCodes;
#ifdef __EMSCRIPTEN__
    std::unordered_map<int, int> _keyboardCodesCached;
    std::unordered_map<int, int> _mouseCodesCached;
#endif

    std::vector<uint32_t> _charBuffer;
    double _scrollY = 0.0;
    bool _terminalActive = false;
    std::function<void()> _overlayRenderCallback;

public:
    Window(const uint16_t &width, const uint16_t &height, const std::string &windowName);
    void WinInit();

    [[nodiscard]] bool IsOpen() const noexcept;
    void Destroy() const noexcept;
    void SwapBuffers() const noexcept;
    void PullEvent() noexcept;

    void ClearWithValue(const int &value) noexcept;

    void SwapWithValue(const int &target, const int &newValue) noexcept;

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

    void OnChar(uint32_t codepoint) noexcept {
        if (!_terminalActive) return;
        _charBuffer.push_back(codepoint);
    }

    [[nodiscard]] const std::vector<uint32_t>& GetCharBuffer() const noexcept {
        return _charBuffer;
    }

    void ClearCharBuffer() noexcept {
        _charBuffer.clear();
    }

    void OnScroll([[maybe_unused]] double xOffset, double yOffset) noexcept {
        _scrollY += yOffset;
    }

    [[nodiscard]] double ConsumeScrollY() noexcept {
        double val = _scrollY;
        _scrollY = 0.0;
        return val;
    }

    void SetTerminalActive(bool active) noexcept {
        _terminalActive = active;
    }

    [[nodiscard]] bool IsTerminalActive() const noexcept {
        return _terminalActive;
    }

    void OnResize(const uint16_t& width,const uint16_t& height) noexcept {
        _width = width;
        _height = height;
    }

    void OnKeyCode(const int &code, const int &mode) noexcept {
#if DEBUG_ENGINE_WINDOW
        std::cout << code << " " << mode << std::endl;
#endif

#if __EMSCRIPTEN__
        _keyboardCodesCached[code] = mode;
#else
        _keyboardCodes[code] = mode;
#endif
    }

    void OnMouse(const int &code, const int &mode) noexcept {
#if DEBUG_ENGINE_WINDOW
        std::cout << code << " " << mode << std::endl;
#endif

#if __EMSCRIPTEN__
        _mouseCodesCached[code] = mode;
#else
        _mouseCodes[code] =  mode;
#endif
    }

    [[nodiscard]] bool IsFocus() const noexcept;

    void GetMousePosition(double *x, double *y) const noexcept;

    void SetOverlayRenderCallback(std::function<void()> callback) noexcept {
        _overlayRenderCallback = std::move(callback);
    }

    void ClearOverlayRenderCallback() noexcept {
        _overlayRenderCallback = nullptr;
    }

    void RenderOverlay() const {
        if (_overlayRenderCallback) {
            _overlayRenderCallback();
        }
    }

};