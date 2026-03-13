#pragma once
#include <glm/glm.hpp>

class PlayerControllerComponent final {
private:
    float _moveSpeed = 5.0f;
    float _destinationDistance = 1.0f;  // How far ahead to set destination
    glm::vec3 _inputDirection{0.0f};
    bool _hasInput = false;

public:
    PlayerControllerComponent() = default;

    explicit PlayerControllerComponent(float moveSpeed, float destinationDistance = 1.0f)
        : _moveSpeed(moveSpeed), _destinationDistance(destinationDistance) {}

    [[nodiscard]] float GetMoveSpeed() const noexcept { return _moveSpeed; }
    void SetMoveSpeed(float speed) noexcept { _moveSpeed = speed; }

    [[nodiscard]] float GetDestinationDistance() const noexcept { return _destinationDistance; }
    void SetDestinationDistance(float dist) noexcept { _destinationDistance = dist; }

    [[nodiscard]] glm::vec3 GetInputDirection() const noexcept { return _inputDirection; }
    void SetInputDirection(const glm::vec3& dir) noexcept { _inputDirection = dir; }

    [[nodiscard]] bool HasInput() const noexcept { return _hasInput; }
    void SetHasInput(bool has) noexcept { _hasInput = has; }
};
