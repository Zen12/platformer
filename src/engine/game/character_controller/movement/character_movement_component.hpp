#pragma once
#include "../../../entity/entity.hpp"
#include "character_movement_settings.hpp"
#include <glm/vec2.hpp>

#include "../../../physics/physics_world.hpp"
#include "../../../renderer/transform/transform_component.hpp"

class CharacterMovementComponent final : public Component {
private:
    CharacterMovementSettings _settings{};
    glm::vec2 _currentDirection{};

    glm::vec2 _characterSize{1.0f, 2.0f};
    glm::vec2 _halfCharacterSize{0.5f, 1.0f};
    glm::vec2 _fourthPartCharacterSize{0.25f, 0.5f};

    glm::vec2 _topRightCorner = {_characterSize.x, _characterSize.y};
    glm::vec2 _topLeftCorner = {-_characterSize.x, _characterSize.y};
    glm::vec2 _downRightCorner = {_characterSize.x, -_characterSize.y};
    glm::vec2 _downLeftCorner = {_characterSize.x, -_characterSize.y};


    float _currentJumpDuration{100000};

    glm::vec3 _velocity{0};

    std::weak_ptr<Transform> _transform{};
    std::weak_ptr<PhysicsWorld> _world{};

private:
    void ResetJump() noexcept;

    [[nodiscard]] glm::vec2 GetCenter() const noexcept;

    bool IsHitUp(glm::vec2 &hitPos) const;

    bool IsHitDir(glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const;

    bool IsHitLeft(glm::vec2 &hitPos) const;

    bool IsHitRight(glm::vec2 &hitPos) const;

    bool IsGrounded(glm::vec2 &hitPos) const;

public:
    explicit CharacterMovementComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetSettings(const CharacterMovementSettings &data) noexcept{
        _settings = data;
    }

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }

    void SetWorld(const std::weak_ptr<PhysicsWorld> &world) noexcept {
        _world = world;
    }

    void SetDirection(const glm::vec2 &direction) noexcept {
        _currentDirection = direction;
    }

    [[nodiscard]] glm::vec3 GetVelocity() const noexcept {
        return _velocity;
    }

    [[nodiscard]] glm::vec2 GetCharacterSize() const noexcept {
        return _characterSize;
    }

    void Update(const float &deltaTime) override;
};
