#pragma once

#include "window.hpp"
#include "GLFW/glfw3.h"

enum class InputKey {
    Unknown,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape, Enter, Tab, Backspace, Insert, Delete,
    Right, Left, Down, Up,
    PageUp, PageDown, Home, End,
    CapsLock, ScrollLock, NumLock,
    LeftShift, RightShift, LeftCtrl, RightCtrl,
    LeftAlt, RightAlt, Space
    // Add more as needed
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
    Unknown
};


const std::unordered_map<int, MouseButton> GLFWToMouseButtonMap = {
    {GLFW_MOUSE_BUTTON_LEFT,   MouseButton::Left},
    {GLFW_MOUSE_BUTTON_RIGHT,  MouseButton::Right},
    {GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::Middle},
    {GLFW_MOUSE_BUTTON_4,      MouseButton::Button4},
    {GLFW_MOUSE_BUTTON_5,      MouseButton::Button5},
    {GLFW_MOUSE_BUTTON_6,      MouseButton::Button6},
    {GLFW_MOUSE_BUTTON_7,      MouseButton::Button7},
    {GLFW_MOUSE_BUTTON_8,      MouseButton::Button8},
};

const std::unordered_map<MouseButton, int> MouseButtonMapToGLFW = {
    {MouseButton::Left, GLFW_MOUSE_BUTTON_LEFT},
    {MouseButton::Right, GLFW_MOUSE_BUTTON_RIGHT},
    {MouseButton::Middle, GLFW_MOUSE_BUTTON_MIDDLE},
    {MouseButton::Button4, GLFW_MOUSE_BUTTON_4},
    {MouseButton::Button5, GLFW_MOUSE_BUTTON_5},
    {MouseButton::Button6, GLFW_MOUSE_BUTTON_6},
    {MouseButton::Button7, GLFW_MOUSE_BUTTON_7},
    {MouseButton::Button8, GLFW_MOUSE_BUTTON_8}
    // Note: MouseButton::Unknown intentionally omitted
};



const std::unordered_map<int, InputKey> GLFWToInputKeyMap = {
    {GLFW_KEY_A, InputKey::A}, {GLFW_KEY_B, InputKey::B}, {GLFW_KEY_C, InputKey::C}, {GLFW_KEY_D, InputKey::D},
    {GLFW_KEY_E, InputKey::E}, {GLFW_KEY_F, InputKey::F}, {GLFW_KEY_G, InputKey::G}, {GLFW_KEY_H, InputKey::H},
    {GLFW_KEY_I, InputKey::I}, {GLFW_KEY_J, InputKey::J}, {GLFW_KEY_K, InputKey::K}, {GLFW_KEY_L, InputKey::L},
    {GLFW_KEY_M, InputKey::M}, {GLFW_KEY_N, InputKey::N}, {GLFW_KEY_O, InputKey::O}, {GLFW_KEY_P, InputKey::P},
    {GLFW_KEY_Q, InputKey::Q}, {GLFW_KEY_R, InputKey::R}, {GLFW_KEY_S, InputKey::S}, {GLFW_KEY_T, InputKey::T},
    {GLFW_KEY_U, InputKey::U}, {GLFW_KEY_V, InputKey::V}, {GLFW_KEY_W, InputKey::W}, {GLFW_KEY_X, InputKey::X},
    {GLFW_KEY_Y, InputKey::Y}, {GLFW_KEY_Z, InputKey::Z},

    {GLFW_KEY_0, InputKey::Num0}, {GLFW_KEY_1, InputKey::Num1}, {GLFW_KEY_2, InputKey::Num2}, {GLFW_KEY_3, InputKey::Num3},
    {GLFW_KEY_4, InputKey::Num4}, {GLFW_KEY_5, InputKey::Num5}, {GLFW_KEY_6, InputKey::Num6}, {GLFW_KEY_7, InputKey::Num7},
    {GLFW_KEY_8, InputKey::Num8}, {GLFW_KEY_9, InputKey::Num9},

    {GLFW_KEY_SPACE, InputKey::Space},
    {GLFW_KEY_ESCAPE, InputKey::Escape},
    {GLFW_KEY_LEFT_SHIFT, InputKey::LeftShift},
    {GLFW_KEY_RIGHT_SHIFT, InputKey::RightShift},
    {GLFW_KEY_LEFT_CONTROL, InputKey::LeftCtrl},
    {GLFW_KEY_RIGHT_CONTROL, InputKey::RightCtrl},
    {GLFW_KEY_LEFT_ALT, InputKey::LeftAlt},
    {GLFW_KEY_RIGHT_ALT, InputKey::RightAlt},
    {GLFW_KEY_ENTER, InputKey::Enter},
    {GLFW_KEY_BACKSPACE, InputKey::Backspace},
    {GLFW_KEY_TAB, InputKey::Tab},
    {GLFW_KEY_LEFT, InputKey::Left},
    {GLFW_KEY_RIGHT, InputKey::Right},
    {GLFW_KEY_UP, InputKey::Up},
    {GLFW_KEY_DOWN, InputKey::Down}
};

const std::unordered_map<InputKey, int> InputKeyMapToGLFW = {
    {InputKey::A, GLFW_KEY_A}, {InputKey::B, GLFW_KEY_B}, {InputKey::C, GLFW_KEY_C}, {InputKey::D, GLFW_KEY_D},
    {InputKey::E, GLFW_KEY_E}, {InputKey::F, GLFW_KEY_F}, {InputKey::G, GLFW_KEY_G}, {InputKey::H, GLFW_KEY_H},
    {InputKey::I, GLFW_KEY_I}, {InputKey::J, GLFW_KEY_J}, {InputKey::K, GLFW_KEY_K}, {InputKey::L, GLFW_KEY_L},
    {InputKey::M, GLFW_KEY_M}, {InputKey::N, GLFW_KEY_N}, {InputKey::O, GLFW_KEY_O}, {InputKey::P, GLFW_KEY_P},
    {InputKey::Q, GLFW_KEY_Q}, {InputKey::R, GLFW_KEY_R}, {InputKey::S, GLFW_KEY_S}, {InputKey::T, GLFW_KEY_T},
    {InputKey::U, GLFW_KEY_U}, {InputKey::V, GLFW_KEY_V}, {InputKey::W, GLFW_KEY_W}, {InputKey::X, GLFW_KEY_X},
    {InputKey::Y, GLFW_KEY_Y}, {InputKey::Z, GLFW_KEY_Z},

    {InputKey::Num0, GLFW_KEY_0}, {InputKey::Num1, GLFW_KEY_1}, {InputKey::Num2, GLFW_KEY_2}, {InputKey::Num3, GLFW_KEY_3},
    {InputKey::Num4, GLFW_KEY_4}, {InputKey::Num5, GLFW_KEY_5}, {InputKey::Num6, GLFW_KEY_6}, {InputKey::Num7, GLFW_KEY_7},
    {InputKey::Num8, GLFW_KEY_8}, {InputKey::Num9, GLFW_KEY_9},

    {InputKey::Space, GLFW_KEY_SPACE},
    {InputKey::Escape, GLFW_KEY_ESCAPE},
    {InputKey::LeftShift, GLFW_KEY_LEFT_SHIFT},
    {InputKey::RightShift, GLFW_KEY_RIGHT_SHIFT},
    {InputKey::LeftCtrl, GLFW_KEY_LEFT_CONTROL},
    {InputKey::RightCtrl, GLFW_KEY_RIGHT_CONTROL},
    {InputKey::LeftAlt, GLFW_KEY_LEFT_ALT},
    {InputKey::RightAlt, GLFW_KEY_RIGHT_ALT},
    {InputKey::Enter, GLFW_KEY_ENTER},
    {InputKey::Backspace, GLFW_KEY_BACKSPACE},
    {InputKey::Tab, GLFW_KEY_TAB},
    {InputKey::Left, GLFW_KEY_LEFT},
    {InputKey::Right, GLFW_KEY_RIGHT},
    {InputKey::Up, GLFW_KEY_UP},
    {InputKey::Down, GLFW_KEY_DOWN}
    // InputKey::Unknown intentionally omitted
};


class InputSystem {
private:
    std::weak_ptr<Window> _window;
    std::vector<std::tuple<int, int>> _keyCodes;
    std::vector<std::tuple<int, int>> _mouseCodes;

    // Copy GLFW. When support multiple windows, this should be refactored
    const int INPUT_PRESS    = 1;
    const int INPUT_REPEAT   = 2;
    const int INPUT_RELEASE  = 0;

public:
    explicit InputSystem(const std::weak_ptr<Window> &window)
        : _window(window)
    {}

    void Update() {
        if (const auto window = _window.lock()) {
            _keyCodes = window->GetKeyboardCodes();
            _mouseCodes = window->GetMouseCodes();
        }
    }

    bool IsKeyPress(const InputKey &key) {
        const auto value = InputKeyMapToGLFW.at(key);
        for (const auto &[code, state] : _keyCodes) {
            if (code == value && state == INPUT_PRESS) {
                return true;
            }
        }
        return false;
    }

    bool IsKeyPressing(const InputKey &key) {
        const auto value = InputKeyMapToGLFW.at(key);
        for (const auto &[code, state] : _keyCodes) {
            if (code == value && state == INPUT_REPEAT) {
                return true;
            }
        }
        return false;
    }

    bool IsKeyUp(const InputKey &key) {
        const auto value = InputKeyMapToGLFW.at(key);
        for (const auto &[code, state] : _keyCodes) {
            if (code == value && state == INPUT_RELEASE) {
                return true;
            }
        }
        return false;
    }

    bool IsMousePress(const MouseButton &button) {
        const auto value = MouseButtonMapToGLFW.at(button);
        for (const auto &[code, state] : _mouseCodes) {
            if (code == value && state == INPUT_PRESS) {
                return true;
            }
        }
        return false;
    }

    bool IsMousePressing(const MouseButton &button) {
        const auto value = MouseButtonMapToGLFW.at(button);
        for (const auto &[code, state] : _mouseCodes) {
            if (code == value && state == INPUT_REPEAT) {
                return true;
            }
        }
        return false;
    }

    bool IsMouseUp(const MouseButton &button) {
        const auto value = MouseButtonMapToGLFW.at(button);
        for (const auto &[code, state] : _mouseCodes) {
            if (code == value && state == INPUT_RELEASE) {
                return true;
            }
        }
        return false;
    }



};

