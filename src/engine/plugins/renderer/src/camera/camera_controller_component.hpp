#pragma once
#include "glm/vec3.hpp"

class CameraControllerComponent final {
private:
    float _moveSpeed = 10.0f;
    float _mouseSensitivity = 0.1f;
    glm::vec2 _lastMousePosition = glm::vec2(0.0f);
    bool _wasRightMousePressed = false;

public:
    [[nodiscard]] float GetMoveSpeed() const noexcept {
        return _moveSpeed;
    }

    void SetMoveSpeed(float speed) noexcept {
        _moveSpeed = speed;
    }

    [[nodiscard]] float GetMouseSensitivity() const noexcept {
        return _mouseSensitivity;
    }

    void SetMouseSensitivity(float sensitivity) noexcept {
        _mouseSensitivity = sensitivity;
    }

    [[nodiscard]] glm::vec2 GetLastMousePosition() const noexcept {
        return _lastMousePosition;
    }

    void SetLastMousePosition(const glm::vec2& position) noexcept {
        _lastMousePosition = position;
    }

    [[nodiscard]] bool WasRightMousePressed() const noexcept {
        return _wasRightMousePressed;
    }

    void SetRightMousePressed(bool pressed) noexcept {
        _wasRightMousePressed = pressed;
    }
};
