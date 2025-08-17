#include "input_system.hpp"

void InputSystem::Update() {
    if (const auto window = _window.lock()) {
        window->SwapWithValue(INPUT_PRESS, INPUT_REPEAT);
        window->ClearWithValue(INPUT_RELEASE);
        window->PullEvent();
        _keyCodes = window->GetKeyboardCodes();
        _mouseCodes = window->GetMouseCodes();
    }
}

bool InputSystem::IsKeyPress(const InputKey &key) {
    const auto value = InputKeyMapToGLFW.at(key);
    for (const auto &[code, state] : _keyCodes) {
        if (code == value && state == INPUT_PRESS) {
            return true;
        }
    }
    return false;
}

bool InputSystem::IsKeyPressing(const InputKey &key) {
    const auto value = InputKeyMapToGLFW.at(key);
    for (const auto &[code, state] : _keyCodes) {
        if (code == value && state == INPUT_REPEAT) {
            return true;
        }
    }
    return false;
}

bool InputSystem::IsKeyUp(const InputKey &key) {
    const auto value = InputKeyMapToGLFW.at(key);
    for (const auto &[code, state] : _keyCodes) {
        if (code == value && state == INPUT_RELEASE) {
            return true;
        }
    }
    return false;
}

bool InputSystem::IsMousePress(const MouseButton &button) {
    const auto value = MouseButtonMapToGLFW.at(button);
    for (const auto &[code, state] : _mouseCodes) {
        if (code == value && state == INPUT_PRESS) {
            return true;
        }
    }
    return false;
}

bool InputSystem::IsMousePressing(const MouseButton &button) {
    const auto value = MouseButtonMapToGLFW.at(button);
    for (const auto &[code, state] : _mouseCodes) {
        if (code == value && (state == INPUT_REPEAT || state == INPUT_PRESS)) {
            return true;
        }
    }
    return false;
}

bool InputSystem::IsMouseUp(const MouseButton &button) {
    const auto value = MouseButtonMapToGLFW.at(button);
    for (const auto &[code, state] : _mouseCodes) {
        if (code == value && state == INPUT_RELEASE) {
            return true;
        }
    }
    return false;
}

glm::vec2 InputSystem::GetMouseWindowPosition() const noexcept {
    if (const auto window = _window.lock()) {
        double x, y;
        window->GetMousePosition(&x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    return glm::vec2(0.0f);
}

glm::vec2 InputSystem::GetMouseScreenSpace() const noexcept {
    if (const auto window = _window.lock()) {
        const auto pos = GetMouseWindowPosition();
        const auto size = glm::vec2{static_cast<float>(pos.x), static_cast<float>(pos.y)};
        return {(pos.x + size.x), (pos.y+ size.y)};
    }

    return glm::vec2(0.0f);
}
