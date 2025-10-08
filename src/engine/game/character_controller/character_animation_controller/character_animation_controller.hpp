#pragma once
#include "../../../entity/entity.hpp"
#include "../character_animation/character_animation_component.hpp"
#include "../movement/character_movement_component.hpp"

class CharacterAnimationController final : public Component {
private:
    std::weak_ptr<CharacterAnimationComponent> _animationComponent;
    std::weak_ptr<CharacterMovementComponent> _movementComponent;
    std::weak_ptr<Transform> _transform;
    glm::vec3 _lookAt{0};

public:
    explicit CharacterAnimationController(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetAnimationComponent(const std::weak_ptr<CharacterAnimationComponent> &animationComponent) noexcept {
        _animationComponent = animationComponent;
    }

    void SetMovementComponent(const std::weak_ptr<CharacterMovementComponent> &movementComponent) noexcept {
        _movementComponent = movementComponent;
    }

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }

    void SetLookAt(const glm::vec3 &lookAt) noexcept {
        _lookAt = lookAt;
    }

    void Update(const float &deltaTime) override;
};
