#pragma once
#include <string>
#include <glm/glm.hpp>

class TopDownCameraComponent final {
private:
    std::string _targetTag;
    glm::vec3 _offsetPosition{0.0f, 10.0f, 10.0f};
    glm::vec3 _offsetRotation{-45.0f, 0.0f, 0.0f};
    float _maxLookAhead{5.0f};
    float _smoothSpeed{5.0f};

public:
    // Mutable for lerp state tracking
    mutable glm::vec3 CurrentCameraPosition{0.0f};
    mutable bool Initialized{false};

    TopDownCameraComponent() = default;

    explicit TopDownCameraComponent(std::string targetTag,
        const glm::vec3& offsetPosition = glm::vec3(0.0f, 10.0f, 10.0f),
        const glm::vec3& offsetRotation = glm::vec3(-45.0f, 0.0f, 0.0f),
        float maxLookAhead = 5.0f,
        float smoothSpeed = 5.0f)
        : _targetTag(std::move(targetTag)), _offsetPosition(offsetPosition),
          _offsetRotation(offsetRotation), _maxLookAhead(maxLookAhead), _smoothSpeed(smoothSpeed) {}

    [[nodiscard]] const std::string& GetTargetTag() const noexcept { return _targetTag; }
    void SetTargetTag(const std::string& tag) noexcept { _targetTag = tag; }

    [[nodiscard]] glm::vec3 GetOffsetPosition() const noexcept { return _offsetPosition; }
    void SetOffsetPosition(const glm::vec3& offset) noexcept { _offsetPosition = offset; }

    [[nodiscard]] glm::vec3 GetOffsetRotation() const noexcept { return _offsetRotation; }
    void SetOffsetRotation(const glm::vec3& rotation) noexcept { _offsetRotation = rotation; }

    [[nodiscard]] float GetMaxLookAhead() const noexcept { return _maxLookAhead; }
    void SetMaxLookAhead(float lookAhead) noexcept { _maxLookAhead = lookAhead; }

    [[nodiscard]] float GetSmoothSpeed() const noexcept { return _smoothSpeed; }
    void SetSmoothSpeed(float speed) noexcept { _smoothSpeed = speed; }
};
