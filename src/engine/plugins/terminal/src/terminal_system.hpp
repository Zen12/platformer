#pragma once

#ifndef __EMSCRIPTEN__

#include "esc/esc_core.hpp"
#include "scene.hpp"
#include "terminal_emulator.hpp"
#include "terminal_renderer.hpp"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

class TerminalSystem final : public ISystem {
public:
    explicit TerminalSystem(std::weak_ptr<Scene> scene, std::string projectRoot = "")
        : _scene(std::move(scene)), _projectRoot(std::move(projectRoot)) {}

    void OnTick() override {
        auto scene = _scene.lock();
        if (!scene) return;
        auto window = scene->GetWindow().lock();
        if (!window) return;

        const auto& keys = window->GetKeyboardCodes();

        bool ctrlHeld = false;
        {
            auto it = keys.find(GLFW_KEY_LEFT_CONTROL);
            if (it != keys.end() && it->second != 0) ctrlHeld = true;
            it = keys.find(GLFW_KEY_RIGHT_CONTROL);
            if (it != keys.end() && it->second != 0) ctrlHeld = true;
        }
        auto toggleIt = keys.find(GLFW_KEY_BACKSLASH);
        bool backslashDown = (toggleIt != keys.end() && toggleIt->second != 0);
        bool toggleCombo = backslashDown && ctrlHeld;
        if (toggleCombo && !_toggleWasPressed) {
            _visible = !_visible;
            if (_visible && !_initialized) {
                LazyInit(window->GetWidth(), window->GetHeight());
            }
            window->SetTerminalActive(_visible);
        }
        _toggleWasPressed = toggleCombo;

        if (!_visible || !_initialized) {
            window->ClearOverlayRenderCallback();
            return;
        }

        if (_emulator && !_emulator->IsAlive()) {
            _visible = false;
            window->SetTerminalActive(false);
            window->ClearOverlayRenderCallback();
            return;
        }

        _emulator->ProcessOutput();

        const auto& charBuffer = window->GetCharBuffer();
        for (uint32_t c : charBuffer) {
            _emulator->SendChar(c);
        }
        window->ClearCharBuffer();

        HandleSpecialKeys(keys);

        double scrollY = window->ConsumeScrollY();
        if (scrollY != 0.0) {
            int lines = static_cast<int>(scrollY * 3);
            if (lines != 0) _emulator->Scroll(lines);
        }

        auto* renderer = &_renderer;
        auto* emulator = _emulator.get();
        int w = window->GetWidth();
        int h = window->GetHeight();
        window->SetOverlayRenderCallback([renderer, emulator, w, h]() {
            renderer->Render(*emulator, w, h);
        });
    }

private:
    void LazyInit(int windowWidth, int windowHeight) {
        std::string fontPath;

#ifdef __APPLE__
        fontPath = "/System/Library/Fonts/Menlo.ttc";
#else
        fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
#endif

        _renderer.Init(fontPath, FONT_SIZE);

        int cols = _renderer.GetCellWidth() > 0 ? windowWidth / _renderer.GetCellWidth() : 80;
        int rows = _renderer.GetCellHeight() > 0 ? windowHeight / _renderer.GetCellHeight() : 24;
        cols = std::max(cols, 20);
        rows = std::max(rows, 5);

        _emulator = std::make_unique<TerminalEmulator>(cols, rows, _projectRoot);
        _initialized = true;
    }

    void HandleSpecialKeys(const std::unordered_map<int, int>& keys) {
        auto check = [&](int glfwKey) -> bool {
            auto it = keys.find(glfwKey);
            return it != keys.end() && it->second == 1;
        };

        bool ctrl = false;
        {
            auto it = keys.find(GLFW_KEY_LEFT_CONTROL);
            if (it != keys.end() && it->second != 0) ctrl = true;
            it = keys.find(GLFW_KEY_RIGHT_CONTROL);
            if (it != keys.end() && it->second != 0) ctrl = true;
        }

        if (check(GLFW_KEY_ESCAPE))    _emulator->SendKey(GLFW_KEY_ESCAPE, 0);
        if (check(GLFW_KEY_ENTER))     _emulator->SendKey(GLFW_KEY_ENTER, 0);
        if (check(GLFW_KEY_BACKSPACE)) _emulator->SendKey(GLFW_KEY_BACKSPACE, 0);
        if (check(GLFW_KEY_TAB))       _emulator->SendKey(GLFW_KEY_TAB, 0);
        if (check(GLFW_KEY_UP))        _emulator->SendKey(GLFW_KEY_UP, 0);
        if (check(GLFW_KEY_DOWN))      _emulator->SendKey(GLFW_KEY_DOWN, 0);
        if (check(GLFW_KEY_LEFT))      _emulator->SendKey(GLFW_KEY_LEFT, 0);
        if (check(GLFW_KEY_RIGHT))     _emulator->SendKey(GLFW_KEY_RIGHT, 0);
        if (check(GLFW_KEY_HOME))      _emulator->SendKey(GLFW_KEY_HOME, 0);
        if (check(GLFW_KEY_END))       _emulator->SendKey(GLFW_KEY_END, 0);
        if (check(GLFW_KEY_DELETE))    _emulator->SendKey(GLFW_KEY_DELETE, 0);
        if (check(GLFW_KEY_PAGE_UP))   _emulator->SendKey(GLFW_KEY_PAGE_UP, 0);
        if (check(GLFW_KEY_PAGE_DOWN)) _emulator->SendKey(GLFW_KEY_PAGE_DOWN, 0);

        if (ctrl) {
            if (check(GLFW_KEY_C)) _emulator->SendKey(GLFW_KEY_C, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_D)) _emulator->SendKey(GLFW_KEY_D, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_Z)) _emulator->SendKey(GLFW_KEY_Z, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_L)) _emulator->SendKey(GLFW_KEY_L, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_A)) _emulator->SendKey(GLFW_KEY_A, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_E)) _emulator->SendKey(GLFW_KEY_E, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_U)) _emulator->SendKey(GLFW_KEY_U, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_K)) _emulator->SendKey(GLFW_KEY_K, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_W)) _emulator->SendKey(GLFW_KEY_W, GLFW_MOD_CONTROL);
            if (check(GLFW_KEY_R)) _emulator->SendKey(GLFW_KEY_R, GLFW_MOD_CONTROL);
        }
    }

    std::weak_ptr<Scene> _scene;
    std::string _projectRoot;
    std::unique_ptr<TerminalEmulator> _emulator;
    TerminalRenderer _renderer;
    bool _visible = false;
    bool _initialized = false;
    bool _toggleWasPressed = false;

    static constexpr int FONT_SIZE = 20;
};

#endif
