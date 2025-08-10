#include "input_system.hpp"

void InputSystem::Update() {
    if (const auto window = _window.lock()) {
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
